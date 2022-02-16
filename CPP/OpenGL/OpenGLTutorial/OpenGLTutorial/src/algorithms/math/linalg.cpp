#include "linalg.h"

#include <limits>
#include <glm/glm.hpp>

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

LinePlaneIntCase linePlaneIntersection(glm::vec3 P1, glm::vec3 norm, glm::vec3 U1, glm::vec3 side, float& t) {
    // calculate the parameter t of the line { U1 + side * t } at the point of intersection
    /*
        t = (N dot U1P1) / (N dot U1U2)
    */
    glm::vec3 U1P1 = P1 - U1;

    float tnum = glm::dot(norm, U1P1);
    float tden = glm::dot(norm, side);

    if (tden == 0.0f) {
        return tnum == 0.0f ? LinePlaneIntCase::CASE0 : LinePlaneIntCase::CASE1;
    }
    else {
        // can do division
        t = tnum / tden;
        return t >= 0.0f && t <= 1.0f ? LinePlaneIntCase::CASE2 : LinePlaneIntCase::CASE3;
    }
}

glm::vec3 mat4vec3mult(glm::mat4& m, glm::vec3& v) {
    glm::vec3 ret;
    for (int i = 0; i < 3; i++) {
        ret[i] = v[0] * m[0][i] + v[1] * m[1][i] + v[2] * m[2][i] + m[3][i];
    }
    return ret;
}

glm::vec3 linCombSolution(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 point) {
    // represent the point as a linear combination of the 3 basis vectors
    glm::mat4x3 m(A, B, C, point);

    // do RREF
    rref<4, 3>(m);

    return m[3];
}

bool faceContainsPointRange(glm::vec3 A, glm::vec3 B, glm::vec3 N, glm::vec3 point, float radius) {
    glm::vec3 c = linCombSolution(A, B, N, point);

    return c[0] >= -radius && c[1] >= -radius && c[0] + c[1] <= 1.0f + radius;
}

bool faceContainsPoint(glm::vec3 A, glm::vec3 B, glm::vec3 N, glm::vec3 point) {
    return faceContainsPointRange(A, B, N, point, 0.0f);
}
