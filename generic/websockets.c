#include "websockets.h"

static int
WebsocketCmd(ClientData clientData, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[])
{
    const char* options[] = { "-async", NULL };
    enum optionIdx { OptionAsyncIdx };
    int optIdx = -1;
    int async = 0;
    char* host;
    int port;
    char* path = NULL;

    objc--;
    objv++;
    if (objc == 0) {
        Tcl_WrongNumArgs(interp, 0, objv, "?-async? host ?port? ?path?");
        return TCL_ERROR;
    }

    Tcl_GetIndexFromObj(interp, objv[0], options, "option", 0, &optIdx);
    if (optIdx == OptionAsyncIdx) {
        async = 1;
        objc--;
        objv++;
    }
    
    if (objc == 0) {
        Tcl_WrongNumArgs(interp, 0, objv, "?-async? host ?port? ?path?");
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
    Tcl_CreateObjCommand(interp, "websocket", (Tcl_ObjCmdProc*)WebsocketCmd, (ClientData)NULL,
        (Tcl_CmdDeleteProc*)Websockets_CleanupCmd);
}
#ifdef __cplusplus
}
#endif  /* __cplusplus */

