#include <mpi.h>
#include <string>

class MPIProcessorInfo {
public:
    MPIProcessorInfo() {
        MPI_Comm_size(MPI_COMM_WORLD, &_iWorldSize); // Get WorldSize = number of parallel processes
        MPI_Comm_rank(MPI_COMM_WORLD, &_iWorldRank); // Get World Rank = idx of current process

        char acProcessorName[MPI_MAX_PROCESSOR_NAME]; // Temperory variable for  Processor name
        int iNameLen;
        MPI_Get_processor_name(acProcessorName, &iNameLen);
        _ProcessorName = std::string(acProcessorName);

    }

    std::string sName() {
        return _ProcessorName;
    }

    const char * acName() {
        return _ProcessorName.c_str();
    }
    int iSize() const {
        return _iWorldSize;
    }
    int iRank() const {
        return _iWorldRank;
    }

    
protected:
    int _iWorldSize = 0;
    int _iWorldRank = 0;
    std::string _ProcessorName;
};