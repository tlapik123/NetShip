#include "client.hpp"

#include <iostream>

namespace net_comms {
    using asio::ip::tcp;

    Client::Client(std::string ipAdress, int portNumber) {
        mEstablishedSocket.connect(tcp::endpoint(asio::ip::address::from_string(ipAdress), portNumber));
    }
}// namespace net_comms