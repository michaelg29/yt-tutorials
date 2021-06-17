#include "rigidbody.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

// test for equivalence of two rigid bodies
bool RigidBody::operator==(RigidBody rb) {
    return instanceId == rb.instanceId;
}

// test for equivalence of two rigid bodies
bool RigidBody::operator==(std::string id) {
    return instanceId == id;
}

/*
    constructor
*/

// construct with parameters and default
RigidBody::RigidBody(std::string modelId, glm::vec3 size, float mass, glm::vec3 pos, glm::vec3 rot)
    : modelId(modelId), size(size), mass(mass), pos(pos), rot(rot),
    velocity(0.0f), acceleration(0.0f), state(0),
    lastCollision(COLLISION_THRESHOLD), lastCollisionID("") {
    update(0.0f);
}

/*
    transformation functions
*/

// update position with velocity and acceleration
void RigidBody::update(float dt) {
    pos += velocity * dt + 0.5f * acceleration * (dt * dt);
    velocity += acceleration * dt;

    // calculate rotation matrix
    glm::mat4 rotMat = glm::toMat4(glm::quat(rot));

    // model = trans * rot * scale = T * R * S
    model = glm::translate(glm::mat4(1.0f), pos); // M = I * T = T
    model = model * rotMat; // M = M * R = T * R
    model = glm::scale(model, size); // M = M * S = T * R * S

    normalModel = glm::transpose(glm::inverse(glm::mat3(model)));

    lastCollision += dt;
}

// apply a force
void RigidBody::applyForce(glm::vec3 force) {
    acceleration += force / mass;
}

// apply a force
void RigidBody::applyForce(glm::vec3 direction, float magnitude) {
    applyForce(direction * magnitude);
}

// apply an acceleration (remove redundancy of dividing by mass)
void RigidBody::applyAcceleration(glm::vec3 a) {
    acceleration += a;
}

// apply an acceleration (remove redundancy of dividing by mass)
void RigidBody::applyAcceleration(glm::vec3 direction, float magnitude) {
    applyAcceleration(direction * magnitude);
}

// apply force over time
void RigidBody::applyImpulse(glm::vec3 force, float dt) {
    velocity += force / mass * dt;
}

// apply force over time
void RigidBody::applyImpulse(glm::vec3 direction, float magnitude, float dt) {
    applyImpulse(direction * magnitude, dt);
}

// transfer potential or kinetic energy from another object
void RigidBody::transferEnergy(float joules, glm::vec3 direction) {
    if (joules == 0) {
        return;
    }

    // comes from formula: KE = 1/2 * m * v^2
    glm::vec3 deltaV = sqrt(2 * abs(joules) / mass) * direction;

    velocity += joules > 0 ? deltaV : -deltaV;
}

/*
    collisions
*/
void RigidBody::handleCollision(RigidBody* inst, glm::vec3 norm) {
    if (lastCollision >= COLLISION_THRESHOLD || lastCollisionID != inst->instanceId) {
        this->velocity = glm::reflect(this->velocity, glm::normalize(norm)); // register (elastic) collision
        lastCollision = 0.0f; // reset counter
    }

    lastCollisionID = inst->instanceId;
}