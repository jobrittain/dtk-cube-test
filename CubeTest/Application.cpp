//
// Application.cpp
//

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

Application::Application() noexcept :
	m_window(nullptr),
	m_outputWidth(800),
	m_outputHeight(600),
	m_featureLevel(D3D_FEATURE_LEVEL_9_1)
{
}

static std::string GetExecutablePath()
{
	std::string s = std::string(MAX_PATH, '/0');
	auto c = GetModuleFileNameA(NULL, s.data(), s.size());
	/*if (c > s.length())
	{
		s.reserve(c);
		c = GetModuleFileNameA(NULL, s.data(), s.size());
	}*/
	return s.substr(0, s.find_last_of('\\'));
}

static void ReadAllBytes(const std::string& filename, std::vector<char>& container)
{
	std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
	ifs.seekg(0, std::ios::end);
	std::ifstream::pos_type pos = ifs.tellg();

	container.reserve(pos);

	//ifs.seekg(0, ifs.beg);
	ifs.seekg(0, std::ios::beg);
	ifs.read(container.data(), pos);
}

struct VERTEX
{
	VERTEX() {};
	VERTEX(float x, float y, float z, float r, float g, float b, float a)
	{
		auto position = XMFLOAT3(x, y, z);
		this->Position = XMLoadFloat3(&position);
		this->Color = PackedVector::XMCOLOR(r, g, b, a);
	}

	XMVECTOR Position;      // position
	PackedVector::XMCOLOR Color;   // color
};

struct VERTEX2
{
	XMFLOAT3 Position;      // position
	XMFLOAT4 Color;   // color
};


ID3D11Buffer *pVBuffer;
ID3D11InputLayout *pLayout;

