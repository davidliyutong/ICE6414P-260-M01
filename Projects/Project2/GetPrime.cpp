#include "mpi.h"
#include <stdio.h>
#include <sstream>
#include <numeric>
#include <iostream>
#include <math.h>
#include "MPITimer.hpp"
#include "MPIProcessorInfo.hpp"
#include "block.hpp"
#include "debug.h"

static int iError = 0;
#define EXIT_ON_ERROR(E) \
        iError = E; \
        goto error

#define SQUARE(x) ((x) * (x))

int main(int argc, char** argv) {
    /** Input Arguments variables**/
    int   iN, iNCopy;      /* Sieving from 2, ..., 'n' */

    /** Block information variables **/
    int   iBlockHighValue; /* Highest value to test (lcl) */
    int   iBlockLowValue;  /* Lowest value to test (lcl) */
    int   iBlockSize;      /* Elements in 'marked' (lcal)*/
    char* pacMarked;       /* Array to apply mask (lcl)(alloc) */
    int   iProc0Size;      /* Size of proc 0's array */

    /** Temporary variables **/
    int   iFirst;          /* Index of first prime multiple (lcl)*/
    int   iIndex;          /* Index of current prime (lcl)(tmp)*/
    int   iPrime;          /* Current prime (not index) (lcl)*/

    /** Result variables **/
    int   iLocalCount;     /* Prime counter (lcl) */
    int   iGlobalCount;    /* Prime counter (glb)*/


    /** Init MPI context, start timer **/
    MPI_Init(&argc, &argv);
    MPIProcessorInfo Processor;
    MPI_Barrier(MPI_COMM_WORLD);
    MPITimer Timer;


    /** Parse iN from command line args
     * @arg argc
     * @return iN, iNCopy
     * **/
    if (argc != 2) {
        LOGE_S("Incorrect number of arguments. Correct usage: \n$ <EXECUTABLE> <LIMIT:int>");
        EXIT_ON_ERROR(MPI_ERR_ARG);
    }
    std::istringstream(argv[1]) >> iN;
    iN = std::abs(iN);
    iNCopy = iN;  // Save a copy of original iN
    iN = (iN % 2 == 0) ? (iN / 2 - 1) : ((iN - 1) / 2);
    LOGI_S("Number of odd integers to test: %d, max()=%d", iN + 1, 3 + 2 * (iN - 1));
    if (iN == 0) {
        LOGI_S("There are 1 prime less than or equal to %d", iNCopy);
        goto error;
    }

    /** Use BLOCK_* macros to calculate current process's share of
     * the array
     * @arg Processor
     * @return iBlockLowValue, iBlockHighValue, iBlockSize**/
    iBlockLowValue = BLOCK_LOW(Processor.iRank(), Processor.iSize(), iN);
    iBlockHighValue = BLOCK_HIGH(Processor.iRank(), Processor.iSize(), iN);
    iBlockSize = BLOCK_SIZE(Processor.iRank(), Processor.iSize(), iN);
    // iBlockSize = (iBlockHighValue - iBlockLowValue) /2 + 1;
    LOGD("[%d]L=%d, H=%d, Sz=%d", Processor.iRank(), iBlockLowValue, iBlockHighValue, iBlockSize);


    /** If all the primes used for sieving are
     * not `all` held by process 0, quit because their are too many
     * processes
     * @arg Processor, iNCopy**/
    iProc0Size = (iN - 1) / Processor.iSize();
    if ((2 + iProc0Size) < (int)std::sqrt(iNCopy)) {//
        LOGE_S("Too many processes(%d) for problem %d\n", Processor.iSize(), iNCopy);
        EXIT_ON_ERROR(MPI_ERR_ARG);
    }

    /** Allocate sieving array for current process
     * @arg Processor, iBlockSize, iBlockLowValue, iBlockHighValue
     * FIXME: Why use char not bool(bit array) **/
    pacMarked = (char*)calloc(iBlockSize, 1);
    if (pacMarked == NULL) {
        LOGE("[%d]Failed to allocate %d bytes of memory\n", iBlockSize, Processor.iRank());
        EXIT_ON_ERROR(MPI_ERR_NO_MEM);
    }

    /** ----------- BEGIN SIEVING ----------- **/
    if (Processor.iRank() == 0) iIndex = 0; // Index
    iPrime = 3; // Skip 2, start from 3
    do {
        /** Find index of `first` multiple of odd number **/
        if (SQUARE(iPrime) > iBlockLowValue)
            iFirst = (SQUARE(iPrime) - iBlockLowValue) / 2;
        else {
            if (!(iBlockLowValue % iPrime))
                iFirst = 0;
            else
                iFirst = ((iPrime - iBlockLowValue % iPrime) % 2 == 0) ? ((iPrime - iBlockLowValue % iPrime) / 2) : ((iPrime - iBlockLowValue % iPrime + iPrime) / 2);
        }

        /** Mark multiples of prime **/
        for (auto i = iFirst; i < iBlockSize; i += iPrime) pacMarked[i] = 1;
        if (Processor.iRank() == 0) {
            while (pacMarked[++iIndex]);
            iPrime = 2 * iIndex + 3; // Prime -> Next unmarked prime (not index)
        }
        /** Broadcast the found prime to other processes **/
        if (Processor.iSize() > 1) MPI_Bcast(&iPrime, 1, MPI_INT, 0, MPI_COMM_WORLD);
    } while (SQUARE(iPrime) <= 3 + 2 * (iN - 1));
    /** ------------ END SIEVING ------------ **/

    /** Summarize **/
    iLocalCount = iBlockSize - std::accumulate(pacMarked, pacMarked + iBlockSize, 0);
    LOGD("[%d]iLocalCount=%d, iBlockSize=%d", Processor.iRank(), iLocalCount, iBlockSize);
    if (Processor.iSize() > 1) {
        /** Sum with reduce only when multiple processes **/
        MPI_Reduce(&iLocalCount, &iGlobalCount, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    } else {
        iGlobalCount = iLocalCount;
    }

    /** Print the results **/
    LOGI_S("There are %d primes <= %d\n", iGlobalCount + 1, iNCopy);
    /*** `+1` since we ignored the prime number 2    ~~^ **/
    LOGI_S("Duration with (%d) procs=%.6fs\n", Processor.iSize(), Timer.TimeDelta());

error:
    MPI_Finalize();
    exit(iError);
}