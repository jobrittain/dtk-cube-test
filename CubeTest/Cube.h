#pragma once
#include "GeometricPrimitive.h"
#include "GraphicsDevice.h"

class Cube
{
private:
	std::shared_ptr<GraphicsDevice> _graphicsDevice;
	std::unique_ptr<DirectX::GeometricPrimitive> _cubePrimitive;
	int _dimension;

public:
	Cube(std::shared_ptr<GraphicsDevice> device);
	~Cube();

	void Initialize(int dimensionInVoxels);
};

