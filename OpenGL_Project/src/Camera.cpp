#include "Camera.h"

Camera::Camera()
	:
	viewDirection( 0.0f, 0.0f, -1.0f ),
	UP( 0.0f, 1.0f, 0.0f )
{}

void Camera::MouseUpdate( const glm::vec2 & mousePos_new )
{
	glm::vec2 delta = mousePos_new - mousePos_old;
	{
		// turn horizontal
		viewDirection = glm::mat3( glm::rotate( delta.x * -mouse_Sensitivity, UP ) ) * viewDirection;

		// turn vertical
		glm::vec3 toRotateAround = glm::cross( viewDirection, UP );
		viewDirection = glm::mat3( glm::rotate( delta.y * -mouse_Sensitivity, toRotateAround ) ) * viewDirection;
	}
	mousePos_old = mousePos_new;
}

glm::mat4 Camera::GetMatrix() const
{
	return glm::lookAt( position, (position + viewDirection), UP );
}

void Camera::moveFoward()
{
	position += moveSpeed * viewDirection;
}

void Camera::moveBackward()
{
	position -= moveSpeed * viewDirection;
}

void Camera::strafeLeft()
{
	glm::vec3 strafeDirection = glm::cross( viewDirection, UP );
	position += -moveSpeed * strafeDirection;
}

void Camera::strafeRight()
{
	glm::vec3 strafeDirection = glm::cross( viewDirection, UP );
	position += moveSpeed * strafeDirection;
}

void Camera::moveUp()
{
	position += moveSpeed * UP;
}

void Camera::moveDown()
{
	position += -moveSpeed * UP;
}
