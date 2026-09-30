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

#include "..//Inc/Text.h"

LRESULT CALLBACK UI::Text::MessageHandled(UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result{};
    PAINTSTRUCT ps;
    MouseTrackEvents track;

    switch (message)
    {
    case WM_LBUTTONDOWN:
    {
        RECT rc; Error(GetClientRect(GetHwnd(), &rc));
        m_pressing = track.OnButtonDown(GetHwnd(), rc);

        // Determine the places where, when clicked with a mouse, the images will change.
        RECT backward{ rc }, forward{ rc };

        backward.right = backward.left + gFlipPlace;
        forward.left = forward.right - gFlipPlace;

        if (track.ElementSelected(GetHwnd(), backward, lParam))
        {
            m_index--;
            m_index = std::clamp(m_index, 0, (static_cast<INT>(m_numberOfLayers) - 1));
        }
        else if (track.ElementSelected(GetHwnd(), forward, lParam))
        {
            m_index++;
            m_index = std::clamp(m_index, 0, (static_cast<INT>(m_numberOfLayers) - 1));
        }

    }
    return 0;
    break;
    case WM_LBUTTONUP:
    {
        RECT rc; Error(GetClientRect(GetHwnd(), &rc));
        m_pressing = track.OnButtonUp(GetHwnd(), rc);
    }
    return 0;
    break;
    case WM_MOUSEMOVE:
    {
        RECT rc; Error(GetClientRect(GetHwnd(), &rc));
        m_select = track.OnMouseMove(GetHwnd(), rc, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

        // Using the auxiliary function we get the number of layers.
        m_numberOfLayers = UI::ReadVar<size_t>(m_config.ImgTxt);

        RECT backward{ rc }, center{ rc }, forward{ rc };

        backward.right = backward.left + gFlipPlace;
        center.left = center.left + gFlipPlace;
        center.right = center.right - gFlipPlace;
        forward.left = forward.right - gFlipPlace;

        if (track.ElementSelected(GetHwnd(), backward, lParam))
            m_flipPlaceSelect = -1;
        else if (track.ElementSelected(GetHwnd(), center, lParam))
            m_flipPlaceSelect = 0;
        else if (track.ElementSelected(GetHwnd(), forward, lParam))
            m_flipPlaceSelect = 1;
    }
    return 0;
    break;
    case WM_MOUSELEAVE:
    {
        m_select = track.Reset(GetHwnd());
        m_flipPlaceSelect = m_select;
    }
    return 0;
    break;
    case WM_COMMAND:
    {
        UINT wmId = LOWORD(wParam);

        switch (wmId)
        {
        case 0:
        {
        }
        break;
        default:
        {
            return DefWindowProc(GetHwnd(), message, wParam, lParam);
        }
        break;
        }
    }
    break;
    case WM_SIZE:
    {
        UINT nWidth = GET_X_LPARAM(lParam);
        UINT nHeight = GET_Y_LPARAM(lParam);
        GetComposition()->OnResize(GetHwnd(), nWidth, nHeight);
    }
    result = 0;
    break;
    case WM_PAINT:
    case WM_DISPLAYCHANGE:
    {
        BeginPaint(GetHwnd(), &ps);
        UI::ThrowIfFailed(Draw());
        EndPaint(GetHwnd(), &ps);
    }
    result = 0;
    break;
    case WM_DESTROY:
    {
        return 0;
    }
    break;
    default:
    {
        return DefWindowProc(GetHwnd(), message, wParam, lParam);
    }
    break;
    }

    return result;
}

void UI::Text::DrawingSimpleText(
    const Microsoft::WRL::ComPtr<ID2D1DeviceContext6>& pDeviceContext,
    const Microsoft::WRL::ComPtr<IDWriteFactory7>& pDWriteFactory,
    const varimtx& inputText,
    const D2D1_SIZE_F& size,
    size_t index
)
{
    // Using the helper function we will get the text.
    UI::FontConfig txt = UI::ReadVar<UI::FontConfig>(inputText, index);

    // Creating text format.
    Microsoft::WRL::ComPtr<IDWriteTextFormat> pTextFormat;

    // Create a DirectWrite text format object.
    UI::ThrowIfFailed(pDWriteFactory->CreateTextFormat(
        txt.FontName,
        NULL,
        txt.FontWeight,
        txt.FontStyle,
        txt.FontStretch,
        txt.FontSize,
        L"", //locale
        &pTextFormat
    ));

    // Center the text horizontally and vertically and vertically.
    pTextFormat->SetTextAlignment(txt.TextAlignment);
    pTextFormat->SetParagraphAlignment(txt.ParagraphAlignment);

    // Drawing text.

    if (txt.Input)
    {
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pFontBrush;
        D2D1_COLOR_F color = D2D1::ColorF(txt.FontRGB, 1.0f);
        UI::ThrowIfFailed(pDeviceContext->CreateSolidColorBrush(color, &m_pFontBrush));

        pDeviceContext->DrawText(
            txt.Input, 
            lstrlen(txt.Input), 
            pTextFormat.Get(), 
            {0.0f, 0.0f, size.width, size.height }, 
            m_pFontBrush.Get()
        );
    }
    
    // Drawing the flip area.

    // Left side.
    if (index && m_flipPlaceSelect == -1)
    {
        m_tools->DrawFlipIcon(pDeviceContext, GetCaching()->GetIconButtonFlipPressed(), GetCaching()->GetIconButtonFlipSelected(),
            { 0.0f, (size.height / 2) - (gFlipPlace / 2) }, { 1.0f, 1.0f }, m_pressing);
    }

    // Right side.
    if (index < (m_numberOfLayers - 1) && m_flipPlaceSelect == 1)
    {        
        m_tools->DrawFlipIcon(pDeviceContext, GetCaching()->GetIconButtonFlipPressed(), GetCaching()->GetIconButtonFlipSelected(),
            { size.width, (size.height / 2) - (gFlipPlace / 2) }, { -1.0f, 1.0f }, m_pressing);
    }
}

void UI::Text::DrawingLabelText(
    const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget, 
    const varimtx& inputText, 
    const D2D1_SIZE_F& size, 
    size_t index
)
{
    // Using the helper function we will get the text.
    UI::FontConfig txt = UI::ReadVar<UI::FontConfig>(inputText, index);

    if (txt.Input)
    {
        pRenderTarget->DrawTextW(
            txt.Input,
            lstrlen(txt.Input),
            GetCaching()->GetLabelTextFormat(),
            D2D1::RectF(0.0f, 0.0f, size.width, size.height),
            GetCaching()->GetFieldTextBrush()
        );
    }
}

HRESULT UI::Text::Draw()
{
    HRESULT hr{ S_OK };

    // D2D1DeviceContext needs to be updated.
    GetComposition()->ConfigureSwapChain(GetHwnd());

    auto deviceContext = GetComposition()->GetID2D1DeviceContext();
    auto writeFactory = GetComposition()->GetIDWriteFactory();
    auto factory = GetComposition()->GetID2D1Factory();
    auto swapChain = GetComposition()->GetIDXGISwapChain();

    if (deviceContext && swapChain)
    {
        D2D1_SIZE_F size = deviceContext->GetSize();

        deviceContext->BeginDraw();
        deviceContext->Clear();

        switch (m_draw)
        {
        case ui_draw::text::simple:
            DrawingSimpleText(deviceContext, writeFactory, m_config.ImgTxt, size, m_index);
            break;   
        case ui_draw::text::label:
            DrawingLabelText(deviceContext, m_config.ImgTxt, size, m_index);
            break;
        default:
            throw std::runtime_error("Drawing function not found.");
            break;
        }

        hr = deviceContext->EndDraw();
        // Make the swap chain available to the composition engine.
        hr = swapChain->Present(0, 0);
    }

    return hr;
}