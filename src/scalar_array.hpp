#pragma once
/*
  HMat-OSS (HMatrix library, open source software)

  Copyright (C) 2014-2015 Airbus Group SAS

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  http://github.com/jeromerobert/hmat-oss
*/

/*! \file
  \ingroup HMatrix
  \brief Scalar Array type used by the HMatrix library.
*/
#pragma once

#include <cstddef>

#include "data_types.hpp"
#include "hmat/hmat.h"

namespace hmat {

/*! \brief Templated dense Matrix type.

  The template parameter represents the scalar type of the matrix elements.  The
  supported types are \a S_t, \a D_t, \a C_t and \a Z_t, as defined in
  @data_types.hpp.
 */
template<typename T> class ScalarArray {
  /*! True if the matrix owns its memory, ie has to free it upon destruction */
  char ownsMemory:1;

public:
  /// Fortran style pointer (columnwise)
  T* m;
  /// Number of rows
  int rows;
  /// Number of columns
  int cols;
  /*! Leading dimension, as in BLAS */
  int lda;

  /** \brief Initialize the ScalarArray with existing ScalarArray.

      In this case the matrix doesn't own the data (the memory is not
      freed at the object destruction).

      \param d a ScalarArray
   */
  ScalarArray(const ScalarArray& d) : ownsMemory(false), m(d.m), rows(d.rows), cols(d.cols), lda(d.lda) {}
  /** \brief Initialize the matrix with existing data.

      In this case the matrix doesn't own the data (the memory is not
      freed at the object destruction).

      \param _m Pointer to the data
      \param _rows Number of rows
      \param _cols Number of cols
      \param lda Leading dimension, as in BLAS
   */
  ScalarArray(T* _m, int _rows, int _cols, int _lda=-1);
  /** \brief Create an empty matrix, filled with 0s.

     In this case, the memory is freed when the object is destroyed.

     \param _rows Number of rows
     \param _cols Number of columns
   */
  ScalarArray(int _rows, int _cols);
  /** \brief Initialize the ScalarArray with subset of existing ScalarArray.
   */
  ScalarArray(const ScalarArray& d, const int rowsOffset, const int rowsSize, const int colsOffset, const int colsSize):ScalarArray(d.m+rowsOffset+colsOffset*d.lda, rowsSize, colsSize, d.lda){}

  ~ScalarArray();

  /** This <- 0.
   */
  void clear();
  /** \brief Returns number of allocated zeros
   */
  size_t storedZeros() const;
  /** \brief this *= alpha.

      \param alpha The scaling factor.
   */
  void scale(T alpha);
  /** \brief Transpose in place.
   */
  void transpose();
  /** \brief Conjugate in place.
   */
  void conjugate();
  /** Return a copy of this.
   */
  ScalarArray<T>* copy(ScalarArray<T>* result = NULL) const;
  /** \brief Return a new matrix that is a transposed version of this.

    This new matrix is created in \a result (if provided)
   */
  ScalarArray<T>* copyAndTranspose(ScalarArray<T>* result = NULL) const;
  /**  Returns a pointer to a new ScalarArray representing a subset of row indices.
       Columns and leading dimension are unchanged.
       \param rowOffset offset to apply on the rows
       \param rowSize new number of rows
       \return pointer to a new ScalarArray.
   */
  ScalarArray<T> rowsSubset(const int rowOffset, const int rowSize) const ;

  /** this = alpha * op(A) * op(B) + beta * this

      Standard "GEMM" call, as in BLAS.

      \param transA 'N' or 'T', as in BLAS
      \param transB 'N' or 'T', as in BLAS
      \param alpha alpha
      \param a the matrix A
      \param b the matrix B
      \param beta beta
   */
  void gemm(char transA, char transB, T alpha, const ScalarArray<T>* a,
            const ScalarArray<T>* b, T beta);
  /*! Copy a matrix A into 'this' at offset (rowOffset, colOffset) (indices start at 0).

    \param a the matrix A
    \param rowOffset the row offset
    \param colOffset the column offset
   */
  void copyMatrixAtOffset(const ScalarArray<T>* a, int rowOffset, int colOffset);
  /*! Copy a matrix A into 'this' at offset (rowOffset, colOffset) (indices start at 0).

    In this function, only copy a sub-matrix of size (rowsToCopy, colsToCopy).

    \param a the matrix A
    \param rowOffset the row offset
    \param colOffset the column offset
    \param rowsToCopy number of rows to copy
    \param colsToCopy number of columns to copy
   */
  void copyMatrixAtOffset(const ScalarArray<T>* a, int rowOffset, int colOffset,
                          int rowsToCopy, int colsToCopy);
  /*! \brief add term by term a random value

    \param epsilon  x *= (1 + a),  a = epsilon*(1.0-2.0*rand()/(double)RAND_MAX)
   */
  void addRand(double epsilon);
  /*! \brief this += alpha * A

    \param a the Matrix A
   */
  void axpy(T alpha, const ScalarArray<T>* a);
  /*! \brief Return square of the Frobenius norm of the matrix.

    \return the matrix norm.
   */
  double normSqr() const;
  /*! \brief Return the Frobenius norm of the matrix.

    \return the matrix norm.
   */
  double norm() const;
  /*! \brief Return square of the Frobenius norm of the matrix 'this' x B^T.

    \return the matrix norm.
   */
  double norm_abt_Sqr(ScalarArray<T> &b) const ;
  /*! \brief Write the matrix to a binary file.

    \param filename output filename
   */
  void toFile(const char *filename) const;

  void fromFile(const char * filename);
  /** Simpler accessors for the data.

      There are 2 types to allow matrix modification or not.
   */
  inline T& get(int i, int j) {
    return m[i + ((size_t) lda) * j];
  }
  inline T get(int i, int j) const {
    return m[i + ((size_t) lda) * j];
  }

  /** Simpler accessors for the pointer on the data (i,j) in the scalar array.

      There are 2 types to allow matrix modification or not (const or not).
   */
  inline T* ptr(int i=0, int j=0) const {
    // here I might modify the data with this pointer
    return &m[i + ((size_t) lda) * j];
  }
  inline const T * const_ptr(int i=0, int j=0) const {
    // here this pointer is not supposed to allow content modification (unless casted into non-const)
    return &m[i + ((size_t) lda) * j];
  }

  /*! Check the matrix for the presence of NaN numbers.

    If a NaN is found, an assertion is triggered.
   */
  void checkNan() const;

  /*! Returns true if the matrix contains only zero values.
   */
  bool isZero() const ;

  size_t memorySize() const;

  /*! \brief Return a short string describing the content of this ScalarArray for debug (like: "ScalarArray [320 x 100] norm=22.34758")
    */
  std::string description() const {
    std::ostringstream convert;   // stream used for the conversion
    convert << "ScalarArray [" << rows << " x " << cols << "] norm=" << norm() ;
    return convert.str();
  }
  /*! \brief performs the rank 1 operation this := alpha*x*y**T + this,

     where alpha is a scalar, x and y are 2 Vector<T> of size 'm' and 'n', and this is a ScalarArray of size m x n
  */
  void rankOneUpdate(const T alpha, const ScalarArray<T> &x, const ScalarArray<T> &y);

  /*! \brief Write the ScalarArray data 'm' in a stream (FILE*, unix fd, ...)
    */
  void writeArray(hmat_iostream writeFunc, void * userData) const;

  /*! \brief Read the ScalarArray data 'm' from a stream (FILE*, unix fd, ...)
    */
  void readArray(hmat_iostream writeFunc, void * userData) ;

  /*! \brief LU decomposition (in-place)
    */
  void luDecomposition(int *pivots) ;

  /*! \brief Solve the system L X = B, with B = X on entry, and L = this.

    This function requires the matrix to be factored by
    HMatrix::luDecomposition() beforehand.

    \param x B on entry, the solution on exit.
   */
  void solveLowerTriangularLeft(ScalarArray<T>* x, int* pivots, bool unitriangular) const;

  /*! \brief Solve the system X U = B, with B = X on entry, and U = this.

    This function requires the matrix to be factored by
    HMatrix::luDecomposition() beforehand.

    \param x B on entry, the solution on exit.
   */
  void solveUpperTriangularRight(ScalarArray<T>* x, bool unitriangular, bool lowerStored) const;

  /*! \brief Solve the system U X = B, with B = X on entry, and U = this.

    This function requires the matrix to be factored by
    HMatrix::luDecomposition() beforehand.

    \param x B on entry, the solution on exit.
   */
  void solveUpperTriangularLeft(ScalarArray<T>* x, bool unitriangular, bool lowerStored) const;

  /*! \brief Solve the system U X = B, with B = X on entry, and U = this.

    This function requires the matrix to be factored by
    HMatrix::luDecomposition() beforehand.

    \param x B on entry, the solution on exit.
   */
  void solve(ScalarArray<T>* x, int *pivots) const;

  /*! \brief Compute the inverse of this in place.
   */
  void inverse();
  /** Makes an SVD of 'this' with LAPACK.

      \param u
      \param sigma
      \param vt
      \return
   */
  int svdDecomposition(ScalarArray<T>** u, ScalarArray<double>** sigma, ScalarArray<T>** vt) const;

  /** QR matrix decomposition.

    Warning: m is modified!

    \param tau
    \return
  */
  T* qrDecomposition();

  /** Do the product by Q.

      this=qr has to be factored using \a qrDecomposition.
      The arguments side and trans have the same meaning as in the
      LAPACK xORMQR function. Beware, only the 'L', 'N' case has been
      tested !

      \param side either 'L' or 'R', as in xORMQR
      \param trans either 'N' or 'T' as in xORMQR
      \param tau as created by \a qrDecomposition
      \param c as in xORMQR
      \return 0 for success
   */
  int productQ(char side, char trans, T* tau, ScalarArray<T>* c) const;


  /** Multiplication used in RkMatrix::truncate()

       A B -> computing "AB^t" with A=this and B full upper triangular
       (non-unitary diagonal)

   */
  void myTrmm(const ScalarArray<T>* bTri);

  /** \brief this = alpha.a.x + beta.this (x and this must have 1 column)
   */
  void gemv(char trans, T alpha, const ScalarArray<T>* a, const ScalarArray<T>* x,
            T beta);

  /** modified Gram-Schmidt algorithm of A='this'

      Computes a QR-decomposition of a matrix A=[a_1,...,a_n] thanks to the
      modified Gram-Schmidt procedure with column pivoting.

      The matrix A is overwritten with a matrix Q=[q_1,...,q_r] whose columns are
      orthonormal and are a basis of Im(A).
      A pivoting strategy is used to improve stability:
      Each new qj vector is computed from the vector with the maximal 2-norm
      amongst the remaining a_k vectors.

      To further improve stability for each newly computed q_j vector its
      component is removed from the remaining columns a_k of A.

      Stopping criterion:
      whenever the maximal norm of the remaining vectors is smaller than
      prec * max(||ai||) the algorithm stops and the numerical rank at precision
      prec is the number of q_j vectors computed.

      Eventually the computed decomposition is:
      [a_{perm[0]},...,a_{perm[rank-1]}] = [q_1,...,q_{rank-1}] * [r]
      where [r] is an upper triangular matrix.

      \param prec is a small parameter describing a relative precision thus
      0 < prec < 1.
      WARNING: the lowest precision allowed is 1e-6.
      \return rank

      NB: On exit the orthonormal matrix stored in A is 'full' and not represented
      as a product of Householder reflectors. OR/ZU-MQR from LAPACK is NOT
      the way to apply the matrix: one has to use matrix-vector product instead.
  */
  int modifiedGramSchmidt(ScalarArray<T> *r, double prec );
  };

