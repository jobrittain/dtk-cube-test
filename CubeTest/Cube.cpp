#include "Cube.h"



Cube::Cube(std::shared_ptr<GraphicsDevice> device) :
	_graphicsDevice(device)
{
}


Cube::~Cube()
{
}

void Cube::Initialize(int dimensionInVoxels)
{
	_cubePrimitive = DirectX::GeometricPrimitive::CreateBox(
		_graphicsDevice->DeviceContext(),
		DirectX::XMFLOAT3(1, 1, 1));

	_voxels = std::vector<Voxel>(dimensionInVoxels * 3, Voxel { true });
}
