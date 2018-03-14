#include "Camera.h"

Camera::Camera()
	:
	viewDirection( 0.0f, 0.0f, -1.0f ),
	UP( 0.0f, 1.0f, 0.0f )
{}

void Camera::MouseUpdate( const glm::vec2 & mousePos_new )
{
	glm::vec2 delta = mousePos_new - mousePos_old;
	viewDirection = glm::mat3( glm::rotate( delta.x * sensitivity, UP ) ) * viewDirection;
	mousePos_old = mousePos_new;
}

glm::mat4 Camera::GetMatrix() const
{
	return glm::lookAt( position, (position + viewDirection), UP );
}
