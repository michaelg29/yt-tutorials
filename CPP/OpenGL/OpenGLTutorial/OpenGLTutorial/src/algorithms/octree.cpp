#include "octree.h"

// calculate bounds of specified quadrant in bounding region
void Octree::calculateBounds(BoundingRegion &out, Octant octant, BoundingRegion parentRegion) {
    // find min and max points of corresponding octant
    
    glm::vec3 center = parentRegion.calculateCenter();
    if (octant == Octant::O1) {
        out = BoundingRegion(center, parentRegion.max);
    }
    else if (octant == Octant::O2) {
        out = BoundingRegion(glm::vec3(parentRegion.min.x, center.y, center.z), glm::vec3(center.x, parentRegion.max.y, parentRegion.max.z));
    }
    else if (octant == Octant::O3) {
        out = BoundingRegion(glm::vec3(parentRegion.min.x, parentRegion.min.y, center.z), glm::vec3(center.x, center.y, parentRegion.max.z));
    }
    else if (octant == Octant::O4) {
        out = BoundingRegion(glm::vec3(center.x, parentRegion.min.y, center.z), glm::vec3(parentRegion.max.x, center.y, parentRegion.max.z));
    }
    else if (octant == Octant::O5) {
        out = BoundingRegion(glm::vec3(center.x, center.y, parentRegion.min.z), glm::vec3(parentRegion.max.x, parentRegion.max.y, center.z));
    }
    else if (octant == Octant::O6) {
        out = BoundingRegion(glm::vec3(parentRegion.min.x, center.y, parentRegion.min.z), glm::vec3(center.x, parentRegion.max.y, center.z));
    }
    else if (octant == Octant::O7) {
        out = BoundingRegion(parentRegion.min, center);
    }
    else if (octant == Octant::O8) {
        out = BoundingRegion(glm::vec3(center.x, parentRegion.min.y, parentRegion.min.z), glm::vec3(parentRegion.max.x, center.y, center.z));
    }
}

// default constructor
Octree::node::node()
    : region(BoundTypes::AABB) {}

// initialize with bounds (no objects yet)
Octree::node::node(BoundingRegion bounds)
    : region(bounds) {}

// initialize with bounds and list of objects
Octree::node::node(BoundingRegion bounds, std::vector<BoundingRegion> objectList)
    : region(bounds) {
    objects.insert(objects.end(), objectList.begin(), objectList.end());
}

void Octree::node::addToPending(RigidBody* instance, trie::Trie<Model*> models) {
    // get all bounding regions of model
    for (BoundingRegion br : models[instance->modelId]->boundingRegions) {
        br.instance = instance;
        br.transform();
        queue.push(br);
    }
}

// build tree (called during initialization)
void Octree::node::build() {
    /*
        termination conditions (don't subdivide further)
        - 1 or less objects (ie an empty leaf node or node with 1 object)
        - dimesnions are too small
    */

    // <= 1 objects
    if (objects.size() <= 1) {
        // set state variables
        treeBuilt = true;
        treeReady = true;
        return;
    }

    // too small
    glm::vec3 dimensions = region.calculateDimensions();
    for (int i = 0; i < 3; i++) {
        if (dimensions[i] < MIN_BOUNDS) {
            // set state variables
            treeBuilt = true;
            treeReady = true;
            return;
        }
    }

    // create regions
    BoundingRegion octants[NO_CHILDREN];
    for (int i = 0; i < NO_CHILDREN; i++) {
        calculateBounds(octants[i], (Octant)(1 << i), region);
    }

    // determine which octants to place objects in
    std::vector<BoundingRegion> octLists[NO_CHILDREN]; // array of lists of objects in each octant
    std::stack<int> delList; // list of objects that have been placed

    for (int i = 0, length = objects.size(); i < length; i++) {
        BoundingRegion br = objects[i];
        for (int j = 0; j < NO_CHILDREN; j++) {
            if (octants[j].containsRegion(br)) {
                // octant contains region
                octLists[j].push_back(br);
                delList.push(i);
                break;
            }
        }
    }

    // remove objects on delList
    while (delList.size() != 0) {
        objects.erase(objects.begin() + delList.top());
        delList.pop();
    }

    // populate octants
    for (int i = 0; i < NO_CHILDREN; i++) {
        if (octLists[i].size() != 0) {
            // if children go into this octant
            children[i] = new node(octants[i], octLists[i]);
            States::activateIndex(&activeOctants, i); // activate octant
            children[i]->parent = this;
            children[i]->build();
            hasChildren = true;
        }
    }

    // set state variables
    treeBuilt = true;
    treeReady = true;
}

