/**
 * @file MatMul.hpp
 * @author davidliyutong (davidliyutong@sjtu.edu.cn)
 * @brief
 * @version 0.1
 * @date 2022-02-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef MATMUL_HPP
#define MATMUL_HPP

#include "Matrix.hpp"
#include "MPIProcessorInfo.hpp"
#include <vector>
#include "block.hpp"
 // #include "debug.h"

namespace mpimath {
    /**
     * @brief dtype of matrix
     *
     */
    enum class emMatrixType {
        INT32 = 0,
        INT64 = 1,
        FLOAT64 = 2,
        INVALID = -1,
    };

    /**
     * @brief Message tag used in MPI
     *
     * @enum MAT_N      The matrix N
     * @enum MATMUL_CTX The MaMul context
     * @enum BLOCK_SZ   The block size
     * @enum BLOCK      The block content
     * @enum RESULT     The result
     */
    enum class emMsgType {
        MAT_N,
        MATMUL_CTX,
        BLOCK_SZ,
        BLOCK,
        RESULT,
    };
    /**
     * @brief The MatMul context for M @ N
     *
     * @struct lNRow row of N
     * @struct lNCol col of N
     * @struct lMRow row of M
     * @struct lMCol col of M
     *
     */
    typedef struct {
        long lNRow;
        long lNCol;
        long lMRow;
        long lMCol;
        bool bValid;
    }tMatMulCtx;

    /**
     * @brief Calculate M @ N, the main process (process 0)
     *
     * @param MatM
     * @param MatN
     * @param Processor
     * @return Matrix2D<double>
     */
    Matrix2D<double> MPIMatMulMain(const Matrix2D<double>& MatM,
                                   const Matrix2D<double>& MatN,
                                   MPIProcessorInfo Processor) {

        /** Initiate MatMulCtx */
        tMatMulCtx Ctx = {
            .lNRow = (long)(MatN.ulRow()),
            .lNCol = (long)(MatN.ulCol()),
            .lMRow = (long)(MatM.ulRow()),
            .lMCol = (long)(MatM.ulCol()),
            .bValid = (MatM.ulCol() == MatN.ulRow()) ? true : false
        };
        /** Broadcast process context*/
        MPI_Bcast(&Ctx, sizeof(Ctx), MPI_CHAR, 0, MPI_COMM_WORLD);
        if (not Ctx.bValid) {
            return { 0, 0 };
        }

        /** Broadcast Matrix N */
        MPI_Bcast(MatN.pData(), (int)MatN.Size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);

        auto aRequests = new MPI_Request[Processor.iSize()];/** Store Requests */
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
            MPI_Send(&MatM.pData()[lLineIndex * Ctx.lMCol],
                     lLineNum * Ctx.lMCol,
                     MPI_DOUBLE,
                     iProcID,
                     (int)emMsgType::BLOCK,
                     MPI_COMM_WORLD);

            /** Receive from workers */
            // LOGD("[%d] Receiving {RESULT} size=%ld", Processor.iRank(), lLineNum * Ctx.lNCol);
            MPI_Irecv(&MatRes.pData()[lLineIndex * Ctx.lNCol],
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
        // LOGD("[%d] Freeing {aRequests} %p", Processor.iRank(), aRequests);
        if (aRequests != nullptr) {
            delete[] aRequests;
            aRequests = nullptr;
        }

        return MatRes;
    }

    /**
     * @brief Calculate M @ N, the worker processes (process != 0)
     *
     * @param Processor
     * @return int
     */
    int MPIMatMulWorker(MPIProcessorInfo Processor) {

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
        MPI_Bcast(MatN.pData(),
                  (int)MatN.Size(),
                  MPI_DOUBLE,
                  0,
                  MPI_COMM_WORLD);

        /** Receive slice */
        MPI_Recv(MatMSlice.pData(),
                 MatMSlice.Size(),
                 MPI_DOUBLE, 0,
                 (int)emMsgType::BLOCK,
                 MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);

        /** Compute */
        auto MatRes = MatMSlice * MatN;

        /** Send result to proc 0 */
        // LOGD("[%d] Sending {RESULT} size=%ld", Processor.iRank(), MatRes.Size());
        MPI_Send(MatRes.pData(),
                 MatRes.Size(),
                 MPI_DOUBLE,
                 0,
                 (int)emMsgType::RESULT,
                 MPI_COMM_WORLD);

        return 0;
    }
}
#endif