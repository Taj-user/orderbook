cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0167 NEW)
project(orderbook VERSION 1.0 LANGUAGES CXX)

# C++17 required
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ─── Dependencies ─────────────────────────────────────────────────────────────

# Catch2 via FetchContent
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.2
)
FetchContent_MakeAvailable(Catch2)

# System-wide packages via pacman
find_package(Boost REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)

# ─── Core Library ─────────────────────────────────────────────────────────────

add_library(orderbook_core
    src/common/utils.cpp
    src/engine/PriceLevel.cpp
    src/engine/OrderBook.cpp
    src/engine/MatchingEngine.cpp
    src/feed/MarketDataPublisher.cpp
)

target_include_directories(orderbook_core PUBLIC include)

target_link_libraries(orderbook_core
    PUBLIC
        Boost::boost
        fmt::fmt
        spdlog::spdlog
        nlohmann_json::nlohmann_json
)

# ─── Main Executable ──────────────────────────────────────────────────────────

add_executable(orderbook src/main.cpp)
target_link_libraries(orderbook PRIVATE orderbook_core)
target_link_options(orderbook PRIVATE -mconsole)

# ─── Tests ────────────────────────────────────────────────────────────────────

add_executable(orderbook_tests
    tests/test_matching_engine.cpp
    tests/test_orderbook.cpp
    tests/test_price_level.cpp
    tests/test_feed.cpp
)

target_link_libraries(orderbook_tests
    PRIVATE
        orderbook_core
        Catch2::Catch2WithMain
)

include(CTest)
include(Catch)
catch_discover_tests(orderbook_tests)
