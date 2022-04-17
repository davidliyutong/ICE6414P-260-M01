/**
 * @file Matrix.hpp
 * @author davidliyutong (davidliyutong@sjtu.edu.cn)
 * @brief
 * @version 0.1
 * @date 2022-02-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <array>
#include <cstring>
#include <initializer_list>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

namespace mpimath {
    /**
     * @brief Matrix return codes
     * @enum MATRIX_OK          Every thing is OK
     * @enum MATRIX_ERR_SHAPE   Shape error
     * @enum MATRIX_ERR_NULL    nullptr error
     * @enum MATRIX_ERR_DATA    Data error
     * @enum MATRIX_ERR_IO      IO error
     *
     */
    typedef enum {
        MATRIX_OK = 0,
        MATRIX_ERR_SHAPE = 1,
        MATRIX_ERR_NULL = 2,
        MATRIX_ERR_DATA = 3,
        MATRIX_ERR_IO,
    } emMatrixError;


    template<typename T>
    class Matrix2DRow;

    /**
     * @brief Matrix 2D class, stores 2D matrix
     *
     * @tparam T data type
     */
    template<typename T>
    class Matrix2D {
    public:
        /**
         * @brief Construct a new empty Matrix2D object
         *
         */
        Matrix2D() {};

        /**
         * @brief Construct a new Matrix 2D object from existing one
         *
         * @param Src
         */
        Matrix2D(const Matrix2D<T>& Src) {
            Init(Src._ulRow, Src._ulCol, false);
            if (_pData != nullptr) {
                memcpy(_pData, Src._pData, Src._ulDataSize);
            }
        }

        /**
         * @brief Construct a new Matrix 2D object with parames
         *
         * @param ulRow Number of rows
         * @param ulCol Number of columns
         * @param bFillZero If to fill the matrix with 0
         */
        Matrix2D(size_t ulRow, size_t ulCol, bool bFillZero = false) {
            Init(ulRow, ulCol, bFillZero);
        }

        /**
         * @brief Init the matrix
         *
         * @param ulRow
         * @param ulCol
         * @param bFillZero
         */
        void Init(size_t ulRow, size_t ulCol, bool bFillZero = false) {
            if (_pData) {
                delete [] _pData;
            }
            this->_ulRow = ulRow;
            this->_ulCol = ulCol;
            this->_ulDataSize = sizeof(T) * ulRow * ulCol;
            if (this->_ulDataSize > 0) {
                _pData = new T[_ulDataSize];
                if (_pData != nullptr and bFillZero) {
                    bzero(_pData, _ulDataSize);
                }
            } else {
                _pData = nullptr;
            }
        }

        /**
         * @brief Destroy the Matrix2D object
         *
         */
        ~Matrix2D() {
            if (_pData != nullptr) {
                delete[] _pData;
                _pData = nullptr;
            }
        }

        inline T* pData() const { return _pData; };

        inline size_t ulRow() const { return _ulRow; };

        inline size_t ulCol() const { return _ulCol; };

        inline size_t ulDataSize() const { return _ulDataSize; };
        /**
         * @brief Get the shape of the matrix
         *
         * @return std::array<size_t, 2> {Row, Col}
         */
        inline std::array<size_t, 2> Shape() const {
            return std::array<size_t, 2>(_ulRow, _ulCol);
        }

        /**
         * @brief Get the size = row * col of matrix
         *
         * @return size_t
         */
        inline size_t Size() const {
            return _ulRow * _ulCol;
        }

        /**
         * @brief Assigment operator
         *
         * @param Src
         * @return Matrix2D<T>&
         */
        Matrix2D<T>& operator=(const Matrix2D<T>& Src) {
            if (this != &Src) {
                if (_pData != nullptr) {
                    delete[] _pData;
                    _pData = nullptr;
                }
                _ulCol = Src._ulCol;
                _ulRow = Src._ulRow;
                _ulDataSize = Src._ulDataSize;

                _pData = new T[Src._ulDataSize];
                memcpy(_pData, Src._pData, Src._ulDataSize);
            }

            return *this;
        }

        /**
         * @brief Support slicing
         *
         * @param ulRow The Row index
         * @return Matrix2DRow<T>
         */
        Matrix2DRow<T> operator[](size_t ulRowIdx) {
            return Matrix2DRow<T>(*this, ulRowIdx);
        }

        /**
         * @brief Support << output
         *
         * @param OutStream Out stream
         * @param Src
         * @return std::ostream&
         */
        friend std::ostream& operator<<(std::ostream& OutStream, const Matrix2D<T>& Src) {
            int row = 0, col = 0;
            OutStream << "array([";
            for (auto row = 0; row < Src._ulRow; ++row) {
                OutStream << "[";
                for (auto col = 0; col < Src._ulCol; ++col) {
                    OutStream << Src._pData[row * Src._ulCol + col] << ", ";
                }
                if (row < (Src._ulRow - 1)) {
                    OutStream << "\b\b],\n       ";
                } else {
                    OutStream << "\b\b]])\n\n";
                }
            }
            return OutStream;
        }


        /**
         * @brief  Fill matrix with given value
         *
         * @param Value
         */
        void fill(const T& Value) {
            for (auto idx = 0; idx < Size(); ++idx) {
                _pData[idx] = Value;
            }
        }

        /**
         * @brief Inplace transpose of matrix
         *
         */
        void Transpose() {
            /** If the matrix is valid */
            if (IsValid()) {
                /** Make new place for data */
                auto pNewData = new T[this->_ulDataSize];
                /** Copy the matrix */
                for (auto i = 0; i < _ulRow; ++i) {
                    for (auto j = 0; j < _ulCol; ++j) {
                        pNewData[j * _ulRow + i] = _pData[i * _ulCol + j];
                    }
                }
                std::swap(_ulCol, _ulRow);
                delete[] _pData;
                _pData = nullptr;
                _pData = pNewData;
            } else {
                return;
            }
        }

        /**
         * @brief Return the result of matrix multiplication
         *
         * @param N The matrix to multiply: Self @ N
         * @return Matrix2D<T> Result
         */
        Matrix2D<T> operator*(const Matrix2D<T>& N) {
            /** Check shape */
            if (this->_ulCol != N._ulRow) {
                throw MATRIX_ERR_SHAPE;
            }
            /**
             * @brief Transpose and multiply to optimize cache access
             *
             */

#if CONFIG_EN_CACHE_OPTIM
            auto N_T = N;
            N_T.Transpose();
#endif

            Matrix2D<T> Res(this->_ulRow, N._ulCol);
            /** Normal matmul operation */
            for (auto i = 0; i < Res._ulRow; ++i) {
                for (auto j = 0; j < Res._ulCol; ++j) {
                    T Sum = 0;
                    for (auto k = 0; k < this->_ulCol; ++k) {
#if CONFIG_EN_CACHE_OPTIM
                        Sum += this->pData[i * this->ulCol + k] * N_T.pData[j * N_T.ulCol + k];
#else
                        Sum += this->_pData[i * this->_ulCol + k] * N._pData[k * N._ulCol + j];
#endif

                    }
                    Res._pData[i * Res._ulCol + j] = Sum;
                }
            }
            return Res;
        }

        void operator *=(const Matrix2D<T>& N) {
            *this = *this * N;
        }

        /**
         * @brief Return if a matrix is valid
         *
         * @return true
         * @return false
         */
        bool IsValid() {
            return (_pData != nullptr);
        }


        /**
         * @brief Read matrix from a comma seperated .csv file
         *
         * @param sPath Path to csv file
         * @return emMatrixError Status
         */
        emMatrixError ReadCSV(const std::string& sPath) {
            std::ifstream InFile;
            std::string sLine;
            std::vector<std::vector<double>> vecData;

            /** Open file */
            InFile.open(sPath, std::ios::in);
            if (not InFile.is_open()) {
                return emMatrixError::MATRIX_ERR_IO;
            }

            /** Read CSV file */
            while (std::getline(InFile, sLine)) {
                /** For each line end with '\n' */
                std::stringstream ssReadStr(sLine);
                std::vector<double> vecDataLine;
                std::string sNumber;

                /** For each item seperated by ',' */
                while (std::getline(ssReadStr, sNumber, ',')) {
                    /** string -> double conversion */
                    vecDataLine.push_back(std::stod(sNumber));
                }
                vecData.push_back(vecDataLine);

                if (vecData.size() > 1) {
                    /** Check the size of incoming matrix */
                    if (vecData[vecData.size() - 1].size() != vecData[0].size()) {
                        InFile.close();
                        return emMatrixError::MATRIX_ERR_SHAPE;
                    }
                }
            }

            /** Cheke if the CSV contains valid data */
            if (vecData.size() < 0) {
                InFile.close();
                return emMatrixError::MATRIX_ERR_DATA;
            }

            /** Free memory to avoid memory leak */
            if (_pData != nullptr) {
                delete[] _pData;
                _pData = nullptr;
            }

            /** Change Matrix according to size */
            Init(vecData.size(), vecData[0].size(), false);
            for (auto row = 0; row < vecData.size(); ++row) {
                for (auto col = 0; col < vecData[0].size(); ++col) {
                    /** Convert data from double to type T */
                    _pData[row * _ulCol + col] = (T)(vecData[row][col]);
                }
            }

            /** Close input and return */
            InFile.close();
            return emMatrixError::MATRIX_OK;
        }

        /**
         * @brief Dump the matrix to a .csv file
         *
         * @param sPath Path to csv file
         * @return emMatrixError Status
         */
        emMatrixError DumpCSV(const std::string& sPath) {
            /** Check validity of matrix */
            if (IsValid()) {
                /** Open file */
                std::ofstream OutFile;
                OutFile.open(sPath, std::ios::out);
                if (not OutFile.is_open()) {
                    return emMatrixError::MATRIX_ERR_IO;
                }
                /** Dump matrix content to file */
                for (auto row = 0; row < _ulRow; ++row) {
                    OutFile << _pData[row * _ulCol + 0];
                    for (auto col = 1; col < _ulCol; ++col) {
                        OutFile << "," << _pData[row * _ulCol + col];
                    }
                    OutFile << "\n";
                }
                OutFile.close();

                return emMatrixError::MATRIX_OK;
            }
            return emMatrixError::MATRIX_ERR_NULL;

        }
    protected:
        T* _pData = nullptr;
        size_t _ulRow = 0, _ulCol = 0, _ulDataSize = 0;

    };

    /**
     * @brief Matrix2DRow class that stores a row view of Matrix 2D
     *
     * @tparam T
     */
    template<typename T>
    class Matrix2DRow {
    public:
        /**
         * @brief Construct a new Matrix2DRow object
         *
         * @param Mat
         * @param ulRow
         */
        Matrix2DRow(const Matrix2D<T>& Mat, const size_t ulRow) {
            if (Mat.pData() == NULL) {
                throw MATRIX_ERR_NULL;
            }
            if (ulRow < Mat.ulRow()) {
                _pData = Mat.pData() + ulRow * Mat.ulCol();
                _ulCol = Mat.ulCol();
                _ulDataSize = Mat.ulCol() * sizeof(T);
            } else {
                throw MATRIX_ERR_SHAPE;
            }
        }
        /**
         * @brief Destroy the Matrix2DRow object
         *
         */
        ~Matrix2DRow() = default;

        /**
         * @brief Assignment of row from anothor row
         *
         * @param Src
         * @return Matrix2DRow<T>&
         */
        Matrix2DRow<T>& operator=(const Matrix2DRow<T>& Src) {
            if (this->_ulCol == Src._ulCol) {
                memcpy(this->_pData, Src._pData, this->_ulCol);
                return *this;
            } else {
                throw MATRIX_ERR_SHAPE;
            }
        }

        /**
         * @brief Assignment of row from initializer list
         *
         * @param Src
         * @return Matrix2DRow<T>&
         */
        Matrix2DRow<T>& operator=(std::initializer_list<T> Src) {
            if (this->_ulCol == Src.size()) {
                for (auto i = 0; i < this->_ulCol; ++i) {
                    this->_pData[i] = (T) * (Src.begin() + i);
                }
                return *this;
            } else {
                throw MATRIX_ERR_SHAPE;
            }
        }

        inline T* pData() const { return _pData; };

        inline size_t ulCol() const { return _ulCol; };

        inline size_t ulDataSize() const { return _ulDataSize; };

        /**
         * @brief Return the shape of row
         *
         * @return std::array<size_t, 1> Shape
         */
        inline std::array<size_t, 1> Shape() const {
            return std::array<size_t, 1>(_ulCol);
        }

        /**
         * @brief Return the size of row
         *
         * @return size_t Size
         */
        inline size_t Size() const {
            return _ulCol;
        }

        /**
         * @brief Assignment of elements in row
         *
         * @param ulCol
         * @return T&
         */
        inline T& operator[](size_t ulCol) {
            return _pData[ulCol];
        }

        /**
         * @brief Support << output
         *
         * @param OutStream Out stream
         * @param Src
         * @return std::ostream&
         */
        friend std::ostream& operator<<(std::ostream& OutStream, const Matrix2DRow<T>& Src) {
            int row = 0, col = 0;
            OutStream << "array([";
            for (col = 0; col < Src._ulCol; ++col) {
                OutStream << Src._pData[row * Src._ulCol + col] << ", ";
            }
            OutStream << "\b\b])\n\n";
            return OutStream;
        }

    protected:
        T* _pData = nullptr; /** Data buffer */
        size_t _ulCol = 0; /** Number of columns */
        size_t _ulDataSize = 0; /** Size fo Data */
    };
}


#endif