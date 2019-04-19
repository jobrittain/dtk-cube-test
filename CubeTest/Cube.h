#pragma once
#include "GeometricPrimitive.h"
#include "GraphicsDevice.h"
#include "Camera.h"

class Cube
{
private:
	GraphicsDevice& _graphicsDevice;
	Camera& _camera;
	
	std::unique_ptr<DirectX::GeometricPrimitive> _cubePrimitive;
	std::unique_ptr<DirectX::GeometricPrimitive> _voxelPrimitive;

	DirectX::BoundingOrientedBox _cubeBoundingBox;

	DirectX::XMVECTOR _positionCenter;
	DirectX::XMVECTOR _positionVoxelZero;
	DirectX::XMVECTOR _rotation;
	DirectX::XMVECTOR _color;

	struct Voxel
	{
		bool active;
	};

	std::vector<Voxel> _voxels;

public:
	Cube(GraphicsDevice& device, Camera& camera);
	~Cube();

	void Initialize(
		int dimensionInVoxels, 
		DirectX::XMVECTOR position,
		DirectX::XMVECTOR color = DirectX::Colors::White);
	void Draw();
};

