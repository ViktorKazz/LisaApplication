// -------------------------------------------------------------------------------------- 
// 
// LisaApplication. Creating 3D primitives and editing their attributes. 
// Copyright (C) 18.8.2024 - 30.9.2026 Deputatov Viktor Maxwellrender@yandex.ru 
// 
// This program is free software: you can redistribute it and/or modify 
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version. 
// 
// This program is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
// GNU General Public License for more details. 
// 
// You should have received a copy of the GNU General Public License 
// along with this program. If not, see https://www.gnu.org/licenses/. 
// 
// Project blog https://lisaapplicationblog.blogspot.com/. 
// 
// --------------------------------------------------------------------------------------

#include "..//Inc/D3D11DeviceResources.h"

// Class D11DeviceResources

HRESULT UI::D11DeviceResources::CreateD3D11Device(HWND hwnd)
{
	HRESULT hr{ S_OK };

	// Create the Direct3D device.

	// This flag is required in order to enable compatibility with Direct2D.
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
	// If the project is in a debug build, enable debugging via SDK Layers with this flag.
	//creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// This array defines the set of DirectX hardware feature levels this app  supports.
	// The ordering is important and you should  preserve it.
	// Don't forget to declare your app's minimum required feature level in its
	// description.  All apps are assumed to support 9.1 unless otherwise stated.

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	D3D_FEATURE_LEVEL featureLevel;

	hr = D3D11CreateDevice(
		nullptr,                                                 // specify null to use the default adapter
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		creationFlags,                                           // optionally set debug and Direct2D compatibility flags
		featureLevels,                                           // list of feature levels this app can support
		ARRAYSIZE(featureLevels),                                // number of possible feature levels
		D3D11_SDK_VERSION,
		m_pD3D11Device.ReleaseAndGetAddressOf(),          // returns the Direct3D device created
		&featureLevel,                                           // returns feature level of device created
		m_pD3D11DeviceContext.ReleaseAndGetAddressOf()    // returns the device immediate context
	);
	if (hr != S_OK)
	{
		MessageBox(hwnd, L"Error creating device", L"Error", MB_OK);
		return 1;
	}

	// Exit if it's not supported
	if (featureLevel != D3D_FEATURE_LEVEL_11_0) {
		MessageBox(hwnd, L"Your computer does not support DirectX 11", L"Error", MB_OK);
		return 1;
	}

	return hr;
}

HRESULT UI::D11DeviceResources::CreateD2D1Factory()
{
	HRESULT hr{ S_OK };

	D2D1_FACTORY_OPTIONS options;

	ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)
	// If the project is in a debug build, enable debugging via SDK Layers with this flag.
	options.debugLevel = D2D1_DEBUG_LEVEL::D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	// Create a Direct2D factory.
	UI::ThrowIfFailed(D2D1CreateFactory(
		D2D1_FACTORY_TYPE_MULTI_THREADED,
		__uuidof(ID2D1Factory7),
		&options,
		reinterpret_cast<void**>(m_pD2D1Factory.ReleaseAndGetAddressOf())
	));

	// Create a DirectWrite factory.
	UI::ThrowIfFailed(DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(m_pDWriteFactory.Get()),
		reinterpret_cast<IUnknown**>(m_pDWriteFactory.ReleaseAndGetAddressOf())
	));

	// Create WIC factory.
	UI::ThrowIfFailed(CoCreateInstance(
		CLSID_WICImagingFactory2,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		reinterpret_cast<void**>(m_pWICFactory.ReleaseAndGetAddressOf())
	));

	return hr;
}

