#ifndef WEBSOCKETSCHANNEL_H
#define WEBSOCKETSCHANNEL_H

#include "tclWebsocket.hpp"

int WebsocketCloseProc(ClientData instanceData, Tcl_Interp* interp);
int WebsocketInputProc(ClientData instanceData, char* buf, int toRead, int* errorCodePtr);
int WebsocketOutputProc(ClientData instanceData, CONST84 char* buf, int toWrite, int* errorCodePtr);
int WebsocketSetOptionProc(ClientData instanceData, Tcl_Interp* interp, const char* optionName, const char* value);
int WebsocketGetOptionProc(ClientData instanceData, Tcl_Interp* interp, CONST84 char* optionName, Tcl_DString* dsPtr);
void WebsocketWatchProc(ClientData instanceData, int mask);
/* int WebsocketGetHandleProc(ClientData instanceData, int direction, ClientData* handlePtr);*/
int WebsocketBlockModeProc(ClientData instanceData, int mode);
int WebsocketFlushProc(ClientData instanceData);
int WebsocketHandlerProc(ClientData instanceData, int interestMask);

static Tcl_ChannelType WSChannelType = {
    .typeName = (char*)"websocket",
    .version = TCL_CHANNEL_VERSION_5,
    .closeProc = WebsocketCloseProc,
    .inputProc = WebsocketInputProc,
    .outputProc = WebsocketOutputProc,
    .seekProc = (Tcl_DriverSeekProc*)NULL,
    .setOptionProc = WebsocketSetOptionProc,
    .getOptionProc = WebsocketGetOptionProc,
    .watchProc = WebsocketWatchProc,
    .getHandleProc = (Tcl_DriverGetHandleProc*)NULL,
    .close2Proc = (Tcl_DriverClose2Proc*)NULL,
    .blockModeProc = WebsocketBlockModeProc,
    .flushProc = (Tcl_DriverFlushProc*)NULL, // reserved for future use according to docs
    .handlerProc = WebsocketHandlerProc,
    .wideSeekProc = (Tcl_DriverWideSeekProc*)NULL,
    .threadActionProc = (Tcl_DriverThreadActionProc*)NULL, // later
    .truncateProc = (Tcl_DriverTruncateProc*)NULL
};


#endif /* WEBSOCKETSCHANNEL_H */
