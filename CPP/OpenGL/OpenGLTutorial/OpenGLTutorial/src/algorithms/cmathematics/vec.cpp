#include "vec.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

const vec VEC_UNDEFINED = {0, NULL}; // undefined vector (no dimension)

/**
 * allocate memory for a vector
 * @param dim the dimension of the vector
 * @return the vector
 */
vec allocateVec(unsigned int dim)
{
    vec ret;

    ret.dim = dim;
    ret.elements = (float*)malloc(dim * sizeof(float));

    return ret;
}

/**
 * construct a vector with a default value for all the values
 * @param dim the dimension of the vector
 * @param val the default value
 * @return the vector
 */
vec constructDefaultVector(unsigned int dim, float val)
{
    vec ret = allocateVec(dim);

    for (unsigned int i = 0; i < dim; i++)
    {
        ret.elements[i] = val;
    }

    return ret;
}

/**
 * construct a vector with a default value of 0
 * @param dim the dimension of the vector
 * @return the vector
 */
vec constructEmptyVector(unsigned int dim)
{
    return constructDefaultVector(dim, 0.0f);
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
vec newVector(unsigned int dim, ...)
{
    vec ret = allocateVec(dim);

    va_list list;
    va_start(list, dim);

    for (int i = 0; i < dim; i++)
    {
        ret.elements[i] = va_arg(list, double);
    }

    va_end(list);

    return ret;
}

/**
 * copies a vector's values
 * @param v the pointer to the vector to be copied
 * @return the copy
 */
vec copyVec(vec *v)
{
    vec ret = allocateVec(v->dim);

    for (unsigned int i = 0; i < ret.dim; i++)
    {
        ret.elements[i] = v->elements[i];
    }

    return ret;
}

/**
 * prints the values of a vector
 * @param v the vector
 */
void printVec(vec v)
{
    printf("[ ");

    for (unsigned int i = 0; i < v.dim; i++)
    {
        printf("%f", v.elements[i]);

        if (i < v.dim - 1)
        {
            printf(", ");
        }
        else
        {
            printf(" ");
        }
    }

    printf("]\n");
}

/**
 * tests for equality between two vectors
 * @param v1 the first vector
 * @param v2 the second vector
 * @return true if the vectors have same dimension and equal corresponding values
 */
bool vecEquals(vec v1, vec v2)
{
    if (v1.dim == v2.dim)
    {
        for (unsigned int i = 0; i < v1.dim; i++)
        {
            if (v1.elements[i] != v2.elements[i])
            {
                return false;
            }
        }

        return true;
    }

    return false;
}

/**
 * adds a scalar to the values of a vector
 * @param v the vector
 * @param k the scalar
 * @return the resultant sum
 */
vec vecScalarAddition(vec v, float k)
{
    vec ret = allocateVec(v.dim);

    for (unsigned int i = 0; i < ret.dim; i++)
    {
        ret.elements[i] = v.elements[i] + k;
    }

    return ret;
}

/**
 * directly adds a scalar to the values of a vector
 * @param v the pointer to the vector
 * @param k the scalar
 */
void vecScalarAdditionTo(vec *v, float k)
{
    for (unsigned int i = 0; i < v->dim; i++)
    {
        v->elements[i] += k;
    }
}

/**
 * subtracts a scalar from the values of a vector
 * @param v the vector
 * @param k the scalar
 * @return the resultant sum
 */
vec vecScalarSubtraction(vec v, float k)
{
    vec ret = allocateVec(v.dim);

    for (unsigned int i = 0; i < ret.dim; i++)
    {
        ret.elements[i] = v.elements[i] - k;
    }

    return ret;
}

/**
 * directly subtracts a scalar from the values of a vector
 * @param v the pointer to the vector
 * @param k the scalar
 */
void vecScalarSubtractionFrom(vec *v, float k)
{
    for (unsigned int i = 0; i < v->dim; i++)
    {
        v->elements[i] -= k;
    }
}

/**
 * multiplies the values of a vector by a scalar
 * @param v the vector
 * @param k the scalar
 * @return the multiplied vector
 */
vec vecScalarMultiplication(vec v, float k)
{
    vec ret = allocateVec(v.dim);

    for (unsigned int i = 0; i < ret.dim; i++)
    {
        ret.elements[i] = v.elements[i] * k;
    }

    return ret;
}

/**
 * directly multiplies the values of a vector by a scalar
 * @param v the pointer to the vector
 * @param k the scalar
 */
void vecScalarMultiplicationBy(vec *v, float k)
{
    for (unsigned int i = 0; i < v->dim; i++)
    {
        v->elements[i] *= k;
    }
}

/**
 * divides the values of a vector by a scalar
 * @param v the vector
 * @param k the scalar
 * @return the divided vector
 */
vec vecScalarDivision(vec v, float k)
{
    vec ret = allocateVec(v.dim);

    for (unsigned int i = 0; i < ret.dim; i++)
    {
        ret.elements[i] = v.elements[i] / k;
    }

    return ret;
}

/**
 * directly divides the values of a vector by a scalar
 * @param v the pointer to the vector
 * @param k the scalar
 */
void vecScalarDivisionBy(vec *v, float k)
{
    for (unsigned int i = 0; i < v->dim; i++)
    {
        v->elements[i] /= k;
    }
}

/**
 * raises the values of a vector to a scalar
 * @param v the vector
 * @param k the scalar
 * @return the resultant vector
 */
vec vecPower(vec v, float k)
{
    vec ret = allocateVec(v.dim);

    for (unsigned int i = 0; i < ret.dim; i++)
    {
        ret.elements[i] = powf(v.elements[i], k);
    }

    return ret;
}

/**
 * directly raises the values of a vector to a power
 * @param v the pointer to the vector
 * @param k the power
 */
void vecPowerOf(vec *v, float k)
{
    for (unsigned int i = 0; i < v->dim; i++)
    {
        v->elements[i] = powf(v->elements[i], k);
    }
}

/**
 * adds the corresponding elements in two vectors
 * @param v1 the first vector
 * @param v2 the second vector
 * @return the sum of the vectors or VEC_UNDEFINED if the vectors have different dimensions
 */
vec vecAdd(vec v1, vec v2)
{
    if (v1.dim != v2.dim)
    {
        return VEC_UNDEFINED;
    }

    vec ret = allocateVec(v1.dim);

    for (unsigned int i = 0; i < ret.dim; i++)
    {
        ret.elements[i] = v1.elements[i] + v2.elements[i];
    }

    return ret;
}

/**
 * add the coresponding elements of a vector to another vector
 * @param v1 the vector to be added to
 * @param v2 the vector to be added
 * @return true if the vectors have the same dimension
 */
bool vecAddTo(vec *v1, vec v2)
{
    if (v1->dim != v2.dim)
    {
        return false;
    }

    for (unsigned int i = 0; i < v1->dim; i++)
    {
        v1->elements[i] += v2.elements[i];
    }

    return true;
}

/**
 * subtracts the corresponding elements in two vectors
 * @param v1 the first vector
 * @param v2 the second vector
 * @return the difference of the vectors or VEC_UNDEFINED if the vectors have different dimensions
 */
vec vecSubtract(vec v1, vec v2)
{
    if (v1.dim != v2.dim)
    {
        return VEC_UNDEFINED;
    }

    vec ret = allocateVec(v1.dim);

    for (unsigned int i = 0; i < ret.dim; i++)
    {
        ret.elements[i] = v1.elements[i] - v2.elements[i];
    }

    return ret;
}

/**
 * subtracts the coresponding elements of a vector from another vector
 * @param v1 the vector to be subtracted from
 * @param v2 the vector to be subtracted
 * @return true if the vectors have the same dimension
 */
bool vecSubtractFrom(vec *v1, vec v2)
{
    if (v1->dim != v2.dim)
    {
        return false;
    }

    for (unsigned int i = 0; i < v1->dim; i++)
    {
        v1->elements[i] -= v2.elements[i];
    }

    return true;
}

/**
 * multiplies the corresponding elements in two vectors
 * @param v1 the first vector
 * @param v2 the second vector
 * @return the element-wise multiplication of the vectors or VEC_UNDEFINED if the vectors have different dimensions
 */
vec vecMultiply(vec v1, vec v2)
{
    if (v1.dim != v2.dim)
    {
        return VEC_UNDEFINED;
    }

    vec ret = allocateVec(v1.dim);

    for (unsigned int i = 0; i < ret.dim; i++)
    {
        ret.elements[i] = v1.elements[i] * v2.elements[i];
    }

    return ret;
}

/**
 * multiplies the coresponding elements of a vector by another vector
 * @param v1 the vector to be multiplied by
 * @param v2 the vector to be multiplied
 * @return true if the vectors have the same dimension
 */
bool vecMultiplyBy(vec *v1, vec v2)
{
    if (v1->dim != v2.dim)
    {
        return false;
    }

    for (unsigned int i = 0; i < v1->dim; i++)
    {
        v1->elements[i] *= v2.elements[i];
    }

    return true;
}

/**
 * divides the corresponding elements in two vectors
 * @param v1 the first vector
 * @param v2 the second vector
 * @return the element-wise division of the vectors or VEC_UNDEFINED if the vectors have different dimensions
 */
vec vecDivide(vec v1, vec v2)
{
    if (v1.dim != v2.dim)
    {
        return VEC_UNDEFINED;
    }

    vec ret = allocateVec(v1.dim);

    for (unsigned int i = 0; i < ret.dim; i++)
    {
        ret.elements[i] = v1.elements[i] / v2.elements[i];
    }

    return ret;
}

/**
 * divides the coresponding elements of a vector by another vector
 * @param v1 the vector to be divided by
 * @param v2 the vector to be divided
 * @return true if the vectors have the same dimension
 */
bool vecDivideBy(vec *v1, vec v2)
{
    if (v1->dim != v2.dim)
    {
        return false;
    }

    for (unsigned int i = 0; i < v1->dim; i++)
    {
        v1->elements[i] /= v2.elements[i];
    }

    return true;
}

/**
 * calculates the dot product of two vectors
 * @param v1 the first vector
 * @param v2 the second vector
 * @return the dot product of the vectors, 0 if they have different dimensions
 */
float dot(vec v1, vec v2)
{
    float ret = 0.0f;

    if (v1.dim == v2.dim)
    {
        for (unsigned int i = 0; i < v1.dim; i++)
        {
            ret += v1.elements[i] * v2.elements[i];
        }
    }

    return ret;
}

/**
 * determines if two vectors are orthogonal
 * @param v1 the first vector
 * @param v2 the second vector
 * @return true if the vectors have the same dimension and the dot product is zero
 */
bool orthogonal(vec v1, vec v2)
{
    return v1.dim == v2.dim ? dot(v1, v2) == 0.0f : false;
}

/**
 * calculates the cross product of two vectors in three dimensions
 * cross product = vector perpendicular to both vectors
 * @param v1 the first vector
 * @param v2 the second vector
 * @return the cross product of the two vectors, VEC_UNDEFINED if the vectors are not in 3D
 */
vec cross(vec v1, vec v2)
{
    if (v1.dim != 3 || v2.dim != 3)
    {
        return VEC_UNDEFINED;
    }

    vec ret = allocateVec(3);

    ret.elements[0] = (v1.elements[1] * v2.elements[2]) - (v1.elements[2] * v2.elements[1]);
    ret.elements[1] = -1 * ((v1.elements[0] * v2.elements[2]) - (v1.elements[2] * v2.elements[0]));
    ret.elements[2] = (v1.elements[0] * v2.elements[1]) - (v1.elements[1] * v2.elements[0]);

    return ret;
}

/**
 * calculates the magnitude of the vector
 * @param v the vector
 * @return the magnitude (square root of the sum of the squares of the components)
 */
float vecMagnitude(vec v)
{
    return sqrt(vecMagnitudeSquared(v));
}

/**
 * calculates the square of the magnitude of the vector
 * @param v the vector
 * @return the magnitude squared (sum of the squares of the components)
 */
float vecMagnitudeSquared(vec v)
{
    float ret = 0.0f;

    for (unsigned int i = 0; i < v.dim; i++)
    {
        ret += v.elements[i] * v.elements[i];
    }

    return ret;
}

/**
 * calculates the unit (normal) vector for a vector
 * normalized = divide each component by the magnitude
 * @param v the vector
 * @return the normal vector
 */
vec vecNormalized(vec v)
{
    vec ret = allocateVec(v.dim);
    float mag = vecMagnitude(v);

    for (unsigned int i = 0; i < ret.dim; i++)
    {
        ret.elements[i] = v.elements[i] / mag;
    }

    return ret;
}

/**
 * normalizes the vector
 * @param v the pointer to the vector to be normalized
 */
void vecNormalize(vec *v)
{
    float mag = vecMagnitude(*v);

    for (unsigned int i = 0; i < v->dim; i++)
    {
        v->elements[i] /= mag;
    }
}