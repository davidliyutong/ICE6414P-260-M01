if [ ! -d "./build" ]; then
    mkdir build
fi
cd build && cmake .. && make && cd ..

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
mpirun -n $N_PROCS ./build/test_MatMulMPI M.csv N.csv result.csv
