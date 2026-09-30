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

#include "..//Inc/Field.h"

// Function for sending data from the input field to the mediator window for further allocation.
std::wstring UI::Field::SendFieldData(const FieldConfig& fConfig, const std::wstring& text, const std::wstring& buffer) const
{
    std::wstring txt;

    // Update the text entered in the field.
    // For example, if the input field has a floating type, 
    // then instead of the entered .0123 it will be converted to 0.0123.

    // Also here the value entered in the input field is checked for a range.
    // The string value remains unchanged for now.

    using T = std::decay_t<decltype(fConfig.Value)>;

    T temp, min, max;

    if (std::holds_alternative<std::wstring>(fConfig.Value))
        return text;

    if (text == L"-" || text == L"." || text == L"-.")
    {
        return buffer;
    }

    // Convert from string to number only for comparison.

    if (std::holds_alternative<INT>(fConfig.Value))
    {
        temp = std::stoi(text);
        min = std::get<INT>(fConfig.Min);
        max = std::get<INT>(fConfig.Max);
    }
    else if (std::holds_alternative<UINT>(fConfig.Value))
    {
        // The unsigned long type has a size of 4 bytes, just like the unsigned integer.
        // Therefore, it was decided to use the std::stoul function.
        temp = std::stoul(text);
        min = std::get<UINT>(fConfig.Min);
        max = std::get<UINT>(fConfig.Max);
    }
    else if (std::holds_alternative<std::double_t>(fConfig.Value))
    {      
        temp = m_tools->WstringToDouble(text);
        min = std::get<std::double_t>(fConfig.Min);
        max = std::get<std::double_t>(fConfig.Max);
    }

    if (temp >= min && temp <= max)
    {
        // Give the meaning its proper form.
        // For example, for floating point numbers, 0.42 -> 0.420000, -.36 -> -0.360000, 1. -> 1.000000, etc.
        txt = m_pTextField->ProcessNumber(text, fConfig.Value, fConfig.DecimalPlaces);

        // The command call function takes responsibility for converting from string to number.
        if (m_config.Command)
            m_config.Command(txt);
    }
    else
    {
        txt = buffer;
    }

    return txt;
};

