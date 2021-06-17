#include "collisionmesh.h"
#include "collisionmodel.h"
#include "rigidbody.h"

#include <limits>

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

char linePlaneIntersection(glm::vec3 P1, glm::vec3 norm, glm::vec3 U1, glm::vec3 side, float& t) {
	// calculate the parameter t of the line { U1 + side * t } at the point of intersection
	/*
		t = (N dot U1P1) / (N dot U1U2)
	*/
	glm::vec3 U1P1 = P1 - U1;

	float tnum = glm::dot(norm, U1P1);
	float tden = glm::dot(norm, side);

	if (tden == 0.0f) {
		return tnum == 0.0f ? CASE0 : CASE1;
	}
	else {
		// can do division
		t = tnum / tden;
		return t >= 0.0f && t <= 1.0f ? CASE2 : CASE3;
	}
}

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
	rref(m);

	return m[3];
}

bool faceContainsPointRange(glm::vec3 A, glm::vec3 B, glm::vec3 N, glm::vec3 point, float radius) {
	glm::vec3 c = linCombSolution(A, B, N, point);

	return c[0] >= -radius && c[1] >= -radius && c[0] + c[1] <= 1.0f + radius;
}

bool faceContainsPoint(glm::vec3 A, glm::vec3 B, glm::vec3 N, glm::vec3 point) {
	return faceContainsPointRange(A, B, N, point, 0.0f);
}

bool Face::collidesWithFace(RigidBody* thisRB, Face& face, RigidBody* faceRB, glm::vec3& retNorm) {
	// transform coordinates so that P1 is the origin
	glm::vec3 P1 = mat4vec3mult(thisRB->model, this->mesh->points[this->i1]);
	glm::vec3 P2 = mat4vec3mult(thisRB->model, this->mesh->points[this->i2]) - P1;
	glm::vec3 P3 = mat4vec3mult(thisRB->model, this->mesh->points[this->i3]) - P1;
	glm::vec3 lines[3] = {
		P2,
		P3,
		P3 - P2
	};

	glm::vec3 thisNorm = thisRB->normalModel * this->norm;

	glm::vec3 U1 = mat4vec3mult(faceRB->model, face.mesh->points[face.i1]) - P1;
	glm::vec3 U2 = mat4vec3mult(faceRB->model, face.mesh->points[face.i2]) - P1;
	glm::vec3 U3 = mat4vec3mult(faceRB->model, face.mesh->points[face.i3]) - P1;

	retNorm = faceRB->normalModel * face.norm;

	// set P1 as the origin
	P1[0] = 0.0f; P1[1] = 0.0f; P1[2] = 0.0f;

	// placeholders
	float c1, c2, c3;

	/*
		iterate through each bounding line of the face
	*/
	glm::vec3 sideOrigins[3] = {
		U1,
		U1,
		U2
	};
	glm::vec3 sides[3] = {
		U2 - U1,
		U3 - U1,
		U3 - U2
	};
	for (unsigned int i = 0; i < 3; i++) {
		// get intersection with the plane
		float t = 0.0f;

		char currentCase = linePlaneIntersection(P1, thisNorm,
			sideOrigins[i], sides[i],
			t);

		switch (currentCase) {
		case CASE0:
			// determine intersection with the lines of this face

			for (int j = 0; j < 3; j++) {
				glm::mat3 m(lines[j], -1.0f * sides[i], sideOrigins[i]);

				rref(m);

				if (m[2][2] != 0.0f) {
					// no intersection
					continue;
				}

				c1 = m[2][0];
				c2 = m[2][1];

				if (0.0f <= c1 && c1 <= 1.0f &&
					0.0f <= c2 && c2 <= 1.0f) {
					return true;
				}
			}

			return false;
		case CASE1:
			// no intersection, no collision
			continue;
		case CASE2:
			// intersection in the plane in range, determine if inside the triangle of this face

			// get point of intersection
			glm::vec3 intersection = sideOrigins[i] + t * sides[i];

			if (faceContainsPoint(P2, P3, thisNorm, intersection)) {
				return true;
			}

			continue;
		case CASE3:
			// intersection in the plane out of range, no collision
			continue;
		};
	}

	return false;
}

bool Face::collidesWithSphere(RigidBody* thisRB, BoundingRegion& br, glm::vec3& retNorm) {
	if (br.type != BoundTypes::SPHERE) {
		return false;
	}

	// apply model transformations
	glm::vec3 P1 = mat4vec3mult(thisRB->model, this->mesh->points[i1]);
	glm::vec3 P2 = mat4vec3mult(thisRB->model, this->mesh->points[i2]);
	glm::vec3 P3 = mat4vec3mult(thisRB->model, this->mesh->points[i3]);

	glm::vec3 norm = thisRB->normalModel * this->norm;
	glm::vec3 unitN = norm / glm::length(norm);

	glm::vec3 distanceVec = br.center - P1;
	float distance = glm::dot(distanceVec, unitN);

	if (abs(distance) < br.radius) {
		glm::vec3 circCenter = br.center + distance * unitN;

		retNorm = unitN;

		return faceContainsPointRange(P2 - P1, P3 - P1, norm, circCenter - P1, br.radius);
	}

	return false;
}

CollisionMesh::CollisionMesh(unsigned int noPoints, float* coordinates,
	unsigned int noFaces, unsigned int* indices)
	: points(noPoints), faces(noFaces) {
	glm::vec3 min(std::numeric_limits<float>::infinity());
	glm::vec3 max = -1.0f * min;

	// insert all points into list
	for (unsigned int i = 0; i < noPoints; i++) {
		points[i] = { coordinates[i * 3 + 0], coordinates[i * 3 + 1], coordinates[i * 3 + 2] };

		for (int j = 0; j < 3; j++) {
			if (points[i][j] < min[j]) {
				// found a new minimum
				min[j] = points[i][j];
			}
			if (points[i][j] > max[j]) {
				// found a new maximum
				max[j] = points[i][j];
			}
		}
	}

	glm::vec3 center = (min + max) / 2.0f;

	float maxRadiusSquared = 0.0f;
	for (unsigned int i = 0; i < noPoints; i++) {
		float radiusSquared = 0.0f;
		for (int j = 0; j < 3; j++) {
			radiusSquared += (points[i][j] - center[j]) * (points[i][j] - center[j]);
		}
		if (radiusSquared > maxRadiusSquared) {
			maxRadiusSquared = radiusSquared;
		}
	}

	this->br = BoundingRegion(center, sqrt(maxRadiusSquared));
	this->br.collisionMesh = this;

	// calculate face normals
	for (unsigned int i = 0; i < noFaces; i++) {
		unsigned int i1 = indices[i * 3 + 0];
		unsigned int i2 = indices[i * 3 + 1];
		unsigned int i3 = indices[i * 3 + 2];

		glm::vec3 A = points[i2] - points[i1]; // A = P2 - P1
		glm::vec3 B = points[i3] - points[i1]; // B = P3 - P1
		glm::vec3 N = glm::cross(A, B); // N = A x B

		faces[i] = {
			this,
			i1, i2, i3,	// indices making up the triangle
			N,			// normal placeholder
			N			// normal placeholder
		};
	}
}