#pragma once

#include <algorithm>
#include <functional>
#include <unordered_set>
#include <utility>
#include <vector>

#include "ship.hpp"

namespace NetShip {
    using position = std::pair<std::size_t, std::size_t>;
    using dimensions = std::pair<std::size_t, std::size_t>;
    using direction = std::pair<int, int>;
    using bool_2d_vec = std::vector<std::vector<bool>>;
    using opt_bool_2d_vec = std::optional<bool_2d_vec>;
    using ship_set = std::unordered_set<Ship>;
    using opt_ship = std::optional<Ship>;
    using pos_vec = std::vector<position>;
    using is_horiz = bool;

    bool IsPosValid(position pos, dimensions dim) {
        auto&& [posX, posY] = pos;
        auto&& [dimX, dimY] = dim;
        return posX < dimX && posY < dimY;
    }

    namespace {
        using int_arr = std::array<int, 8>;

        static constexpr int_arr XModif = {-1, 1, 0, 0, -1, -1, 1, 1};
        static constexpr int_arr YModif = {0, 0, -1, 1, -1, 1, -1, 1};

        bool IsHit(const Ship& s, position pos) {
            return std::ranges::any_of(s.mLoc, [=](auto&& loc) { return loc == pos; });
        }

        position FindTopLeftOfShip(const bool_2d_vec& hits, position hit, bool isHorizontal) {
            auto&& [posX, posY] = hit;
            dimensions dim {hits.size(), hits.at(0).size()};
            position check = isHorizontal ? position{posX, posY - 1} : position{posX - 1, posY};
            if (IsPosValid(check, dim) && hits.at(check.first).at(check.second)) {
                return FindTopLeftOfShip(hits, check, isHorizontal);
            }
            return hit;
        }

        is_horiz FindShipDir(const bool_2d_vec& hits, position hit) {
            auto&& [posX, posY] = hit;
            dimensions dim {hits.size(), hits.at(0).size()};
            // try left/right
            for (auto&& check : pos_vec{{posX, posY + 1}, {posX, posY - 1}}) {
                if (IsPosValid(check, dim) && hits.at(check.first).at(check.second)) {
                    return true;
                }
            }
            // it has to be up/down
            return false;
        }
    }// namespace

    opt_bool_2d_vec TryShootOrSink(const bool_2d_vec& hits, position pos) {
        auto&& [posX, posY] = pos;
        if (hits.at(posX).at(posY)) return {};
        auto retHit(hits);
        retHit[posX][posY] = true;
        return {std::move(retHit)};
    }

    /**
     * @brief Start and end are on the same "line".
     * 
     * @param start start position
     * @param end end position
     * @return true They are on the same line.
     * @return false They are not on the same line.
     */
    bool IsAligned(position start, position end) {
        auto&& [startX, startY] = start;
        auto&& [endX, endY] = end;
        return startX == endX || startY == endY;
    }// TODO: remove this if not used

    Ship CreateShip(position pos, bool horizontal, std::size_t length) {
        auto&& [posX, posY] = pos;
        pos_vec loc;
        for (auto i = 0; i < length; i++) {
            if (horizontal) {
                loc.emplace_back(posX, posY + i);
            } else {
                loc.emplace_back(posX + i, posY);
            }
        }
        return {std::move(loc)};
    }

    bool IsCloseOrSame(position a, position b) {
        if (a == b) return true;
        auto&& [aX, aY] = a;
        auto&& [bX, bY] = b;

        bool res = false;
        for (auto i = 0; i < XModif.size(); i++) {
            if ((aX + XModif[i]) == bX && (aY + YModif[i]) == bY) {
                return true;
            }
        }
        return false;
    }

    pos_vec FindSunkShip(const bool_2d_vec& hits, position lastHit) {
        bool isHorizontal = FindShipDir(hits, lastHit);
        auto&& [posX, posY] = FindTopLeftOfShip(hits, lastHit, isHorizontal);
        pos_vec retVec;
        retVec.emplace_back(posX, posY);
        dimensions dim {hits.size(), hits.at(0).size()};
        auto&& [dirX, dirY] = isHorizontal ? direction{0, 1} : direction{1, 0};
        position check = {posX + dirX, posY + dirY};
        // out of bounds check && ship check
        while (IsPosValid(check, dim) && hits.at(check.first).at(check.second)) {
            retVec.push_back(check);
            check = {check.first + dirX, check.second + dirY};
        }
        return retVec;
    }

    opt_ship TryFindHitShip(const ship_set& ships, position shot) {
        auto it = std::ranges::find_if(ships, std::bind(IsHit, _1, shot));
        if (it == ships.end()) return {};
        return {*it};
    }

}// namespace NetShip