HRESULT UI::D11DeviceResources::CreateD2D1Device()
{
	HRESULT hr = ((m_pD3D11Device == nullptr) || (m_pD2D1Factory == nullptr)) ? E_UNEXPECTED : S_OK;

	// Obtain the underlying DXGI device of the Direct3D11 device.
	UI::ThrowIfFailed(m_pD3D11Device.Get()->QueryInterface((IDXGIDevice4**)m_pDXGIDevice.ReleaseAndGetAddressOf()));

	// Obtain the Direct2D device for 2-D rendering.
	UI::ThrowIfFailed(m_pD2D1Factory.Get()->CreateDevice(m_pDXGIDevice.Get(), m_pD2D1Device.ReleaseAndGetAddressOf()));

	// Get Direct2D device's corresponding device context object.
	Microsoft::WRL::ComPtr<ID2D1DeviceContext6> pD2DDeviceContext;

	UI::ThrowIfFailed(m_pD2D1Device.Get()->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &pD2DDeviceContext));

	UI::ThrowIfFailed(pD2DDeviceContext->QueryInterface((ID2D1DeviceContext6**)m_pD2D1DeviceContext.ReleaseAndGetAddressOf()));

	return hr;
}

void UI::D11DeviceResources::CreateDevice()
{
	UI::ThrowIfFailed(CreateD3D11Device(NULL));
	UI::ThrowIfFailed(CreateD2D1Factory());
	UI::ThrowIfFailed(CreateD2D1Device());
}

// Class SwapChainAndComposition

HRESULT UI::Composition::CreateSwapChain(HWND hwnd)
{
	HRESULT hr{ S_OK };

	// If the swap chain does not exist, create it.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };

	swapChainDesc.Stereo = false;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.Scaling = (hwnd != NULL) ? DXGI_SCALING::DXGI_SCALING_NONE : DXGI_SCALING::DXGI_SCALING_STRETCH;
	swapChainDesc.Flags = 0;
	swapChainDesc.AlphaMode = hwnd ? DXGI_ALPHA_MODE_IGNORE : DXGI_ALPHA_MODE_PREMULTIPLIED;

	// Use automatic sizing ???. 
	// I haven’t figured this out at the moment, so instead of the value 0 I put 1.
	swapChainDesc.Width = 1;
	swapChainDesc.Height = 1;

	// This is the most common swap chain format.
	swapChainDesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;

	// Don't use multi-sampling.
	swapChainDesc.SampleDesc.Count = 1; // don't use multi-sampling
	swapChainDesc.SampleDesc.Quality = 0;

	// Use two buffers to enable the flip effect.
	swapChainDesc.BufferCount = 2;

	// We using this swap effect for all applications.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;


	Microsoft::WRL::ComPtr<IDXGIAdapter> pDXGIAdapter;

	// Get the parent factory from the DXGI Device.
	UI::ThrowIfFailed(m_pDXGIDevice.Get()->GetAdapter(&pDXGIAdapter));

	Microsoft::WRL::ComPtr<IDXGIFactory5> pDXGIFactory2;

	UI::ThrowIfFailed(pDXGIAdapter->GetParent(IID_PPV_ARGS(&pDXGIFactory2)));

	if (hwnd != NULL)
	{
		//The DXGI factory will ask the window for the size of the client area.
		UI::ThrowIfFailed(pDXGIFactory2->CreateSwapChainForHwnd(
			m_pD3D11Device.Get(),
			hwnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			m_pDXGISwapChain.ReleaseAndGetAddressOf()
		));
	}
	else
	{
		// Creates a swap chain that you can use to send Direct3D content 
		// into the DirectComposition API.
		UI::ThrowIfFailed(pDXGIFactory2->CreateSwapChainForComposition(
			m_pD3D11Device.Get(),
			&swapChainDesc,
			nullptr,
			m_pDXGISwapChain.ReleaseAndGetAddressOf()
		));
	}

	// Ensure that DXGI does not queue more than one frame at a time. This both reduces
	// latency and ensures that the application will only render after each VSync, minimizing
	// power consumption.
	UI::ThrowIfFailed(m_pDXGIDevice.Get()->SetMaximumFrameLatency(1));

	return hr;
}

