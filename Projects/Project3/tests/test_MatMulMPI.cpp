#include "MatMul.hpp"
#include "MPIProcessorInfo.hpp"
#include "MPITimer.hpp"
#include "Matrix.hpp"
#include "debug.h"
#include <iostream>

using namespace mpimath;
/**
 * @brief test_MatMulMPI
 *
 * @param argc expected to be 4
 * @param argv MatM.csv MatN.csv Result.csv
 * @return int
 */
int main(int argc, char** argv) {
    using mpimath::Matrix2D;
    MPI_Init(NULL, NULL);
    MPIProcessorInfo Processor;
    if (argc < 4) {
        LOGE_S("Insufficient number of argument, usage ./program MatM.csv MatN.csv Result.csv");
        return -1;
    }

    auto sMatMPath = std::string(argv[1]);
    auto sMatNPath = std::string(argv[2]);
    auto sResultPath = std::string(argv[3]);
    int iRet = 0;

    Matrix2D<double> M{};
    Matrix2D<double> N{};
    Matrix2D<double> Res{};

    ON_MAIN_PROC(Processor) {
        iRet = M.ReadCSV(sMatMPath);
        if (iRet != emMatrixError::MATRIX_OK) {
            LOGE_S("Read Error: %d", iRet);
        }
        iRet = N.ReadCSV(sMatNPath);
        if (iRet != emMatrixError::MATRIX_OK) {
            LOGE_S("Read Error: %d", iRet);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    /** Invoke MPI functions */
    auto Timer = MPITimer();
    if (Processor.iSize() < 2) {
        Res = M * N;
    } else {
        if (Processor.iRank() == 0) {
            Res = mpimath::MPIMatMulMain(M, N, Processor);
        } else {
            mpimath::MPIMatMulWorker(Processor);
        }
    }

    ON_MAIN_PROC(Processor) {
        LOGI("Time elapsed: %f", Timer.TimeDelta());
        // std::cout << Res;
        Res.DumpCSV(sResultPath);
    }

    MPI_Finalize();
    return 0;

}