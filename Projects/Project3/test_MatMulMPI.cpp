#include "MatMul.hpp"
#include "MPIProcessorInfo.hpp"
#include "MPITimer.hpp"
#include "debug.h"
#include <iostream>

int main() {
    using mpimath::Matrix2D;
    MPI_Init(NULL, NULL);
    MPIProcessorInfo Processor;
    Matrix2D<double> A(9, 9, true);
    A[0][0] = 1;
    A[0][1] = 2;
    A[1] = { 1,2,3,4,5,6,7,8,9 };
    if (Processor.iRank() == 0) {
        std::cout << A;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    /** Invoke MPI functions */
    if (Processor.iRank() == 0) {
        auto Res = mpimath::MPIMatMulMain(A, A, Processor);
        std::cout << Res;
    } else {
        mpimath::MPIMatMulSub(Processor);
    }


    MPI_Finalize();
    return 0;

}