LRESULT CALLBACK UI::Field::MessageHandled(UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result{};
    PAINTSTRUCT ps;
    MouseTrackEvents track;

    switch (message)
    {
    case WM_TIMER:
    {
        // In order for the caret to disappear and reappear, 
        // we must fulfill the following function.

        m_pTextField->TimerForCaret(GetHwnd());
    }
    return 0;
    break;
    case WM_KILLFOCUS:
    {
        // Update the text entered in the field.
        // For example, if the input field has a floating type, 
        // then instead of the entered .0123 it will be converted to 0.0123.
        m_pTextField->SetText(SendFieldData(m_config,
            m_pTextField->OnDrawText().InputText,
            m_pTextField->GetBufferText()
        )
        );

        m_pTextField->DeactivateTextField(GetHwnd());
    }
    return 0;
    break;
    case WM_KEYDOWN:
    {
        m_pTextField->Typesetting(
            GetHwnd(), GetCaching()->GetVirtualKey(wParam, HIBYTE(GetKeyState(VK_SHIFT))), m_config.Value);
    }
    return 0;
    break;
    case WM_LBUTTONDOWN:
    {
        RECT rc; Error(GetClientRect(GetHwnd(), &rc));
        m_pressing = track.OnButtonDown(GetHwnd(), rc);

        if (m_draw & ui_draw::field::simple || m_draw & ui_draw::field::hide)
        {
            m_pTextField->LeftButtonClick(GetHwnd(), true);

            // Update the text entered in the field.
            // For example, if the input field has a floating type, 
            // then instead of the entered .0123 it will be converted to 0.0123.

            m_pTextField->SetText(
                // When the next input field is selected, the data from the previous input field will be sent to the intermediary window.
                SendFieldData(
                    m_config,
                    m_pTextField->OnDrawText().InputText,
                    m_pTextField->GetBufferText()
                )
            );
        }
        else if (m_draw & ui_draw::field::dubleclick)
        {
            // When pressed once, the text field is highlighted in color without the ability to type.
            //pWindow->SelectDubleClickTextField(hwnd);
        }
        else
            // Set the focus to the windows where other elements are drawn, 
            // so that when they are used, the text fields become inactive.
            SetFocus(GetHwnd());
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
    }
    return 0;
    break;
    case WM_MOUSELEAVE:
    {
        m_select = track.Reset(GetHwnd());
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

void UI::Field::DrawBackground(const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget, const D2D1_SIZE_F& size)
{
    D2D1_ROUNDED_RECT roundedRectButton{};
    roundedRectButton = D2D1::RoundedRect(
        D2D1::RectF(0.0f, 0.0f, size.width, size.height), 0.0f, 0.0f);

    pRenderTarget->FillRoundedRectangle(roundedRectButton, GetCaching()->GetExternalBackingBrush());
}

D2D1_ROUNDED_RECT UI::Field::DefiningInputArea(const D2D1_SIZE_F& size, float indentLeftEdgeWindow)
{
    return { D2D1::RectF(indentLeftEdgeWindow + 1.4f, 1.4f, size.width - 1.4f, size.height - 1.4f), 6.0f, 6.0f };
}

void UI::Field::DrawInputText(
    const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget,
    const Microsoft::WRL::ComPtr<IDWriteFactory7>& pDWriteFactory,
    const D2D1_SIZE_F& size,
    const OutputField& field,
    float indentLeftEdgeWindow
)
{
    Microsoft::WRL::ComPtr<IDWriteTextLayout> pTextLayoutEndCaret;
    Microsoft::WRL::ComPtr<IDWriteTextLayout> pTextLayoutBeginCaret;

    DWRITE_TEXT_METRICS textMetricsEndCaret = { 0 };
    DWRITE_TEXT_METRICS textMetricsBeginCaret = { 0 };

    float endCaretX{};
    float beginCaretX{};

    if (field.InputText.size())
    {
        float indent{ 8.0f };

        const float maxWidth{ indentLeftEdgeWindow + size.width - indent };
        const float maxHeight{ size.height - indent };

        // In this case, CreateTextLayout is needed to determine the size of the characters, 
        // namely the width.
        UI::ThrowIfFailed(pDWriteFactory->CreateTextLayout(
            field.InputText.c_str(),
            lstrlenW(field.InputText.c_str()) - (lstrlenW(field.InputText.c_str()) - static_cast<UINT32>(field.EndCaret)),
            GetCaching()->GetFieldTextFormat(),
            maxWidth,
            maxHeight,
            pTextLayoutEndCaret.ReleaseAndGetAddressOf()
        ));
        pTextLayoutEndCaret->GetMetrics(&textMetricsEndCaret);

        endCaretX = textMetricsEndCaret.widthIncludingTrailingWhitespace;

        UI::ThrowIfFailed(pDWriteFactory->CreateTextLayout(
            field.InputText.c_str(),
            lstrlenW(field.InputText.c_str()) - (lstrlenW(field.InputText.c_str()) - static_cast<UINT32>(field.BeginCaret)),
            GetCaching()->GetFieldTextFormat(),
            maxWidth,
            maxHeight,
            pTextLayoutBeginCaret.ReleaseAndGetAddressOf()
        ));
        pTextLayoutBeginCaret->GetMetrics(&textMetricsBeginCaret);

        beginCaretX = textMetricsBeginCaret.widthIncludingTrailingWhitespace;

        indent = 4.0f;

        const float addLeftRight{ indentLeftEdgeWindow + indent };
        const float bottom{ size.height - indent };

        // Highlight the text with color when the text field is activated.
        if (field.IsTextHighlighted)
        {
            pRenderTarget->FillRectangle(
                D2D1::RectF(
                    addLeftRight + endCaretX,
                    indent,
                    addLeftRight + beginCaretX,
                    bottom
                ),
                GetCaching()->GetFieldTextSelectionBrush()
            );
        }

        // Drawing text.            
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pText;

        if (field.IsSelection && !field.SetFocus)
            pText = GetCaching()->GetFieldTextDClickSelectionBrush();
        else
            pText = GetCaching()->GetFieldTextBrush();

        pRenderTarget->DrawTextW(
            field.InputText.c_str(),
            lstrlenW(field.InputText.c_str()),
            GetCaching()->GetFieldTextFormat(),
            D2D1::RectF(addLeftRight, indent, size.width - indent, bottom),
            pText.Get()
        );
    }

    // Drawing a caret.
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pCaretBrush{ GetCaching()->GetFieldCaretBrush() };
    pCaretBrush->SetOpacity(static_cast<float>(field.DrawingACaret));

    float indent{ 4.0f };
    const float point{ indentLeftEdgeWindow + indent + endCaretX };

    const D2D1_POINT_2F pointA{ point, indent };
    const D2D1_POINT_2F pointB{ point, size.height - indent };

    pRenderTarget->DrawLine(pointA, pointB, pCaretBrush.Get(), 1);
}

//void UI::Field::DrawSimpleFieldA(
//    const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget,
//    const Microsoft::WRL::ComPtr<IDWriteFactory7>& pDWriteFactory,
//    const Microsoft::WRL::ComPtr<IWICImagingFactory2>& pWICFactory,
//    ui_modes style,
//    const D2D1_SIZE_F& size,
//    const varimtx& imagePath,
//    const OutputField& field
//)
//{
//    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pBrush{};
//
//    D2D1_COLOR_F color{ D2D1::ColorF(UI::Colors::InternalBacking, 1.0f) };
//
//    UI::ThrowIfFailed(pRenderTarget->CreateSolidColorBrush(color, &pBrush));
//
//    D2D1_ROUNDED_RECT roundedRectButton{};
//    roundedRectButton = D2D1::RoundedRect(
//        D2D1::RectF(0.0f, 0.0f, size.width, size.height), 0.0f, 0.0f);
//
//    pRenderTarget->FillRoundedRectangle(roundedRectButton, pBrush.Get());
//
//    // Indent from the left edge of the window.
//    float indentLeftEdgeWindow{};
//    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pBackgroundQW;
//
//    if (style == DRAW::FIELD::DUBLECLICK)
//    {
//        indentLeftEdgeWindow = gIndentLeftEdgeWindow;
//
//        if (field.IsSelection)
//        {
//            UI::ThrowIfFailed(
//                pRenderTarget->CreateSolidColorBrush(
//                    D2D1::ColorF(UI::Colors::ItemSelected), pBackgroundQW.ReleaseAndGetAddressOf()));
//
//            pRenderTarget->FillRectangle(
//                D2D1::RectF(0.0f, 0.0f, size.width, size.height),
//                pBackgroundQW.Get()
//            );
//        }
//    }
//
//    // Drawing a input field.
//    // 
//    // Drawing the background for the text.
//    // An indent of two pixels is needed to neatly round the corners.          
//
//    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pBackground;
//
//    if (!field.SetFocus)
//    {
//        if (style == DRAW::FIELD::SIMPLE)
//            UI::ThrowIfFailed(
//                pRenderTarget->CreateSolidColorBrush(
//                    D2D1::ColorF(UI::Colors::FieldBackgroundA), pBackground.ReleaseAndGetAddressOf()));
//        else if (style == DRAW::FIELD::HIDE)
//            UI::ThrowIfFailed(
//                pRenderTarget->CreateSolidColorBrush(
//                    D2D1::ColorF(UI::Colors::InternalBacking), pBackground.ReleaseAndGetAddressOf()));
//        else if (style == DRAW::FIELD::DUBLECLICK)
//        {
//            if (field.IsSelection)
//                UI::ThrowIfFailed(
//                    pRenderTarget->CreateSolidColorBrush(
//                        D2D1::ColorF(UI::Colors::ItemSelected), pBackground.ReleaseAndGetAddressOf()));
//            else
//                UI::ThrowIfFailed(
//                    pRenderTarget->CreateSolidColorBrush(
//                        D2D1::ColorF(UI::Colors::InternalBacking), pBackground.ReleaseAndGetAddressOf()));
//        }
//    }
//    else
//        UI::ThrowIfFailed(
//            pRenderTarget->CreateSolidColorBrush(
//                D2D1::ColorF(UI::Colors::FieldBackgroundB), pBackground.ReleaseAndGetAddressOf()));
//
//
//    pRenderTarget->FillRoundedRectangle(
//        D2D1::RoundedRect(
//            D2D1::RectF(
//                indentLeftEdgeWindow + 1.4f, 1.4f, size.width - 1.4f, size.height - 1.4f),
//            6.f,
//            6.f
//        ),
//        pBackground.Get()
//    );
//
//    // If the text input field is not active, 
//    // it is not highlighted with a colored frame.
//    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pOutliner;
//
//    if (!field.SetFocus)
//    {
//        if (style == DRAW::FIELD::SIMPLE)
//            UI::ThrowIfFailed(
//                pRenderTarget->CreateSolidColorBrush(
//                    D2D1::ColorF(UI::Colors::FieldFrameNoActive), pOutliner.ReleaseAndGetAddressOf()));
//        else if (style == DRAW::FIELD::HIDE)
//            UI::ThrowIfFailed(
//                pRenderTarget->CreateSolidColorBrush(
//                    D2D1::ColorF(UI::Colors::InternalBacking), pOutliner.ReleaseAndGetAddressOf()));
//        else if (style == DRAW::FIELD::DUBLECLICK)
//        {
//            if (field.IsSelection)
//                UI::ThrowIfFailed(
//                    pRenderTarget->CreateSolidColorBrush(
//                        D2D1::ColorF(UI::Colors::ItemSelected), pOutliner.ReleaseAndGetAddressOf()));
//            else
//                UI::ThrowIfFailed(
//                    pRenderTarget->CreateSolidColorBrush(
//                        D2D1::ColorF(UI::Colors::InternalBacking), pOutliner.ReleaseAndGetAddressOf()));
//        }
//    }
//
//    // If a text input field is active, it is highlighted with a colored frame.
//    if (field.SetFocus)
//        UI::ThrowIfFailed(
//            pRenderTarget->CreateSolidColorBrush(
//                D2D1::ColorF(UI::Colors::FieldFrameActive), pOutliner.ReleaseAndGetAddressOf()));
//
//    pRenderTarget->DrawRoundedRectangle(
//        D2D1::RoundedRect(
//            D2D1::RectF(indentLeftEdgeWindow + 1.4f, 1.4f, size.width - 1.4f, size.height - 1.4f),
//            6.f,
//            6.f
//        ),
//        pOutliner.Get(), 1
//    );
//
//
//    // Creating text format.
//    Microsoft::WRL::ComPtr<IDWriteTextLayout> pTextLayoutEndCaret;
//    Microsoft::WRL::ComPtr<IDWriteTextLayout> pTextLayoutBeginCaret;
//
//    DWRITE_TEXT_METRICS textMetricsEndCaret = { 0 };
//    DWRITE_TEXT_METRICS textMetricsBeginCaret = { 0 };
//
//    float endCaretX{};
//    float beginCaretX{};
//
//    if (field.InputText.size())
//    {
//        Microsoft::WRL::ComPtr<IDWriteTextFormat> pTextFormat;
//
//        // Create a DirectWrite text format object.
//        UI::ThrowIfFailed(pDWriteFactory->CreateTextFormat(
//            L"Verdana",
//            NULL,
//            DWRITE_FONT_WEIGHT_MEDIUM,
//            DWRITE_FONT_STYLE_NORMAL,
//            DWRITE_FONT_STRETCH_NORMAL,
//            12,
//            L"", //locale
//            pTextFormat.ReleaseAndGetAddressOf()
//        ));
//
//        // Center the text horizontally and vertically and vertically.
//        pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
//        pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
//
//
//        // In this case, CreateTextLayout is needed to determine the size of the characters, 
//        // namely the width.
//        UI::ThrowIfFailed(pDWriteFactory->CreateTextLayout(
//            field.InputText.c_str(),
//            lstrlenW(field.InputText.c_str()) - (lstrlenW(field.InputText.c_str()) - static_cast<UINT32>(field.EndCaret)),
//            pTextFormat.Get(),
//            indentLeftEdgeWindow + size.width - 8.0f,
//            size.height - 8.0f,
//            pTextLayoutEndCaret.ReleaseAndGetAddressOf()
//        ));
//        pTextLayoutEndCaret->GetMetrics(&textMetricsEndCaret);
//
//        endCaretX = textMetricsEndCaret.widthIncludingTrailingWhitespace;
//
//        UI::ThrowIfFailed(pDWriteFactory->CreateTextLayout(
//            field.InputText.c_str(),
//            lstrlenW(field.InputText.c_str()) - (lstrlenW(field.InputText.c_str()) - static_cast<UINT32>(field.BeginCaret)),
//            pTextFormat.Get(),
//            indentLeftEdgeWindow + size.width - 8.0f,
//            size.height - 8.0f,
//            pTextLayoutBeginCaret.ReleaseAndGetAddressOf()
//        ));
//        pTextLayoutBeginCaret->GetMetrics(&textMetricsBeginCaret);
//
//        beginCaretX = textMetricsBeginCaret.widthIncludingTrailingWhitespace;
//
//        float indent{ 4.0f };
//
//        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pTextSelection;
//        UI::ThrowIfFailed(
//            pRenderTarget->CreateSolidColorBrush(
//                D2D1::ColorF(UI::Colors::TextSelection), pTextSelection.ReleaseAndGetAddressOf()));
//
//        // Highlight the text with color when the text field is activated.
//        if (field.IsTextHighlighted)
//        {
//            pRenderTarget->FillRectangle(
//                D2D1::RectF(
//                    indentLeftEdgeWindow + indent + endCaretX,
//                    indent,
//                    indentLeftEdgeWindow + indent + beginCaretX,
//                    size.height - indent
//                ),
//                pTextSelection.Get()
//            );
//        }
//
//        // Drawing text.            
//        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pText;
//        D2D1_COLOR_F text{};
//        if (field.IsSelection && !field.SetFocus)
//            text = { D2D1::ColorF(UI::Colors::ButtonTxtActive, 1.0f) };
//        else
//            text = { D2D1::ColorF(UI::Colors::Text, 1.0f) };
//
//        UI::ThrowIfFailed(pRenderTarget->CreateSolidColorBrush(text, pText.ReleaseAndGetAddressOf()));
//
//        pRenderTarget->DrawTextW(
//            field.InputText.c_str(),
//            lstrlenW(field.InputText.c_str()),
//            pTextFormat.Get(),
//            D2D1::RectF(
//                indentLeftEdgeWindow + indent, indent,
//                size.width - indent,
//                size.height - indent
//            ),
//            pText.Get()
//        );
//    }
//
//    // Drawing an image.
//    if (style == DRAW::FIELD::DUBLECLICK)
//    {
//        std::wstring img{};
//
//        // Using the helper function we get the paths to the images.
//        img = UI::ReadVar<std::wstring>(imagePath);
//
//        if (!img.empty())
//        {
//            HRESULT hr{ S_OK };
//            hr = m_tools->LoadBitmapFromFile(
//                pRenderTarget, pWICFactory, m_pBitmap.GetAddressOf(), img.c_str(), 0, 0);
//
//            if (SUCCEEDED(hr))
//            {
//                // Draw a bitmap.  indentLeftEdgeWindow
//                D2D1_SIZE_F sizeBitmap{ m_pBitmap->GetSize() };
//
//                pRenderTarget->DrawBitmap(
//                    m_pBitmap.Get(),
//                    D2D1::RectF(
//                        ((indentLeftEdgeWindow + 2.0f) - sizeBitmap.width),
//                        2.0f,
//                        (indentLeftEdgeWindow - 2.0f),
//                        (sizeBitmap.height - 4.0f)
//                    )
//                );
//
//                m_pBitmap.Reset();
//            }
//        }
//    }
//
//    // Drawing a caret.
//    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pCaretBrush;
//
//    UI::ThrowIfFailed(pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(UI::Colors::Caret, static_cast<float>(field.DrawingACaret)),
//        pCaretBrush.ReleaseAndGetAddressOf()));
//
//    float indent{ 4.0f };
//
//    D2D1_POINT_2F pointA{ indentLeftEdgeWindow + indent + endCaretX, indent };
//    D2D1_POINT_2F pointB{ indentLeftEdgeWindow + indent + endCaretX, size.height - indent };
//
//    pRenderTarget->DrawLine(pointA, pointB, pCaretBrush.Get(), 1);
//}

void UI::Field::DrawSimpleField(
    const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget,
    const Microsoft::WRL::ComPtr<IDWriteFactory7>& pDWriteFactory,
    const D2D1_SIZE_F& size,
    const OutputField& field
)
{
    // Draw the main background.
    //DrawBackground(pRenderTarget, size);

    // Drawing a input field.
    // 
    // Drawing the background for the text.
    // An indent of two pixels is needed to neatly round the corners.          

    // Indent from the left edge of the window.
    float indentLeftEdgeWindow{};
    const D2D1_ROUNDED_RECT fieldRect{ DefiningInputArea(size, indentLeftEdgeWindow) };

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pBackground;

    if (!field.SetFocus)
        pBackground = GetCaching()->GetFieldBackgroundABrush();    
    else
        pBackground = GetCaching()->GetFieldBackgroundBBrush();

    pRenderTarget->FillRoundedRectangle(fieldRect, pBackground.Get());

    // If the text input field is not active, 
    // it is not highlighted with a colored frame.
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pOutliner;

    if (!field.SetFocus)
        pOutliner = GetCaching()->GetFieldFrameNoActiveBrush();      
    // If a text input field is active, it is highlighted with a colored frame.
    else if (field.SetFocus)
        pOutliner = GetCaching()->GetFieldFrameActiveBrush();

    pRenderTarget->DrawRoundedRectangle(fieldRect, pOutliner.Get(), 1);

    // Creating text format.
    DrawInputText(pRenderTarget, pDWriteFactory, size, field, indentLeftEdgeWindow);
}

void UI::Field::DrawHideField(
    const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget,
    const Microsoft::WRL::ComPtr<IDWriteFactory7>& pDWriteFactory,
    const D2D1_SIZE_F& size,
    const OutputField& field
)
{
    // Draw the main background.
    //DrawBackground(pRenderTarget, size);

    // Drawing a input field.
    // 
    // Drawing the background for the text.
    // An indent of two pixels is needed to neatly round the corners.          

    // Indent from the left edge of the window.
    float indentLeftEdgeWindow{};
    const D2D1_ROUNDED_RECT fieldRect{ DefiningInputArea(size, indentLeftEdgeWindow) };

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pBackground;

    if (!field.SetFocus)
        pBackground = GetCaching()->GetExternalBackingBrush();
    else
        pBackground = GetCaching()->GetFieldBackgroundBBrush();

    pRenderTarget->FillRoundedRectangle(fieldRect, pBackground.Get());

    // If the text input field is not active, 
    // it is not highlighted with a colored frame.
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pOutliner;

    if (!field.SetFocus)
        pOutliner = GetCaching()->GetExternalBackingBrush();
    // If a text input field is active, it is highlighted with a colored frame.
    else if (field.SetFocus)
        pOutliner = GetCaching()->GetFieldFrameActiveBrush();

    pRenderTarget->DrawRoundedRectangle(fieldRect, pOutliner.Get(), 1);

    // Creating text format.
    DrawInputText(pRenderTarget, pDWriteFactory, size, field, indentLeftEdgeWindow);
}

HRESULT UI::Field::Draw()
{
    HRESULT hr{ S_OK };

    // D2D1DeviceContext needs to be updated.
    GetComposition()->ConfigureSwapChain(GetHwnd());

    auto deviceContext = GetComposition()->GetID2D1DeviceContext();
    //auto factory = GetResourcesUI()->GetID2D1Factory();
    auto writeFactory = GetComposition()->GetIDWriteFactory();
    auto WICFactory = GetComposition()->GetWICFactory();
    auto swapChain = GetComposition()->GetIDXGISwapChain();

    if (deviceContext && swapChain)
    {
        D2D1_SIZE_F size = deviceContext->GetSize();

        deviceContext->BeginDraw();
        deviceContext->Clear();

        switch (m_draw)
        {
        case ui_draw::field::simple:
            DrawSimpleField(deviceContext, writeFactory, size, m_pTextField->OnDrawText());
            break;
        case ui_draw::field::hide:
            DrawHideField(deviceContext, writeFactory, size, m_pTextField->OnDrawText());
            break;
        case ui_draw::field::dubleclick:
            
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