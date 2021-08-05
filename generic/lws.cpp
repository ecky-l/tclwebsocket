
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

struct lws_context* Lws_GetClientContext()
{
	struct lws_context* context = NULL;
	struct lws_context_creation_info info;
	memset(&info, 0, sizeof info);

	info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	info.port = CONTEXT_PORT_NO_LISTEN;
	info.protocols = _protocols;
	context = lws_create_context(&info);
	return context;
}

void Lws_DestroyContext(struct lws_context* context)
{
	lws_context_destroy(context);
}

struct lws_client_connect_info* Lws_GetClientConnectInfo(struct lws_context* context, char* host, int port, const char* path, int ssl, void* userData)
{
	struct lws_client_connect_info* info = (struct lws_client_connect_info*)ckalloc(sizeof(struct lws_client_connect_info));
	info->context = context;
	info->port = port;
	info->address = host;
	info->path = path;
	info->host = info->address;
	info->origin = info->address;
	info->ssl_connection = ssl ? LCCSCF_USE_SSL : 0;
	info->protocol = _protocols[0].name;
	info->pwsi = NULL;
	info->userdata = userData;
	return info;
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

struct lws_context* LwsClient::_create_context()
{
	struct lws_context* context = nullptr;
	struct lws_context_creation_info info;
	memset(&info, 0, sizeof info);

	info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
	info.port = CONTEXT_PORT_NO_LISTEN;
	info.protocols = _protocols;
	context = lws_create_context(&info);
	return context;
}
