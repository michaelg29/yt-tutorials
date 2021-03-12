#include "matrix.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

const mat MAT_UNDEFINED = {0, 0, NULL}; // undefined vector (no dimension)

/**
 * allocate memory for a matrix
 * @param rows the number of rows
 * @param cols the number of cols
 * @return the matrix
 */
mat allocateMat(unsigned int rows, unsigned int cols)
{
    mat ret;

    ret.rows = rows;
    ret.cols = cols;

    // allocate the array of rows
    ret.elements = (float**)malloc(rows * sizeof(float));
    for (unsigned int i = 0; i < rows; i++)
    {
        // allocate the row's memory
        ret.elements[i] = (float*)malloc(cols * sizeof(float));
    }

    return ret;
}

/**
 * frees a matrix's memory
 * @param m the matrix
 */
void freeMat(mat *m)
{
    free(m->elements);
    m->elements = NULL;
    m->rows = 0;
    m->cols = 0;
}

/**
 * construct an identity matrix:
 * square matrix with ones along the main diagonal
 * @param dim the dimension of the matrix (the number of rows and cols)
 * @return the matrix
 */
mat identity(unsigned int dim)
{
    // allocate a square matrix
    mat ret = allocateMat(dim, dim);

    for (unsigned int r = 0; r < dim; r++)
    {
        for (unsigned int c = 0; c < dim; c++)
        {
            // if on the main diagonal, set to a 1, otherwise set to a 0
            ret.elements[r][c] = (r == c) ? 1.0f : 0.0f;
        }
    }

    return ret;
}

/**
 * construct a matrix with all zeros
 * @param rows the number of rows
 * @param cols the number of cols
 * @return the matrix
 */
mat zeroMatrix(unsigned int rows, unsigned int cols)
{
    mat ret = allocateMat(rows, cols);

    for (unsigned int r = 0; r < rows; r++)
    {
        for (unsigned int c = 0; c < cols; c++)
        {
            ret.elements[r][c] = 0.0f;
        }
    }

    return ret;
}

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
mat newMatrix(unsigned int rows, unsigned int cols, unsigned int numVals, ...)
{
    mat ret = allocateMat(rows, cols);

    va_list list;
    va_start(list, numVals);

    unsigned int i = 0;

    for (unsigned int r = 0; r < rows; r++)
    {
        for (unsigned int c = 0; c < cols; c++)
        {
            if (i++ < numVals)
            {
                // get next element in list
                ret.elements[r][c] = va_arg(list, double);
            }
            else
            {
                // put a zero if no more elements passed in
                ret.elements[r][c] = 0.0f;
            }
        }
    }

    va_end(list);

    return ret;
}

/**
 * construct a new matrix with passed in columns
 * @param rows the number of rows
 * @param cols the number of columns
 * @param ... the column vectors
 * @return the matrix
 */
mat newColMat(unsigned int rows, unsigned int cols, ...)
{
    mat ret = allocateMat(rows, cols);

    // get list
    va_list list;
    va_start(list, cols);

    // iterate through each passed-in column
    for (unsigned int c = 0; c < cols; c++)
    {
        vec col = va_arg(list, vec);

        for (unsigned int r = 0; r < rows; r++)
        {
            ret.elements[r][c] = col.elements[r];
        }
    }

    va_end(list);

    return ret;
}

/**
 * construct a new matrix with passed in rows
 * @param rows the number of rows
 * @param cols the number of columns
 * @param ... the row vectors
 * @return the matrix
 */
mat newRowMat(unsigned int rows, unsigned int cols, ...)
{
    mat ret = allocateMat(rows, cols);

    // get list
    va_list list;
    va_start(list, cols);

    // iterate through each passed in row
    for (unsigned int r = 0; r < rows; r++)
    {
        vec row = va_arg(list, vec);

        for (unsigned int c = 0; c < cols; c++)
        {
            ret.elements[r][c] = row.elements[c];
        }
    }

    va_end(list);

    return ret;
}

