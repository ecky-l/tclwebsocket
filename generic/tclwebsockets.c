#include "tclwebsockets.h"


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

    TclWebsocketsState* statePtr;
    TclWebsocket* websocketPtr;

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

    statePtr = (TclWebsocketsState*)clientData;
    websocketPtr = ckalloc(sizeof(TclWebsocket));
    websocketPtr->statePtr = statePtr;
    websocketPtr->handleName = (Tcl_DString*)ckalloc(sizeof(Tcl_DString));
    Tcl_DStringInit(websocketPtr->handleName);
    Tcl_DStringAppend(websocketPtr->handleName, "websocket", -1);
    {
        char num[10];
        itoa(statePtr->socketUid++, num, 10);
        Tcl_DStringAppend(websocketPtr->handleName, num, -1);
    }

    websocketPtr->lwsContext = Lws_GetClientContext();
    websocketPtr->lwsClientConnectInfo = Lws_GetClientConnectInfo(websocketPtr->lwsContext, host, port, path, ssl);
    
    {
        int isNew;
        Tcl_HashEntry* entry = Tcl_CreateHashEntry(&statePtr->tclWebsockets, Tcl_DStringValue(websocketPtr->handleName), &isNew);
        Tcl_SetHashValue(entry, websocketPtr);
    }

    Tcl_SetObjResult(interp, Tcl_NewStringObj(Tcl_DStringValue(websocketPtr->handleName), -1));
    return TCL_OK;
}

static void
WebsocketDestroy(TclWebsocket* websocketPtr)
{

    if (websocketPtr->lwsContext != NULL) {
        Lws_DestroyContext(websocketPtr->lwsContext);
    }

    {
        Tcl_HashEntry* entry = Tcl_FindHashEntry(&websocketPtr->statePtr->tclWebsockets, Tcl_DStringValue(websocketPtr->handleName));
        if (entry != NULL) {
            Tcl_DeleteHashEntry(entry);
        }
        websocketPtr->statePtr = NULL;
    }

    Tcl_DStringFree(websocketPtr->handleName);
    ckfree(websocketPtr->handleName);
    ckfree(websocketPtr);
}

static void
Websockets_CleanupCmd(ClientData data)
{
    TclWebsocketsState* wsState = (TclWebsocketsState*)data;
    Tcl_HashEntry* entry;
    Tcl_HashSearch search;

    entry = Tcl_FirstHashEntry(&wsState->tclWebsockets, &search);
    while (entry != NULL) {
        /* delete the websocket handle */
        TclWebsocket* websocketPtr = (TclWebsocket*)Tcl_GetHashValue(entry);
        WebsocketDestroy(websocketPtr);
        entry = Tcl_FirstHashEntry(&wsState->tclWebsockets, &search);
    }

    Tcl_DeleteHashTable(&wsState->tclWebsockets);
    Tcl_Free(wsState);
    wsState = NULL;
}

static void
WebSocketLogNothingFcn(int logLevel, const char* message)
{
}

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
TCLWEBSOCKETS_EXTERN
int Tclwebsockets_Init(Tcl_Interp* interp)
{
    TclWebsocketsState* tclWebSocketsState = NULL;
#ifdef USE_TCL_STUBS
    if (Tcl_InitStubs(interp, "8.1", 0) == NULL) {
        return TCL_ERROR;
    }
#endif //USE_TCL_STUBS

    if (tclWebSocketsState == NULL) {
        tclWebSocketsState = (TclWebsocketsState*)Tcl_Alloc(sizeof(TclWebsocketsState));
        Tcl_InitHashTable(&tclWebSocketsState->tclWebsockets, TCL_STRING_KEYS);
        tclWebSocketsState->socketUid = 0;
    }
    
    // TODO hook up a tcl proc for log levels
    lws_set_log_level(LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE, WebSocketLogNothingFcn);

    Tcl_CreateObjCommand(interp, "websocket", (Tcl_ObjCmdProc*)WebsocketCmd, (ClientData)tclWebSocketsState,
        (Tcl_CmdDeleteProc*)Websockets_CleanupCmd);

    return TCL_OK;
}
#ifdef __cplusplus
}
#endif  /* __cplusplus */

