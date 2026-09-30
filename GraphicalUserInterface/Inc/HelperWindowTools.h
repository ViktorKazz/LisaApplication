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

#ifndef HELPER_WINDOW_TOOLS_H
#define HELPER_WINDOW_TOOLS_H

#include "pch.h"

class HelperWTools final
{
public:
	HelperWTools() = default;
	~HelperWTools() = default;

    // This function compares the size of the client area with the size of the monitor screen.
    bool ComparisonWindowSizes(HWND hwnd);

    void MinMaxWindow(LPARAM lParam, UINT minX, UINT minY);
    void ButtonDoubleClick(const HWND& hwnd, const LPARAM& lParam, INT minY, INT maxY, bool resize);
    LRESULT NCHitTest(const HWND& hwnd, const LPARAM& lParam, INT nonClientAreaSize, INT menuBarHeight, bool resize);

    template<typename T>
    T FindTextLength(
        const Microsoft::WRL::ComPtr<IDWriteFactory7>& pDWriteFactory,
        const Microsoft::WRL::ComPtr<IDWriteTextFormat>& pTextFormat,
        const wchar_t* text
    )
    {
        // In this case, CreateTextLayout is needed to determine the size of the characters, 
        // namely the width.

        UI::ThrowIfFailed(pDWriteFactory->CreateTextLayout(
            text,
            lstrlenW(text),
            pTextFormat.Get(),
            1000.0f,
            1000.0f,
            m_pTextLayout.ReleaseAndGetAddressOf()
        ));

        m_pTextLayout->GetMetrics(&m_textMetrics);

        // Add pixels for the right and left sides.
        float addPixels{ 10.0f };
        float ret = m_textMetrics.widthIncludingTrailingWhitespace + (addPixels * 2.0f);

        return static_cast<T>(ret);
    }

    bool SearchByOneKey(const wchar_t* source, const wchar_t* key);
    std::wstring GetKey(UI::ui_type type);
    std::wstring CreateClass(UI::ui_type type, const std::wstring& name);

    HRESULT LoadBitmapFromFile(
        const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget,
        const Microsoft::WRL::ComPtr<IWICImagingFactory2>& pWICFactory,
        ID2D1Bitmap** ppBitmap,
        PCWSTR uri,
        UINT destinationWidth,
        UINT destinationHeight
    );

    std::wstring FloatToWstring(std::float_t value);
    std::float_t WstringToFloat(const std::wstring& str);

    std::wstring DoubleToWstring(std::double_t value);
    std::double_t WstringToDouble(const std::wstring& str);

    void DrawFlipIcon(
        const Microsoft::WRL::ComPtr<ID2D1DeviceContext6>& pRenderTarget,
        ID2D1Image* input1,
        ID2D1Image* input2,
        D2D1_SIZE_F size,
        D2D1_SIZE_F scale,
        bool pressed
    );

private:
    // FindTextLength members. 
    DWRITE_TEXT_METRICS m_textMetrics{ 0 };
    Microsoft::WRL::ComPtr<IDWriteTextLayout> m_pTextLayout;
};

#endif // !HELPER_WINDOW_TOOLS_H