/**
 * copies a matrix's values
 * @param m the pointer to the matrix to be copied
 * @return the copy
 */
mat copyMat(mat *m)
{
    mat ret = allocateMat(m->rows, m->cols);

    for (unsigned int r = 0; r < ret.rows; r++)
    {
        for (unsigned int c = 0; c < ret.cols; c++)
        {
            ret.elements[r][c] = m->elements[r][c];
        }
    }

    return ret;
}

/**
 * prints the values of a matrix
 * @param m the matrix
 */
void printMat(mat m)
{
    for (unsigned int r = 0; r < m.rows; r++)
    {
        printf("\n");

        printf("|");

        for (unsigned int c = 0; c < m.cols; c++)
        {
            printf(" %f", m.elements[r][c]);
        }

        printf(" |");
    }
    printf("\n");
}

/**
 * get the row of a matrix
 * @param m the pointer to the matrix
 * @param row the index of the row to be copied (count from 1)
 * @return the row as a vector
 */
vec getMatRow(mat *m, unsigned int row)
{
    row--; // subtract one because start counting from 1
    if (row >= m->rows)
    {
        // index out of bounds
        return VEC_UNDEFINED;
    }

    vec ret = allocateVec(m->cols);

    for (unsigned int i = 0; i < ret.dim; i++)
    {
        // get ith element in row
        ret.elements[i] = m->elements[row][i];
    }

    return ret;
}

/**
 * get the column of a matrix
 * @param m the pointer to the matrix
 * @param row the index of the column to be copied (count from 1)
 * @return the column as a vector
 */
vec getMatCol(mat *m, unsigned int col)
{
    col--; // subtract one because start counting from 1
    if (col >= m->cols)
    {
        // index out of bounds
        return VEC_UNDEFINED;
    }

    vec ret = allocateVec(m->rows);

    for (unsigned int i = 0; i < ret.dim; i++)
    {
        // get ith element in column
        ret.elements[i] = m->elements[i][col];
    }

    return ret;
}

/**
 * convert a vector to a single row matrix
 * @param v the pointer to the vector
 * @return the matrix with the vector as the first row
 */
mat toRowVec(vec *v)
{
    mat ret = allocateMat(1, v->dim); // 1 row, n columns

    for (unsigned int i = 0; i < v->dim; i++)
    {
        // put the values in the first row
        ret.elements[0][i] = v->elements[i];
    }

    return ret;
}

/**
 * convert a vector to a single column matrix
 * @param v the pointer to the vector
 * @return the matrix with the vector as the first column
 */
mat toColVec(vec *v)
{
    mat ret = allocateMat(v->dim, 1); // n rows, 1 column

    for (unsigned int i = 0; i < v->dim; i++)
    {
        // put the values in the first column
        ret.elements[i][0] = v->elements[i];
    }

    return ret;
}

/**
 * tests for equality between two matrices
 * @param m1 the first matrix
 * @param m2 the second matrix
 * @return true if the matrices have same dimension and equal corresponding values
 */
bool matEquals(mat m1, mat m2)
{
    if (m1.rows == m2.rows &&
        m1.cols == m2.cols)
    {
        // dimensions match
        for (unsigned int r = 0; r < m1.rows; r++)
        {
            for (unsigned int c = 0; c < m1.cols; c++)
            {
                if (m1.elements[r][c] != m2.elements[r][c])
                {
                    return false;
                }
            }
        }

        return true;
    }

    return false;
}

/**
 * adds a scalar to the values of a matrix
 * @param m the matrix
 * @param k the scalar
 * @return the resultant sum
 */
mat matScalarAddition(mat m, float k)
{
    mat ret = allocateMat(m.rows, m.cols);

    for (unsigned int r = 0; r < ret.rows; r++)
    {
        for (unsigned int c = 0; c < ret.cols; c++)
        {
            ret.elements[r][c] = m.elements[r][c] + k;
        }
    }

    return ret;
}

