#include "wsChannel.hpp"

#include <thread>
#include <string>
#include <sstream>

int WebsocketCloseProc(ClientData instanceData, Tcl_Interp* interp)
{
    auto wsPtr = (WebsocketClient*)instanceData;
    delete wsPtr;
    return TCL_OK;
}

int WebsocketInputProc(ClientData instanceData, char* buf, int toRead, int* errorCodePtr)
{
    auto wsPtr = (WebsocketClient*)instanceData;
    memset(buf, 0, toRead);
    // return 0; indicates eof
    return wsPtr->get_input(&buf, toRead, errorCodePtr);
}

int WebsocketOutputProc(ClientData instanceData, CONST84 char* buf, int toWrite, int* errorCodePtr)
{
    auto wsPtr = (WebsocketClient*)instanceData;
    wsPtr->add_output(buf, toWrite);
    return toWrite;
}

int WebsocketSetOptionProc(ClientData instanceData, Tcl_Interp* interp, const char* optionName, const char* value)
{
    auto wsPtr = (WebsocketClient*)instanceData;
    if (std::string("-transmission").compare(optionName) == 0) {
        size_t strLen = strlen(value);
        if (std::string("binary").compare(0, strLen, value, strLen) == 0) {
            wsPtr->set_transmission(WsTransmission::BINARY);
        }
        else if (std::string("text").compare(0, strLen, value, strLen) == 0) {
            wsPtr->set_transmission(WsTransmission::TEXT);
        }
        else {
            std::stringstream ss;
            ss << "Wrong value for -transmission: " << value << ". Must be \"text\" or \"binary\"";
            Tcl_SetObjResult(interp, Tcl_NewStringObj(ss.str().c_str(), -1));
            return TCL_ERROR;
        }
    }
    else {
        std::stringstream ss;
        ss << "No such option " << optionName;
        Tcl_SetObjResult(interp, Tcl_NewStringObj(ss.str().c_str(), -1));
        return TCL_ERROR;
    }
    return TCL_OK;
}

#define WSPTR_GET_TRANSMISSION_STR(wsPtr) (wsPtr->get_transmission() == WsTransmission::TEXT ? "text" : "binary")

int WebsocketGetOptionProc(ClientData instanceData, Tcl_Interp* interp, CONST84 char* optionName, Tcl_DString* dsPtr)
{
    auto wsPtr = (WebsocketClient*)instanceData;
    if (optionName == nullptr) {
        Tcl_DStringAppendElement(dsPtr, "-transmission");
        Tcl_DStringAppendElement(dsPtr, WSPTR_GET_TRANSMISSION_STR(wsPtr));
    }
    else {
        if (std::string("-transmission").compare(optionName) == 0) {
            Tcl_DStringAppend(dsPtr, WSPTR_GET_TRANSMISSION_STR(wsPtr), -1);
        }
        else {
            std::stringstream ss;
            ss << "No such option " << optionName;
            Tcl_SetObjResult(interp, Tcl_NewStringObj(ss.str().c_str(), -1));
            return TCL_ERROR;
        }
    }
    return TCL_OK;
}

void WebsocketWatchProc(ClientData instanceData, int mask)
{
    int i = 0;
}

int WebsocketGetHandleProc(ClientData instanceData, int direction, ClientData* handlePtr)
{
    return TCL_OK;
}

int WebsocketBlockModeProc(ClientData instanceData, int mode)
{
    auto wsPtr = (WebsocketClient*)instanceData;
    wsPtr->blocking(mode == TCL_MODE_BLOCKING);
    return TCL_OK;
}

int WebsocketFlushProc(ClientData instanceData)
{
    return TCL_OK;
}

int WebsocketHandlerProc(ClientData instanceData, int interestMask)
{
    int i = 0;
    return interestMask;
}
