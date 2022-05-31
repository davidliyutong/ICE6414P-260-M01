/**
 * @file gemm.h
 * @author davidliyutong@sjtu.edu.cn
 * @brief
 * @version 0.1
 * @date 2022-05-27
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

namespace mpimath {
    int gemm_f32(float* dout,
                 float* m,
                 float* n,
                 size_t m_hgt,
                 size_t m_wid,
                 size_t n_hgt,
                 size_t n_wid);
    int gemm_f64(double* dout,
                 double* m,
                 double* n,
                 size_t m_hgt,
                 size_t m_wid,
                 size_t n_hgt,
                 size_t n_wid);
}