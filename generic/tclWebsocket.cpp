
#include "tclwebsocket.hpp"
#include "wschannel.hpp"

#include <sstream>
#include <list>

WebsocketClient::WebsocketClient(const char* host, int port, const char* path, int ssl)
    :
    m_name(_generate_name()),
    m_lwsClient(host, port, path, ssl, this),
    m_channel(Tcl_CreateChannel(&WSChannelType, m_name.c_str(), this, TCL_READABLE|TCL_WRITABLE)),
    m_connected(false),
    m_blocking(true)
{
}

WebsocketClient::~WebsocketClient()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex_input);
        m_input.clear();
    }

    {
        std::lock_guard<std::mutex> lock(m_mutex_output);
        m_output.clear();
    }
}

void WebsocketClient::blocking(bool blocking)
{
    std::lock_guard<std::mutex> lock(m_mutex_input);
    m_blocking = blocking;
}

bool WebsocketClient::blocking() const
{
    return m_blocking;
}

const std::string& WebsocketClient::name() const
{
    return m_name;
}

bool WebsocketClient::service()
{
    using namespace std::chrono_literals;

    m_lwsClient.service();

    // wait for connection to be established in sync mode
    std::unique_lock<std::mutex> lock(m_mutex_connected);
    m_cond_connected.wait_for(lock, 5s, [&]() { return m_connected; });

    if (!m_connected) {
        m_lwsClient.cancel_service();
    }

    return m_connected;
}

void WebsocketClient::connected()
{
    std::lock_guard<std::mutex> lock(m_mutex_connected);
    m_connected = true;
    m_cond_connected.notify_one();
}


void WebsocketClient::register_channel(Tcl_Interp* interp) const
{
    Tcl_RegisterChannel(interp, m_channel);
}
void WebsocketClient::unregister_channel(Tcl_Interp* interp) const
{
    if (Tcl_IsChannelRegistered(interp, m_channel)) {
        Tcl_UnregisterChannel(interp, m_channel);
    }
}

void WebsocketClient::shutdown()
{
    m_lwsClient.reset_wsi();
    if (!m_lwsClient.is_shutting_down()) {
        Tcl_NotifyChannel(m_channel, TCL_EXCEPTION);
    }
}

void WebsocketClient::add_input(void* in, int len)
{
    std::lock_guard<std::mutex> lock(m_mutex_input);
    auto& data = m_input.emplace_back(std::vector<char>(size_t(len)));
    memcpy(&data[0], in, len);

    // For text input we must add an extra newline, because we cannot rely on getting
    // an ending newline from the server. It screws up [gets] if we don't get an ending 
    // newline, since [gets] reads all characters up to the first newline it finds. If
    // there is no newline, [gets] will hang in an endless loop.
    // For binary data nothing must be appended.
    data.push_back('\n');
    m_cond_input.notify_one();
}

int WebsocketClient::get_input(char** buf, int toRead, int* errorCodePtr)
{
    std::unique_lock<std::mutex> lock(m_mutex_input);
    if (m_blocking) {
        m_cond_input.wait(lock, [&]() { return !m_input.empty(); });
    }
    else {
        // Nonblocking mode. Indicate to the tcl channel layer that no input
        // is available by setting the errorCode to EAGAIN and return -1. This
        // will lead to [fblocked] returning true.
        if (m_input.empty()) {
            *errorCodePtr = EAGAIN;
            return -1;
        }
    }

    // Avoid pessimistic locking while assembling all inputs for the Tcl layer.
    // The only points where the lock is needed is when the std::list is modified.
    lock.unlock();

    size_t read = 0;
    while (!m_input.empty()) {
        auto& first = m_input.front();
        int len = first.size();

        if ((read + len) > toRead) {
            size_t toAdd = toRead - read;
            size_t remaining = len - toAdd;
            memcpy(*buf + read, &first[0], toAdd);

            auto tmp = first;

            lock.lock();
            m_input.pop_front();
            auto& data = m_input.emplace_front(std::vector<char>(remaining));
            lock.unlock();

            memcpy(&data[0], &tmp[toAdd], remaining);
            break;
        }
        else {
            memcpy(*buf + read, &first[0], len);
            read += len;

            lock.lock();
            m_input.pop_front();
            lock.unlock();
        }
    }

    return read;
}

bool WebsocketClient::has_input() const
{
    std::lock_guard<std::mutex> lock(m_mutex_input);
    return !m_input.empty();
}

void WebsocketClient::add_output(const char* buf, size_t len)
{
    std::lock_guard<std::mutex> lock(m_mutex_output);
    auto& data = m_output.emplace_back(std::vector<char>(LWS_PRE + len));
    memcpy(&data[LWS_PRE], buf, len);
    m_lwsClient.callback_on_writable();
}

bool WebsocketClient::get_output(const char** buf, size_t* len) const
{
    std::lock_guard<std::mutex> lock(m_mutex_output);
    if (m_output.empty()) {
        return false;
    }
    *buf = m_output.front().data() + LWS_PRE;
    *len = m_output.front().size() - LWS_PRE;
    return true;
}

void WebsocketClient::next_output()
{
    std::lock_guard<std::mutex> lock(m_mutex_output);
    if (m_output.empty()) {
        // This can happen if the socket is destroyed while we are here, since the
        // destructor empties both, input and output buffers.
        // In this case the following pop_front must not happen, but there is nothing
        // more to do anyway.
        return;
    }

    m_output.pop_front();
    if (!m_output.empty()) {
        m_lwsClient.callback_on_writable();
    }
}

std::string WebsocketClient::_generate_name()
{
    static int uid = 0;
    std::stringstream ss;
    ss << "websocket" << uid++;
    return ss.str();
}