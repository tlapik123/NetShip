#pragma once

#include <vector>
#include <utility>

namespace NetShip {
    using position = std::pair<std::size_t, std::size_t>;
    using pos_vec = std::vector<position>;
    
    struct Ship {
        pos_vec mLoc;
        int mHitPoints;

        Ship(pos_vec&& loc) : mLoc(std::move(loc)), mHitPoints(mLoc.size()) {}
        Ship(const pos_vec& loc) : mLoc(loc), mHitPoints(mLoc.size()) {}

        bool operator ==(const Ship& other) const {
            return mHitPoints == other.mHitPoints && other.mLoc == mLoc;
        }
    };
}

template<>
struct std::hash<NetShip::Ship> {
    std::size_t operator ()(NetShip::Ship const& s) const noexcept {
        return std::hash<decltype(s.mHitPoints)>{}(s.mHitPoints);
    }
};