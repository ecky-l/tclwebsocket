#ifndef LWS_HPP
#define LWS_HPP

#include <libwebsockets.h>

class LwsClient
{
public:
	LwsClient(const char* host, int port, const char* path, int ssl, void* userData);
	virtual ~LwsClient();

	void service() const;
private:
	struct lws_context* m_context;
	struct lws_client_connect_info m_client_connect_info;

	static struct lws_context* _create_context();
};


#endif // !LWS_HPP
