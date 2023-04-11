cmake_minimum_required(VERSION 3.0.0)
project(netShip VERSION 0.1.0)

# include(CTest)
# enable_testing()

find_package(PkgConfig REQUIRED)
pkg_check_modules(asio REQUIRED IMPORTED_TARGET asio)

set(NETSHIP_TARGET netShip)
add_executable(${NETSHIP_TARGET} 
    src/main.cpp
    src/net-comms/out-facing/common_cs.cpp
    src/net-comms/out-facing/client.cpp
    src/net-comms/out-facing/server.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${NETSHIP_TARGET}
    PkgConfig::asio
)

install(
    TARGETS ${NETSHIP_TARGET} 
    RUNTIME DESTINATION bin
)
