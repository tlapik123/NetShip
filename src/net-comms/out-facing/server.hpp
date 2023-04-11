#pragma once

#include "common_cs.hpp"


namespace net_comms {
    class Server : public CommonCS {        
        asio::ip::tcp::acceptor mAcceptor;

    public:
        Server(int portNumber);
    };
}