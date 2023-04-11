#pragma once

#include "common_cs.hpp"

namespace net_comms {
    class Client : public CommonCS {        

    public:
        Client(std::string ipAdress, int portNumber);
    };
}