#pragma once

#include "data/data.hpp"

namespace net_comms {
    data::position_t DataToPosition(const std::vector<std::byte>& data) {
        // NOTE: this can be potentially unsafe
        assert(data.size() >= sizeof(data::position_t));
        auto rawDataPtr = reinterpret_cast<const std::size_t*>(data.data());
        return {rawDataPtr[0], rawDataPtr[1]};
    }

    std::vector<std::byte> PositionToData(data::position_t pos) {
        auto rawDataPtr = reinterpret_cast<const std::byte*>(pos.data()); 
        return {rawDataPtr, rawDataPtr + sizeof(data::position_t)};
    }
}