#pragma once

#include <array>
#include <unordered_map>
#include <unordered_set>
#include <vector>


namespace data {
    using idx_t = std::size_t;
    using position_t = std::array<idx_t, 2>;
    using board_t = std::vector<std::vector<bool>>;

    //// not needed since using lambda is nicer
    // struct PositionHash {
    //     std::size_t operator()(const position_t& pos) const {
    //         return pos[0] ^ pos[1];
    //     }
    // };


    auto positionHash = [](const position_t& arr) constexpr { return arr[0] ^ arr[1]; };

    using ship_coords_t = std::unordered_set<position_t, decltype(positionHash)>;
    using ships_t = std::vector<ship_coords_t>;

    using num_of_ships_t = std::size_t;
    using ship_size_t = std::size_t;
    using fleet_rules_t = std::unordered_map<ship_size_t, num_of_ships_t>;


    enum class HitStatus {
        Miss,
        Hit,
        Sunken,
    };
}// namespace data