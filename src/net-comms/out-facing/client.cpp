#include "client.hpp"

#include <iostream>

namespace net_comms {
    using asio::ip::tcp;

    Client::Client(std::string ipAddress, int portNumber) {
        mEstablishedSocket.connect(tcp::endpoint(asio::ip::address::from_string(ipAddress), portNumber));
    }
}// namespace net_comms