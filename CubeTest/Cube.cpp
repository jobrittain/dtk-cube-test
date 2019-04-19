#include "Cube.h"
#include "Camera.h"


Cube::Cube(GraphicsDevice& device, Camera& camera) :
	_graphicsDevice(device),
	_camera(camera)
{
	_position = DirectX::XMVectorZero();
	_rotation = DirectX::XMQuaternionIdentity();
}


Cube::~Cube()
{
}

void Cube::Initialize(
	int dimensionInVoxels, 
	DirectX::XMVECTOR position, 
	DirectX::XMVECTOR color)
{
	_position = position;
	_color = color;

	_cubePrimitive = DirectX::GeometricPrimitive::CreateBox(
		_graphicsDevice.DeviceContext(),
		DirectX::XMFLOAT3(1, 1, 1));

	_voxels = std::vector<Voxel>(dimensionInVoxels * 3, Voxel { true });
}

void Cube::Draw()
{
	auto rotationMatrix = DirectX::XMMatrixRotationQuaternion(_rotation);
	auto translationMatrix = DirectX::XMMatrixTranslationFromVector(_position);
	auto worldMatrix = rotationMatrix * translationMatrix;

	_cubePrimitive->Draw(
		worldMatrix, 
		_camera.GetViewMatrix(), 
		_camera.GetProjectionMatrix(),
		_color);
}
