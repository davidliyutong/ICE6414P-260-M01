#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <array>
#include <cstring>
#include <initializer_list>
#include <iostream>
#include <algorithm>
#include "debug.h"

namespace mpimath {

    typedef enum {
        MATRIX_ERR_SHAPE = 1,
        MATRIX_ERR_NULL = 2,

    } emMatrixError;


    template<typename T>
    class Matrix2DElem;

    template<typename T>
    class Matrix2DRow;

    template<typename T>
    class Matrix2D {
    public:
        T* pData = nullptr;
        size_t ulRow, ulCol, ulSize = 0;
        Matrix2D() = default;
        Matrix2D(const Matrix2D& Src) {
            this->ulRow = Src.ulRow;
            this->ulCol = Src.ulCol;
            this->ulSize = Src.ulSize;
            pData = new T[ulSize];
            if (pData != nullptr) {
                memcpy(pData, Src.pData, Src.ulSize);
            }
        }
        Matrix2D(size_t ulRow, size_t ulCol, bool bFillZero = false) {
            this->ulRow = ulRow;
            this->ulCol = ulCol;
            this->ulSize = sizeof(T) * ulRow * ulCol;
            if (this->ulSize > 0) {
                pData = new T[ulSize];
                if (pData != nullptr) {
                    if (bFillZero) {
                        bzero(pData, ulSize);
                    }
                }
            } else {
                pData = nullptr;
            }

        }

        ~Matrix2D() {
            if (pData != nullptr) {
                delete[] pData;
                pData = nullptr;
            }
        }

        inline std::array<size_t, 2> Shape() const {
            return std::array<size_t, 2>(ulRow, ulCol);
        }
        inline size_t Size() const {
            return ulRow * ulCol;
        }
        Matrix2D<T>& operator=(const Matrix2D<T>& Src) {
            if (this != &Src) {
                if (pData != nullptr) {
                    delete[] pData;
                    pData = nullptr;
                }
                ulCol = Src.ulCol;
                ulRow = Src.ulRow;
                ulSize = Src.ulSize;

                pData = new T[Src.ulSize];
                memcpy(pData, Src.pData, Src.ulSize);
            }

            return *this;
        }
        Matrix2DRow<T> operator[](size_t ulRow) {
            return Matrix2DRow<T>(*this, ulRow);
        }

        friend std::ostream& operator<<(std::ostream& OutStream, const Matrix2D<T>& Src) {
            int row = 0, col = 0;
            OutStream << "array([";
            for (row = 0; row < Src.ulRow; ++row) {
                OutStream << "[";
                for (col = 0; col < Src.ulCol; ++col) {
                    OutStream << Src.pData[row * Src.ulCol + col] << ", ";
                }
                if (row < (Src.ulRow - 1)) {
                    OutStream << "\b\b],\n       ";
                } else {
                    OutStream << "\b\b]])\n\n";
                }
            }
            return OutStream;
        }

        void fill(T Value) {
            for (auto idx = 0; idx < Size(); ++idx) {
                pData[idx] = Value;
            }
        }

        void Transpose() {
            if (this->ulSize > 0 and pData != nullptr) {
                auto pNewData = new T[this->ulSize];
                for (auto i = 0; i < ulRow; ++i) {
                    for (auto j = 0; j < ulCol; ++j) {
                        pNewData[j * ulRow + i] = pData[i * ulCol + j];
                    }
                }
                std::swap(ulCol, ulRow);
                delete[] pData;
                pData = nullptr;
                pData = pNewData;
            } else {
                return;
            }
        }

        Matrix2D<T> operator*(const Matrix2D<T>& N) {
            if (this->ulCol != N.ulRow) {
                throw MATRIX_ERR_SHAPE;
            }
            Matrix2D<T> Res(this->ulRow, N.ulCol);
            for (auto i = 0; i < Res.ulRow; ++i) {
                for (auto j = 0; j < Res.ulCol; ++j) {
                    T Sum = 0;
                    for (auto k = 0; k < this->ulCol; ++k) {
                        Sum += this->pData[i * this->ulCol + k] * N.pData[k * N.ulCol + j];
                    }
                    Res.pData[i * Res.ulCol + j] = Sum;
                }
            }
            return Res;
        }

        bool IsValid() {
            return (pData != nullptr);
        }
    };

    template<typename T>
    class Matrix2DRow {
    public:
        T* pData = nullptr;
        size_t ulCol = 0;
        size_t ulSize = 0;
        Matrix2DRow(const Matrix2D<T>& Mat, const size_t ulRow) {
            if (Mat.pData == NULL) {
                throw MATRIX_ERR_NULL;
            }
            if (ulRow < Mat.ulRow) {
                pData = Mat.pData + ulRow * Mat.ulCol;
                ulCol = Mat.ulCol;
                ulSize = Mat.ulCol * sizeof(T);
            } else {
                throw MATRIX_ERR_SHAPE;
            }
        }
        ~Matrix2DRow() = default;

        Matrix2DRow<T>& operator=(const Matrix2DRow<T>& Src) {
            if (this->ulCol == Src.ulCol) {
                memcpy(this->pData, Src.pData, this->ulCol);
                return *this;
            } else {
                throw MATRIX_ERR_SHAPE;
            }
        }

        Matrix2DRow<T>& operator=(std::initializer_list<T> Src) {
            if (this->ulCol == Src.size()) {
                for (auto i = 0; i < this->ulCol; ++i) {
                    this->pData[i] = (T) * (Src.begin() + i);
                }
                return *this;
            } else {
                throw MATRIX_ERR_SHAPE;
            }
        }

        inline std::array<size_t, 1> Shape() const {
            return std::array<size_t, 1>(ulCol);
        }
        inline size_t Size() const {
            return ulSize;
        }

        T& operator[](size_t ulCol) {
            return pData[ulCol];
        }

        friend std::ostream& operator<<(std::ostream& OutStream, const Matrix2DRow<T>& Src) {
            int row = 0, col = 0;
            OutStream << "array([";
            for (col = 0; col < Src.ulCol; ++col) {
                OutStream << Src.pData[row * Src.ulCol + col] << ", ";
            }
            OutStream << "\b\b])\n\n";
            return OutStream;
        }
    };
}


#endif