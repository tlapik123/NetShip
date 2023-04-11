#include "server.hpp"

#include <iostream>

namespace net_comms {
    using asio::ip::tcp;

    Server::Server(int portNumber) : mAcceptor{mIOContext, tcp::endpoint(tcp::v4(), portNumber)} {
        mEstablishedSocket = mAcceptor.accept();
    }
}// namespace net_comms