/**
 * directly adds a scalar to the values of a matrix
 * @param m the pointer to the matrix
 * @param k the scalar
 */
void matScalarAdditionTo(mat *m, float k)
{
    for (unsigned int r = 0; r < m->rows; r++)
    {
        for (unsigned int c = 0; c < m->cols; c++)
        {
            m->elements[r][c] += k;
        }
    }
}

/**
 * subtracts a scalar from the values of a matrix
 * @param m the matrix
 * @param k the scalar
 * @return the resultant sum
 */
mat matScalarSubtraction(mat m, float k)
{
    mat ret = allocateMat(m.rows, m.cols);

    for (unsigned int r = 0; r < ret.rows; r++)
    {
        for (unsigned int c = 0; c < ret.cols; c++)
        {
            ret.elements[r][c] = m.elements[r][c] - k;
        }
    }

    return ret;
}

/**
 * directly subtracts a scalar from the values of a matrix
 * @param m the pointer to the matrix
 * @param k the scalar
 */
void matScalarSubtractionFrom(mat *m, float k)
{
    for (unsigned int r = 0; r < m->rows; r++)
    {
        for (unsigned int c = 0; c < m->cols; c++)
        {
            m->elements[r][c] -= k;
        }
    }
}

/**
 * multiplies the values of a matrix by a scalar
 * @param m the matrix
 * @param k the scalar
 * @return the multiplied matrix
 */
mat matScalarMultiplication(mat m, float k)
{
    mat ret = allocateMat(m.rows, m.cols);

    for (unsigned int r = 0; r < ret.rows; r++)
    {
        for (unsigned int c = 0; c < ret.cols; c++)
        {
            ret.elements[r][c] = m.elements[r][c] * k;
        }
    }

    return ret;
}

/**
 * directly multiplies the values of a matrix by a scalar
 * @param m the pointer to the matrix
 * @param k the scalar
 */
void matScalarMultiplicationBy(mat *m, float k)
{
    for (unsigned int r = 0; r < m->rows; r++)
    {
        for (unsigned int c = 0; c < m->cols; c++)
        {
            m->elements[r][c] *= k;
        }
    }
}

/**
 * divides the values of a matrix by a scalar
 * @param m the matrix
 * @param k the scalar
 * @return the divided matrix
 */
mat matScalarDivision(mat m, float k)
{
    mat ret = allocateMat(m.rows, m.cols);

    for (unsigned int r = 0; r < ret.rows; r++)
    {
        for (unsigned int c = 0; c < ret.cols; c++)
        {
            ret.elements[r][c] = m.elements[r][c] / k;
        }
    }

    return ret;
}

/**
 * directly divides the values of a matrix by a scalar
 * @param m the pointer to the matrix
 * @param k the scalar
 */
void matScalarDivisionBy(mat *m, float k)
{
    for (unsigned int r = 0; r < m->rows; r++)
    {
        for (unsigned int c = 0; c < m->cols; c++)
        {
            m->elements[r][c] /= k;
        }
    }
}

/**
 * adds the corresponding elements in two matrices
 * @param m1 the first matrix
 * @param m2 the second matrix
 * @return the sum of the matrices or MAT_UNDEFINED if hte matrices have different dimensions
 */
mat matAdd(mat m1, mat m2)
{
    if (m1.rows != m2.rows ||
        m1.cols != m2.cols)
    {
        // dimensions do not match
        return MAT_UNDEFINED;
    }

    mat ret = allocateMat(m1.rows, m1.cols);

    for (unsigned int r = 0; r < ret.rows; r++)
    {
        for (unsigned int c = 0; c < ret.cols; c++)
        {
            ret.elements[r][c] = m1.elements[r][c] + m2.elements[r][c];
        }
    }

    return ret;
}

