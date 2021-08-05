#ifndef WEBSOCKETS_H
#define WEBSOCKETS_H

#include <unordered_map>
#include <string>
#include <tcl.h>
#include <memory>

#include "lws.hpp"

class WebsocketClient
{
public:
	WebsocketClient(const char* host, int port, const char* path, int ssl);
	virtual ~WebsocketClient();

	const std::string& name() const;
	void register_channel(Tcl_Interp* interp) const;

private:
	std::string m_name;
	LwsClient m_lwsClient;
	Tcl_Channel m_channel;

	static std::string _generate_name();
};


#endif /* WEBSOCKETS_H */
