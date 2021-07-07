#ifndef OCTREE_H
#define OCTREE_H

#define NO_CHILDREN 8
#define MIN_BOUNDS 0.5

#include <vector>
#include <queue>
#include <stack>

#include "list.hpp"
#include "states.hpp"
#include "bounds.h"
#include "ray.h"

#include "../graphics/objects/model.h"

// forward declaration
class Model;
class BoundingRegion;
class Box;

/*
    namespace to tie together all classes and functions relating to octree
*/

namespace Octree {
    /*
        enum to represent octants
    */

    enum class Octant : unsigned char {
        O1 = 0x01,	// = 0b00000001
        O2 = 0x02,	// = 0b00000010
        O3 = 0x04,	// = 0b00000100
        O4 = 0x08,	// = 0b00001000
        O5 = 0x10,	// = 0b00010000
        O6 = 0x20,	// = 0b00100000
        O7 = 0x40,	// = 0b01000000
        O8 = 0x80	// = 0b10000000
    };

    /*
        utility methods callbacks
    */

    // calculate bounds of specified quadrant in bounding region
    void calculateBounds(BoundingRegion &out, Octant octant, BoundingRegion parentRegion);

    /*
        class to represent each node in the octree
    */
    class node {
    public:
        // parent pointer
        node* parent;
        // array of children (8)
        node* children[NO_CHILDREN];

        // switch for active octants
        unsigned char activeOctants;

        // if node has children
        bool hasChildren = false;

        // if tree is ready
        bool treeReady = false;
        // if tree is built
        bool treeBuilt = false;

        // maximum possible lifespan
        short maxLifespan = 8;
        // current lifespace
        short currentLifespan = -1;

        // list of objects in node
        std::vector<BoundingRegion> objects;
        // queue of objects to be dynamically inserted
        std::queue<BoundingRegion> queue;

        // region of bounds of cell (AABB)
        BoundingRegion region;

        /*
            constructors
        */

        // default
        node();
        
        // initialize with bounds (no objects yet)
        node(BoundingRegion bounds);

        // initialize with bounds and list of objects
        node(BoundingRegion bounds, std::vector<BoundingRegion> objectList);

        /*
            functionality
        */

        // add instance to pending queue
        void addToPending(RigidBody* instance, Model *model);

        // build tree (called during initialization)
        void build();

        // update objects in tree (called during each iteration of main loop)
        void update(Box &box);

        // process pending queue
        void processPending();

        // dynamically insert object into node
        bool insert(BoundingRegion obj);

        // check collisions with all objects in node
        void checkCollisionsSelf(BoundingRegion obj);

        // check collisions with all objects in child nodes
        void checkCollisionsChildren(BoundingRegion obj);

        // check collisions with a ray
        BoundingRegion* checkCollisionsRay(Ray r, float& tmin);

        // destroy object (free memory)
        void destroy();
    };
}

#endif