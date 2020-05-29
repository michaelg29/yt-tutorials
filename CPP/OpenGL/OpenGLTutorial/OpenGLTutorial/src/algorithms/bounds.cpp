#include "bounds.h"

/*
	Constructors
*/

// initialize with type
BoundingRegion::BoundingRegion(BoundTypes type) 
	: type(type) {}

// initialize as sphere
BoundingRegion::BoundingRegion(glm::vec3 center, float radius) 
	: type(BoundTypes::SPHERE), center(center), radius(radius) {}

// initialize as AABB
BoundingRegion::BoundingRegion(glm::vec3 min, glm::vec3 max) 
	: type(BoundTypes::AABB), min(min), max(max) {}

/*
	Calculation methods
*/

// method to calculate center of region
glm::vec3 BoundingRegion::calculateCenter() {
	// if box return average, if sphere return center
	return (type == BoundTypes::AABB) ? (min + max) / 2.0f : center;
}

// method to calculate dimesnions of region
glm::vec3 BoundingRegion::calculateDimensions() {
	// if box return difference, if sphere return diameter
	return (type == BoundTypes::AABB) ? abs(max - min) : glm::vec3(2 * radius);
}

/*
	Testing methods (contains, intersects, etc)
*/

// method to determine if point is inside region
bool BoundingRegion::containsPoint(glm::vec3 pt) {
	if (type == BoundTypes::AABB) {
		// box - must be more than min and less than max
		// pt[0] = pt.x ...
		for (int i = 0; i < 3; i++) {
			if (pt[i] < min[i] || pt[i] > max[i]) {
				return false;
			}
		}

		// in between values
		return true;
	}
	else {
		// sphere - distance must be less than the radius
		// x^2 + y^2 + z^2 = r^2
		// if <= r^2 --> within sphere
		float dist = 0.0f;
		for (int i = 0; i < 3; i++) {
			dist += (pt[i] - center[i]) * (pt[i] - center[i]);
		}
		return dist <= (radius * radius);
	}
}

// method to determine if the bounding region is entirely inside
bool BoundingRegion::containsRegion(BoundingRegion br) {
	if (br.type == BoundTypes::AABB) {
		// if br is box, just has to contain min and max
		return containsPoint(br.min) && containsPoint(br.max);
	}
	else if (type == BoundTypes::SPHERE && br.type == BoundTypes::SPHERE) {
		// if both spheres
		// combination of distance between centers and other radius less than this radius
		return glm::length(center - br.center) + br.radius <= radius;
	}
	else {
		// if this is a box and br is a sphere
		if (!containsPoint(br.center)) {
			// center is outside of box
			return false;
		}

		// center is inside box
		/*
			for each axis
			- if distance to each side is smaller than the radius, return false
		*/
		for (int i = 0; i < 3; i++) {
			if (abs(max[i] - br.center[i]) < br.radius ||
				abs(br.center[i] - min[i]) < br.radius) {
				return false;
			}
		}

		return true;
	}
}

// method to determine if the regions intersect (ie collides)
bool BoundingRegion::intersectsWith(BoundingRegion br) {
	if (type == BoundTypes::AABB && br.type == BoundTypes::AABB) {
		// both boxes
		// overlap on all 3 axes

		glm::vec3 rad = calculateDimensions() / 2.0f;		// "radius" of this box
		glm::vec3 rad_br = br.calculateDimensions() / 2.0f;	// "radius" of the other box

		glm::vec3 center = calculateCenter();				// center of this box
		glm::vec3 center_br = br.calculateCenter();			// center of the other box

		glm::vec3 dist = abs(center - center_br);

		for (int i = 0; i < 3; i++) {
			if (dist[i] > (rad[i] + rad_br[i])) {
				// no overlap on this axis
				return false;
			}
		}

		return true;
	}
	else if (type == BoundTypes::SPHERE && br.type == BoundTypes::SPHERE) {
		// both sphere

		// distance between the centers must be less than the combined radii
		return (glm::length(center - br.center) < (radius + br.radius));
	}
	else if (type == BoundTypes::SPHERE) {
		// this is a sphere, br is a box

		// determine if this is above top, below the bottom, etc
		// ie sphere's relative position to box

		// find distance (squared) to each closest plane
		float distSquared = 0.0f;
		for (int i = 0; i < 3; i++) {
			if (center[i] < br.min[i]) {
				// beyond min
				distSquared += (br.min[i] - center[i]) * (br.min[i] - center[i]);
			}
			else if (center[i] > br.max[i]) {
				// beyond max
				distSquared += (center[i] - br.max[i]) * (center[i] - br.max[i]);
			}
			// else inside
		}

		return distSquared < (radius * radius);
	}
	else {
		// this is a box, br is a sphere
		// call algorithm for br (defined in reverse in preceding else if block)
		return br.intersectsWith(BoundingRegion(min, max));
	}
}