/**
 * add the corresponding elements of a matrix to another matrix
 * @param m1 the matrix to be added to
 * @param m2 the matrix to be added
 * @return true if the matrices have the same dimension
 */
bool matAddTo(mat *m1, mat m2)
{
    if (m1->rows != m2.rows ||
        m1->cols != m2.cols)
    {
        // dimensions do not match
        return false;
    }

    for (unsigned int r = 0; r < m1->rows; r++)
    {
        for (unsigned int c = 0; c < m1->cols; c++)
        {
            m1->elements[r][c] += m2.elements[r][c];
        }
    }

    return true;
}

/**
 * subtracts the corresponding elements in two matrices
 * @param m1 the first matrix
 * @param m2 the second matrix
 * @return the difference of the matrices or MAT_UNDEFINED if hte matrices have different dimensions
 */
mat matSubtract(mat m1, mat m2)
{
    if (m1.rows != m2.rows ||
        m1.cols != m2.cols)
    {
        // dimensions do not match
        return MAT_UNDEFINED;
    }

    mat ret = allocateMat(m1.rows, m1.cols);

    for (unsigned int r = 0; r < ret.rows; r++)
    {
        for (unsigned int c = 0; c < ret.cols; c++)
        {
            ret.elements[r][c] = m1.elements[r][c] - m2.elements[r][c];
        }
    }

    return ret;
}

/**
 * subtract the corresponding elements of a matrix from another matrix
 * @param m1 the matrix to be subtracted from
 * @param m2 the matrix to be subtracted
 * @return true if the matrices have the same dimension
 */
bool matSubtractFrom(mat *m1, mat m2)
{
    if (m1->rows != m2.rows ||
        m1->cols != m2.cols)
    {
        // dimensions do not match
        return false;
    }

    for (unsigned int r = 0; r < m1->rows; r++)
    {
        for (unsigned int c = 0; c < m1->cols; c++)
        {
            m1->elements[r][c] -= m2.elements[r][c];
        }
    }

    return true;
}

/**
 * multiplies a matrix by a vector
 * uses the row view of matrix-vector multiplication
 * @param m the matrix
 * @param v the vector
 * @return the vector product
 */
vec matVecMultiplication(mat m, vec v)
{
    if (m.cols != v.dim)
    {
        // number of columns in the matix must equal the number of elements of the vector
        return VEC_UNDEFINED;
    }

    vec ret = allocateVec(m.rows);

    for (unsigned int r = 0; r < ret.dim; r++)
    {
        // dot product of the vector with the corresponding row
        ret.elements[r] = dot(v, getMatRow(&m, r + 1));
    }

    return ret;
}

/**
 * multiplies a matrix by a matrix
 * @param m1 the first matrix
 * @param m2 the second matrix
 * @return the matrix product
 */
mat matMatMultiplication(mat m1, mat m2)
{
    if (m1.cols != m2.rows)
    {
        // number of columns in the first matix must equal the number of rows in the second matrix
        return MAT_UNDEFINED;
    }

    // create quick reference for the rows of the first matrix and columns of the second matrix
    vec *m1Rows = (vec*)malloc(m1.rows * sizeof(vec));
    vec *m2Cols = (vec*)malloc(m2.cols * sizeof(vec));

    for (unsigned int r = 0; r < m1.rows; r++)
    {
        m1Rows[r] = getMatRow(&m1, r + 1);
    }
    for (unsigned int c = 0; c < m2.cols; c++)
    {
        m2Cols[c] = getMatCol(&m2, c + 1);
    }

    mat ret = allocateMat(m1.rows, m2.cols);

    for (unsigned int r = 0; r < ret.rows; r++)
    {
        for (unsigned int c = 0; c < ret.cols; c++)
        {
            // dot product of the rth row of m1 with the cth column of m2
            ret.elements[r][c] = dot(m1Rows[r], m2Cols[c]);
        }
    }

    return ret;
}

