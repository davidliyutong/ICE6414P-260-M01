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

    LOGD("%p, A[0][0] = %d", A.pData, A[0][0]);
    A[0][0] = 1;
    A[0][1] = 2;
    Matrix2D<double> B;
    B = A;
    LOGD("%p, %p", A.pData, B.pData);

    LOGD("%p, A[0][0] = %d", A.pData, A[0][0]);


    LOGD("A[1][1] = %d", A[1][1]);
    A[1] = { 1,2,3,4,5,6,7,8,9 };
    LOGD("A[0][0] = %d", A[0][0]);
    LOGD("A[1][1] = %d", A[1][1]);
    std::cout << A;
    std::cout << A[1];
    std::cout << B;
    std::cout << A * A;
    LOGD("%p, %p", A.pData, B.pData);
    return 0;

}