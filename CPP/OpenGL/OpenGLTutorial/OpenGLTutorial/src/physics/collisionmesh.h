#ifndef COLLISIONMESH_H
#define COLLISIONMESH_H

#include <vector>

#include "../algorithms/bounds.h"

// forward declarations
class CollisionModel;
class CollisionMesh;
class RigidBody;

typedef struct Face {
	CollisionMesh* mesh;
	unsigned int i1, i2, i3;

	glm::vec3 baseNormal;
	glm::vec3 norm;

	bool collidesWithFace(RigidBody* thisRB, struct Face& face, RigidBody* faceRB);
	bool collidesWithSphere(RigidBody* thisRB, BoundingRegion& br);
} Face;

class CollisionMesh {
public:
	CollisionModel* model;
	BoundingRegion br;

	std::vector<glm::vec3> points;
	std::vector<Face> faces;

	CollisionMesh(unsigned int noPoints, float* coordinates, unsigned int noFaces, unsigned int* indices);
};

#endif