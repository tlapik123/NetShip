cmake_minimum_required(VERSION 3.0.0)
project(netShip VERSION 0.1.0)

# include(CTest)
# enable_testing()

find_package(PkgConfig REQUIRED)

# find asio
pkg_check_modules(asio REQUIRED IMPORTED_TARGET asio)

# find ftxui
find_package(ftxui REQUIRED)


set(NETSHIP_TARGET netShip)
add_executable(${NETSHIP_TARGET} 
    src/main.cpp
    src/net-comms/out-facing/common_cs.cpp
    src/net-comms/out-facing/client.cpp
    src/net-comms/out-facing/server.cpp
)

target_compile_options(${NETSHIP_TARGET} PUBLIC "--std=c++20")

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
