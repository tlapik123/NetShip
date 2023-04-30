#pragma once

#include "data/data.hpp"
#include <cstddef>

namespace net_comms {
    inline data::position_t DataToPosition(const std::vector<std::byte>& data) {
        // NOTE: this can be potentially unsafe
        assert(data.size() >= sizeof(data::position_t));
        auto rawDataPtr = reinterpret_cast<const std::size_t*>(data.data());
        return {rawDataPtr[0], rawDataPtr[1]};
    }

    inline std::vector<std::byte> PositionToData(data::position_t pos) {
        auto rawDataPtr = reinterpret_cast<const std::byte*>(pos.data()); 
        return {rawDataPtr, rawDataPtr + sizeof(data::position_t)};
    }

    inline data::HitStatus DataToHitStatus(const std::vector<std::byte>& data) {
        assert(data.size() >= 2); // one byte represents hit/miss one if sunken
        if (std::to_integer<int>(data[1]) == 1) return data::HitStatus::Sunken;
        return std::to_integer<int>(data[0]) == 0 ? data::HitStatus::Miss : data::HitStatus::Hit;
    }

    inline std::vector<std::byte> HitStatusToData(data::HitStatus hitStatus) {
        std::vector<std::byte> retData = {std::byte{0}, std::byte{0}};
        switch (hitStatus) {
        case data::HitStatus::Sunken: 
            retData[1] = std::byte{1};
            return retData;
        case data::HitStatus::Hit:
            retData[0] = std::byte{1};
            return retData;
        }
        return retData;
    }
}