#include <stdarg.h>
#include <stdio.h>

#include "cmathematics.h"
#include "vec.h"

#ifndef MATRIX_H
#define MATRIX_H

/**
 * structure representing a matrix
 */
typedef struct
{
    unsigned int rows; // the number of rows in the matrix
    unsigned int cols; // the number of columns in the matrix
    float **elements;  // pointer to the first row of the matrix (array)
} mat;

extern const mat MAT_UNDEFINED; // undefined vector (no dimension)

/**
 * allocate memory for a matrix
 * @param rows the number of rows
 * @param cols the number of cols
 * @return the matrix
 */
mat allocateMat(unsigned int rows, unsigned int cols);

/**
 * frees a matrix's memory
 * @param m the matrix
 */
void freeMat(mat *m);

/**
 * construct an identity matrix:
 * square matrix with ones along the main diagonal
 * @param dim the dimension of the matrix (the number of rows and cols)
 * @return the matrix
 */
mat identity(unsigned int dim);

/**
 * construct a matrix with all zeros
 * @param rows the number of rows
 * @param cols the number of cols
 * @return the matrix
 */
mat zeroMatrix(unsigned int rows, unsigned int cols);

/**
 * construct a new matrix with specified values
 * <b>NOTE: must pass in floats as arguments</b>
 * <b>NOTE: pass values in row by row</b>
 * @param rows the number of rows
 * @param cols the number of cols
 * @param numVals the number of values passed in
 * @param ... the values to put in the matrix
 * @return the matrix
 */
mat newMatrix(unsigned int rows, unsigned int cols, unsigned int numVals, ...);

/**
 * construct a new matrix with passed in columns
 * @param rows the number of rows
 * @param cols the number of columns
 * @param ... the column vectors
 * @return the matrix
 */
mat newColMat(unsigned int rows, unsigned int cols, ...);

/**
 * construct a new matrix with passed in rows
 * @param rows the number of rows
 * @param cols the number of columns
 * @param ... the row vectors
 * @return the matrix
 */
mat newRowMat(unsigned int rows, unsigned int cols, ...);

/**
 * construct a new matrix with specified values
 * will automatically calculate the number of arguments
 * <b>NOTE: must pass in floats as arguments</b>
 * @param rows the number of rows
 * @param cols the number of cols
 * @param .. the values to put in the matrix
 * @return the matrix
 */
#define matrix(rows, cols, ...) newMatrix(rows, cols, NUMARGS(float, __VA_ARGS__), ##__VA_ARGS__)

/**
 * copies a matrix's values
 * @param m the pointer to the matrix to be copied
 * @return the copy
 */
mat copyMat(mat *m);

/**
 * prints the values of a matrix
 * @param m the matrix
 */
void printMat(mat m);

/**
 * get the row of a matrix
 * @param m the pointer to the matrix
 * @param row the index of the row to be copied (count from 1)
 * @return the row as a vector
 */
vec getMatRow(mat *m, unsigned int row);

/**
 * get the column of a matrix
 * @param m the pointer to the matrix
 * @param row the index of the column to be copied (count from 1)
 * @return the column as a vector
 */
vec getMatCol(mat *m, unsigned int col);

/**
 * convert a vector to a single row matrix
 * @param v the pointer to the vector
 * @return the matrix with the vector as the first row
 */
mat toRowVec(vec *v);

/**
 * convert a vector to a single column matrix
 * @param v the pointer to the vector
 * @return the matrix with the vector as the first column
 */
mat toColVec(vec *v);

/**
 * tests for equality between two matrices
 * @param m1 the first matrix
 * @param m2 the second matrix
 * @return true if the matrices have same dimension and equal corresponding values
 */
bool matEquals(mat m1, mat m2);

/**
 * adds a scalar to the values of a matrix
 * @param m the matrix
 * @param k the scalar
 * @return the resultant sum
 */
mat matScalarAddition(mat m, float k);

/**
 * directly adds a scalar to the values of a matrix
 * @param m the pointer to the matrix
 * @param k the scalar
 */
void matScalarAdditionTo(mat *m, float k);

/**
 * subtracts a scalar from the values of a matrix
 * @param m the matrix
 * @param k the scalar
 * @return the resultant sum
 */
mat matScalarSubtraction(mat m, float k);

/**
 * directly subtracts a scalar from the values of a matrix
 * @param m the pointer to the matrix
 * @param k the scalar
 */
void matScalarSubtractionFrom(mat *m, float k);

/**
 * multiplies the values of a matrix by a scalar
 * @param m the matrix
 * @param k the scalar
 * @return the multiplied matrix
 */
mat matScalarMultiplication(mat m, float k);

/**
 * directly multiplies the values of a matrix by a scalar
 * @param m the pointer to the matrix
 * @param k the scalar
 */
void matScalarMultiplicationBy(mat *m, float k);

/**
 * divides the values of a matrix by a scalar
 * @param m the matrix
 * @param k the scalar
 * @return the divided matrix
 */
mat matScalarDivision(mat m, float k);

/**
 * directly divides the values of a matrix by a scalar
 * @param m the pointer to the matrix
 * @param k the scalar
 */
void matScalarDivisionBy(mat *m, float k);

/**
 * adds the corresponding elements in two matrices
 * @param m1 the first matrix
 * @param m2 the second matrix
 * @return the sum of the matrices or MAT_UNDEFINED if hte matrices have different dimensions
 */
mat matAdd(mat m1, mat m2);

/**
 * add the corresponding elements of a matrix to another matrix
 * @param m1 the matrix to be added to
 * @param m2 the matrix to be added
 * @return true if the matrices have the same dimension
 */
bool matAddTo(mat *m1, mat m2);

