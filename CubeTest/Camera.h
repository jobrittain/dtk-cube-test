#pragma once
#include <mutex>
#include <d3d11.h>
#include "SimpleMath.h"

class Camera
{
private:
	DirectX::SimpleMath::Vector3 _position;

	float _movementSpeed;
	float _keyMovementSpeed;
	float _zoomSpeed;
	float _keyZoomSpeed;

	std::mutex posLock;

public:
	Camera();
	~Camera();

	void SetMovementSpeed(float speed);
	void SetZoomSpeed(float speed);

	DirectX::SimpleMath::Matrix GetViewMatrix();

	void MoveUp(float deltaTime);
	void MoveDown(float deltaTime);
	void MoveLeft(float deltaTime);
	void MoveRight(float deltaTime);
	void MoveForwards(float deltaTime);
	void MoveBackwards(float deltaTime);
	void ZoomIn(float deltaTime);
	void ZoomOut(float deltaTime);

	void MouseMove(int x, int y);
	void MouseZoom(int scroll);
};

