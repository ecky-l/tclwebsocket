#include "websockets.h"

static int
WebsocketCmd(ClientData clientData, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[])
{
    const char* options[] = { "-async", "-ssl", NULL};
    enum optionIdx { OptionAsyncIdx, OptionsSslIdx };
    int optIdx = -1;
    int async = 0, ssl = 0;
    char* host;
    int port;
    char* path = NULL;
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
    } else {
        port = 443;
    }

    if (objc > 0) {
        path = Tcl_GetString(objv[0]);
    } else {
        path = "";
    }

    
    return TCL_OK;
}

static void
Websockets_CleanupCmd(ClientData data)
{
}


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
int Websockets_Init(Tcl_Interp* interp)
{
#ifdef USE_TCL_STUBS
    if (Tcl_InitStubs(interp, "8.1", 0) == NULL) {
        return TCL_ERROR;
    }
#endif //USE_TCL_STUBS

    Tcl_CreateObjCommand(interp, "websocket", (Tcl_ObjCmdProc*)WebsocketCmd, (ClientData)NULL,
        (Tcl_CmdDeleteProc*)Websockets_CleanupCmd);

    return TCL_OK;
}
#ifdef __cplusplus
}
#endif  /* __cplusplus */

