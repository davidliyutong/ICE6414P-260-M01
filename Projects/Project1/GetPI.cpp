#include "MPIProcessorInfo.hpp"
#include "debug.h"

#ifndef CONFIG_PRECISION
#define CONFIG_PRECISION 1E9
#endif
#define CONFIG_USE_MAPREDUCE 0

/**
 * @brief Get Pi using trapezoid algorithm
 * 
 * We admit that $$ \pi =\int_0^1\frac{4}{(1+x^2)} $$
 * 
 * The function use MPI_Reduce to collect partial sum from processes
 * @param Processor MPIProcessorInfo
 * @return double Result, equals to PI
 */
double GetPIMapReduce(const MPIProcessorInfo& Processor) {
    double dPI, dPartialSum, dSum;
    const long long llN = CONFIG_PRECISION; // Avoid float computation
    const double delta = 1 / ((double)llN * (double)llN); //delta = (1 / ((double)N ) / ((double)N 

    /** Sync between processes **/
    MPI_Barrier(MPI_COMM_WORLD);
    auto begin = MPI_Wtime();

    /** Calculate sum, each calculate a 1 / Processor.iSize() part **/
    for (int i = Processor.iRank(); i < llN; i += Processor.iSize()) {
        dPartialSum += 4.0 / (1.0 + delta * i * i);
    }
    /** Sum the result with MPI_Reduce **/
    MPI_Reduce(&dPartialSum, &dSum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    /** Sync  between processes **/
    MPI_Barrier(MPI_COMM_WORLD);
    auto end = MPI_Wtime();

    /** Only output on Rank 0 **/
    if (Processor.iRank() == 0) {
        dPI = dSum / llN; // Get pi from dSum variable bu multiply 1 / N
        LOGI("NumProcesses=%2d;  Time(Second)=%fs;  PI=%0.15lf\n", Processor.iSize(), end - begin, dPI);
    }

    return dPI;
}

/**
 * @brief Get Pi using trapezoid algorithm
 * 
 * We admit that $$ \pi =\int_0^1\frac{4}{(1+x^2)} $$
 * 
 * The function use MPI_Send and MPI_Recv to collect partial sum from processes
 * @param Processor MPIProcessorInfo
 * @return double Result, equals to PI
 */

double GetPISendRecv(const MPIProcessorInfo& Processor) {
    double dPI, dPartialSum;
    const long long llN = CONFIG_PRECISION; // Avoid float computation
    const double delta = 1 / ((double)llN * (double)llN); //delta = (1 / ((double)N ) / ((double)N 

    /** Sync between processes **/
    MPI_Barrier(MPI_COMM_WORLD);
    auto begin = MPI_Wtime();
    MPI_Request* apRequests=new MPI_Request[Processor.iSize()-1];

    /** Calculate sum, each calculate a 1 / Processor.iSize() part **/
    for (int i = Processor.iRank(); i < llN; i += Processor.iSize()) {
        dPartialSum += 4.0 / (1.0 + delta * i * i);
    }

    /** Sync between processes **/
    MPI_Barrier(MPI_COMM_WORLD);
    auto end = MPI_Wtime();

    if (Processor.iRank() != 0) {
        MPI_Isend(&dPartialSum, 1, MPI_DOUBLE, 0, Processor.iRank(), MPI_COMM_WORLD, &apRequests[Processor.iRank()-1]);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    /** Only output on Rank 0 **/
    if (Processor.iRank() == 0) {
        double dSum = dPartialSum;
        double dPartialSumFromOtherProc;
        for (int iSrcRank = 1 ; iSrcRank < Processor.iSize(); ++iSrcRank) {
            MPI_Recv(&dPartialSumFromOtherProc, 1, MPI_DOUBLE, iSrcRank, iSrcRank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            dSum += dPartialSumFromOtherProc;
        }
        dPI = dSum / llN; // Get pi from dSum variable bu multiply 1 / N

        LOGI("NumProcesses=%2d;  Time(Second)=%fs;  PI=%0.15lf\n", Processor.iSize(), end - begin, dPI);

    }
    /** Sync between processes **/
    MPI_Barrier(MPI_COMM_WORLD);
    delete[] apRequests;
    return dPI;
}

int main(int argc, char** argv) {
    /** Init MPI framework **/
    MPI_Init(nullptr, nullptr);

    /** Get Current Processor Name **/
    MPIProcessorInfo Processor;

#if CONFIG_USE_MAPREDUCE
    GetPIMapReduce(Processor);
#else
    printf("%s","");
    GetPISendRecv(Processor);
#endif

error:
    /** Release resources **/
    MPI_Finalize();
}