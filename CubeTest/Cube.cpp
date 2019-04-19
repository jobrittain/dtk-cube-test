#include "Cube.h"
#include "Camera.h"


Cube::Cube(GraphicsDevice& device, Camera& camera) :
	_graphicsDevice(device),
	_camera(camera)
{
	_positionCenter = DirectX::XMVectorZero();
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
	_positionCenter = position;

	DirectX::XMFLOAT3 positionStored;
	DirectX::XMStoreFloat3(&positionStored, position);
	
	_positionVoxelZero = DirectX::XMVectorSet(
		positionStored.x - (dimensionInVoxels / 2) + 0.5f, 
		positionStored.y - (dimensionInVoxels / 2) + 0.5f, 
		positionStored.z - (dimensionInVoxels / 2) + 0.5f,
		0);
	_color = color;

	_cubePrimitive = DirectX::GeometricPrimitive::CreateBox(
		_graphicsDevice.DeviceContext(),
		DirectX::XMFLOAT3(dimensionInVoxels, dimensionInVoxels, dimensionInVoxels));
	_voxelPrimitive = DirectX::GeometricPrimitive::CreateBox(
		_graphicsDevice.DeviceContext(),
		DirectX::XMFLOAT3(1, 1, 1));

	_voxels = std::vector<Voxel>(dimensionInVoxels * 3, Voxel { true });
}

void Cube::Draw()
{
	auto rotationMatrix = DirectX::XMMatrixRotationQuaternion(_rotation);
	auto cubeTranslation = DirectX::XMMatrixTranslationFromVector(_positionCenter);
	auto cubeWorld = rotationMatrix * cubeTranslation;

	_cubePrimitive->Draw(
		cubeWorld, 
		_camera.GetViewMatrix(), 
		_camera.GetProjectionMatrix(),
		DirectX::Colors::LimeGreen,
		nullptr,
		true);

	{
		auto voxelTranslation = DirectX::XMMatrixTranslationFromVector(_positionVoxelZero);
		auto voxelWorld = rotationMatrix * voxelTranslation;
		_voxelPrimitive->Draw(
			voxelWorld,
			_camera.GetViewMatrix(),
			_camera.GetProjectionMatrix(),
			_color);
	}

}
