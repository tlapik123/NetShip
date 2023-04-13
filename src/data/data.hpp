#pragma once

#include <array>
#include <vector>
#include <unordered_set>


namespace data {
    using idx_t = std::size_t;
    using position_t = std::array<idx_t, 2>;
    using board_t = std::vector<bool>;

    using ship_coords_t = std::unordered_set<position_t>;
    using ships_t = std::unordered_set<ship_coords_t>;  


    enum class HitStatus {
        Miss,
        Hit,
        Sunken,
    };
}