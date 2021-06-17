#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <glm/glm.hpp>

#include <string>

// switches for instance states
#define INSTANCE_DEAD		(unsigned char)0b00000001
#define INSTANCE_MOVED		(unsigned char)0b00000010

#define COLLISION_THRESHOLD 0.05f

/*
    Rigid Body class
    - represents physical body and holds all parameters
*/

class RigidBody {
public:
    // combination of switches above
    unsigned char state;

    // mass in kg
    float mass;

    // position in m
    glm::vec3 pos;
    // velocity in m/s
    glm::vec3 velocity;
    // acceleration in m/s^2
    glm::vec3 acceleration;

    // dimensions of object
    glm::vec3 size;

    // rotation in Euler angles
    glm::vec3 rot;

    // model matrix
    glm::mat4 model;
    glm::mat3 normalModel;

    // ids for quick access to instance/model
    std::string modelId;
    std::string instanceId;

    // data of previous collision
    float lastCollision;
    std::string lastCollisionID;

    // test for equivalence of two rigid bodies
    bool operator==(RigidBody rb);
    bool operator==(std::string id);

    /*
        constructor
    */

    // construct with parameters and default
    RigidBody(std::string modelId = "",
        glm::vec3 size = glm::vec3(1.0f),
        float mass = 1.0f,
        glm::vec3 pos = glm::vec3(0.0f),
        glm::vec3 rot = glm::vec3(0.0f));

    /*
        transformation functions
    */

    // update position with velocity and acceleration
    void update(float dt);

    // apply a force
    void applyForce(glm::vec3 force);
    void applyForce(glm::vec3 direction, float magnitude);

    // apply an acceleration (remove redundancy of dividing by mass)
    void applyAcceleration(glm::vec3 acceleration);
    void applyAcceleration(glm::vec3 direction, float magnitude);

    // apply force over time
    void applyImpulse(glm::vec3 force, float dt);
    void applyImpulse(glm::vec3 direction, float magnitude, float dt);

    // transfer potential or kinetic energy from another object
    void transferEnergy(float joules, glm::vec3 direction);

    /*
        collisions
    */
    void handleCollision(RigidBody* inst, glm::vec3 norm);
};

#endif