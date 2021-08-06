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
	void service();
	void close();

	bool has_input() const;
	void add_input(void* in, int len);
	int get_input(char** buf, int toRead);
private:
	const std::string m_name;
	LwsClient m_lwsClient;
	const Tcl_Channel m_channel;

	mutable std::mutex m_mutex_input;
	mutable std::mutex m_mutex_output;
	std::list<std::vector<char>> m_input;
	std::list<std::vector<char>> m_output;

	static std::string _generate_name();
};


#endif /* WEBSOCKETS_H */
