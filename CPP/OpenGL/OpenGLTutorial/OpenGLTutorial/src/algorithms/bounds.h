#ifndef BOUNDS_H
#define BOUNDS_H

#include <glm/glm.hpp>

enum class BoundTypes : unsigned char {
	AABB = 0x00,	// 0x00 = 0	// Axis-aligned bounding box
	SPHERE = 0x01	// 0x01 = 1
};

class BoundingRegion {
public:
	BoundTypes type;

	// sphere values
	glm::vec3 center;
	float radius;

	// bounding box values
	glm::vec3 min;
	glm::vec3 max;

	/*
		Constructors
	*/

	// initialize with type
	BoundingRegion(BoundTypes type);

	// initialize as sphere
	BoundingRegion(glm::vec3 center, float radius);

	// initialize as AABB
	BoundingRegion(glm::vec3 min, glm::vec3 max);

	/*
		Calculating values for the region
	*/

	// center
	glm::vec3 calculateCenter();

	// calculate dimensions
	glm::vec3 calculateDimensions();

	/*
		testing methods
	*/

	// determine if point inside
	bool containsPoint(glm::vec3 pt);

	// determine if region completely inside
	bool containsRegion(BoundingRegion br);

	// determine if region intersects (partial containment)
	bool intersectsWith(BoundingRegion br);
};

#endif