/**
 * subtracts the corresponding elements in two matrices
 * @param m1 the first matrix
 * @param m2 the second matrix
 * @return the difference of the matrices or MAT_UNDEFINED if hte matrices have different dimensions
 */
mat matSubtract(mat m1, mat m2);

/**
 * subtract the corresponding elements of a matrix from another matrix
 * @param m1 the matrix to be subtracted from
 * @param m2 the matrix to be subtracted
 * @return true if the matrices have the same dimension
 */
bool matSubtractFrom(mat *m1, mat m2);

/**
 * multiplies a matrix by a vector
 * uses the row view of matrix-vector multiplication
 * @param m the matrix
 * @param v the vector
 * @return the vector product
 */
vec matVecMultiplication(mat m, vec v);

/**
 * multiplies a matrix by a matrix
 * @param m1 the first matrix
 * @param m2 the second matrix
 * @return the matrix product
 */
mat matMatMultiplication(mat m1, mat m2);

/**
 * transpose a matrix (swap rows and columns)
 * @param m the matrix
 * @return the transpose
 */
mat transpose(mat *m);

/**
 * elementary row operation - swap rows
 * @param m the pointer to the matrix
 * @param r1 the index of the first row (count from 1)
 * @param r2 the index of the second row (count from 1)
 * @return true if the row indices are in bounds and are unique
 */
bool swapRows(mat *m, unsigned int r1, unsigned int r2);

/**
 * elementary row operation - add one row to a nother
 * @param m the pointer to the matrix
 * @param r1 the index of the row to be added to (count from 1)
 * @param r2 the index of the row to be added (count from 1)
 * @return true if the row indices are in bounds and are unique
 */
bool addRows(mat *m, unsigned int r1, unsigned int r2);

/**
 * elementary row operation - multiply a row by a factor
 * @param m the pointer to the matrix
 * @param r1 the index of the row to be multiplied (count from 1)
 * @param k the factor
 * @return true if the row index is in bounds and the factor is not 0
 */
bool multiplyRow(mat *m, unsigned int r, float k);

/**
 * elementary row operation - add a multiple of one row to another row
 * @param m the pointer to the matrix
 * @param r1 the index of the row to be added to (count from 1)
 * @param r2 the index of the row to be added (count from 1)
 * @param k the factor
 * @return true if the row indices are in bounds, they are unique, and the factor is not 0
 */
bool addMultiple(mat *m, unsigned int r1, unsigned int r2, float k);

/**
 * performs Gaussian elimination to transform the matrix to row echelon form (REF)
 * @param m the matrix
 */
void ref(mat *m);

/**
 * performs Gaussian elimination to transform the matrix to reduced row echelon form (REF)
 * @param m the matrix
 */
void rref(mat *m);

/**
 * augment a vector on the end of the matrix
 * @param m the matrix
 * @param v the vector
 * @return the augmented matrix, MAT_UNDEFINED if the dimensions do not match
 */
mat augmentVector(mat *m, vec *v);

/**
 * augment a matrix on the end of the matrix
 * @param m the original matrix
 * @param m2 the matrix to be augmented
 * @return the augmented matrix, MAT_UNDEFINED if the dimensions do not match
 */
mat augmentMatrix(mat *m, mat *m2);

/**
 * copy matrix without an excluded row and column
 * @param m the matrix
 * @param exclRow the index of the row to be excluded (count from 1)
 * @param exclCol the index of the col to be excluded (count from 1)
 * @return the matrix without the row and column
 */
mat spliceMat(mat *m, unsigned int exclRow, unsigned int exclCol);

/**
 * calculate the determinant of a matrix through cofactor expansion
 * @param m the matrix
 * @return the determinant value, 0 if not a square matrix
 */
float determinant(mat *m);

/**
 * base method to calculate the determinant of the matrix through cofactor expansion with an exclusion list
 * @param m the matrix
 * @return the determinant value, 0 if not a square matrix
 */
float determinantExclusion(mat *m);

/**
 * calculate the determinant of a matrix through cofactor expansion using an exclusion list
 * @param m the matrix
 * @param row the row to expand on
 * @param col the new column to exclude
 * @param skipCols the existing list of columns to exclude
 * @param noSkipCols the number of columns in the list to skip
 * @return the determinant
 */
float _determinantExclusion(mat *m,
                            unsigned int row,
                            unsigned int col,
                            unsigned int *skipCols,
                            unsigned int *noSkipCols);

/**
 * calculates the cofactor of a matrix at the corresponding row and column
 * @param m the matrix
 * @param r the row index (count from 1)
 * @param c the col index (count from 1)
 * @return the cofactor value
 */
float cofactor(mat *m, unsigned int r, unsigned int c);

/**
 * calculate the cofactor matrix (entries are the corresponding cofactors)
 * @param m the matrix
 * @return the cofactor matrix, MAT_UNDEFINED if not a square matrix
 */
mat cofactorMatrix(mat *m);

/**
 * calculate the adjugate of a matrix (transpose of the cofactor matrix)
 * @param m the matrix
 * @return the adjugate, MAT_UNDEFINED if not a square matrix
 */
mat adjugate(mat *m);

/**
 * determine if a matrix is invertible
 * @param m the matrix
 * @return if the matrix is square and has a nonzero determinant
 */
bool invertible(mat *m);

/**
 * calculate the inverse of a matrix using the adjugate
 * @param m the matrix
 * @return the inverse, MAT_UNDEFINED if not invertible
 */
mat inverseMat(mat *m);

/**
 * calculate the inverse of a matrix using Gaussian Elimination
 * @param m the matrix
 * @return the inverse, MAT_UNDEFINED if not invertible
 */
mat inverseMatRREF(mat *m);

#endif