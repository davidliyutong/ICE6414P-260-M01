#ifndef MATMUL_HPP
#define MATMUL_HPP

#include "Matrix.hpp"
#include "MPIProcessorInfo.hpp"
#include <vector>
#include "block.hpp"

namespace mpimath {
    enum class emMatrixType {
        INT32 = 0,
        INT64 = 1,
        FLOAT64 = 2,
        INVALID = -1,
    };

    enum class emMsgType {
        MAT_N,
        MATMUL_CTX,
        BLOCK_SZ,
        BLOCK,
        RESULT,
    };

    typedef struct {
        long lNRow;
        long lNCol;
        long lMRow;
        long lMCol;
        bool bValid;
    }tMatMulCtx;

    Matrix2D<double> MPIMatMulMain(const Matrix2D<double>& MatM,
                                   const Matrix2D<double>& MatN,
                                   MPIProcessorInfo Processor) {

        /** Initiate MatMulCtx */
        tMatMulCtx Ctx = {
            .lNRow = (long)(MatN.ulRow),
            .lNCol = (long)(MatN.ulCol),
            .lMRow = (long)(MatM.ulRow),
            .lMCol = (long)(MatM.ulCol),
            .bValid = (MatM.ulCol == MatN.ulRow) ? true : false
        };
        /** Broadcast process context*/
        MPI_Bcast(&Ctx, sizeof(Ctx), MPI_CHAR, 0, MPI_COMM_WORLD);
        if (not Ctx.bValid) {
            return { 0, 0 };
        }

        /** Broadcast Matrix N */
        MPI_Bcast(MatN.pData, (int)MatN.Size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);

        auto aRequests = new MPI_Request[Processor.iRank()];/** Store Requests */
        Matrix2D<double> MatRes(Ctx.lMRow, Ctx.lNCol);/** Store Result */

        FOR_ALL_SUB_PROC(Processor) {
            /** Compute line index and number of lines to send for each proc */
            long lLineIndex = BLOCK_LOW(iProcID - 1,
                                        Processor.iSize() - 1,
                                        Ctx.lMRow);
            long lLineNum = BLOCK_SIZE(iProcID - 1,
                                       Processor.iSize() - 1,
                                       Ctx.lMRow);

            /** Send slice */
            MPI_Send(&MatM.pData[lLineIndex * Ctx.lMCol],
                     lLineNum * Ctx.lMCol,
                     MPI_DOUBLE,
                     iProcID,
                     (int)emMsgType::BLOCK,
                     MPI_COMM_WORLD);

            /** Receive from workers */
            MPI_Irecv(&MatRes.pData[lLineIndex * Ctx.lNCol],
                      lLineNum * Ctx.lNCol,
                      MPI_DOUBLE,
                      iProcID,
                      (int)emMsgType::RESULT,
                      MPI_COMM_WORLD,
                      &aRequests[iProcID]);

        }

        /** Make sure all blocks are received */
        FOR_ALL_SUB_PROC(Processor) {
            MPI_Wait(&aRequests[iProcID], MPI_STATUS_IGNORE);
        }

        /** Free Requests **/
        delete[] aRequests;

        return MatRes;
    }

    int MPIMatMulSub(MPIProcessorInfo Processor) {

        tMatMulCtx Ctx = { 0 };
        /** Broadcast process context */
        MPI_Bcast(&Ctx, sizeof(Ctx), MPI_CHAR, 0, MPI_COMM_WORLD);

        /** Compute Number of lines in block */
        long lLineNum = BLOCK_SIZE(Processor.iRank() - 1,
                                   Processor.iSize() - 1,
                                   Ctx.lMRow);

        /** Create Buffer for MatN and MatM's slice */
        Matrix2D<double> MatMSlice(lLineNum, Ctx.lMCol);
        Matrix2D<double> MatN(Ctx.lNRow, Ctx.lNCol); /** MatN */

        /** Broadcast Matrix N */
        MPI_Bcast(MatN.pData,
                  (int)MatN.Size(),
                  MPI_DOUBLE,
                  0,
                  MPI_COMM_WORLD);

        /** Receive slice */
        MPI_Recv(MatMSlice.pData,
                 MatMSlice.Size(),
                 MPI_DOUBLE, 0,
                 (int)emMsgType::BLOCK,
                 MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);

        /** Compute */
        auto MatRes = MatMSlice * MatN;

        /** Send result to proc 0 */
        MPI_Send(MatRes.pData,
                 MatRes.Size(),
                 MPI_DOUBLE,
                 0,
                 (int)emMsgType::RESULT,
                 MPI_COMM_WORLD);

        return 0;
    }
}
#endif