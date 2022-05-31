project(Hellowrold CXX)
cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_COMPILER "mpicxx")
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BUILD_TYPE "Release")

IF (USE_AVX)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-mavx2")
add_definitions(-DOPTIMIZE_GEMM=2)
ENDIF()

include_directories("include")
include_directories("/usr/include/aarch64-linux-gnu/mpich")
# set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -DCONFIG_LOG_LEVEL=2)

add_library(gemm SHARED src/gemm.cpp)

add_definitions(-DCONFIG_LOG_LEVEL=LEVEL_INFO)

add_executable(test_MatMul tests/test_MatMul.cpp)
target_link_libraries(test_MatMul gemm)

add_executable(test_MatMulCMD tests/test_MatMulCMD.cpp)
target_link_libraries(test_MatMulCMD gemm)

add_executable(test_MatMulMPI tests/test_MatMulMPI.cpp)
target_link_libraries(test_MatMulMPI gemm)

add_executable(test_MatMulCSV tests/test_MatMulCSV.cpp)
target_link_libraries(test_MatMulCSV gemm)




