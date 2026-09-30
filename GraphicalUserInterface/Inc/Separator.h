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

#ifndef SEPARATOR_CLASS_H
#define SEPARATOR_CLASS_H

#include "WndInitialize.h"
#include "WindowTransformation.h"

namespace UI
{
    struct DrawnSeparatorConfig
    {
        bool test{};
    };

    class DrawnSeparator : public WndInitialize, public WindowTransformation
    {
    public:
        using WndInitialize::WndInitialize;
        ~DrawnSeparator() = default;


        template<typename T>
        void GetConfigure(const T& t)
        {
            m_parent = t.Parent;           
            m_type = t.Flags.Type;
            m_modes = t.Flags.Modes;
            m_transform = t.Flags.Transform;

            if (std::holds_alternative<ui_draw::separator>(t.Flags.Draw))
                m_draw = std::get<ui_draw::separator>(t.Flags.Draw);

            m_config = t.Extra;
        };

        LRESULT CALLBACK MessageHandled(UINT message, WPARAM wParam, LPARAM lParam);

        void PopUpSeparator(const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget, const D2D1_SIZE_F& size);

        HRESULT Draw();

    private:
        HWND m_parent{ nullptr };

        ui_type m_type;
        ui_modes m_modes;
        ui_draw::separator m_draw;
        ui_transform m_transform;
        
        DrawnSeparatorConfig m_config{};
    };


    struct SeparatorConfig
    {
        INT Indent{};
        INT FirstLimitation{};
        INT SecondLimitation{};
        bool SplitX{};
        bool SplitY{};
        ui_transform Transform{}; // RESTORE_LX or RESTORE_TY
    };

    class Separator : public WndInitialize, public WindowTransformation
    {
    public:
        using WndInitialize::WndInitialize;
        ~Separator() = default;


        template<typename T>
        void GetConfigure(const T& t)
        {
            m_parent = t.Parent;
            m_type = t.Flags.Type;
            m_modes = t.Flags.Modes;
            m_transform = t.Flags.Transform;

            if (std::holds_alternative<ui_draw::separator>(t.Flags.Draw))
                m_draw = std::get<ui_draw::separator>(t.Flags.Draw);
            
            m_config = t.Extra;

            if (m_config.SplitX)
                m_currentCursor = (HCURSOR)::LoadImageW(NULL, IDC_SIZEWE, IMAGE_CURSOR, 0, 0, LR_SHARED);
            else
                m_currentCursor = (HCURSOR)::LoadImageW(NULL, IDC_SIZENS, IMAGE_CURSOR, 0, 0, LR_SHARED);
        };

        void MovingSubstrate();

        LRESULT CALLBACK MessageHandled(UINT message, WPARAM wParam, LPARAM lParam);

        void DrawSeparator(const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget,
            const D2D1_SIZE_F& size, UINT pressing
        );

        HRESULT Draw();

    private:
        HWND m_parent{ nullptr };
        
        ui_type m_type;
        ui_modes m_modes;
        ui_draw::separator m_draw;
        ui_transform m_transform;
        
        SeparatorConfig m_config{};

        LONG m_parentWindowHeight{};
        LONG m_parentWindowWidth{};

        bool m_select{};
        bool m_pressing{};

        std::tuple<HWND, HWND, HWND, HWND> m_getHwndAdjacentElements{};
        // WM_WINDOWPOSCHANGING is executed before OBTAIN_HWND_OF_NEIGHBORING_SEPARATORS.
        // Thus, if you change the parent window before the first change of the separator, 
        // it will change its position to the value of the minimum indent.
        // With the help of this variable we will prevent this nuance.
        bool m_rewrite{};

        LONG m_difference{};
        LONG m_deltaBuffer{};

        HCURSOR m_currentCursor{ nullptr };
    };
}

#endif // !SEPARATOR_CLASS_H