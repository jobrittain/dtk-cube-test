#pragma once

#include "StepTimer.h"
#include "GraphicsDevice.h"
#include "Camera.h"
#include "Cube.h"

#include "Mouse.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d11_4.h>
#include <memory>

class Application
{
public:

	Application() noexcept;

	// Initialization and management
	void Initialize(HWND window, int width, int height);

	// Basic game loop
	void Tick();

	// Messages
	void OnActivated();
	void OnDeactivated();
	void OnSuspending();
	void OnResuming();
	void OnWindowSizeChanged(int width, int height);

	// Properties
	void GetDefaultSize(int& width, int& height) const;

private:


	void Update(StepTimer const& timer);
	void Render();

	void Clear();
	void Present();

	void CreateDevice();
	void CreateResources();

	void OnDeviceLost();

	// Device resources.
	HWND                                            m_window;
	int                                             m_outputWidth = 800;
	int                                             m_outputHeight = 600;

	DirectX::Mouse									_mouse;

	GraphicsDevice									_device;
	Camera											_camera;
	Cube											_cube;
	StepTimer										m_timer;

	std::unique_ptr<DirectX::GeometricPrimitive>    _mouseCube;
	bool _drawMouseCube = false;
	int _mousePosX;
	int _mousePosY;

	//D3D_FEATURE_LEVEL                               m_featureLevel;


	//Microsoft::WRL::ComPtr<ID3D11Device1>           m_d3dDevice;
	//Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    m_d3dContext;

	//Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain;
	//Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
	//Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;

	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			// Set a breakpoint on this line to catch DirectX API errors
			throw std::exception();
		}
	}
};

