#include "Camera.h"

Camera::Camera()
	:
	position( 0.0f, 0.0f, 3.0f ),
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
		strafeDirection = glm::cross( viewDirection, UP );
		viewDirection = glm::mat3( glm::rotate( delta.y * -mouse_Sensitivity, strafeDirection ) ) * viewDirection;
	}
	mousePos_old = mousePos_new;
}

void Camera::updateDelta( float delta )
{
	this->deltaTime = delta;
}

glm::mat4 Camera::View() const
{
	return glm::lookAt( position, (position + viewDirection), UP );
}

void Camera::moveFoward()
{
	position += moveSpeed * deltaTime * viewDirection;
}

void Camera::moveBackward()
{
	position -= moveSpeed * deltaTime  * viewDirection;
}

void Camera::strafeLeft()
{
	position += -moveSpeed * deltaTime  * strafeDirection;
}

void Camera::strafeRight()
{
	position += moveSpeed * deltaTime  * strafeDirection;
}

void Camera::moveUp()
{
	position += moveSpeed * deltaTime  * UP;
}

void Camera::moveDown()
{
	position += -moveSpeed * deltaTime * UP;
}