// update objects in tree (called during each iteration of main loop)
void Octree::node::update(Box &box) {
    if (treeBuilt && treeReady) {
        box.positions.push_back(region.calculateCenter());
        box.sizes.push_back(region.calculateDimensions());

        // countdown timer
        if (objects.size() == 0) {
            if (!hasChildren) {
                // ensure no child leaves
                if (currentLifespan == -1) {
                    // initial check
                    currentLifespan = maxLifespan;
                }
                else if (currentLifespan > 0) {
                    // decrement
                    currentLifespan--;
                }
            }
        }
        else {
            if (currentLifespan != -1) {
                if (maxLifespan <= 64) {
                    // extend lifespan because "hotspot"
                    maxLifespan <<= 2;
                }
            }
        }

        // remove objects that don't exist anymore

        for (int i = 0, listSize = objects.size(); i < listSize; i++) {
            // remove if on list of dead objects
            if (States::isActive(&objects[i].instance->state, INSTANCE_DEAD)) {
                objects.erase(objects.begin() + i);
                // update counter/size accordingly
                i--;
                listSize--;
            }
        }

        // get moved objects that were in this leaf in previous frame
        std::stack<std::pair<int, BoundingRegion>> movedObjects;
        for (int i = 0, listSize = objects.size(); i < listSize; i++) {
            if (States::isActive(&objects[i].instance->state, INSTANCE_MOVED)) {
                objects[i].transform();
                movedObjects.push({ i, objects[i] });
            }
            box.positions.push_back(objects[i].calculateCenter());
            box.sizes.push_back(objects[i].calculateDimensions());
        }

        // remove dead branches
        unsigned char flags = activeOctants;
        for (int i = 0;
            flags > 0;
            flags >>= 1, i++) {
            if (States::isIndexActive(&flags, 0) && children[i]->currentLifespan == 0) {
                // active and run out of time
                if (children[i]->objects.size() > 0) {
                    // branch is dead but has children, so reset
                    children[i]->currentLifespan = -1;
                }
                else {
                    // branch is dead
                    children[i] = nullptr;
                    States::deactivateIndex(&activeOctants, i);
                }
            }
        }

        // update child nodes
        if (children != nullptr) {
            // go through each octant using flags
            for (unsigned char flags = activeOctants, i = 0;
                flags > 0;
                flags >>= 1, i++) {
                if (States::isIndexActive(&flags, 0)) {
                    // active octant
                    if (children[i] != nullptr) {
                        // child not null
                        children[i]->update(box);
                    }
                }
            }
        }
        
        // move moved objects into new nodes
        BoundingRegion movedObj; // placeholder
        while (movedObjects.size() != 0) {
            /*
                for each moved object
                - traverse up tree (start with current node) until find a node that completely encloses the object
                - call insert (push object as far down as possible)
            */

            movedObj = movedObjects.top().second; // set to top object in stack
            node* current = this; // placeholder

            while (!current->region.containsRegion(movedObj)) {
                if (current->parent != nullptr) {
                    // set current to current's parent (recursion)
                    current = current->parent;
                }
                else {
                    break; // if root node, the leave
                }
            }

            /*
                once finished
                - remove from objects list
                - remove from movedObjects stack
                - insert into found region
            */
            objects.erase(objects.begin() + movedObjects.top().first);
            movedObjects.pop();
            current->insert(movedObj);

            // collision detection
            // TODO
        }
    }
    else {
        // process pending results
        if (queue.size() > 0) {
            processPending();
        }
    }
}