// Initialize the Direct3D resources required to run.
void Application::Initialize(HWND window, int width, int height)
{
	m_window = window;
	m_outputWidth = std::max(width, 1);
	m_outputHeight = std::max(height, 1);

	CreateDevice();

	CreateResources();

	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	/*
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
	*/

	auto exeDirPath = GetExecutablePath();

	std::vector<char> p_data, v_data;

	ReadAllBytes(exeDirPath + "\\PixelShader.cso", p_data);
	ReadAllBytes(exeDirPath + "\\VertexShader.cso", v_data);
	
	//m_pixelShader.Reset();

	ThrowIfFailed(m_d3dDevice->CreateVertexShader(v_data.data(), v_data.capacity(), NULL, m_vertexShader.GetAddressOf()));
	ThrowIfFailed(m_d3dDevice->CreatePixelShader(p_data.data(), p_data.capacity(), NULL, m_pixelShader.GetAddressOf()));

	m_d3dContext->VSSetShader(m_vertexShader.Get(), 0, 0);
	m_d3dContext->PSSetShader(m_pixelShader.Get(), 0, 0);

	// create the input layout object
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	ThrowIfFailed(m_d3dDevice->CreateInputLayout(ied, 2, v_data.data(), v_data.capacity(), &pLayout));
	m_d3dContext->IASetInputLayout(pLayout);

	// Init graphics

	// create a triangle using the VERTEX struct
	//VERTEX OurVertices[] =
	//{
	//	VERTEX(0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f),
	//	VERTEX(0.45f, -0.5, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f),
	//	VERTEX(-0.45f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f)
	//};

	VERTEX2 OurVertices[] =
	{
		{ XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(0.45f, -0.5, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-0.45f, -0.5f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
	bd.ByteWidth = sizeof(VERTEX2) * 3;             // size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

	ThrowIfFailed(m_d3dDevice->CreateBuffer(&bd, NULL, &pVBuffer));       // create the buffer

	// copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	m_d3dContext->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
	memcpy(ms.pData, OurVertices, sizeof(OurVertices));                 // copy the data
	m_d3dContext->Unmap(pVBuffer, NULL);                                      // unmap the buffer
	
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(XMFLOAT4X4);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	m_d3dDevice->CreateBuffer(&bd, NULL, m_constBuffer.GetAddressOf());
	m_d3dContext->VSSetConstantBuffers(0, 1, m_constBuffer.GetAddressOf());
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

	// set the new values for the constant buffer
	m_d3dContext->UpdateSubresource(m_constBuffer.Get(), 0, 0, &matFinalFloat, 0, 0);
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

	 // select which vertex buffer to display
	UINT stride = sizeof(VERTEX2);
	UINT offset = 0;
	m_d3dContext->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);

	// select which primtive type we are using
	m_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// draw the vertex buffer to the back buffer
	m_d3dContext->Draw(3, 0);

	Present();
}

// Helper method to clear the back buffers.
void Application::Clear()
{
	// Clear the views.
	m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::CornflowerBlue);
	m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

	// Set the viewport.
	CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
	m_d3dContext->RSSetViewports(1, &viewport);
}

// Presents the back buffer contents to the screen.
void Application::Present()
{
	// The first argument instructs DXGI to block until VSync, putting the application
	// to sleep until the next VSync. This ensures we don't waste any cycles rendering
	// frames that will never be displayed to the screen.
	HRESULT hr = m_swapChain->Present(1, 0);

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

	CreateResources();

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
	UINT creationFlags = 0;

#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	static const D3D_FEATURE_LEVEL featureLevels[] =
	{
		// TODO: Modify for supported Direct3D feature levels
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	// Create the DX11 API device object, and get a corresponding context.
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
	ThrowIfFailed(D3D11CreateDevice(
		nullptr,                            // specify nullptr to use the default adapter
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags,
		featureLevels,
		_countof(featureLevels),
		D3D11_SDK_VERSION,
		device.ReleaseAndGetAddressOf(),    // returns the Direct3D device created
		&m_featureLevel,                    // returns feature level of device created
		context.ReleaseAndGetAddressOf()    // returns the device immediate context
	));

#ifndef NDEBUG
	ComPtr<ID3D11Debug> d3dDebug;
	if (SUCCEEDED(device.As(&d3dDebug)))
	{
		ComPtr<ID3D11InfoQueue> d3dInfoQueue;
		if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
		{
#ifdef _DEBUG
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
			D3D11_MESSAGE_ID hide[] =
			{
				D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
				// TODO: Add more message IDs here as needed.
			};
			D3D11_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = _countof(hide);
			filter.DenyList.pIDList = hide;
			d3dInfoQueue->AddStorageFilterEntries(&filter);
		}
	}
#endif

	ThrowIfFailed(device.As(&m_d3dDevice));
	ThrowIfFailed(context.As(&m_d3dContext));

	// TODO: Initialize device dependent objects here (independent of window size).
}

// Allocate all memory resources that change on a window SizeChanged event.
void Application::CreateResources()
{
	// Clear the previous window size specific context.
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
	m_renderTargetView.Reset();
	m_depthStencilView.Reset();
	m_d3dContext->Flush();

	UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
	UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
	DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
	DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	UINT backBufferCount = 2;

	// If the swap chain already exists, resize it, otherwise create one.
	if (m_swapChain)
	{
		HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			// If the device was removed for any reason, a new device and swap chain will need to be created.
			OnDeviceLost();

			// Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
			// and correctly set up the new device.
			return;
		}
		else
		{
			ThrowIfFailed(hr);
		}
	}
	else
	{
		// First, retrieve the underlying DXGI Device from the D3D Device.
		ComPtr<IDXGIDevice1> dxgiDevice;
		ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

		// Identify the physical adapter (GPU or card) this device is running on.
		ComPtr<IDXGIAdapter> dxgiAdapter;
		ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

		// And obtain the factory object that created it.
		ComPtr<IDXGIFactory2> dxgiFactory;
		ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

		// Create a descriptor for the swap chain.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = backBufferWidth;
		swapChainDesc.Height = backBufferHeight;
		swapChainDesc.Format = backBufferFormat;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = backBufferCount;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
		fsSwapChainDesc.Windowed = TRUE;

		// Create a SwapChain from a Win32 window.
		ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
			m_d3dDevice.Get(),
			m_window,
			&swapChainDesc,
			&fsSwapChainDesc,
			nullptr,
			m_swapChain.ReleaseAndGetAddressOf()
		));

		// This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
		ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
	}

	// Obtain the backbuffer for this window which will be the final 3D rendertarget.
	ComPtr<ID3D11Texture2D> backBuffer;
	ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

	// Create a view interface on the rendertarget to use on bind.
	ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

	// Allocate a 2-D surface as the depth/stencil buffer and
	// create a DepthStencil view on this surface to use on bind.
	CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

	ComPtr<ID3D11Texture2D> depthStencil;
	ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

	// TODO: Initialize windows-size dependent objects here.
}

void Application::OnDeviceLost()
{
	// TODO: Add Direct3D resource cleanup here.

	m_depthStencilView.Reset();
	m_renderTargetView.Reset();
	m_swapChain.Reset();
	m_d3dContext.Reset();
	m_d3dDevice.Reset();

	CreateDevice();

	CreateResources();
}