/**
 * transpose a matrix (swap rows and columns)
 * @param m the matrix
 * @return the transpose
 */
mat transpose(mat *m)
{
    mat ret = allocateMat(m->cols, m->rows);

    for (unsigned int r = 0; r < ret.rows; r++)
    {
        for (unsigned int c = 0; c < ret.cols; c++)
        {
            // A_transpose(i, j) = A(i, j)
            ret.elements[r][c] = m->elements[c][r];
        }
    }

    return ret;
}

/**
 * elementary row operation - swap rows
 * @param m the pointer to the matrix
 * @param r1 the index of the first row (count from 1)
 * @param r2 the index of the second row (count from 1)
 * @return true if the row indices are in bounds and are unique
 */
bool swapRows(mat *m, unsigned int r1, unsigned int r2)
{
    r1--;
    r2--;

    if (r1 >= m->rows || r2 >= m->rows ||
        r1 == r2)
    {
        return false;
    }

    // swap pointers
    float *tmp = m->elements[r1];
    m->elements[r1] = m->elements[r2];
    m->elements[r2] = tmp;

    return true;
}

/**
 * elementary row operation - add one row to a nother
 * @param m the pointer to the matrix
 * @param r1 the index of the row to be added to (count from 1)
 * @param r2 the index of the row to be added (count from 1)
 * @return true if the row indices are in bounds and are unique
 */
bool addRows(mat *m, unsigned int r1, unsigned int r2)
{
    r1--;
    r2--;

    if (r1 >= m->rows || r2 >= m->rows ||
        r1 == r2)
    {
        return false;
    }

    for (unsigned int c = 0; c < m->cols; c++)
    {
        // add corresponding elements
        m->elements[r1][c] += m->elements[r2][c];
    }

    return true;
}

/**
 * elementary row operation - multiply a row by a factor
 * @param m the pointer to the matrix
 * @param r1 the index of the row to be multiplied (count from 1)
 * @param k the factor
 * @return true if the row index is in bounds and the factor is not 0
 */
bool multiplyRow(mat *m, unsigned int r, float k)
{
    r--;

    if (r >= m->rows ||
        k == 0.0f)
    {
        return false;
    }

    for (unsigned int c = 0; c < m->cols; c++)
    {
        // multiply each element
        m->elements[r][c] *= k;
    }

    return true;
}

/**
 * elementary row operation - add a multiple of one row to another row
 * @param m the pointer to the matrix
 * @param r1 the index of the row to be added to (count from 1)
 * @param r2 the index of the row to be added (count from 1)
 * @param k the factor
 * @return true if the row indices are in bounds, they are unique, and the factor is not 0
 */
bool addMultiple(mat *m, unsigned int r1, unsigned int r2, float k)
{
    r1--;
    r2--;

    if (r1 >= m->rows || r2 >= m->rows ||
        r1 == r2 ||
        k == 0.0f)
    {
        return false;
    }

    for (unsigned int c = 0; c < m->cols; c++)
    {
        // add corresponding elements
        m->elements[r1][c] += k * m->elements[r2][c];
    }
}

/**
 * performs Gaussian elimination to transform the matrix to row echelon form (REF)
 * @param m the matrix
 */
void ref(mat *m)
{
    unsigned int currentRow = 0;
    for (unsigned int c = 0; c < m->cols; c++)
    {
        unsigned int r = currentRow;
        if (r >= m->rows)
        {
            // no more rows
            break;
        }

        // find nonzero entry
        for (; r < m->rows; r++)
        {
            if (m->elements[r][c] != 0.0f)
            {
                // non-zero element
                break;
            }
        }

        // didn't find a nonzero entry in column
        if (r == m->rows)
        {
            continue;
        }

        // swap with proper row
        swapRows(m, currentRow + 1, r + 1);

        // multiply row by 1 / value
        // don't call function because only need to multiply values to right
        float factor = 1 / m->elements[currentRow][c];
        for (unsigned int col = c; col < m->cols; col++)
        {
            m->elements[currentRow][col] *= factor;
        }

        // clear out rows below
        for (r = currentRow + 1; r < m->rows; r++)
        {
            addMultiple(m, r + 1, currentRow + 1, -1 * m->elements[r][c]);
        }

        currentRow++;
    }
}

