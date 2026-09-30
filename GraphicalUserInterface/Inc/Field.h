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

#ifndef FIELD_CLASS_H
#define FIELD_CLASS_H

#include "WndInitialize.h"
#include "WindowTransformation.h"
#include "TextFieldTools.h"

namespace UI
{
    struct OutgoingData
    {
        UINT BelongingToAGroup{};
        UINT Purpose{};
        std::wstring Data{};
    };

    struct FieldConfig
    {
        DefaultValue Value;
        MinValue Min{};
        MaxValue Max{};
        UINT DecimalPlaces{};
        CallCommand Command;
    };

    class Field : public WndInitialize, public WindowTransformation
    {
    public:
        using WndInitialize::WndInitialize;
        ~Field() = default;

        // Accessors.

        template<typename T>
        void GetConfigure(const T& t)
        {
            m_parent = t.Parent;
            m_type = t.Flags.Type;
            m_modes = t.Flags.Modes;
            m_transform = t.Flags.Transform;

            if (std::holds_alternative<ui_draw::field>(t.Flags.Draw))
                m_draw = std::get<ui_draw::field>(t.Flags.Draw);

            m_config = t.Extra;

            m_pTextField->SetDefaultValue(m_config.Value, m_config.DecimalPlaces);
        };

        std::wstring SendFieldData(const FieldConfig& fConfig, const std::wstring& text, const std::wstring& buffer) const;

        LRESULT CALLBACK MessageHandled(UINT message, WPARAM wParam, LPARAM lParam);

        void DrawBackground(const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget, const D2D1_SIZE_F& size);
        D2D1_ROUNDED_RECT DefiningInputArea(const D2D1_SIZE_F& size, float indentLeftEdgeWindow);
        void DrawInputText(
            const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget,
            const Microsoft::WRL::ComPtr<IDWriteFactory7>& pDWriteFactory,
            const D2D1_SIZE_F& size,
            const OutputField& field,
            float indentLeftEdgeWindow
        );

        //void DrawSimpleFieldA(
        //    const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget,
        //    const Microsoft::WRL::ComPtr<IDWriteFactory7>& pDWriteFactory,
        //    const Microsoft::WRL::ComPtr<IWICImagingFactory2>& pWICFactory,
        //    ui_modes style,
        //    const D2D1_SIZE_F& size,
        //    const varimtx& imagePath,
        //    const OutputField& field
        //);

        void DrawSimpleField(
            const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget,
            const Microsoft::WRL::ComPtr<IDWriteFactory7>& pDWriteFactory,
            const D2D1_SIZE_F& size,
            const OutputField& field
        );

        void DrawHideField(
            const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget,
            const Microsoft::WRL::ComPtr<IDWriteFactory7>& pDWriteFactory,
            const D2D1_SIZE_F& size,
            const OutputField& field
        );

        HRESULT Draw();

    private:
        HelperWTools* m_tools{};

        std::unique_ptr<UI::TextField> m_pTextField = std::make_unique<UI::TextField>();
        
        bool m_select{};
        bool m_pressing{};

        HWND m_parent{ nullptr };
        
        ui_type m_type;
        ui_modes m_modes;
        ui_draw::field m_draw;
        ui_transform m_transform;
        
        FieldConfig m_config{};

        Microsoft::WRL::ComPtr<ID2D1Bitmap> m_pBitmap{ nullptr };
    };

}

#endif // !FIELD_CLASS_H
