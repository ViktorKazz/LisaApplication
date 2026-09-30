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

#pragma once

#include "pch.h"
#include "D3D11DeviceResources.h"
#include "Caching.h"

namespace UI
{
    using CallWindow = std::function<HWND(
        const std::shared_ptr<UI::D11DeviceResources>&, 
        const std::shared_ptr<Caching>& pCaching, HWND, HINSTANCE)>;
    
    using CallCommand = std::function<void(const std::wstring&)>;

    template<typename T>
    struct ElementConfig
    {
        HINSTANCE hInstance{};
        std::wstring ClassName;
        std::wstring TitleName;
        RECT Rect{};
        HWND Parent{};
        FlagsConfig Flags;

        T Extra;
    };

    class WndInitialize
    {
    public:
        WndInitialize();

        WndInitialize(
            LONG left,
            LONG top,
            LONG right,
            LONG bottom,
            HINSTANCE hInstance,
            std::wstring windowClass,
            std::wstring windowTitle,
            UINT windowClassStyle,
            DWORD dwExStyle,
            DWORD dwStyle
        ) :
            m_left{ left },
            m_top{ top },
            m_right{ right },
            m_bottom{ bottom },
            m_hInstance{ hInstance },
            m_windowClass{ windowClass },
            m_windowTitle{ windowTitle },
            m_windowClassStyle{ windowClassStyle },
            m_dwExStyle{ dwExStyle },
            m_dwStyle{ dwStyle }
        {
        }

        virtual ~WndInitialize();

        // Accessors.
    
        HWND GetHwnd()                  const noexcept { return m_hwnd; };
        HINSTANCE GetHInstance()        const noexcept { return m_hInstance; };
        LONG GetLeft()                  const noexcept { return m_left; };
        LONG GetTop()                   const noexcept { return m_top; };
        LONG GetRight()                 const noexcept { return m_right; };
        LONG GetBottom()                const noexcept { return m_bottom; };
        LONG GetNonClientAreaSize()     const noexcept { return m_nonClientAreaSize; };

        auto GetWindowClass()           const noexcept { return m_windowClass; };
        auto GetWindowTitle()           const noexcept { return m_windowTitle; };

        auto GetD11Device()             const noexcept { return m_pD11Device; };
        auto GetComposition()           const noexcept { return m_pComposition.get(); };
        auto GetCaching()               const noexcept { return m_pCaching; };

        // For viewport procedure only.
        // Until I change the Viewport Window Proc to a new style.
        void SetHwnd(HWND hwnd) { m_hwnd = hwnd; };

        void SetLeft(LONG left)                        { m_left = left; };
        void SetTop(LONG top)                          { m_top = top; };
        void SetRight(LONG right)                      { m_right = right; };
        void SetBottom(LONG bottom)                    { m_bottom = bottom; };
        void SetNonClientAreaSize(UINT set)            { m_nonClientAreaSize = set; };

        void SetD11Device(const std::shared_ptr<D11DeviceResources>& device) { m_pD11Device = device; };
        void SetCaching(const std::shared_ptr<Caching>& caching) { m_pCaching = caching; };

        std::vector<HWND> FindChild(HWND hwnd);

        // Creates the application window and initializes.
        [[nodiscard]] HWND Initialize(
            const WndInitialize& object, 
            WNDPROC wndProc, 
            HWND parentHwnd = nullptr, 
            BOOL showWindow = true
        );

        ///virtual LRESULT CALLBACK MessageHandled(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;

        template<typename T>
        static LRESULT CALLBACK WindowProcL(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            LRESULT result{};
            T* t{ nullptr };

            if (message == WM_CREATE)
            {
                LPCREATESTRUCT pcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
                t = reinterpret_cast<T*>(pcs->lpCreateParams);

                ::SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(t));
                
                //UI::ThrowIfFailed(RoInitialize(RO_INIT_MULTITHREADED));

                // Initializing D3D11/D2D window resources.
                UI::ThrowIfFailed(t->GetComposition()->CreateSwapChain(NULL));
                UI::ThrowIfFailed(t->GetComposition()->ConfigureSwapChain(NULL));
                UI::ThrowIfFailed(t->GetComposition()->CreateDCompositionSwapChain(hwnd));
                //RoUninitialize();
                
                // Save the working HWND.
                t->m_hwnd = hwnd;
            }
            else
            {
                t = reinterpret_cast<T*>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA));

                if (t)
                    result = t->MessageHandled(message, wParam, lParam);
                else
                    result = DefWindowProc(hwnd, message, wParam, lParam);

                if (message == WM_DESTROY)
                {                  
                    if (t)
                    {
                        for (const auto& i : t->FindChild(t->m_hwnd))
                            SendMessageW(i, WM_DESTROY, 0, lParam);

                        delete t;
                        t = nullptr;
                        SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0); // Critically important!
                    }    
                }
            }

            return result;
        }

    private:
        LONG m_left{};
        LONG m_top{};
        LONG m_right{};
        LONG m_bottom{};

        HINSTANCE m_hInstance{ nullptr };

        std::wstring m_windowClass{};
        std::wstring m_windowTitle{};

        UINT m_windowClassStyle{};
        DWORD m_dwExStyle{};
        DWORD m_dwStyle{};

        HWND m_hwnd{ nullptr };
        LONG m_nonClientAreaSize{ gNonClientAreaSize };

    protected:
        std::shared_ptr<D11DeviceResources> m_pD11Device{ nullptr };
        std::shared_ptr<Caching> m_pCaching{ nullptr };
        std::unique_ptr<Composition> m_pComposition = std::make_unique<Composition>();
    };
}
