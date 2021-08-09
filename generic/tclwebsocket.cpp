
#include "tclwebsocket.hpp"
#include "wschannel.hpp"

#include <sstream>
#include <list>

WebsocketClient::WebsocketClient(const char* host, int port, const char* path, int ssl)
	:
	m_name(_generate_name()),
	m_lwsClient(host, port, path, ssl, this),
	m_channel(Tcl_CreateChannel(&WSChannelType, m_name.c_str(), this, TCL_READABLE|TCL_WRITABLE))
{
}

WebsocketClient::~WebsocketClient()
{
	int e = Tcl_IsChannelExisting(m_name.c_str());
}

const std::string& WebsocketClient::name() const
{
	return m_name;
}

void WebsocketClient::service()
{
	m_lwsClient.service();
}

void WebsocketClient::register_channel(Tcl_Interp* interp) const
{
	Tcl_RegisterChannel(interp, m_channel);
}

void WebsocketClient::close()
{
	m_lwsClient.reset_wsi();
	Tcl_NotifyChannel(m_channel, TCL_EXCEPTION);
}

void WebsocketClient::add_input(void* in, int len)
{
	std::scoped_lock<std::mutex> lock(m_mutex_input);
	// TODO: handle for binary data (add at all?)
	auto& data = m_input.emplace_back(std::vector<char>(size_t(len) + 1));
	memcpy(&data[0], in, len);
	data[len] = '\n';
}

int WebsocketClient::get_input(char** buf, int toRead)
{
	std::scoped_lock<std::mutex> lock(m_mutex_input);
	if (m_input.empty()) {
		return 0;
	}
	auto first = m_input.front();
	int len = first.size();
	memcpy(*buf, &first[0], len);
	m_input.pop_front();
	return ++len;
}

bool WebsocketClient::has_input() const
{
	std::scoped_lock<std::mutex> lock(m_mutex_input);
	return !m_input.empty();
}

void WebsocketClient::add_output(const char* buf, size_t len)
{
	std::scoped_lock<std::mutex> lock(m_mutex_output);
	if (buf[len - 1] == '\n') {
		len--;
	}
	auto& data = m_output.emplace_back(std::vector<char>(LWS_PRE + len));
	memcpy(&data[LWS_PRE], buf, len);
	m_lwsClient.callback_on_writable();
}

bool WebsocketClient::get_output(const char** buf, size_t* len) const
{
	std::scoped_lock<std::mutex> lock(m_mutex_output);
	if (m_output.empty()) {
		return false;
	}
	*buf = m_output.front().data() + LWS_PRE;
	*len = m_output.front().size() - LWS_PRE;
	return true;
}

void WebsocketClient::next_output()
{
	std::scoped_lock<std::mutex> lock(m_mutex_output);
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