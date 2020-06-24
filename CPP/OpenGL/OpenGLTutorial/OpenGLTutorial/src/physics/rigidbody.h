#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <glm/glm.hpp>

#include <string>

class RigidBody {
public:
	float mass;

	glm::vec3 pos;
	glm::vec3 velocity;
	glm::vec3 acceleration;

	glm::vec3 size;

	std::string* modelId;
	std::string instanceId;

	bool operator==(RigidBody rb);
	bool operator==(std::string id);

	RigidBody();

	RigidBody(std::string *modelId, glm::vec3 size = glm::vec3(1.0f), float mass = 1.0f, glm::vec3 pos = glm::vec3(0.0f));

	void update(float dt);

	void applyForce(glm::vec3 force);
	void applyForce(glm::vec3 direction, float magnitude);

	void applyAcceleration(glm::vec3 acceleration);
	void applyAcceleration(glm::vec3 direction, float magnitude);

	void applyImpulse(glm::vec3 force, float dt);
	void applyImpulse(glm::vec3 direction, float magnitude, float dt);

	void transferEnergy(float joules, glm::vec3 direction);
};

#endif