  /*! \brief Templated Vector class = a ScalarArray with 1 column

    As for \a ScalarArray, the template parameter is the scalar type.
   */
  template<typename T> class Vector : public ScalarArray<T> {

  public:
    Vector(T* _m, int _rows):ScalarArray<T>(_m, _rows, 1){}
    Vector(int _rows):ScalarArray<T>(_rows, 1){}
    /** \brief Create Vector with column 'col' of existing ScalarArray
     */
    Vector(const ScalarArray<T> &d, int col):ScalarArray<T>(d.m+col*d.lda, d.rows, 1, d.lda){}
    Vector(const ScalarArray<T> *d, int col):Vector<T>(*d,col){}
    //~Vector(){}
    /** \brief this += x
     */
    void addToMe(const Vector<T>* x);
    /** \brief this -= x
     */
    void subToMe(const Vector<T>* x);
    /** L2 norm of the vector.
     */
    int absoluteMaxIndex(int startIndex = 0) const;
    /** Compute the dot product of two \Vector.

        For real-valued vectors, this is the usual dot product. For
        complex-valued ones, this is defined as:
           <x, y> = \bar{x}^t \times y
        as in BLAS

        \warning DOES NOT work with vectors with >INT_MAX elements

        \param x
        \param y
        \return <x, y>
     */
    static T dot(const Vector<T>* x, const Vector<T>* y);

    /** Simpler accessors for the vector data.
     */
    inline T& operator[](std::size_t i){
      return this->m[i];
    }
    inline const T& operator[] (std::size_t i) const {
      return this->m[i];
    }
  private:
    /// Disallow the copy
    Vector<T>(const Vector<T>& o);
  };

}  // end namespace hmat