/**
 * performs Gaussian elimination to transform the matrix to reduced row echelon form (REF)
 * @param m the matrix
 */
void rref(mat *m)
{
    unsigned int currentRow = 0;
    for (unsigned int c = 0; c < m->cols; c++)
    {
        unsigned int r = currentRow;
        if (r >= m->rows)
        {
            // no more rows
            break;
        }

        // find nonzero entry
        for (; r < m->rows; r++)
        {
            if (m->elements[r][c] != 0.0f)
            {
                // non-zero value
                break;
            }
        }

        // didn't find a nonzero entry in column
        if (r == m->rows)
        {
            continue;
        }

        // swap with proper row
        swapRows(m, currentRow + 1, r + 1);

        // multiply row by 1 / value
        // don't call function because only need to multiply values to right
        float factor = 1 / m->elements[currentRow][c];
        for (unsigned int col = c; col < m->cols; col++)
        {
            m->elements[currentRow][col] *= factor;
        }

        // clear out rows above and below
        for (r = 0; r < m->rows; r++)
        {
            if (r == currentRow)
            {
                continue;
            }
            addMultiple(m, r + 1, currentRow + 1, -1 * m->elements[r][c]);
        }

        currentRow++;
    }
}

/**
 * augment a vector on the end of the matrix
 * @param m the matrix
 * @param v the vector
 * @return the augmented matrix, MAT_UNDEFINED if the dimensions do not match
 */
mat augmentVector(mat *m, vec *v)
{
    // number of rows of the matrix must equal the number of elements in the vector
    if (m->rows != v->dim)
    {
        return MAT_UNDEFINED;
    }

    // allocate new matrix
    mat ret = allocateMat(m->rows, m->cols + 1);

    // input values
    for (unsigned int r = 0; r < m->rows; r++)
    {
        // copy matrix values
        unsigned int c = 0;
        for (; c < m->cols; c++)
        {
            ret.elements[r][c] = m->elements[r][c];
        }

        // put vector values at end
        ret.elements[r][c] = v->elements[r];
    }

    return ret;
}

/**
 * augment a matrix on the end of the matrix
 * @param m the original matrix
 * @param m2 the matrix to be augmented
 * @return the augmented matrix, MAT_UNDEFINED if the dimensions do not match
 */
mat augmentMatrix(mat *m, mat *m2)
{
    // the number of rows must be the same
    if (m->rows != m2->rows)
    {
        return MAT_UNDEFINED;
    }

    // allocate new matrix
    mat ret = allocateMat(m->rows, m->cols + m2->cols);

    // input values
    for (unsigned int r = 0; r < m->rows; r++)
    {
        // copy original matrix
        unsigned int c = 0;
        for (; c < m->cols; c++)
        {
            ret.elements[r][c] = m->elements[r][c];
        }

        // augment values from second matrix
        for (; c < ret.cols; c++)
        {
            ret.elements[r][c] = m2->elements[r][c - m->cols];
        }
    }

    return ret;
}

/**
 * copy matrix without an excluded row and column
 * @param m the matrix
 * @param exclRow the index of the row to be excluded (count from 1)
 * @param exclCol the index of the col to be excluded (count from 1)
 * @return the matrix without the row and column
 */
