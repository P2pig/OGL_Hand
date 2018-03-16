#pragma once

#include <glm\gtx\transform.hpp>

class Camera
{
private:
	glm::vec3 position;       //eye position in worldspace
	glm::vec3 viewDirection;  //the point where we look at
	glm::vec3 UP;             //the vector of upwords(your head is up)
	glm::vec3 strafeDirection;
	glm::vec2 mousePos_old;
	const float moveSpeed = 3.0f;
	float mouse_Sensitivity = 0.001f;
	float deltaTime;          // frameTime

public:
	Camera();
	void MouseUpdate( const glm::vec2& _mPosition );
	void updateDelta( float in_delta );
	glm::mat4 View() const;
	void moveFoward();
	void moveBackward();
	void strafeLeft();
	void strafeRight();
	void moveUp();
	void moveDown();
};