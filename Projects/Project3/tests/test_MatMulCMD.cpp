#include "MatMul.hpp"
#include "Matrix.hpp"
#include "debug.h"
#include <iostream>
#include <chrono>   


int main(int argc, char** argv) {
    using mpimath::Matrix2D;
    using mpimath::emMatrixError;
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

    iRet = M.ReadCSV(sMatMPath);
    if (iRet != emMatrixError::MATRIX_OK) {
        LOGE_S("Read Error: %d", iRet);
    }
    iRet = N.ReadCSV(sMatNPath);
    if (iRet != emMatrixError::MATRIX_OK) {
        LOGE_S("Read Error: %d", iRet);
    }

    auto start = std::chrono::high_resolution_clock::now();
    Res = M * N;
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = end - start;
    std::cout << "C++ Single Thread Time elapsed: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() * 1e-6  << std::endl;

    Res.DumpCSV(sResultPath);

    MPI_Finalize();
    return 0;

}