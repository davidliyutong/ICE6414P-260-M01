cmake .
make
mpirun -n 4 ./Helloworld
mpirun -n 4 ./GetPI
