#include "wschannel.hpp"

#include <thread>

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
	return TCL_OK;
}

int WebsocketGetOptionProc(ClientData instanceData, Tcl_Interp* interp, CONST84 char* optionName, Tcl_DString* dsPtr)
{
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
