if [ ! -d "./build" ]; then
mkdir build
fi
cd build && cmake .. && make && cd ..

mpirun -n 4 ./build/test_MatMul 100000
