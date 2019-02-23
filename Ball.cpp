#include "Ball.h"

#include <cmath>
#include <stdio.h>


Ball::Ball(float _angularVelocity, float _initVelocity, float _xAngle, float _yAngle, glm::vec3 _spinDirection)
{
	const float PI = atan(1.0f)*4.0f;

	angularVelocity = _angularVelocity;
	initVelocity = _initVelocity;
	xAngle = (PI)-(_xAngle * PI) / 180.0f;
	yAngle = (PI/2)-(_yAngle * PI) / 180.0f;

	spinDirection = _spinDirection;

	liftConst = (spinConst*angularVelocity*radius) / initVelocity;
	K = (airDensity*dragConst*crossArea) / 2.0f;

	velocity.x = initVelocity * sin(yAngle)*sin(xAngle);
	velocity.y = initVelocity * cos(yAngle);
	velocity.z = initVelocity * sin(yAngle)*cos(xAngle);

	position = glm::vec3(0.0f, 0.0f, -10.5f);
}

glm::vec3 Ball::euler()
{
	glm::vec3 acceleration;

	float velocitySquare = powf(velocity.x, 2.0f) + powf(velocity.y, 2.0f) + powf(velocity.z, 2.0f);
	float crossProduct = glm::length(glm::cross(spinDirection, velocity));
	float Kl = (liftConst*airDensity*crossArea*velocitySquare) / (2.0f*crossProduct);
	

	float velocityAbs = abs(velocity.x + velocity.y + velocity.z);
	acceleration.x = (-K * velocityAbs*velocity.x - Kl * (spinDirection.y*velocity.z - spinDirection.z*velocity.y))/mass;
	acceleration.y = (-mass*gravity-K * velocityAbs * velocity.y - Kl * (spinDirection.z*velocity.x - spinDirection.x*velocity.z)) / mass;
	acceleration.z = (-K * velocityAbs*velocity.z - Kl * (spinDirection.x*velocity.y - spinDirection.y*velocity.x)) / mass;

	if (position.y < 0) return position;

	position.x = (position.x + velocity.x*0.01f);
	position.y = (position.y + velocity.y*0.01f);
	position.z = (position.z + velocity.z*0.01f);

	velocity.x = velocity.x + acceleration.x*0.01f;
	velocity.y = velocity.y + acceleration.y*0.01f;
	velocity.z = velocity.z + acceleration.z*0.01f;

	return position;
}

glm::vec3 Ball::getPosition()
{
	return position;
}

Ball::~Ball()
{
	
}