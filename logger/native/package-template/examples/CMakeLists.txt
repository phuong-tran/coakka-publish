cmake_minimum_required(VERSION 3.20)
project(CoAkkaLoggerNativeExamples LANGUAGES C CXX)

find_package(CoAkkaLoggerNative CONFIG REQUIRED)

add_executable(coakka_logger_c_basic c/basic.c)
target_link_libraries(coakka_logger_c_basic PRIVATE CoAkkaLoggerNative::core)

add_executable(coakka_logger_cpp_basic cpp/basic.cpp)
target_link_libraries(coakka_logger_cpp_basic PRIVATE CoAkkaLoggerNative::native_cpp_connector)
