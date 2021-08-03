#ifndef WEBSOCKETS_H
#define WEBSOCKETS_H

#include <libwebsockets.h>
#include <tcl.h>

#if defined(BUILD_tclwebsockets) && !defined(STATIC_BUILD)
#   define TCLWEBSOCKETS_EXTERN extern DLLEXPORT
#else
#   define TCLWEBSOCKETS_EXTERN extern
#endif

TCLWEBSOCKETS_EXTERN int Tclwebsockets_Init(Tcl_Interp* interp);

typedef struct _tclWebsocketsState {
	Tcl_HashTable tclWebsockets;
	int socketUid;
} TclWebsocketsState;

typedef struct _tclWebsocket {
	Tcl_DString* handleName;
	TclWebsocketsState* statePtr;
	struct lws_context* lwsContext;
	struct lws_client_connect_info lwsClientConnectInfo;
} TclWebsocket;


struct lws_context* Lws_GetClientContext();
struct lws_client_connect_info Lws_GetClientConnectInfo(struct lws_context* context, char* host, int port, char* path, int ssl);
#endif /* WEBSOCKETS_H */
