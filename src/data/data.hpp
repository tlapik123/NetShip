#pragma once

#include <array>
#include <unordered_map>
#include <unordered_set>
#include <vector>


namespace data {
    using idx_t = std::size_t;
    using position_t = std::array<idx_t, 2>;
    using board_t = std::vector<std::vector<bool>>;

    /** @brief Create a hash for the position_t*/
    inline auto positionHash = [](const position_t& arr) constexpr { return arr[0] ^ arr[1]; };

    using coords_t = std::vector<position_t>;
    using ship_coords_t = std::unordered_set<position_t, decltype(positionHash)>;
    using ships_t = std::vector<ship_coords_t>;

    using num_of_ships_t = std::size_t;
    using ship_size_t = std::size_t;
    using fleet_rules_t = std::unordered_map<ship_size_t, num_of_ships_t>;

    /**
     * @brief This is used as result/send type for the network communication
    */
    enum class HitStatus {
        Miss,   // We missed
        Hit,    // We hit a ship
        Sunken, // We sunk the ship
    };

    /**
     * @brief is used by a UI layer to display differentiate spaces
    */
    enum class DisplayType {
        Empty,
        Miss,
        Ship,
        Hit,
        Sunken,
    };

    using display_board_t = std::vector<std::vector<DisplayType>>;
}// namespace data