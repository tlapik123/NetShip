#pragma once

#include <asio/ip/tcp.hpp>


namespace net_comms {
    class Server {        
        asio::io_context mIOContext;
        asio::ip::tcp::acceptor mAcceptor;
        asio::ip::tcp::socket mEstablishedSocket;

    public:
        Server(int portNumber);

        void Send();
        void Recieve();
    };
}