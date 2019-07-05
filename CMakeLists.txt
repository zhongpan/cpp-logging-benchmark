cmake_minimum_required (VERSION 3.12.0)
project(cpp_logging_benchmark CXX)
include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)
if(WIN32)
add_definitions(-DUNICODE -D_UNICODE -UMBCS -U_MBCS) #for log4cplus
endif()
add_executable(cpp_logging_benchmark main.cpp)
target_link_libraries(cpp_logging_benchmark CONAN_PKG::benchmark CONAN_PKG::spdlog CONAN_PKG::log4cplus CONAN_PKG::boost)