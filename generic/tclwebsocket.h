#ifndef WEBSOCKETS_H
#define WEBSOCKETS_H

#include <libwebsockets.h>
#include <tcl.h>

#if defined(BUILD_tclwebsocket) && !defined(STATIC_BUILD)
#   define TCLWEBSOCKET_EXTERN extern DLLEXPORT
#else
#   define TCLWEBSOCKET_EXTERN extern
#endif

TCLWEBSOCKET_EXTERN int Tclwebsocket_Init(Tcl_Interp* interp);

typedef struct _tclWebsocketState {
	Tcl_HashTable tclWebsockets;
	int socketUid;
} TclWebsocketsState;

typedef struct _tclWebsocket {
	Tcl_DString* handleName;
	TclWebsocketsState* statePtr;
	struct lws_context* lwsContext;
	struct lws_client_connect_info lwsClientConnectInfo;
	Tcl_Channel wsChannel;
} TclWebsocket;

void TclWebsocket_WebsocketDestroy(TclWebsocket* websocketPtr);

struct lws_context* Lws_GetClientContext();
struct lws_client_connect_info Lws_GetClientConnectInfo(struct lws_context* context, char* host, int port, char* path, int ssl);
#endif /* WEBSOCKETS_H */
