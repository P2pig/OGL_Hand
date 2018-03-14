#pragma once

#include <glm\gtx\transform.hpp>

class Camera
{
private:
	glm::vec3 position;       //eye position in worldspace
	glm::vec3 viewDirection;  //the point where we look at
	glm::vec3 UP;             //the vector of upwords(your head is up)
	glm::vec2 mousePos_old;
	float sensitivity = 0.0005f;
public:
	Camera();
	void MouseUpdate( const glm::vec2& _mPosition );
	glm::mat4 GetMatrix() const;
};