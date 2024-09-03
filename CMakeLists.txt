cmake_minimum_required (VERSION 2.8.11)
project (secure-settlement-reporting)
set(NAME "secure-settlement-reporting")

find_path(CMAKE_FOLDER NAMES cmake/emp-tool-config.cmake)
include(${CMAKE_FOLDER}/cmake/common.cmake)
include(${CMAKE_FOLDER}/cmake/enable_rdseed.cmake)
include(${CMAKE_FOLDER}/cmake/enable_float.cmake)

find_package(emp-sh2pc REQUIRED)
include_directories(${EMP-SH2PC_INCLUDE_DIRS})

# Test cases
macro (add_test _name)
        add_executable(${_name} "test/${_name}.cpp")
        target_link_libraries(${_name} ${EMP-SH2PC_LIBRARIES})
endmacro()

add_test (ssrep)
add_test (parser)
