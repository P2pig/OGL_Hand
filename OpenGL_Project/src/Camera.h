#pragma once

#include <glm\gtx\transform.hpp>

class Camera
{
private:
	glm::vec3 position;       //eye position in worldspace
	glm::vec3 viewDirection;  //the point where we look at
	glm::vec3 UP;             //the vector of upwords(your head is up)
	glm::vec2 mousePos_old;
	float mouse_Sensitivity = 0.0005f;
	const float moveSpeed = 0.1f;
public:
	Camera();
	void MouseUpdate( const glm::vec2& _mPosition );
	glm::mat4 View() const;
	void moveFoward();
	void moveBackward();
	void strafeLeft();
	void strafeRight();
	void moveUp();
	void moveDown();
};