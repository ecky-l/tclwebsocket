#include "wschannel.hpp"

#include <thread>

int WebsocketCloseProc(ClientData instanceData, Tcl_Interp* interp)
{
	WebsocketClient* websocketPtr = (WebsocketClient*)instanceData;
	delete websocketPtr;
    return TCL_OK;
}

int WebsocketInputProc(ClientData instanceData, char* buf, int toRead, int* errorCodePtr)
{
	WebsocketClient* wsPtr = (WebsocketClient*)instanceData;
	while (!wsPtr->has_input()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	memset(buf, 0, toRead);
	return wsPtr->get_input(&buf, toRead);
}

int WebsocketOutputProc(ClientData instanceData, CONST84 char* buf, int toWrite, int* errorCodePtr)
{
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

}

int WebsocketGetHandleProc(ClientData instanceData, int direction, ClientData* handlePtr)
{
	return TCL_OK;
}

int WebsocketBlockModeProc(ClientData instanceData, int mode)
{
	return TCL_OK;
}

int WebsocketFlushProc(ClientData instanceData)
{
	return TCL_OK;
}
