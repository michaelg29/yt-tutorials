#include "collisionmesh.h"
#include "collisionmodel.h"

#include "../algorithms/cmathematics/matrix.h"

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
#define CASE0 (char)0
#define CASE1 (char)1
#define CASE2 (char)2
#define CASE3 (char)3

char linePlaneIntersection(vec P1, vec norm, vec U1, vec side, float& t) {
	// calculate the parameter t of the line { U1 + side * t } at the point of intersection
	/*
		t = (N dot U1P1) / (N dot U1U2)
	*/
	vec U1P1 = vecSubtract(P1, U1);

	// calculate numerator and denominator
	float tnum = dot(norm, U1P1);
	float tden = dot(norm, side);

	if (tden == 0.0f) {
		return tnum == 0.0f ? CASE0 : CASE1;
	}
	else {
		// can do division
		t = tnum / tden;
		return t >= 0.0f && t <= 1.0f ? CASE2 : CASE3;
	}
}

bool Face::collidesWith(Face& face) {
	// transform coordinates so that P1 is the origin
	vec P1 = this->mesh->points[this->i1];
	vec P2 = vecSubtract(this->mesh->points[this->i2], P1);
	vec P3 = vecSubtract(this->mesh->points[this->i3], P1);
	vec lines[3] = {
		P2, // P2 - P1 = A
		P3, // P3 - P1 = B
		vecSubtract(P3, P2) // (P3 - P1) - (P2 - P1) = P3 - P2 = C
	};

	vec U1 = vecSubtract(face.mesh->points[face.i1], P1);
	vec U2 = vecSubtract(face.mesh->points[face.i2], P1);
	vec U3 = vecSubtract(face.mesh->points[face.i3], P1);
	vec sideOrigins[3] = {
		U1,
		U1,
		U2
	};
	vec sides[3] = {
		vecSubtract(U2, U1),
		vecSubtract(U3, U1),
		vecSubtract(U3, U2)
	};

	// set P1 as the origin (zero vector)
	P1.elements[0] = 0.0f; P1.elements[1] = 0.0f; P1.elements[2] = 0.0f;

	// placeholders
	float c1, c2;

	/*
		iterate through each bounding line of the face
		check intersections
	*/
	for (unsigned int i = 0; i < 3; i++) {
		// get intersection with this plane
		float t = 0;
		char currentCase = linePlaneIntersection(P1, this->norm,
												 sideOrigins[i], sides[i],
												 t);

		switch (currentCase) {
		case CASE0:
			// line in the plane
			// determine the intersection with the 3 bounding lines of this face
			for (int j = 0; j < 3; j++) {
				mat m = newColMat(3, 3, lines[j], vecScalarMultiplication(sides[i], -1.0f), sideOrigins[i]);

				// do RREF
				rref(&m);

				if (m.elements[2][2] != 0.0f) {
					// no intersection with the lines
					continue;
				}

				c1 = m.elements[0][2];
				c2 = m.elements[1][2];

				if (0.0f <= c1 && c1 <= 1.0f &&
					0.0f <= c2 && c2 <= 1.0f) {
					return true;
				}
			}
			continue;

		case CASE1:
			// no intersection => no collision
			continue;

		case CASE2:
			// intersection in the plane, in range
			// determine if inside this triangle (bounded by P1, P2, P3)

			// get point of intersection
			vec intersection = vecAdd(sideOrigins[i],
				vecScalarMultiplication(sides[i], t));

			printf("%f: ", t);
			printVec(intersection);

			// represent the intersection point as a linear combination
			mat m = newColMat(3, 4, P2, P3, this->norm, intersection);

			// do RREF
			rref(&m);

			// obtain the coefficients for the linear combination
			// c3 ~= 0.0 because the point is in the plane
			c1 = m.elements[0][3];
			c2 = m.elements[1][3];

			if (c1 >= 0.0f && c2 >= 0.0f && c1 + c2 <= 1.0f) {
				return true;
			}

			continue;

		case CASE3:
			// intersection in the plane, out of range
			// => no collision
			continue;
		}
	}

	return false;
}

CollisionMesh::CollisionMesh(unsigned int noPoints, float* coordinates,
	unsigned int noFaces, unsigned int* indices)
	: points(noPoints), faces(noFaces) {
	// insert all points into list
	for (unsigned int i = 0; i < noPoints; i++) {
		points[i] = newVector(3, coordinates[i * 3 + 0], coordinates[i * 3 + 1], coordinates[i * 3 + 2]);
	}

	// calculate face normals
	for (unsigned int i = 0; i < noFaces; i++) {
		unsigned int i1 = indices[i * 3 + 0];
		unsigned int i2 = indices[i * 3 + 1];
		unsigned int i3 = indices[i * 3 + 2];

		vec A = vecSubtract(points[i2], points[i1]); // A = P2 - P1
		vec B = vecSubtract(points[i3], points[i1]); // B = P3 - P1
		vec N = cross(A, B); // N = A x B

		faces[i] = {
			this,
			i1, i2, i3,	// indices making up the triangle
			N,			// normal placeholder
			N			// normal placeholder
		};
	}
}