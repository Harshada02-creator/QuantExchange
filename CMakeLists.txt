cmake_minimum_required(VERSION 3.16)

project(QuantExchange VERSION 1.0.0 LANGUAGES CXX)

add_library(quantexchange_core
    src/Order.cpp
    src/Trade.cpp
    src/OrderBook.cpp
    src/MatchingEngine.cpp
)

target_include_directories(quantexchange_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_features(quantexchange_core PUBLIC cxx_std_14)

if(MSVC)
    target_compile_options(quantexchange_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(quantexchange_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(QuantExchange main.cpp)
target_link_libraries(QuantExchange PRIVATE quantexchange_core)

include(CTest)
if(BUILD_TESTING)
    add_executable(MatchingEngineTests tests/MatchingEngineTests.cpp)
    target_link_libraries(MatchingEngineTests PRIVATE quantexchange_core)
    add_test(NAME MatchingEngineTests COMMAND MatchingEngineTests)
endif()
