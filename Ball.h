#pragma once

#include <glm/glm.hpp>
#include <GL\glew.h>

class Ball
{
public:
	Ball(float _angularVelocity, float _initVelocity, float _xAngle, float _yAngle, glm::vec3 _spinDirection);

	glm::vec3 euler(GLfloat deltaTime, glm::vec3 ballStartPosition);
	glm::vec3 getPosition();
	glm::vec3 getSpinDirection();
	float getAngularVelocity();
	void kick();
	void reset(float _angularVelocity, float _initVelocity, float _xAngle, float _yAngle, glm::vec3 _spinDirection);
	bool getHasBeenKicked();

	~Ball();
private:
	const float airDensity = 1.0f;
	const float crossArea = 0.038f;
	const float radius = 0.11f;
	const float spinConst = 0.5f;
	const float dragConst = 0.2f;
	const float gravity = 9.82f;
	const float mass = 0.45f;
	const float COR = 0.6f;

	float angularVelocity;
	float initVelocity;
	float liftConst;
	float xAngle;
	float yAngle;
	float K;

	bool hasBeenKicked = false;
	
	glm::vec3 spinDirection;
	glm::vec3 position;
	glm::vec3 velocity;
};