// process pending queue
void Octree::node::processPending() {
    if (!treeBuilt) {
        // add objects to be sorted into branches when built
        while (queue.size() != 0) {
            objects.push_back(queue.front());
            queue.pop();
        }
        build();
    }
    else {
        // insert the objects immediately
        while (queue.size() != 0) {
            insert(queue.front());
            queue.pop();
        }
    }
}

// dynamically insert object into node
bool Octree::node::insert(BoundingRegion obj) {
    /*
        termination conditions
        - no objects (an empty leaf node)
        - dimensions are less than MIN_BOUNDS
    */

    glm::vec3 dimensions = region.calculateDimensions();
    if (objects.size() == 0 ||
        dimensions.x < MIN_BOUNDS ||
        dimensions.y < MIN_BOUNDS ||
        dimensions.z < MIN_BOUNDS
        ) {
        objects.push_back(obj);
        return true;
    }

    // safeguard if object doesn't fit
    if (!region.containsRegion(obj)) {
        return parent == nullptr ? false : parent->insert(obj);
    }

    // create regions if not defined
    BoundingRegion octants[NO_CHILDREN];
    for (int i = 0; i < NO_CHILDREN; i++) {
        if (children[i] != nullptr) {
            // child exists, so take its region
            octants[i] = children[i]->region;
        }
        else {
            // get region for this octant
            calculateBounds(octants[i], (Octant)(1 << i), region);
        }
    }

    //std::vector<BoundingRegion> octLists[NO_CHILDREN];
    //objects.push_back(obj);

    //for (int i = objects.size() - 1; i >= 0; i--) {
    //    for (int j = 0; j < NO_CHILDREN; j++) {
    //        if (octants[j].containsRegion(objects[i])) {
    //            // octant contains region
    //            octLists[j].push_back(objects[i]);
    //            objects.erase(objects.begin() + i);
    //            break;
    //        }
    //    }
    //}

    //// populate octants
    //for (int i = 0; i < NO_CHILDREN; i++) {
    //    if (octLists[i].size() != 0) {
    //        // children go into octant
    //        if (children[i]) {
    //            for (int j = 0, length = octLists[i].size(); i < length; i++) {
    //                children[i]->insert(octLists[i][j]);
    //            }
    //        }
    //        else {
    //            // create new node
    //            if (Keyboard::key(GLFW_KEY_F)) {
    //                std::cout << "";
    //            }
    //            children[i] = new node(octants[i], octLists[i]);
    //            States::activateIndex(&activeOctants, i);
    //            children[i]->parent = this;
    //            children[i]->build();
    //            hasChildren = true;
    //        }
    //    }
    //}

    // find region that fits item entirely
    for (int i = 0; i < NO_CHILDREN; i++) {
        if (octants[i].containsRegion(obj)) {
            if (children[i] != nullptr) {
                // insert into existing child
                return children[i]->insert(obj);
            }
            else {
                // create node for child
                children[i] = new node(octants[i], { obj });
                States::activateIndex(&activeOctants, i);
                children[i]->parent = this;
                children[i]->build();
                hasChildren = true;
                return true;
            }
        }
    }

    // doesn't fit into children
    objects.push_back(obj);
    return true;
}

// destroy object (free memory)
void Octree::node::destroy() {
    // clearing out children
    if (children != nullptr) {
        for (int flags = activeOctants, i = 0;
            flags > 0;
            flags >> 1, i++) {
            if (States::isIndexActive(&flags, 0)) {
                // active
                if (children[i] != nullptr) {
                    children[i]->destroy();
                    children[i] = nullptr;
                }
            }
        }
    }

    // clear this node
    objects.clear();
    while (queue.size() != 0) {
        queue.pop();
    }
}