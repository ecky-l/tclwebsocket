
#include <tcl.h>
#include "tclwebsocket.hpp"

static int
WebsocketCmd(ClientData clientData, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[])
{
    const char* options[] = { "-async", "-ssl", NULL };
    enum optionIdx { OptionAsyncIdx, OptionsSslIdx };
    int optIdx = -1;
    int async = 0, ssl = 0;
    char* host;
    int port;
    const char* path = NULL;
    char* arg;

    objc--;
    objv++;
    if (objc == 0) {
        Tcl_WrongNumArgs(interp, 0, objv, "?-async? ?-ssl? host ?port? ?path?");
        return TCL_ERROR;
    }

    do {
        if (*Tcl_GetString(objv[0]) != '-') {
            break;
        }

        if (Tcl_GetIndexFromObj(interp, objv[0], options, "option", 0, &optIdx) != TCL_OK) {
            return TCL_ERROR;
        }

        switch (optIdx) {
        case OptionAsyncIdx:
            async = 1;
            break;
        case OptionsSslIdx:
            ssl = 1;
            break;
        default:
            break;
        }
        objc--;
        objv++;
    } while (objc > 0);

    if (objc == 0) {
        Tcl_WrongNumArgs(interp, 0, objv, "host ?port? ?path?");
        return TCL_ERROR;
    }
    host = Tcl_GetString(objv[0]);
    objc--;
    objv++;

    if (objc > 0) {
        if (Tcl_GetIntFromObj(interp, objv[0], &port) != TCL_OK) {
            return TCL_ERROR;
        }
        objc--;
        objv++;
    }
    else {
        port = 443;
    }

    if (objc > 0) {
        path = Tcl_GetString(objv[0]);
    }
    else {
        path = "/";
    }

    auto wsPtr = new WebsocketClient(host, port, path, ssl);
    wsPtr->register_channel(interp);
    
    Tcl_SetObjResult(interp, Tcl_NewStringObj(wsPtr->name().c_str(), -1));
    return TCL_OK;
}


static void
WebSocketLogNothingFcn(int logLevel, const char* message)
{
}

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
extern DLLEXPORT
int Tclwebsocket_Init(Tcl_Interp* interp)
{
#ifdef USE_TCL_STUBS
    if (Tcl_InitStubs(interp, "8.1", 0) == nullptr) {
        return TCL_ERROR;
    }
#endif //USE_TCL_STUBS

    // TODO hook up a tcl proc for log levels
    //lws_set_log_level((LLL_USER | LLL_DEBUG |LLL_INFO | LLL_ERR | LLL_WARN | LLL_NOTICE, WebSocketLogNothingFcn);
    lws_set_log_level(LLL_USER | LLL_DEBUG | LLL_INFO | LLL_ERR | LLL_WARN | LLL_NOTICE, NULL);

    Tcl_CreateObjCommand(interp, "websocket", (Tcl_ObjCmdProc*)WebsocketCmd, (ClientData)nullptr, (Tcl_CmdDeleteProc*)nullptr);

    return TCL_OK;
}
#ifdef __cplusplus
}
#endif  /* __cplusplus */
