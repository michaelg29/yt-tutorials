#include <glm/glm.hpp>

#ifndef LINALG_HPP
#define LINALG_HPP

template <int D>
float magsq(glm::vec<D, float, glm::packed_highp> v) {
    float ret = 0.0f;

    for (int i = 0; i < D; i++) {
        ret += v[i] * v[i];
    }

    return ret;
}

/*
    Line-plane intersection cases
    PI is the plane containing the vectors P1P2 and P1P3

    CASE 0
        line U1U2 lies in the plane PI
        t = 0 / 0

    CASE 1
        no planar intersection
        t = (R != 0) / 0

        => intersection when
        !(tnum != 0 && tden == 0)
        = tnum == 0 || tden != 0 (DeMorgan)

    CASE 2
        planar intersection, in between U1 and U2
        t = R / (R != 0) in the range [0, 1]

    CASE 3
        planar intersection, outside of U1 and U2
        t = R / (R != 0) not in the range [0, 1]
*/

enum class LinePlaneIntCase : char {
    CASE0 = 0x00,
    CASE1 = 0x01,
    CASE2 = 0x02,
    CASE3 = 0x03
};

LinePlaneIntCase linePlaneIntersection(glm::vec3 P1, glm::vec3 norm, glm::vec3 U1, glm::vec3 side, float& t);

glm::vec3 mat4vec3mult(glm::mat4& m, glm::vec3& v);

glm::vec3 linCombSolution(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 point);

bool faceContainsPointRange(glm::vec3 A, glm::vec3 B, glm::vec3 N, glm::vec3 point, float radius);

bool faceContainsPoint(glm::vec3 A, glm::vec3 B, glm::vec3 N, glm::vec3 point);

template <int C, int R>
void rref(glm::mat<C, R, float>& m) {
    unsigned int currentRow = 0;
    for (unsigned int c = 0; c < C; c++) {
        unsigned int r = currentRow;
        if (r >= R)
        {
            // no more rows
            break;
        }

        // find nonzero entry
        for (; r < R; r++)
        {
            if (m[c][r] != 0.0f)
            {
                // non-zero value
                break;
            }
        }

        // didn't find a nonzero entry in column
        if (r == R)
        {
            continue;
        }

        // swap with proper row
        if (r != currentRow) {
            for (unsigned int i = 0; i < C; i++) {
                float tmp = m[i][currentRow];
                m[i][currentRow] = m[i][r];
                m[i][r] = tmp;
            }
        }

        // multiply row by 1 / value
        if (m[c][currentRow] != 0.0f) {
            float k = 1 / m[c][currentRow];
            m[c][currentRow] = 1.0f;
            for (unsigned int col = c + 1; col < C; col++)
            {
                m[col][currentRow] *= k;
            }
        }

        // clear out rows above and below
        for (r = 0; r < R; r++)
        {
            if (r == currentRow)
            {
                continue;
            }
            float k = -m[c][r];
            for (unsigned int i = 0; i < C; i++) {

                m[i][r] += k * m[i][currentRow];
            }
        }

        currentRow++;
    }
}

#endif