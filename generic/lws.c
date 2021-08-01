#include "websockets.h"

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

struct lws_context* Lws_GetClientContext(char* host, int port, char* path, int ssl)
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