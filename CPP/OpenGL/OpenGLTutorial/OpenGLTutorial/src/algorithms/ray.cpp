#include "ray.h"

#include "../linalg.h"
#include <limits>

Ray::Ray(glm::vec3 origin, glm::vec3 dir)
	: origin(origin), dir(dir), invdir(1.0f) {
	for (int i = 0; i < 3; i++) {
		invdir[i] /= dir[i];
	}
}

bool Ray::intersectsBoundingRegion(BoundingRegion br, float& tmin, float& tmax) {
	if (br.type == BoundTypes::AABB) {
		// slab algorithm
		tmin = std::numeric_limits<float>::lowest(); // maxOfMin
		tmax = std::numeric_limits<float>::max(); // minOfMax

		for (int i = 0; i < 3; i++) {
			float t1 = (br.min[i] - origin[i]) * invdir[i];
			float t2 = (br.max[i] - origin[i]) * invdir[i];

			tmin = std::fmaxf(tmin, std::fminf(t1, t2));
			tmax = std::fminf(tmax, std::fmaxf(t1, t2));
		}

		return (tmax >= tmin) && tmax >= 0.0f;
	}
	else {
		// ray-sphere collision
		// plug in line equation of ray into sphere equation
		// get a quadratic equation for t, solve for roots

		glm::vec3 cp = origin - br.center; // vector from center of sphere to ray origin
	
		// calculate coefficients of the quadratic
		float a = magsq(dir);
		float b = 2.0f * glm::dot(dir, cp);
		float c = magsq(cp) - br.radius * br.radius;

		// get discriminant
		float D = b * b - 4.0f * a * c;
		if (D < 0) {
			// no real root
			return false;
		}
		D = sqrtf(D);

		// get two roots using quadratic formula
		tmax = (-b + D) / (2.0f * a);
		tmin = (-b - D) / (2.0f * a);

		return true;
	}
}

bool Ray::intersectsMesh(CollisionMesh* mesh, RigidBody* rb, float& t) {
	bool intersects = false;

	for (Face& f : mesh->faces) {
		float tmp = -1.0f;
		glm::vec3 P1 = mat4vec3mult(rb->model, mesh->points[f.i1]);
		glm::vec3 P2 = mat4vec3mult(rb->model, mesh->points[f.i2]) - P1;
		glm::vec3 P3 = mat4vec3mult(rb->model, mesh->points[f.i3]) - P1;
		glm::vec3 norm = rb->normalModel * f.norm;

		glm::vec3 U1 = origin - P1;

		LinePlaneIntCase intCase = linePlaneIntersection(glm::vec3(0.0f), norm, U1, dir, tmp);
	
		if ((char)intCase > 1) {
			// intersection with the inifinite plane at one point
			if (tmp < 0.0f || t < tmp) {
				// collision happens behind the origin
				// or have found a closer collision

				continue;
			}

			// get point of intersection
			glm::vec3 intersection = U1 + tmp * dir;

			if (faceContainsPoint(P2, P3, norm, intersection)) {
				intersects = true;
				t = fminf(t, tmp);
			}
		}
	}

	return intersects;
}