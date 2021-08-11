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
    void cancel_service();
	void shutdown();

	//! Check if the connection is shutting down.
	//!
	//! This is the case if the socket is intentionally closed by the main thread
	//! We need to distinguish it from the case where the connection shutdown was initiated
	//! by the remote, because this is handled differently.
	bool is_shutting_down() const;

	void reset_wsi();
	void callback_on_writable();

private:
	struct lws_context* m_context;
	struct lws_client_connect_info m_client_connect_info;
	struct lws* m_wsi;

	bool m_shutdown;

	std::unique_ptr<std::thread> m_thread;
	mutable std::mutex m_mutex;

	void do_service();

	static struct lws_context* _create_context();
};


#endif // !LWS_HPP
