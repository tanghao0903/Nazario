cmake_minimum_required(VERSION 3.16)
project(NazarioCpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(nazario_cpp
    src_cpp/main.cpp
    src_cpp/common/env.cpp
    src_cpp/connectors/grok_client.cpp
    src_cpp/connectors/market_data_client.cpp
    src_cpp/strategy/signal_engine.cpp
    src_cpp/simulator/paper_broker.cpp
    src_cpp/reporting/trade_logger.cpp
)

target_include_directories(nazario_cpp PRIVATE src_cpp)
