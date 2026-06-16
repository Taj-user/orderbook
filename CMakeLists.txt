cmake_minimum_required(VERSION 3.20)
project(orderbook VERSION 1.0 LANGUAGES CXX)

# C++17 required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ─── Dependencies ─────────────────────────────────────────────────────────────

include(FetchContent)

# nlohmann/json
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

# Catch2 (testing)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.2
)
FetchContent_MakeAvailable(Catch2)

# Redis
find_package(PkgConfig REQUIRED)
pkg_check_modules(hiredis REQUIRED hiredis)
pkg_check_modules(redispp REQUIRED redis++)

# ─── Core Library ─────────────────────────────────────────────────────────────

add_library(orderbook_core
    src/core/PriceLevel.cpp
    src/core/OrderBook.cpp
    src/core/MatchingEngine.cpp
    src/feed/MarketDataPublisher.cpp
)

target_include_directories(orderbook_core PUBLIC
    include
    ${hiredis_INCLUDE_DIRS}
    ${redispp_INCLUDE_DIRS}
)

target_link_libraries(orderbook_core
    PUBLIC
        nlohmann_json::nlohmann_json
        ${hiredis_LIBRARIES}
        ${redispp_LIBRARIES}
)

# ─── Main Executable ──────────────────────────────────────────────────────────

add_executable(orderbook src/main.cpp)

target_link_libraries(orderbook PRIVATE orderbook_core)

# ─── Tests ────────────────────────────────────────────────────────────────────

add_executable(orderbook_tests
    tests/test_matching_engine.cpp
    tests/test_orderbook.cpp
    tests/test_price_level.cpp
)

target_link_libraries(orderbook_tests
    PRIVATE
        orderbook_core
        Catch2::Catch2WithMain
)

include(CTest)
include(Catch)
catch_discover_tests(orderbook_tests)
