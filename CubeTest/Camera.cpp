#include "Camera.h"

using namespace DirectX::SimpleMath;

void Camera::CreateNewProjection(const float screenWidth, const float screenHeight)
{
	_projection = DirectX::XMMatrixPerspectiveFovRH(
		DirectX::XM_PI / 4.f,
		screenWidth / screenHeight,
		0.1f,
		200.f);
}

Camera::Camera(const float screenWidth, const float screenHeight)
{
	CreateNewProjection(screenWidth, screenHeight);
}

Camera::~Camera()
{
}

void Camera::UpdateScreenSize(const float screenWidth, const float screenHeight)
{
	CreateNewProjection(screenWidth, screenHeight);
}

void Camera::SetPosition(const DirectX::XMVECTOR & position)
{
	_position = position;
}

void Camera::SetMovementSpeed(float speed)
{
	_movementSpeed = speed;
	_keyMovementSpeed = speed + 100;
}

void Camera::SetZoomSpeed(float speed)
{
	_zoomSpeed = speed;
	_keyZoomSpeed = speed + 20;
}

DirectX::SimpleMath::Matrix Camera::GetViewMatrix()
{
	std::lock_guard<std::mutex> lock(posLock);
	
	return DirectX::XMMatrixLookAtRH(
		_position,
		Vector3(_position.x, _position.y - 1, _position.z - 1),
		Vector3::UnitY);
}

void Camera::MoveUp(float deltaTime)
{
	std::lock_guard<std::mutex> lock(posLock);
	_position.y += _keyMovementSpeed * deltaTime;
}

void Camera::MoveDown(float deltaTime)
{
	std::lock_guard<std::mutex> lock(posLock);
	_position.y -= _keyMovementSpeed * deltaTime;
}

void Camera::MoveLeft(float deltaTime)
{
	std::lock_guard<std::mutex> lock(posLock);
	_position.x -= _keyMovementSpeed * deltaTime;
}

void Camera::MoveRight(float deltaTime)
{
	std::lock_guard<std::mutex> lock(posLock);
	_position.x += _keyMovementSpeed * deltaTime;
}

void Camera::MoveForwards(float deltaTime)
{
	std::lock_guard<std::mutex> lock(posLock);
	_position.z -= _keyMovementSpeed * deltaTime;
}

void Camera::MoveBackwards(float deltaTime)
{
	std::lock_guard<std::mutex> lock(posLock);
	_position.z += _keyMovementSpeed * deltaTime;
}

void Camera::ZoomIn(float deltaTime)
{
	std::lock_guard<std::mutex> lock(posLock);
	_position.y -= 4 * tanf(DirectX::XM_PI / 4.f) * _keyZoomSpeed * deltaTime;
	_position.z -= 4 * _keyZoomSpeed * deltaTime;
}

void Camera::ZoomOut(float deltaTime)
{
	std::lock_guard<std::mutex> lock(posLock);
	_position.y += 4 * tanf(DirectX::XM_PI / 4.f) * _keyZoomSpeed * deltaTime;
	_position.z += 4 * _keyZoomSpeed * deltaTime;
}

void Camera::MouseMove(int x, int y)
{
	std::lock_guard<std::mutex> lock(posLock);
	_position.x += x * _movementSpeed;
	_position.z += y * _movementSpeed;
}

void Camera::MouseZoom(int scroll)
{
	std::lock_guard<std::mutex> lock(posLock);
	_position.y -= scroll * tanf(DirectX::XM_PI / 4.f) * _zoomSpeed;
	_position.z -= scroll * _zoomSpeed;
}