mat spliceMat(mat *m, unsigned int exclRow, unsigned int exclCol)
{
    exclRow--;
    exclCol--;

    // allocate new matrix
    mat ret = allocateMat(m->rows - 1, m->cols - 1);

    // offset to account for skipped row
    unsigned int rowOffset = 0;
    for (unsigned int r = 0; r < ret.rows; r++)
    {
        // offset to account for skipped column
        unsigned int colOffset = 0;

        if (r == exclRow)
        {
            // access values from next row
            rowOffset++;
        }

        for (unsigned int c = 0; c < ret.cols; c++)
        {
            if (c == exclCol)
            {
                // access values from next col
                colOffset++;
            }

            // assign value
            ret.elements[r][c] = m->elements[r + rowOffset][c + colOffset];
        }
    }

    return ret;
}

/**
 * calculate the determinant of a matrix through cofactor expansion
 * @param m the matrix
 * @return the determinant value, 0 if not a square matrix
 */
float determinant(mat *m)
{
    // must be a square matrix with at least 1 row
    if (m->rows != m->cols || m->rows == 0)
    {
        return 0.0f;
    }

    float ret = 0.0f;

    // expand across first row
    for (unsigned int c = 0; c < m->cols; c++)
    {
        if (m->elements[0][c] != 0.0f)
        {
            ret += m->elements[0][c] * cofactor(m, 1, c + 1);
        }
    }

    return ret;
}

/**
 * base method to calculate the determinant of the matrix through cofactor expansion with an exclusion list
 * @param m the matrix
 * @return the determinant value, 0 if not a square matrix
 */
float determinantExclusion(mat *m)
{
    // must be a square matrix with mroe than 1 row
    if (m->rows != m->cols || m->rows == 0)
    {
        return 0.0f;
    }

    // initialize array
    unsigned int *skipCols = (unsigned int*)malloc(m->cols * sizeof(unsigned int));
    unsigned int noSkipCols = 0;

    float ret = _determinantExclusion(m, 1, 0, skipCols, &noSkipCols);

    free(skipCols);

    return ret;
}

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
                            unsigned int *noSkipCols)
{
    // insert current column into exclusion list
    skipCols[*noSkipCols] = col;
    (*noSkipCols)++;

    if (row == m->rows - 1)
    {
        printf("");
    }

    // base case to return single element
    if (row == m->rows)
    {
        unsigned int c = m->cols;
        // find last column not in exclusion list
        if (*noSkipCols != 0)
        {
            if (skipCols[*noSkipCols - 1] != 0)
            {
                while (containsUint(skipCols, *noSkipCols, c))
                {
                    c--;
                }
                (*noSkipCols)--;
            }
        }
        return m->elements[row - 1][c - 1];
    }

    float ret = 0.0f;
    char cofactorSign = 1;

    // cofactor expansion across current row
    for (unsigned int c = 1; c <= m->cols; c++)
    {
        // skip excluded cols
        if (containsUint(skipCols, *noSkipCols, c))
        {
            continue;
        }

        float res = 0.0f;
        if (m->elements[row - 1][c - 1] != 0.0f)
        {
            // recursive
            res = cofactorSign *
                  m->elements[row - 1][c - 1] *
                  _determinantExclusion(m, row + 1, c, skipCols, noSkipCols);
        }

        ret += res;
        cofactorSign = -cofactorSign;
    }

    // remove current column from the list
    (*noSkipCols)--;
    return ret;
}

/**
 * calculates the cofactor of a matrix at the corresponding row and column
 * @param m the matrix
 * @param r the row index (count from 1)
 * @param c the col index (count from 1)
 * @return the cofactor value
 */
float cofactor(mat *m, unsigned int r, unsigned int c)
{
    // must be a square matrix with at least 1 row
    if (m->rows != m->cols || m->rows == 0)
    {
        return 0.0f;
    }

    // boundary condition
    if (r > m->rows || c > m->cols)
    {
        return 0.0f;
    }

    // alternate signs
    // if row + col is even, positive
    // if row + col is odd, negative
    char sign = ((r + c) & 1) == 0 ? 1 : -1;

    mat splicedMat = spliceMat(m, r, c);

    // cofactor = sign * determinant of the matrix without row and col
    float ret = sign * determinantExclusion(&splicedMat);

    freeMat(&splicedMat);

    return ret;
}

