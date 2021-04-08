#ifndef BOUNDS_H
#define BOUNDS_H

#include <glm/glm.hpp>

#include "../physics/rigidbody.h"

// forward declaration
namespace Octree {
    class node;
}
class CollisionMesh;

/*
    enum for possible Bound Types
*/

enum class BoundTypes : unsigned char {
    AABB    = 0x00,	// 0x00 = 0	// Axis-aligned bounding box
    SPHERE  = 0x01	// 0x01 = 1
};

/*
    class to represent bounding region
*/

class BoundingRegion {
public:
    // type of region
    BoundTypes type;

    // pointer for quick access to instance and collision mesh
    RigidBody* instance;
    CollisionMesh* collisionMesh;

    // pointer for quick access to current octree node
    Octree::node* cell;

    // sphere values
    glm::vec3 center;
    float radius;

    glm::vec3 ogCenter;
    float ogRadius;

    // bounding box values
    glm::vec3 min;
    glm::vec3 max;

    glm::vec3 ogMin;
    glm::vec3 ogMax;

    /*
        Constructors
    */

    // initialize with type
    BoundingRegion(BoundTypes type = BoundTypes::AABB);

    // initialize as sphere
    BoundingRegion(glm::vec3 center, float radius);

    // initialize as AABB
    BoundingRegion(glm::vec3 min, glm::vec3 max);

    /*
        Calculating values for the region
    */

    // transform for instance
    void transform();

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

    // operator overload
    bool operator==(BoundingRegion br);
};

#endif