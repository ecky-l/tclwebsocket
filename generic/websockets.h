#ifndef WEBSOCKETS_H
#define WEBSOCKETS_H

#include <libwebsockets.h>
#include <tcl.h>

#if defined(BUILD_websockets) && !defined(STATIC_BUILD)
#   define WEBSOCKETS_EXTERN extern DLLEXPORT
#else
#   define WEBSOCKETS_EXTERN extern
#endif

WEBSOCKETS_EXTERN int Websockets_Init(Tcl_Interp* interp);

#endif /* WEBSOCKETS_H */
