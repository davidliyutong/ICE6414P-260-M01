#ifndef _MPITIMER_H
#define _MPITIMER_H

#include <mpi.h>

class MPITimer {
public:
    double start = 0;
    MPITimer() {
        start = MPI_Wtime();
    }
    double TimeDelta() {
        return MPI_Wtime() - start;
    }
    void Reset() {
        start = MPI_Wtime();
    }
};

#endif