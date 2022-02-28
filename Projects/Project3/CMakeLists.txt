project(Hellowrold CXX)
set(CMAKE_CXX_COMPILER "/usr/bin/mpicxx")
include_directories("/usr/include/aarch64-linux-gnu/mpich")
# set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -DCONFIG_LOG_LEVEL=2)
add_definitions(-DCONFIG_LOG_LEVEL=LEVEL_INFO)
add_executable(test_MatMul test_MatMul.cpp)
add_executable(test_MatMulMPI test_MatMulMPI.cpp)
add_executable(test_MatMulCSV test_MatMulCSV.cpp)




