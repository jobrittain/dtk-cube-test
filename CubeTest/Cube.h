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

	DirectX::XMVECTOR _position;
	DirectX::XMVECTOR _rotation;

	struct Voxel
	{
		bool active;
	};

	std::vector<Voxel> _voxels;

public:
	Cube(GraphicsDevice& device, Camera& camera);
	~Cube();

	void Initialize(int dimensionInVoxels);
	void Draw();
};

