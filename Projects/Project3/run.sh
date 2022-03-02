if [ ! -d "./build" ]; then
mkdir build
fi
cd build && cmake .. && make && cd ..

python3 ./tests/test_generate_data.py 1000
mpirun -n 5 ./build/test_MatMulMPI M.csv N.csv result.csv