HRESULT UI::Composition::ConfigureSwapChain(HWND hwnd)
{
	HRESULT hr{ S_OK };

	//(GetDpiForWindow) Returns the dots per inch (dpi) value for the specified window.
	//
	UINT nDPI = GetDpiForWindow(hwnd);
	D2D1_BITMAP_PROPERTIES1 bitmapProperties = {};

	bitmapProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	bitmapProperties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	bitmapProperties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

	bitmapProperties.dpiX = static_cast<FLOAT>(nDPI);
	bitmapProperties.dpiY = static_cast<FLOAT>(nDPI);

	Microsoft::WRL::ComPtr<IDXGISurface> pDXGISurface;

	if (m_pDXGISwapChain.Get())
	{
		UI::ThrowIfFailed(m_pDXGISwapChain.Get()->GetBuffer(
			0,
			IID_PPV_ARGS(&pDXGISurface)
		));

		UI::ThrowIfFailed(m_pD2D1DeviceContext.Get()->CreateBitmapFromDxgiSurface(
			pDXGISurface.Get(),
			bitmapProperties,
			m_pD2D1TargetBitmap.ReleaseAndGetAddressOf()
		));

		m_pD2D1DeviceContext.Get()->SetTarget(m_pD2D1TargetBitmap.Get());
	}
	return hr;
}

HRESULT UI::Composition::CreateDCompositionSwapChain(HWND hwnd)
{
	HRESULT hr{ S_OK };

	UI::ThrowIfFailed(DCompositionCreateDevice(
		m_pDXGIDevice.Get(),
		__uuidof(m_pDCompositionDevice.Get()),
		reinterpret_cast<void**>(m_pDCompositionDevice.ReleaseAndGetAddressOf())
	));

	// TRUE if the visual tree should be displayed on top of the children 
    // of the window specified by the hwnd parameter; 
	// otherwise, the visual tree is displayed behind the children.

	UI::ThrowIfFailed(m_pDCompositionDevice.Get()->CreateTargetForHwnd(
		hwnd,
		false,
		m_pDCompositionTarget.ReleaseAndGetAddressOf()
	));

	Microsoft::WRL::ComPtr<IDCompositionVisual> pDCompositionVisual;

	UI::ThrowIfFailed(m_pDCompositionDevice.Get()->CreateVisual(&pDCompositionVisual));

	UI::ThrowIfFailed(pDCompositionVisual->SetContent(m_pDXGISwapChain.Get()));
	UI::ThrowIfFailed(m_pDCompositionTarget.Get()->SetRoot(pDCompositionVisual.Get()));
	UI::ThrowIfFailed(m_pDCompositionDevice.Get()->Commit());

	return hr;
}

// Initializes DirectComposition
HRESULT UI::Composition::CreateDComposition()
{
	HRESULT hr = (m_pD3D11Device == nullptr) ? E_UNEXPECTED : S_OK;

	UI::ThrowIfFailed(DCompositionCreateDevice(
		m_pDXGIDevice.Get(),
		__uuidof(m_pDCompositionDevice.Get()),
		reinterpret_cast<void**>(m_pDCompositionDevice.ReleaseAndGetAddressOf())
	));

	return hr;
}

void UI::Composition::OnResize(HWND hwnd, UINT nWidth, UINT nHeight)
{
	if (m_pDXGISwapChain.Get())
	{
		HRESULT hr{ S_OK };

		if (nWidth != 0 && nHeight != 0)
		{
			m_pD2D1DeviceContext.Get()->SetTarget(nullptr);

			// All outstanding buffer references must be released.
			m_pD2D1TargetBitmap.Reset();

			// Resizing the app window and the swap chain’s buffer.
			hr = m_pDXGISwapChain.Get()->ResizeBuffers(
				2, // Double-buffered swap chain.
				nWidth,
				nHeight,
				DXGI_FORMAT_B8G8R8A8_UNORM,
				0
			);
			if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
			{
				//CreateD3D11Device(hwnd);
				CreateSwapChain(hwnd);
				return;
			}
			else
			{
				UI::ThrowIfFailed(hr);
			}
			ConfigureSwapChain(hwnd);
		}
	}
}
