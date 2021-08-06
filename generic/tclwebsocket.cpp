
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
}

const std::string& WebsocketClient::name() const
{
	return m_name;
}

void WebsocketClient::register_channel(Tcl_Interp* interp) const
{
	Tcl_RegisterChannel(interp, m_channel);
}

void WebsocketClient::service()
{
	m_lwsClient.service();
}

void WebsocketClient::close()
{
}

std::string WebsocketClient::_generate_name()
{
	static int uid = 0;
	std::stringstream ss;
	ss << "websocket" << uid++;
	return ss.str();
}