cmake_minimum_required(VERSION 3.14)
project(FlightScheduleSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

include_directories(src)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(server ${SOURCES})

target_link_libraries(server ${PQXX_LIBRARIES} pthread)