#include "MatMul.hpp"
#include "MPIProcessorInfo.hpp"
#include "MPITimer.hpp"
#include "debug.h"
#include <iostream>

int main() {
    using mpimath::Matrix2D;
    Matrix2D<double> A{};
    A.ReadCSV("../test.csv");
    std::cout << A;
    A *= A;
    std::cout << A;
    A.DumpCSV("../test2.csv");

    return 0;

}