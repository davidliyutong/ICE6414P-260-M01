project(Hellowrold CXX)
set(CMAKE_CXX_COMPILER "/usr/bin/mpicxx")
# include_directories("/usr/include/aarch64-linux-gnu/mpich")
add_executable(Helloworld Helloworld.cpp)
add_executable(GetPI GetPI.cpp)


