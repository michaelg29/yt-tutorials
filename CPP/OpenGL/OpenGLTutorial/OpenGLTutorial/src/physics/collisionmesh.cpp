#include "collisionmesh.h"
#include "collisionmodel.h"
#include "rigidbody.h"

#include "../algorithms/math/linalg.h"

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

		LinePlaneIntCase currentCase = linePlaneIntersection(P1, thisNorm,
			sideOrigins[i], sides[i],
			t);

		switch (currentCase) {
		case LinePlaneIntCase::CASE0:
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
		case LinePlaneIntCase::CASE1:
			// no intersection, no collision
			continue;
		case LinePlaneIntCase::CASE2:
			// intersection in the plane in range, determine if inside the triangle of this face

			// get point of intersection
			glm::vec3 intersection = sideOrigins[i] + t * sides[i];

			if (faceContainsPoint(P2, P3, thisNorm, intersection)) {
				return true;
			}

			continue;
		case LinePlaneIntCase::CASE3:
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