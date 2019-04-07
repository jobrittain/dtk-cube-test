#define NOMINMAX

#include "Application.h"

#include "CommonStates.h"
#include "DDSTextureLoader.h"
//#include "DirectXHelpers.h"
#include "Effects.h"
#include "GamePad.h"
#include "GeometricPrimitive.h"
#include "GraphicsMemory.h"
//#include "Keyboard.h"
//#include "Model.h"
//#include "Mouse.h"
//#include "PostProcess.h"
//#include "PrimitiveBatch.h"
//#include "ScreenGrab.h"
//#include "SimpleMath.h"
//#include "SpriteBatch.h"
//#include "SpriteFont.h"
//#include "VertexTypes.h"
//#include "WICTextureLoader.h"

#include <DirectXPackedVector.h>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>

using namespace DirectX;
using Microsoft::WRL::ComPtr;

namespace fs = std::filesystem;

static std::string GetExecutablePath()
{
	const auto maxPathSize = static_cast<size_t>(MAX_PATH);

	auto s = std::string(maxPathSize, '\000');
	GetModuleFileNameA(NULL, s.data(), maxPathSize);

	return s.substr(0, s.find_last_of('\\'));
}

static void ReadAllBytes(const std::string& filename, std::vector<char>& container)
{
	using ios = std::ios;

	std::ifstream ifs(filename, ios::binary | ios::ate);
	ifs.seekg(0, ios::end);
	auto pos = ifs.tellg();

	if (pos < 0)
	{
		throw std::exception();
	}

	container.reserve(static_cast<size_t>(pos));

	ifs.seekg(0, ios::beg);
	ifs.read(container.data(), pos);
}

Application::Application() noexcept :
	m_window(nullptr),
	m_outputWidth(800),
	m_outputHeight(600),
	m_featureLevel(D3D_FEATURE_LEVEL_11_1)
{
	_device = std::make_shared<GraphicsDevice>();
}

// Initialize the Direct3D resources required to run.
void Application::Initialize(HWND window, int width, int height)
{
	m_window = window;
	m_outputWidth = std::max(width, 1);
	m_outputHeight = std::max(height, 1);

	CreateDevice();

	CreateResources();

	auto exeDirPath = GetExecutablePath();
}

// Executes the basic game loop.
void Application::Tick()
{
	m_timer.Tick([&]()
	{
		Update(m_timer);
	});

	Render();
}

// Updates the world.
void Application::Update(StepTimer const& timer)
{
	float elapsedTime = float(timer.GetElapsedSeconds());

	auto cameraPosition = XMVectorSet(0.f, 0.f, -2.f, 0.f);
	auto lookAtPosition = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	auto upDirection = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	static float rotation = 0.0f; rotation += 2.f * elapsedTime;

	XMMATRIX matRotate, matView, matProjection, matFinal;

	// create a rotation matrix
	matRotate = XMMatrixRotationY(rotation);

	// create a view matrix
	matView = XMMatrixLookAtLH(cameraPosition, lookAtPosition, upDirection);

	// create a projection matrix
	matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.f),												// field of view
		static_cast<float>(m_outputWidth) / static_cast<float>(m_outputHeight), // aspect ratio
		1.f,																	// near view-plane
		100.0f);																// far view-plane

	// create the final transform
	matFinal = matRotate * matView * matProjection;

	XMFLOAT4X4 matFinalFloat;
	XMStoreFloat4x4(&matFinalFloat, matFinal);
}

// Draws the scene.
void Application::Render()
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return;
	}

	Clear();



	Present();
}

// Helper method to clear the back buffers.
void Application::Clear()
{
	// Clear the views.
	_device->Clear();

}

// Presents the back buffer contents to the screen.
void Application::Present()
{
	// The first argument instructs DXGI to block until VSync, putting the application
	// to sleep until the next VSync. This ensures we don't waste any cycles rendering
	// frames that will never be displayed to the screen.
	auto hr = _device->Present();

	// If the device was reset we must completely reinitialize the renderer.
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		OnDeviceLost();
	}
	else
	{
		ThrowIfFailed(hr);
	}
}

// Message handlers
void Application::OnActivated()
{
	// TODO: Application is becoming active window.
}

void Application::OnDeactivated()
{
	// TODO: Application is becoming background window.
}

void Application::OnSuspending()
{
	// TODO: Application is being power-suspended (or minimized).
}

void Application::OnResuming()
{
	m_timer.ResetElapsedTime();

	// TODO: Application is being power-resumed (or returning from minimize).
}

void Application::OnWindowSizeChanged(int width, int height)
{
	m_outputWidth = std::max(width, 1);
	m_outputHeight = std::max(height, 1);

	_device->UpdateScreenSize(m_outputWidth, m_outputHeight);

	// TODO: Application window is being resized.
}

// Properties
void Application::GetDefaultSize(int& width, int& height) const
{
	// TODO: Change to desired default window size (note minimum size is 320x200).
	width = 800;
	height = 600;
}

// These are the resources that depend on the device.
void Application::CreateDevice()
{
	_device->Initialize(m_window, m_outputWidth, m_outputHeight);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Application::CreateResources()
{
}

void Application::OnDeviceLost()
{
	// TODO: Add Direct3D resource cleanup here.

	_device->Reset();
	CreateDevice();
}