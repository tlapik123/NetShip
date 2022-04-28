#include <catch2/catch_test_macros.hpp>
#include "io.hpp"
#include "board.hpp"

using namespace NetShip;

TEST_CASE( "String to position", "[StrToBoardPos]" ) {
    REQUIRE( !StrToBoardPos("22") ); 
    REQUIRE( StrToBoardPos("D7") == opt_pos{{6, 3}});
    REQUIRE( StrToBoardPos("AA22") == opt_pos{{21, 26}});
};


TEST_CASE( "Validate position", "[IsPosValid]" ) {
    REQUIRE( !IsPosValid({-1, 0}, {8, 8}) ); 
    REQUIRE( !IsPosValid({-6, 0}, {8, 8}) ); 
    REQUIRE( !IsPosValid({0, -3}, {8, 8}) ); 
    REQUIRE( !IsPosValid({0, -1}, {8, 8}) ); 
    REQUIRE( IsPosValid({0, 0}, {8, 8}) ); 
    REQUIRE( !IsPosValid({9, 5}, {8, 8}) ); 
    REQUIRE( !IsPosValid({934, 5}, {8, 8}) ); 
    REQUIRE( IsPosValid({4, 0}, {8, 8}) ); 
    REQUIRE( !IsPosValid({4, 345}, {8, 8}) ); 
};


TEST_CASE( "Try Hit postion", "[TryShootOrSink]" ) {
    REQUIRE(*TryShootOrSink({{false, false}, {false, false}}, {0, 0}) == bool_2d_vec{{true, false}, {false, false}}); 
    REQUIRE( !TryShootOrSink({{true, false}, {false, false}}, {0, 0})); 
};


TEST_CASE( "Ship Constructor", "[Ship]" ) {
    REQUIRE(Ship({{0, 0}, {0, 1}}).mLoc == pos_vec{{0, 0}, {0, 1}}); 
    REQUIRE(Ship({{0, 0}, {0, 1}}).mHitPoints == pos_vec{{0, 0}, {0, 1}}.size()); 
};

TEST_CASE( "Ship Creation", "[CreateShip]" ) {
    REQUIRE(CreateShip({0, 0}, true, 3).mHitPoints == 3); 
    REQUIRE(CreateShip({0, 0}, true, 3).mLoc == pos_vec{{0, 0}, {0, 1}, {0, 2}}); 
    REQUIRE(CreateShip({0, 0}, false, 3).mHitPoints == 3); 
    REQUIRE(CreateShip({0, 0}, false, 3).mLoc == pos_vec{{0, 0}, {1, 0}, {2, 0}}); 
};

TEST_CASE( "Is pos close other pos or same", "[IsCloseOrSame]" ) {
    REQUIRE(IsCloseOrSame({0,0}, {0,0})); 
    REQUIRE(IsCloseOrSame({1,1}, {0,0})); 
    REQUIRE(IsCloseOrSame({1,1}, {0,1})); 
    REQUIRE(IsCloseOrSame({1,1}, {0,2}));
    REQUIRE(IsCloseOrSame({1,1}, {1,0}));
    REQUIRE(IsCloseOrSame({1,1}, {1,2}));
    REQUIRE(IsCloseOrSame({1,1}, {2,0}));
    REQUIRE(IsCloseOrSame({1,1}, {2,1}));
    REQUIRE(IsCloseOrSame({1,1}, {2,2}));

    REQUIRE(IsCloseOrSame({0,0}, {1,1})); 
    REQUIRE(IsCloseOrSame({0,1}, {1,1})); 
    REQUIRE(IsCloseOrSame({0,2}, {1,1}));
    REQUIRE(IsCloseOrSame({1,0}, {1,1}));
    REQUIRE(IsCloseOrSame({1,2}, {1,1}));
    REQUIRE(IsCloseOrSame({2,0}, {1,1}));
    REQUIRE(IsCloseOrSame({2,1}, {1,1}));
    REQUIRE(IsCloseOrSame({2,2}, {1,1}));

    REQUIRE(!IsCloseOrSame({0,0}, {2,2}));
    REQUIRE(!IsCloseOrSame({0,0}, {3,2}));
    REQUIRE(!IsCloseOrSame({4,5}, {0,1}));
    REQUIRE(!IsCloseOrSame({4,5}, {3,3}));
};


TEST_CASE( "Find hit ship", "[TryFindHitShip]" ) {
    auto s1 = CreateShip({0, 0}, true, 3);
    auto s2 = CreateShip({1, 1}, false, 3);
    ship_set testShipVec = {s1, s2};
    REQUIRE(!TryFindHitShip(testShipVec, {1,4}));
    REQUIRE(!TryFindHitShip(testShipVec, {5,4}));
    REQUIRE(!TryFindHitShip(testShipVec, {1,0}));
    REQUIRE(TryFindHitShip(testShipVec, {0,0}) == s1);
    REQUIRE(TryFindHitShip(testShipVec, {1,1}) == s2);
    REQUIRE(TryFindHitShip(testShipVec, {0,2}) == s1);
    REQUIRE(TryFindHitShip(testShipVec, {2,1}) == s2);
};

TEST_CASE( "Find ship that sank" "[FindSunkShip]") {
    bool_2d_vec hits {
        {true, false},
        {true, false},
        {true, false},
        {false, false},
        {false, false},
        {false, true},
    };
    pos_vec shipLoc {
        {0,0},
        {1,0},
        {2,0},
    };
    REQUIRE(FindSunkShip(hits, {0,0}) == shipLoc);
    REQUIRE(FindSunkShip(hits, {1,0}) == shipLoc);
    REQUIRE(FindSunkShip(hits, {2,0}) == shipLoc);

    REQUIRE(FindSunkShip(hits, {5,1}) == pos_vec{{5,1}});
}