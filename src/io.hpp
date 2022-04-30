#pragma once

#include <algorithm>
#include <charconv>
#include <functional>
#include <iostream>
#include <locale>
#include <numeric>
#include <optional>
#include <utility>

#include <asio.hpp>

namespace NetShip {
    using position = std::pair<std::size_t, std::size_t>;
    using dimensions = std::pair<std::size_t, std::size_t>;
    using opt_pos = std::optional<position>;
    using opt_size_t = std::optional<std::size_t>;
    using opt_int = std::optional<int>;

    using namespace std::placeholders;
    using asio::ip::tcp;

    enum class ShotResult {
        Miss,
        Hit,
        Sink,  
    };

    namespace {
        auto currLocale = std::locale();

        /**
         * @brief Function used by the column accumulator.
         * 
         * @param acc Accumulator
         * @param c Current item
         * @return opt_int nullopt_t when acc was nullopt or when the char is not alpha
         */
        opt_int ColAcc(opt_int acc, char c) {
            // there was error somewhere
            if (!acc) return acc;
            if (!std::isalpha(c, currLocale)) return {};
            return {(*acc) + 26 + std::tolower(c, currLocale) - 'a'};
        }

        /**
         * @brief Converts string to row number
         * 
         * @param str Str to convert
         * @return opt_size_t nullopt_t when the number couldnt be converted to size_t
         */
        opt_size_t StrToRow(std::string_view str) {
            std::size_t res;
            auto start = str.data();
            auto end = start + str.size();
            auto [ptr, ec] = std::from_chars(start, end, res);

            // TODO? print some error message?
            if (ec == std::errc::invalid_argument) {
                // no number is first
                return {};
            }
            if (ec == std::errc::result_out_of_range) {
                // number too big
                return {};
            }
            if (ptr != end) {
                // there is something behind number
                return {};
            }
            return {res-1};
        }
    }

    /**
     * @brief Convert string to positon
     * 
     * @param str string to convert to postion
     * @return opt_pos positon or nothing
     */
    opt_pos StrToBoardPos(std::string_view str) {
        auto numStart = std::find_if(str.cbegin(), str.cend(), std::bind(std::isdigit<char>, _1, currLocale));

        auto numStartPos = numStart - str.cbegin();
        std::string_view cols = str.substr(0, numStartPos);
        std::string_view rows = str.substr(numStartPos, str.size() - numStartPos);

        if (cols.empty() || rows.empty()) {
            // no collumn or row was supplied
            return {};
        }

        auto optRowPos = StrToRow(rows);
        if (!optRowPos) return {}; // no col pos can be yielded

        auto optColPos = std::accumulate(cols.cbegin(), cols.cend(), opt_int{-26}, ColAcc);
        if (!optColPos) return {}; // no row pos can be yielded

        return {{*optRowPos, *optColPos}};
    };

    position NetGetPos() {
        // TODO implement this
        return {4,2};
    }

    ShotResult NetSendPosGetRes(position pos) {
        return ShotResult::Miss;
    }

    void NetSendRes(ShotResult res) {
        
    }

    position UserGetPos(std::istream& input) {
        std::string line;
        input >> line;
        auto optPos = StrToBoardPos(line);
        if (!optPos) {
            // repromt user
        }
        return *optPos;
    }





    
}// namespace NetShip
