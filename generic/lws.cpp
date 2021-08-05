
#include <tcl.h>

#include "lws.hpp"

static int
callback_minimal(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len);

static const struct lws_protocols _protocols[] = {
	{ "tcl", callback_minimal, 0, 0, },
	{ NULL, NULL, 0, 0 }
};

static int
callback_minimal(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len)
{
	return 0;
}


LwsClient::LwsClient(const char* host, int port, const char* path, int ssl, void* userData)
	:
	m_context(_create_context())
{
	m_client_connect_info.context = m_context;
	m_client_connect_info.port = port;
	m_client_connect_info.address = host;
	m_client_connect_info.path = path;
	m_client_connect_info.host = m_client_connect_info.address;
	m_client_connect_info.origin = m_client_connect_info.address;
	m_client_connect_info.ssl_connection = ssl ? LCCSCF_USE_SSL : 0;
	m_client_connect_info.protocol = _protocols[0].name;
	m_client_connect_info.pwsi = nullptr;
	m_client_connect_info.userdata = userData;
}

LwsClient::~LwsClient()
{
	lws_context_destroy(m_context);
}

void LwsClient::service() const {
	lws_client_connect_via_info(&m_client_connect_info);

	// create a std::thread with lws_service() in the background
	// callback should create events for Tcl event loop, source to be registered

	/*
	* call to lws_service() once should trigger the protocol callback when interesting things happen. The callback reason determines what happened
	* Should react on the things in
	*/
	//lws_service(websocketPtr->lwsContext, 0);
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

