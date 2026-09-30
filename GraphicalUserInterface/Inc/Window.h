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

#ifndef WINDOW_CLASS_H
#define WINDOW_CLASS_H

#include "WndInitialize.h"
#include "WindowTransformation.h"

namespace UI
{
    struct WindowConfig
    {
        HWND Root{ nullptr };
        varimtx ImgTxt;
        bool ShowTitle{};
        bool Resizable{};
    };

    class Window : public WndInitialize, public WindowTransformation
    {
    public:
        using WndInitialize::WndInitialize;
        virtual ~Window() {};

        // Accessors.

        template<typename T>
        void GetConfigure(const T& t)
        {
            m_parent = t.Parent;
            m_type = t.Flags.Type;
            m_modes = t.Flags.Modes;
            m_transform = t.Flags.Transform;

            if (std::holds_alternative<ui_draw::window>(t.Flags.Draw))
                m_draw = std::get<ui_draw::window>(t.Flags.Draw);
            
            m_config = t.Extra;
        };

        void StretchingChildrenElements(const HWND& hwnd);


        LRESULT CALLBACK MessageHandled(UINT message, WPARAM wParam, LPARAM lParam);
        LRESULT CALLBACK SimpleWindow(UINT message, WPARAM wParam, LPARAM lParam);
        LRESULT CALLBACK InbuiltWindow(UINT message, WPARAM wParam, LPARAM lParam);

        void DrawSimpleWindow(
            const Microsoft::WRL::ComPtr<ID2D1DeviceContext6>& pDeviceContext,
            const Microsoft::WRL::ComPtr<IWICImagingFactory2>& pWICFactory,
            const std::wstring& updateTitle,
            const D2D1_SIZE_F& size,
            LONG NonClientAreaSize
        );
        void DrawInbuiltWindow(const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget, const D2D1_SIZE_F& size);
        void DrawPopUpWindow(const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget, const D2D1_SIZE_F& size);

        HRESULT Draw();

        HRESULT CreateGridPatternBrush(
            ID2D1RenderTarget* pRenderTarget,
            ID2D1Bitmap** ppBitmapBrush
        );

    private:
        HelperWTools* m_tools{};
        std::vector<HWND> m_child{};

        std::wstring m_updateTitle{};
        INT m_friendliness{};

        LONG m_parentWindowHeight{};
        LONG m_parentWindowWidth{};
        LONG m_modifiableX{};

        HWND m_parent{ nullptr };

        ui_type m_type;
        ui_modes m_modes;
        ui_draw::window m_draw;
        ui_transform m_transform;

        WindowConfig m_config{};

        Microsoft::WRL::ComPtr<ID2D1Bitmap> m_pBitmap{ nullptr };
        Microsoft::WRL::ComPtr<ID2D1Effect> m_translationEffect{ nullptr };
        Microsoft::WRL::ComPtr<ID2D1Effect> m_scaleEffect{ nullptr };
    };
}

#endif // !WINDOW_CLASS_H