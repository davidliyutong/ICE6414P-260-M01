#include <iostream>
#include <mpi.h>
#include "MPIProcessorInfo.hpp"


int main(int argc, char ** argv) {
    MPI_Init(nullptr, nullptr);

    /** Get Current Processor Name **/
    MPIProcessorInfo Processor;

    printf("Hello world from processor %s, rank %d out of %d processors\n", Processor.acName(), Processor.iRank(), Processor.iSize());

    // 释放 MPI 的一些资源
    MPI_Finalize();
}