#ifndef WEBSOCKETS_H
#define WEBSOCKETS_H

#include <unordered_map>
#include <string>
#include <tcl.h>
#include <memory>
#include <condition_variable>
#include <list>
#include <vector>

#include "lws.hpp"

enum class WsTransmission
{
    TEXT,
    BINARY
};

class WebsocketClient
{
public:
    WebsocketClient(const char* host, int port, const char* path, int ssl);
    virtual ~WebsocketClient();

    void blocking(bool blocking);
    bool blocking() const;

    const std::string& name() const;

    bool service();

    //! Called when the client connection is established (LWS_CALLBACK_CLIENT_ESTABLISHED)
    //!
    //! If in sync mode, notifies a condition_variable that the connection is alive. This is
    //! where the service() method waits upon.
    void connected();

    void register_channel(Tcl_Interp* interp) const;
    void unregister_channel(Tcl_Interp* interp) const;
    int set_channel_option(Tcl_Interp* interp, const char* option, const char* value) const;

    //! called from the lws callback when a LWS_CALLBACK_*_CLOSED event occurs
    void shutdown();

    bool has_input() const;
    void add_input(void* in, int len);
    int get_input(char** buf, int toRead, int *errorCodePtr);

    void add_output(const char* buf, size_t len);
    bool get_output(const char** buf, size_t* len) const;
    void next_output();

    void set_transmission(const WsTransmission transmission);
    const WsTransmission get_transmission() const;
private:
    const std::string m_name;
    LwsClient m_lwsClient;
    const Tcl_Channel m_channel;

    mutable std::mutex m_mutex_connected;
    mutable std::condition_variable m_cond_connected;
    bool m_connected;
    bool m_eof;
    bool m_blocking;

    WsTransmission m_transmission;

    mutable std::mutex m_mutex_input;
    std::condition_variable m_cond_input;
    std::list<std::vector<char>> m_input;

    mutable std::mutex m_mutex_output;
    std::list<std::vector<char>> m_output;

    static std::string _generate_name();
};


#endif /* WEBSOCKETS_H */
