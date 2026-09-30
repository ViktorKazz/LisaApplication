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

#ifndef D3D11_DEVICE_RESOURCES_H
#define D3D11_DEVICE_RESOURCES_H

#include "pch.h"

namespace UI
{
    class D11DeviceResources
    {
    public:
        D11DeviceResources() = default;
        virtual ~D11DeviceResources()
        {
            /*wchar_t msg[128]{};
            swprintf_s(msg, L"~D11DeviceResources: % d\n", 1);
            OutputDebugString(msg);*/
        };;

        D11DeviceResources(D11DeviceResources&&) = default;
        D11DeviceResources& operator= (D11DeviceResources&&) = default;

        D11DeviceResources(D11DeviceResources const&) = delete;
        D11DeviceResources& operator= (D11DeviceResources const&) = delete;

        // Accessors.

        auto GetID3D11Device()                   const noexcept { return m_pD3D11Device; };
        auto GetID2D1Factory()                   const noexcept { return m_pD2D1Factory; };
        auto GetIDWriteFactory()                 const noexcept { return m_pDWriteFactory; };
        auto GetIWICFactory()                    const noexcept { return m_pWICFactory; };
        auto GetID2D1DeviceContext()             const noexcept { return m_pD2D1DeviceContext; };
        auto GetIDXGIDevice()                    const noexcept { return m_pDXGIDevice; };


        HRESULT CreateD3D11Device(HWND hwnd);
        HRESULT CreateD2D1Factory();
        HRESULT CreateD2D1Device();

        void CreateDevice();

    private:
        // Factory.
        Microsoft::WRL::ComPtr<ID2D1Factory7> m_pD2D1Factory;
        Microsoft::WRL::ComPtr<IWICImagingFactory2> m_pWICFactory;
        Microsoft::WRL::ComPtr<IDWriteFactory7> m_pDWriteFactory;

        // Direct2D objects.
        Microsoft::WRL::ComPtr<ID2D1Device6> m_pD2D1Device;
        Microsoft::WRL::ComPtr<ID2D1DeviceContext6> m_pD2D1DeviceContext;

        // Direct3D objects.
        Microsoft::WRL::ComPtr<ID3D11Device> m_pD3D11Device;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pD3D11DeviceContext;
        Microsoft::WRL::ComPtr<IDXGIDevice4> m_pDXGIDevice;
    };

	class Composition
	{
	public:
        Composition() = default;
        virtual ~Composition()
        {
          /*  wchar_t msg[128]{};
            swprintf_s(msg, L"~SwapChainAndComposition: % d\n", 1);
            OutputDebugString(msg);*/
        };

        Composition(Composition&&) = default;
        Composition& operator= (Composition&&) = default;

        Composition(Composition const&) = delete;
        Composition& operator= (Composition const&) = delete;

        void SetDeviceResources(const std::shared_ptr<UI::D11DeviceResources>& pD11Device)
        {
            m_pD3D11Device = pD11Device->GetID3D11Device();
            m_pD2D1Factory = pD11Device->GetID2D1Factory();
            m_pWICFactory = pD11Device->GetIWICFactory();
            m_pDWriteFactory = pD11Device->GetIDWriteFactory();
            m_pD2D1DeviceContext = pD11Device->GetID2D1DeviceContext();
            m_pDXGIDevice = pD11Device->GetIDXGIDevice();
        };

        void SetID3D11Device(const Microsoft::WRL::ComPtr<ID3D11Device>& pD3D11Device)
        {
            m_pD3D11Device = pD3D11Device;
        };

        void SetID2D1Factory(const Microsoft::WRL::ComPtr<ID2D1Factory7>& pD2D1Factory)
        {
            m_pD2D1Factory = pD2D1Factory;
        };

        void SetIWICFactory(const Microsoft::WRL::ComPtr<IWICImagingFactory2>& pWICFactory)
        {
            m_pWICFactory = pWICFactory;
        };

        void SetIDWriteFactory(const Microsoft::WRL::ComPtr<IDWriteFactory7>& pDWriteFactory)
        {
            m_pDWriteFactory = pDWriteFactory;
        };

        void SetID2D1DeviceContext(const Microsoft::WRL::ComPtr<ID2D1DeviceContext6>& pD2D1DeviceContext)
        {
            m_pD2D1DeviceContext = pD2D1DeviceContext;
        };

        void SetIDXGIDevice(const Microsoft::WRL::ComPtr<IDXGIDevice4>& pDXGIDevice)
        {
            m_pDXGIDevice = pDXGIDevice;
        };

        auto GetID2D1Factory()                   const noexcept { return m_pD2D1Factory; };
        auto GetWICFactory()                     const noexcept { return m_pWICFactory; };
        auto GetIDWriteFactory()                 const noexcept { return m_pDWriteFactory; };

        auto GetID2D1DeviceContext()             const noexcept { return m_pD2D1DeviceContext; };
        auto GetIDXGIDevice()                    const noexcept { return m_pDXGIDevice; };

        auto GetIDXGISwapChain()                 const noexcept { return m_pDXGISwapChain; };
        auto GetDCompDevice()                    const noexcept { return m_pDCompositionDevice; };


        void OnResize(HWND hwnd, UINT nWidth, UINT nHeight);

        HRESULT CreateSwapChain(HWND hwnd);
        HRESULT ConfigureSwapChain(HWND hwnd);
        HRESULT CreateDCompositionSwapChain(HWND hwnd);
        HRESULT CreateDComposition();

    private:
        // Factory.
        Microsoft::WRL::ComPtr<ID2D1Factory7> m_pD2D1Factory;
        Microsoft::WRL::ComPtr<IDWriteFactory7> m_pDWriteFactory;
        Microsoft::WRL::ComPtr<IWICImagingFactory2> m_pWICFactory;

        // Direct2D objects.
        Microsoft::WRL::ComPtr<ID2D1DeviceContext6> m_pD2D1DeviceContext;
        Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_pD2D1TargetBitmap;

        // Direct3D objects.
        Microsoft::WRL::ComPtr<ID3D11Device> m_pD3D11Device;
        Microsoft::WRL::ComPtr<IDXGIDevice4> m_pDXGIDevice;
        Microsoft::WRL::ComPtr<IDXGISwapChain1> m_pDXGISwapChain;
        Microsoft::WRL::ComPtr<IDCompositionDevice> m_pDCompositionDevice;
        Microsoft::WRL::ComPtr<IDCompositionTarget> m_pDCompositionTarget;

	};
}

#endif // !D3D11_DEVICE_RESOURCES_H