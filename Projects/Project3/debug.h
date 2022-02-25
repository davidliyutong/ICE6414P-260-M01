//
// Created by 厉宇桐 on 2022/2/8.
//
#ifndef _DEBUG_H
#define _DEBUG_H

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <mpi.h>

#define LEVEL_DISABLE 0
#define LEVEL_ERROR 1
#define LEVEL_WARNING 2
#define LEVEL_INFO 3
#define LEVEL_DEBUG 4

/** Set log level **/
#ifndef CONFIG_LOG_LEVEL
#define LOG_LEVEL LEVEL_DEBUG
#else
#define LOG_LEVEL CONFIG_LOG_LEVEL
#endif

#if LOG_LEVEL >= LEVEL_ERROR
#define LOGE(M, ...) fprintf(stderr, "\033[31;1m[ERROR]\033[0m (%s:%d: errno: %d) " M "\n", __FILE__, __LINE__, errno, ##__VA_ARGS__)
#define LOGE_S(M, ...) \
        { \
            int __iRank = -1; \
            MPI_Comm_rank((MPI_COMM_WORLD), &__iRank); \
            if (__iRank == 0) { \
                LOGE(M, ##__VA_ARGS__); \
            } \
        }
#else
#define LOGE(M, ...)
#define LOGE_S(M, ...)
#endif

#if LOG_LEVEL >= LEVEL_WARNING
#define LOGW(M, ...) fprintf(stderr, "\033[33;1m[WARNING]\033[0m (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define LOGW_S(M, ...) \
        { \
            int __iRank = -1; \
            MPI_Comm_rank((MPI_COMM_WORLD), &__iRank); \
            if (__iRank == 0) { \
                LOGW(M, ##__VA_ARGS__); \
            } \
        }
#else
#define LOGW(M, ...)
#define LOGW_S(M, ...)
#endif

#if LOG_LEVEL >= LEVEL_INFO
#define LOGI(M, ...) fprintf(stderr, "\033[32;1m[INFO]\033[0m (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define LOGI_S(M, ...) \
        { \
            int __iRank = -1; \
            MPI_Comm_rank((MPI_COMM_WORLD), &__iRank); \
            if (__iRank == 0) { \
                LOGI(M, ##__VA_ARGS__); \
            } \
        }
#else
#define LOGI(M, ...)
#define LOGI_S(M, ...)
#endif

#if LOG_LEVEL >= LEVEL_DEBUG
#define LOGD(M, ...) fprintf(stderr, "\033[2m[DEBUG] (%s:%d) " M "\n\033[0m", __FILE__, __LINE__, ##__VA_ARGS__)
#define LOGD_S(M, ...) \
        { \
            int __iRank = -1; \
            MPI_Comm_rank((MPI_COMM_WORLD), &__iRank); \
            if (__iRank == 0) { \
                LOGD(M, ##__VA_ARGS__); \
            } \
        }
#else
#define LOGD(M, ...)
#define LOGD_S(M, ...)
#endif


#define CHECK(exp, M, ...) \
        if(!(exp)) { LOGE(M, ##__VA_ARGS__); errno=0; goto error; }
#define CHECK_S(exp, M, ...) \
        if(!(exp)) { LOGE_S(M, ##__VA_ARGS__); errno=0; goto error; }
#endif
