
#include <tcl.h>
#include <functional>

#include "lws.hpp"
#include "tclwebsocket.hpp"

static int
callback_minimal(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len);

static const struct lws_protocols _protocols[] = {
    { "tcl", callback_minimal, 0, 0, },
    { NULL, NULL, 0, 0 }
};

static int
callback_minimal(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len)
{
    switch (reason) {
    case LWS_CALLBACK_CLIENT_ESTABLISHED: {
        auto wsPtr = (WebsocketClient*)user;
        wsPtr->connected();
        break;
    }
    case LWS_CALLBACK_CLIENT_CLOSED: {
        auto wsPtr = (WebsocketClient*)user;
        wsPtr->shutdown();
        return -1;
    }
    case LWS_CALLBACK_CLIENT_RECEIVE: {
        auto wsPtr = (WebsocketClient*)user;
        wsPtr->add_input(in, len);
        break;
    }
    case LWS_CALLBACK_GET_THREAD_ID: // return thread id
        return std::hash<std::thread::id>{}(std::this_thread::get_id());
    case LWS_CALLBACK_CLIENT_WRITEABLE: {
        auto wsPtr = (WebsocketClient*)user;
        const char* buf;
        size_t len = 0;
        if (wsPtr->get_output(&buf, &len)) {
            int written = lws_write(wsi, ((unsigned char*)buf), len, LWS_WRITE_TEXT);
            if (written < len) {
                // TODO error handling
            }
            wsPtr->next_output();
        }
        break;
    }
    default:
        break;
    }
    return 0;
}


LwsClient::LwsClient(const char* host, int port, const char* path, int ssl, void* userData)
    :
    m_context(_create_context()),
    m_client_connect_info(),
    m_shutdown(false),
    m_thread(nullptr),
    m_wsi(nullptr)
{
    m_client_connect_info.context = m_context;
    m_client_connect_info.port = port;
    m_client_connect_info.address = host;
    m_client_connect_info.path = path;
    m_client_connect_info.host = m_client_connect_info.address;
    m_client_connect_info.origin = m_client_connect_info.address;
    m_client_connect_info.ssl_connection = ssl ? LCCSCF_USE_SSL : 0;
    m_client_connect_info.protocol = _protocols[0].name;
    m_client_connect_info.pwsi = &m_wsi;
    m_client_connect_info.userdata = userData;
}

LwsClient::~LwsClient()
{
    shutdown();
    if (m_thread != nullptr && m_thread->joinable()) {
        m_thread->join();
    }
    lws_context_destroy(m_context);
}

void LwsClient::service()
{
    m_thread = std::make_unique<std::thread>(std::bind(&LwsClient::do_service, this));
}

void LwsClient::do_service()
{
    lws_client_connect_via_info(&m_client_connect_info);
    int n = 0;
    while (n >= 0 && m_wsi && !m_shutdown) {
        // Run as long as the return code of lws_service is positive,
        // the wsi is intact and no shutdown is initiated.
        // The wsi will become nullptr, when a server or client side shutdown was initiated.
        n = lws_service(m_context, 0);
    }
}

void LwsClient::cancel_service()
{
    reset_wsi();
    lws_cancel_service(m_context);
}


void LwsClient::shutdown() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_shutdown = true;
    if (m_wsi != nullptr) { // only if the wsi was not closed before
        lws_close_reason(m_wsi, LWS_CLOSE_STATUS_NORMAL, (unsigned char*)"", 0);
        lws_callback_on_writable(m_wsi);
    }
}

bool LwsClient::is_shutting_down() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_shutdown;
}

void LwsClient::reset_wsi()
{
    m_wsi = nullptr;
}

void LwsClient::callback_on_writable()
{
    lws_callback_on_writable(m_wsi);
}


struct lws_context* LwsClient::_create_context()
{
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof info);

    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
    info.port = CONTEXT_PORT_NO_LISTEN;
    info.protocols = _protocols;
    return lws_create_context(&info);
}

