project(Hellowrold CXX)
set(CMAKE_CXX_COMPILER "/usr/bin/mpicxx")
include_directories("/usr/include/aarch64-linux-gnu/mpich")
add_executable(GetPrime GetPrime.cpp)


