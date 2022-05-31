USE_AVX=0

if [ ! -d "./build" ]; then
    mkdir build
fi

cd build
cmake .. -DUSE_AVX=$USE_AVX
make
cd ..

if [ $# -ge 2 ]; then
    MAT_SIZE=$1
    N_PROCS=$2
else
    MAT_SIZE=1000
    N_PROCS=5
fi
echo "Matrix size = $MAT_SIZE x $MAT_SIZE "
echo "N_PROCS = $N_PROCS"

python3 ./tests/test_generate_data.py $MAT_SIZE
mpirun -n $N_PROCS ./build/test_MatMulMPI M.csv N.csv M@N_MPI.csv

python ./tests/test_numpy_matmul.py M.csv N.csv M@N_Python.csv

./build/test_MatMulCMD M.csv N.csv M@N_SIMD.csv