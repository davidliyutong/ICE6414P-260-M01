#include "MatMul.hpp"
#include "MPIProcessorInfo.hpp"
#include "MPITimer.hpp"
#include "debug.h"
#include <iostream>

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

    Matrix2D<double> M{};
    M.ReadCSV(sMatMPath);
    Matrix2D<double> N{};
    N.ReadCSV(sMatMPath);
    Matrix2D<double> Res{};

    // ON_MAIN_PROC(Processor) {
    //     std::cout << M;
    //     std::cout << N;
    // }

    MPI_Barrier(MPI_COMM_WORLD);

    /** Invoke MPI functions */
    auto Timer = MPITimer();
    if (Processor.iSize() < 2) {
        Res = M * N;
    } else {
        if (Processor.iRank() == 0) {
            Res = mpimath::MPIMatMulMain(M, N, Processor);
        } else {
            mpimath::MPIMatMulSub(Processor);
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