#ifndef LWS_HPP
#define LWS_HPP

#include <libwebsockets.h>
#include <thread>
#include <mutex>
#include <memory>

class LwsClient
{
public:
	LwsClient(const char* host, int port, const char* path, int ssl, void* userData);
	virtual ~LwsClient();

	void service();
	void shutdown();
private:
	struct lws_context* m_context;
	struct lws_client_connect_info m_client_connect_info;
	struct lws* m_wsi;

	bool m_shutdown;

	std::unique_ptr<std::thread> m_thread;
	std::mutex m_mutex;

	void do_service();

	static struct lws_context* _create_context();
};


#endif // !LWS_HPP
