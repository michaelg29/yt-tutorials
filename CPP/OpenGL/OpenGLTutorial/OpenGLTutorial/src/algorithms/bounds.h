#ifndef BOUNDS_H
#define BOUNDS_H

#include <glm/glm.hpp>

enum class BoundTypes {
	AABB, // axis-aligned bounding box
	SPHERE
};

class BoundingRegion {
public:
	BoundTypes type;

	// sphere values
	glm::vec3 center;
	float radius;

	// AABB values
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
		Calculation methods
	*/

	// method to calculate center of region
	glm::vec3 calculateCenter();

	// method to calculate dimesnions of region
	glm::vec3 calculateDimensions();

	/*
		Testing methods (contains, intersects, etc)
	*/

	// method to determine if point is inside region
	bool containsPoint(glm::vec3 pt);

	// method to determine if the bounding region is entirely inside
	bool containsRegion(BoundingRegion br);

	// method to determine if the regions intersect
	bool intersectsWith(BoundingRegion br);
};

#endif