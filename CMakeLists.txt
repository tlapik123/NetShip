cmake_minimum_required(VERSION 3.0.0)
project(netShip VERSION 0.1.0)

# include(CTest)
# enable_testing()

find_package(PkgConfig REQUIRED)

# find asio
pkg_check_modules(asio REQUIRED IMPORTED_TARGET asio)

# find ftxui using FetchContent
include(FetchContent)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v4.0.0
  FIND_PACKAGE_ARGS REQUIRED # use installed package if aviable
)
FetchContent_MakeAvailable(ftxui)


set(NETSHIP_TARGET netShip)
add_executable(${NETSHIP_TARGET} 
    src/main.cpp
    src/game-logic/game.cpp
    src/net-comms/out-facing/common_cs.cpp
    src/net-comms/out-facing/client.cpp
    src/net-comms/out-facing/server.cpp
    src/ui/initial_menu.cpp
    src/ui/placement_menu.cpp
    src/ui/game_screen.cpp
    src/ui/ui_helpers.cpp
)

target_compile_features(${NETSHIP_TARGET} PUBLIC cxx_std_20)

target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${NETSHIP_TARGET}
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component # Not needed for this example.
)

target_link_libraries(${NETSHIP_TARGET}
    PRIVATE PkgConfig::asio
)

install(
    TARGETS ${NETSHIP_TARGET} 
    RUNTIME DESTINATION bin
)