/**
 * calculate the cofactor matrix (entries are the corresponding cofactors)
 * @param m the matrix
 * @return the cofactor matrix, MAT_UNDEFINED if not a square matrix
 */
mat cofactorMatrix(mat *m)
{
    // must be a square matrix with at least 1 row
    if (m->rows != m->cols || m->rows == 0)
    {
        return MAT_UNDEFINED;
    }

    mat ret = allocateMat(m->rows, m->cols);

    for (unsigned int r = 0; r < ret.rows; r++)
    {
        for (unsigned int c = 0; c < ret.cols; c++)
        {
            ret.elements[r][c] = cofactor(m, r + 1, c + 1);
        }
    }

    return ret;
}

/**
 * calculate the adjugate of a matrix (transpose of the cofactor matrix)
 * @param m the matrix
 * @return the adjugate, MAT_UNDEFINED if not a square matrix
 */
mat adjugate(mat *m)
{
    // must be a square matrix with at least 1 row
    if (m->rows != m->cols || m->rows == 0)
    {
        return MAT_UNDEFINED;
    }

    mat ret = allocateMat(m->rows, m->cols);

    for (unsigned int r = 0; r < ret.rows; r++)
    {
        for (unsigned int c = 0; c < ret.cols; c++)
        {
            ret.elements[r][c] = cofactor(m, c + 1, r + 1);
        }
    }

    return ret;
}

/**
 * determine if a matrix is invertible
 * @param m the matrix
 * @return if the matrix is square and has a nonzero determinant
 */
bool invertible(mat *m)
{
    // must be a square matrix with at least 1 row
    if (m->rows != m->cols || m->rows == 0)
    {
        return false;
    }

    return determinant(m) != 0.0f;
}

/**
 * calculate the inverse of a matrix using the adjugate
 * @param m the matrix
 * @return the inverse, MAT_UNDEFINED if not invertible
 */
mat inverseMat(mat *m)
{
    // must be a square matrix with at least 1 row
    if (m->rows != m->cols || m->rows == 0)
    {
        return MAT_UNDEFINED;
    }

    // get the adjugate
    mat adj = adjugate(m);

    // calculate the determinant using the adjugate
    float det = 0.0f;

    // expand across first row of the matrix
    for (unsigned int c = 0; c < m->cols; c++)
    {
        if (m->elements[0][c] != 0.0f)
        {
            det += m->elements[0][c] * adj.elements[c][0]; // [c][0] because the adjugate is the transpose of the cofactor matrix
        }
    }

    if (det == 0.0f)
    {
        return MAT_UNDEFINED;
    }

    // inverse = 1 / det * adj
    mat ret = matScalarMultiplication(adj, 1.0f / det);

    freeMat(&adj);

    return ret;
}

/**
 * calculate the inverse of a matrix using Gaussian Elimination
 * @param m the matrix
 * @return the inverse, MAT_UNDEFINED if not invertible
 */
mat inverseMatRREF(mat *m)
{
    if (!invertible(m))
    {
        return MAT_UNDEFINED;
    }

    // augment the identity matrix
    mat idMat = identity(m->rows);
    mat opMat = augmentMatrix(m, &idMat);

    // row reduce the augmented matrix
    rref(&opMat);

    // take the right half of the matrix -> that is the inverse
    mat ret = allocateMat(m->rows, m->cols);

    for (unsigned int r = 0; r < ret.rows; r++)
    {
        for (unsigned int c = 0; c < ret.cols; c++)
        {
            ret.elements[r][c] = opMat.elements[r][ret.cols + c];
        }
    }

    freeMat(&idMat);
    freeMat(&opMat);

    return ret;
}