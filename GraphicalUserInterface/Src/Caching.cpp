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

#include "..//Inc/Caching.h"
#include "..//Inc/Primitives2D.h"

// Function for writing virtual key codes to the unordered map.
void UI::Caching::VirtualKeyCodes()
{
    m_virtualKey.insert({ { 0xC0, false }, L"`" });
    m_virtualKey.insert({ { 0xC0, true }, L"~" });

    // Numbers. Without pressing the Shift key.
    m_virtualKey.insert({ { 0x30, false }, L"0" });
    m_virtualKey.insert({ { 0x31, false }, L"1" });
    m_virtualKey.insert({ { 0x32, false }, L"2" });
    m_virtualKey.insert({ { 0x33, false }, L"3" });
    m_virtualKey.insert({ { 0x34, false }, L"4" });
    m_virtualKey.insert({ { 0x35, false }, L"5" });
    m_virtualKey.insert({ { 0x36, false }, L"6" });
    m_virtualKey.insert({ { 0x37, false }, L"7" });
    m_virtualKey.insert({ { 0x38, false }, L"8" });
    m_virtualKey.insert({ { 0x39, false }, L"9" });

    // Numbers. With the Shift key pressed.
    m_virtualKey.insert({ { 0x30, true }, L")" });
    m_virtualKey.insert({ { 0x31, true }, L"!" });
    m_virtualKey.insert({ { 0x32, true }, L"@" });
    m_virtualKey.insert({ { 0x33, true }, L"#" });
    m_virtualKey.insert({ { 0x34, true }, L"$" });
    m_virtualKey.insert({ { 0x35, true }, L"%" });
    m_virtualKey.insert({ { 0x36, true }, L"^" });
    m_virtualKey.insert({ { 0x37, true }, L"&" });
    m_virtualKey.insert({ { 0x38, true }, L"*" });
    m_virtualKey.insert({ { 0x39, true }, L"(" });

    // Simbols. Without pressing the Shift key.
    m_virtualKey.insert({ { 0x41, false }, L"a" });
    m_virtualKey.insert({ { 0x42, false }, L"b" });
    m_virtualKey.insert({ { 0x43, false }, L"c" });
    m_virtualKey.insert({ { 0x44, false }, L"d" });
    m_virtualKey.insert({ { 0x45, false }, L"e" });
    m_virtualKey.insert({ { 0x46, false }, L"f" });
    m_virtualKey.insert({ { 0x47, false }, L"g" });
    m_virtualKey.insert({ { 0x48, false }, L"h" });
    m_virtualKey.insert({ { 0x49, false }, L"i" });
    m_virtualKey.insert({ { 0x4A, false }, L"j" });
    m_virtualKey.insert({ { 0x4B, false }, L"k" });
    m_virtualKey.insert({ { 0x4C, false }, L"l" });
    m_virtualKey.insert({ { 0x4D, false }, L"m" });
    m_virtualKey.insert({ { 0x4E, false }, L"n" });
    m_virtualKey.insert({ { 0x4F, false }, L"o" });
    m_virtualKey.insert({ { 0x50, false }, L"p" });
    m_virtualKey.insert({ { 0x51, false }, L"q" });
    m_virtualKey.insert({ { 0x52, false }, L"r" });
    m_virtualKey.insert({ { 0x53, false }, L"s" });
    m_virtualKey.insert({ { 0x54, false }, L"t" });
    m_virtualKey.insert({ { 0x55, false }, L"u" });
    m_virtualKey.insert({ { 0x56, false }, L"v" });
    m_virtualKey.insert({ { 0x57, false }, L"w" });
    m_virtualKey.insert({ { 0x58, false }, L"x" });
    m_virtualKey.insert({ { 0x59, false }, L"y" });
    m_virtualKey.insert({ { 0x5A, false }, L"z" });

    // Simbols. With the Shift key pressed.
    m_virtualKey.insert({ { 0x41, true }, L"A" });
    m_virtualKey.insert({ { 0x42, true }, L"B" });
    m_virtualKey.insert({ { 0x43, true }, L"C" });
    m_virtualKey.insert({ { 0x44, true }, L"D" });
    m_virtualKey.insert({ { 0x45, true }, L"E" });
    m_virtualKey.insert({ { 0x46, true }, L"F" });
    m_virtualKey.insert({ { 0x47, true }, L"G" });
    m_virtualKey.insert({ { 0x48, true }, L"H" });
    m_virtualKey.insert({ { 0x49, true }, L"I" });
    m_virtualKey.insert({ { 0x4A, true }, L"J" });
    m_virtualKey.insert({ { 0x4B, true }, L"K" });
    m_virtualKey.insert({ { 0x4C, true }, L"L" });
    m_virtualKey.insert({ { 0x4D, true }, L"M" });
    m_virtualKey.insert({ { 0x4E, true }, L"N" });
    m_virtualKey.insert({ { 0x4F, true }, L"O" });
    m_virtualKey.insert({ { 0x50, true }, L"P" });
    m_virtualKey.insert({ { 0x51, true }, L"Q" });
    m_virtualKey.insert({ { 0x52, true }, L"R" });
    m_virtualKey.insert({ { 0x53, true }, L"S" });
    m_virtualKey.insert({ { 0x54, true }, L"T" });
    m_virtualKey.insert({ { 0x55, true }, L"U" });
    m_virtualKey.insert({ { 0x56, true }, L"V" });
    m_virtualKey.insert({ { 0x57, true }, L"W" });
    m_virtualKey.insert({ { 0x58, true }, L"X" });
    m_virtualKey.insert({ { 0x59, true }, L"Y" });
    m_virtualKey.insert({ { 0x5A, true }, L"Z" });

    // Simbols. Without pressing the Shift key.
    m_virtualKey.insert({ { 0xBD, false }, L"-" });
    m_virtualKey.insert({ { 0xBB, false }, L"=" });
    m_virtualKey.insert({ { 0xDB, false }, L"[" });
    m_virtualKey.insert({ { 0xDD, false }, L"]" });
    m_virtualKey.insert({ { 0xBA, false }, L";" });
    m_virtualKey.insert({ { 0xDE, false }, L"'" });
    m_virtualKey.insert({ { 0xDC, false }, L"\\" });
    m_virtualKey.insert({ { 0xBC, false }, L"," });
    m_virtualKey.insert({ { 0xBE, false }, L"." });
    m_virtualKey.insert({ { 0xBF, false }, L"/" });

    // Simbols. With the Shift key pressed.
    m_virtualKey.insert({ { 0xBD, true }, L"_" });
    m_virtualKey.insert({ { 0xBB, true }, L"+" });
    m_virtualKey.insert({ { 0xDB, true }, L"{" });
    m_virtualKey.insert({ { 0xDD, true }, L"}" });
    m_virtualKey.insert({ { 0xBA, true }, L":" });
    m_virtualKey.insert({ { 0xDE, true }, L"\"" });
    m_virtualKey.insert({ { 0xDC, true }, L"|" });
    m_virtualKey.insert({ { 0xBC, true }, L"<" });
    m_virtualKey.insert({ { 0xBE, true }, L">" });
    m_virtualKey.insert({ { 0xBF, true }, L"?" });

    // Numbers(Numeric keypad). 
    // Without pressing the Shift key.
    m_virtualKey.insert({ { 0x60, false }, L"0" });
    m_virtualKey.insert({ { 0x61, false }, L"1" });
    m_virtualKey.insert({ { 0x62, false }, L"2" });
    m_virtualKey.insert({ { 0x63, false }, L"3" });
    m_virtualKey.insert({ { 0x64, false }, L"4" });
    m_virtualKey.insert({ { 0x65, false }, L"5" });
    m_virtualKey.insert({ { 0x66, false }, L"6" });
    m_virtualKey.insert({ { 0x67, false }, L"7" });
    m_virtualKey.insert({ { 0x68, false }, L"8" });
    m_virtualKey.insert({ { 0x69, false }, L"9" });
    m_virtualKey.insert({ { 0x6A, false }, L"*" });
    m_virtualKey.insert({ { 0x6B, false }, L"+" });
    //m_virtualKey.insert({ { 0x6C, false }, L"."});
    m_virtualKey.insert({ { 0x6D, false }, L"-" });
    m_virtualKey.insert({ { 0x6E, false }, L"." });
    m_virtualKey.insert({ { 0x6F, false }, L"/" });

    m_virtualKey.insert({ { 0x20, false }, L" " });
}

void UI::Caching::Brushes(const std::shared_ptr<UI::D11DeviceResources>& pDevice)
{
	auto deviceContext = pDevice->GetID2D1DeviceContext();

    using namespace D2D1;
    using namespace UI::Colors;

	UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(InternalBacking, 1.0f), &m_pInternalBacking));
	UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ExternalBacking, 1.0f), &m_pExternalBacking));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(IntermediateOutline, 1.0f), &m_pIntermediateOutline));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(Title, 1.0f), &m_pTitle));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(Shadow, 1.0f), &m_pShadow));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(PopUp, 1.0f), &m_pPopUp));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(PopUpFrame, 1.0f), &m_pPopUpFrame));

    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(FieldBackgroundA, 1.0f), &m_pFieldBackgroundA));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(FieldBackgroundB, 1.0f), &m_pFieldBackgroundB));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(FieldFrameNoActive, 1.0f), &m_pFieldFrameNoActive));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(FieldFrameActive, 1.0f), &m_pFieldFrameActive));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ItemSelected, 1.0f), &m_pItemSelected));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(TextDClickSelection, 1.0f), &m_pTextDClickSelection));

    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(TextSelection, 1.0f), &m_pTextSelection));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(Text, 1.0f), &m_pText));
	UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(Caret, 1.0f), &m_pCaret));

    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonNoActive, 1.0f), &m_pButtonNoActive));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonPreActive, 1.0f), &m_pButtonPreActive));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonActive, 1.0f), &m_pButtonActive));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonMinimize, 1.0f), &m_pButtonMinimize));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonMinimizeOutline, 1.0f), &m_pButtonMinimizeOutline));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonRestore, 1.0f), &m_pButtonRestore));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonRestoreOutline, 1.0f), &m_pButtonRestoreOutline));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonClose, 1.0f), &m_pButtonClose));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonCloseOutline, 1.0f), &m_pButtonCloseOutline));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonNoActiveNF, 1.0f), &m_pButtonNoActiveNF));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonPreActiveNF, 1.0f), &m_pButtonPreActiveNF));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonActiveNF, 1.0f), &m_pButtonActiveNF));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonNoActivePopUp, 1.0f), &m_pButtonNoActivePopUp));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonPreActivePopUp, 1.0f), &m_pButtonPreActivePopUp));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonActivePopUp, 1.0f), &m_pButtonActivePopUp));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonTxtNoActive, 1.0f), &m_pButtonTxtNoActive));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonTxtNFNoActive, 1.0f), &m_pButtonTxtNFNoActive));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonTxtPreActive, 1.0f), &m_pButtonTxtPreActive));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonTxtActive, 1.0f), &m_pButtonTxtActive));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonFrameOutlinerNoActive, 1.0f), &m_pButtonFrameOutlinerNoActive));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonFrameOutlinerPreActive, 1.0f), &m_pButtonFrameOutlinerPreActive));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonFrameOutlinerActive, 1.0f), &m_pButtonFrameOutlinerActive));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonNFrameOutlinerNoActive, 1.0f), &m_pButtonNFrameOutlinerNoActive));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonNFrameOutlinerPreActive, 1.0f), &m_pButtonNFrameOutlinerPreActive));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonNFrameOutlinerActive, 1.0f), &m_pButtonNFrameOutlinerActive));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonPFrameOutlinerNoActive, 1.0f), &m_pButtonPFrameOutlinerNoActive));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonPFrameOutlinerPreActive, 1.0f), &m_pButtonPFrameOutlinerPreActive));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(ButtonPFrameOutlinerActive, 1.0f), &m_pButtonPFrameOutlinerActive));

    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(FlipAreaEllipse, 1.0f), &m_pFlipAreaEllipse));
    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(FlipAreaTriangle, 1.0f), &m_pFlipAreaTriangle));

    UI::ThrowIfFailed(deviceContext->CreateSolidColorBrush(ColorF(Border, 1.0f), &m_pBorder));
}

void UI::Caching::FlipGradientStopCollection(const std::shared_ptr<UI::D11DeviceResources>& pDevice)
{
    auto deviceContext = pDevice->GetID2D1DeviceContext();

    // Create an array of gradient stops to put in the gradient stop
    // collection that will be used in the gradient brush.

    D2D1_GRADIENT_STOP gradientStops[3]{};
    gradientStops[0].color = D2D1::ColorF(Palette::CarouselPink, 0.0f);
    gradientStops[0].position = 0.0f;
    gradientStops[1].color = D2D1::ColorF(Palette::CarouselPink, 0.3f);
    gradientStops[1].position = 0.5f;
    gradientStops[2].color = D2D1::ColorF(Palette::CarouselPink, 1.0f);
    gradientStops[2].position = 1.0f;

    HRESULT hr{ S_OK };

    // Create the ID2D1GradientStopCollection from a previously
    // declared array of D2D1_GRADIENT_STOP structs.
    hr = deviceContext->CreateGradientStopCollection(
        gradientStops,
        3,
        D2D1_GAMMA_2_2,
        D2D1_EXTEND_MODE_CLAMP,
        m_pFlipGradientStops.GetAddressOf()
    );
}

void UI::Caching::CreateTextFormat(const Microsoft::WRL::ComPtr<IDWriteFactory7>& pDWriteFactory)
{
    auto writeFactory{ pDWriteFactory };

    auto TextFormat = [&]() -> Microsoft::WRL::ComPtr<IDWriteTextFormat>
        {
            Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat;

            UI::ThrowIfFailed(writeFactory->CreateTextFormat(
                L"Verdana",
                NULL,
                DWRITE_FONT_WEIGHT_MEDIUM,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                12,
                L"", //locale
                textFormat.ReleaseAndGetAddressOf()
            ));

            return textFormat;
        };

    m_pTitleTextFormat = TextFormat();
    m_pTitleTextFormat.Get()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
    m_pTitleTextFormat.Get()->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    m_pLabelTextFormat = TextFormat();
    m_pLabelTextFormat.Get()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
    m_pLabelTextFormat.Get()->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    m_pFieldTextFormat = TextFormat();
    m_pFieldTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    m_pFieldTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

    m_pButtonTextFormat = TextFormat();
    m_pButtonTextFormat.Get()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    m_pButtonTextFormat.Get()->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    m_pButtonPopUpTextFormat = TextFormat();
    m_pButtonPopUpTextFormat.Get()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    m_pButtonPopUpTextFormat.Get()->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    m_pButtonPopUpExtraTextFormat = TextFormat();
    m_pButtonPopUpExtraTextFormat.Get()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
    m_pButtonPopUpExtraTextFormat.Get()->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}

void UI::Caching::CreateIconBitmap(const std::shared_ptr<UI::D11DeviceResources>& pDevice)
{
    ThrowIfFailed(CreateIconClose(pDevice));
    ThrowIfFailed(CreateIconMaximizeRestore(pDevice));
    ThrowIfFailed(CreateIconMinimize(pDevice));
    ThrowIfFailed(CreateIconGear(pDevice));
    ThrowIfFailed(CreateIconArrow(pDevice));
    ThrowIfFailed(CreateIconFlip(pDevice));
}

HRESULT UI::Caching::CreateIconClose(const std::shared_ptr<UI::D11DeviceResources>& pDevice)
{
    const auto& deviceContext = pDevice->GetID2D1DeviceContext();

    const float& width = static_cast<FLOAT>(gButtonWidth);
    const float& height = static_cast<FLOAT>(gButtonHeight);
    const D2D1_POINT_2F& center{ width / 2.0f, height / 2.0f };

    const float coefficient{ 3.828571f };
    const float radius{ height / coefficient };

    // Create a compatible render target.
    Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> pCompatibleRenderTarget;

    HRESULT hr = S_OK;

    hr = deviceContext->CreateCompatibleRenderTarget(
        D2D1::SizeF(width, height),
        D2D1::SizeU(gButtonWidth, gButtonHeight),
        m_pixelFormat,
        pCompatibleRenderTarget.GetAddressOf()
    );

    if (SUCCEEDED(hr))
    {
        pCompatibleRenderTarget->BeginDraw();
        //pCompatibleRenderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(center.x, center.y), radius, radius), GetButtonCloseOutline(), 1);
        pCompatibleRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(center.x, center.y), radius, radius), GetButtonClose());
        pCompatibleRenderTarget->EndDraw();

        // Retrieve the bitmap from the render target.
        hr = pCompatibleRenderTarget->GetBitmap(m_pIconButtonClose.GetAddressOf());
        pCompatibleRenderTarget.Reset();
    }

    return hr;
}

HRESULT UI::Caching::CreateIconMaximizeRestore(const std::shared_ptr<UI::D11DeviceResources>& pDevice)
{
    const auto& deviceContext = pDevice->GetID2D1DeviceContext();

    const float& width = static_cast<FLOAT>(gButtonWidth);
    const float& height = static_cast<FLOAT>(gButtonHeight);
    const D2D1_POINT_2F& center{ width / 2.0f, height / 2.0f };

    const float coefficient{ 3.828571f };
    const float radius{ height / coefficient };

    // Create a compatible render target.
    Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> pCompatibleRenderTarget;

    HRESULT hr = S_OK;

    hr = deviceContext->CreateCompatibleRenderTarget(
        D2D1::SizeF(width, height),
        D2D1::SizeU(gButtonWidth, gButtonHeight),
        m_pixelFormat,
        pCompatibleRenderTarget.GetAddressOf()
    );

    if (SUCCEEDED(hr))
    {
        pCompatibleRenderTarget->BeginDraw();
        //pCompatibleRenderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(center.x, center.y), radius, radius), GetButtonRestoreOutline(), 1);
        pCompatibleRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(center.x, center.y), radius, radius), GetButtonRestore());
        pCompatibleRenderTarget->EndDraw();

        // Retrieve the bitmap from the render target.
        hr = pCompatibleRenderTarget->GetBitmap(m_pIconButtonRestore.GetAddressOf());
        pCompatibleRenderTarget.Reset();
    }

    return hr;
}

HRESULT UI::Caching::CreateIconMinimize(const std::shared_ptr<UI::D11DeviceResources>& pDevice)
{
    const auto& deviceContext = pDevice->GetID2D1DeviceContext();

    const float& width = static_cast<FLOAT>(gButtonWidth);
    const float& height = static_cast<FLOAT>(gButtonHeight);
    const D2D1_POINT_2F& center{ width / 2.0f, height / 2.0f };

    const float coefficient{ 3.828571f };
    const float radius{ height / coefficient };

    // Create a compatible render target.
    Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> pCompatibleRenderTarget;

    HRESULT hr = S_OK;

    hr = deviceContext->CreateCompatibleRenderTarget(
        D2D1::SizeF(width, height),
        D2D1::SizeU(gButtonWidth, gButtonHeight),
        m_pixelFormat,
        pCompatibleRenderTarget.GetAddressOf()
    );

    if (SUCCEEDED(hr))
    {
        pCompatibleRenderTarget->BeginDraw();
        //pCompatibleRenderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(center.x, center.y), radius, radius), GetButtonMinimizeOutline(), 1);
        pCompatibleRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(center.x, center.y), radius, radius), GetButtonMinimize());
        pCompatibleRenderTarget->EndDraw();

        // Retrieve the bitmap from the render target.
        hr = pCompatibleRenderTarget->GetBitmap(m_pIconButtonMinimize.GetAddressOf());
        pCompatibleRenderTarget.Reset();
    }

    return hr;
}

HRESULT UI::Caching::CreateIconGear(const std::shared_ptr<UI::D11DeviceResources>& pDevice)
{
    const auto& deviceContext = pDevice->GetID2D1DeviceContext();

    const float& width = static_cast<FLOAT>(gPopUpButtonHeight);
    const float& height = static_cast<FLOAT>(gPopUpButtonHeight);
    const D2D1_POINT_2F& center{ width / 2.0f, height / 2.0f };

    float radius{ 7.0f };
    const D2D1_POINT_2F& pointA = { center.x, center.y + radius };
    const D2D1_POINT_2F& pointB = { center.x, center.y - radius };

    // Create a compatible render target.
    Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> pCompatibleRenderTarget;

    std::vector brushesA{ GetButtonTxtNoActive(), GetButtonTxtPreActive(), GetButtonTxtActive() };
    std::vector brushesB{ GetButtonNoActivePopUp(), GetButtonPreActivePopUp(), GetButtonActivePopUp() };

    HRESULT hr{ S_OK };

    for (size_t i{ 0 }; i < 3; i++)
    {
        hr = deviceContext->CreateCompatibleRenderTarget(
            D2D1::SizeF(width, height),
            D2D1::SizeU(gPopUpButtonHeight, gPopUpButtonHeight),
            m_pixelFormat,
            pCompatibleRenderTarget.GetAddressOf()
        );

        if (SUCCEEDED(hr))
        {
            pCompatibleRenderTarget->BeginDraw();

            for (size_t j{ 0 }; j < 4; j++)
            {
                pCompatibleRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(45.0f * j, center));
                pCompatibleRenderTarget->DrawLine(pointA, pointB, brushesA[i], 2);
            }
            pCompatibleRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

            radius = 5.0f;
            pCompatibleRenderTarget->FillEllipse(D2D1::Ellipse(center, radius, radius), brushesA[i]);

            radius = 3.0f;
            pCompatibleRenderTarget->FillEllipse(D2D1::Ellipse(center, radius, radius), brushesB[i]);
            pCompatibleRenderTarget->EndDraw();

            // Retrieve the bitmap from the render target.
            hr = pCompatibleRenderTarget->GetBitmap(m_pIconButtonGear[i].GetAddressOf());
            hr = pCompatibleRenderTarget.Reset();
        }
    }

    return hr;
}

HRESULT UI::Caching::CreateIconArrow(const std::shared_ptr<UI::D11DeviceResources>& pDevice)
{
    const auto& deviceContext = pDevice->GetID2D1DeviceContext();
    const auto& factory = pDevice->GetID2D1Factory().Get();

    const float& width = static_cast<FLOAT>(gPopUpButtonHeight);
    const float& height = static_cast<FLOAT>(gPopUpButtonHeight);
    const float& center{ height / 2.0f };

    D2D1_POINT_2F points[3]{};
    const float anglesDegrees[] = { 90.0f, 180.0f, 270.0f };
    const float radius[] = { 5.0f, 6.0f, 5.0f };

    for (size_t i = 0; i < 3; i++)
    {
        // Convert degrees to radians.
        const float angleRadians = anglesDegrees[i] * (gPI / 180.0f);
        // 
        points[i].x = center + radius[i] * -std::cos(angleRadians);
        points[i].y = center + radius[i] * -std::sin(angleRadians);
    }

    // Create a compatible render target.
    Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> pCompatibleRenderTarget;
    Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry;
    Primitives2D primitives;

    HRESULT hr = S_OK;

    std::vector brushes{ GetButtonTxtNoActive(), GetButtonTxtPreActive(), GetButtonTxtActive() };

    for (size_t i{ 0 }; i < 3; i++)
    {
        hr = deviceContext->CreateCompatibleRenderTarget(
            D2D1::SizeF(width, height),
            D2D1::SizeU(gPopUpButtonHeight, gPopUpButtonHeight),
            m_pixelFormat,
            pCompatibleRenderTarget.GetAddressOf()
        );

        if (SUCCEEDED(hr))
        {
            pCompatibleRenderTarget->BeginDraw();
            geometry = primitives.DrawPathGeometry(
                factory,
                {
                    D2D1::Point2F(points[0].x, points[0].y),
                    D2D1::Point2F(points[1].x, points[1].y),
                    D2D1::Point2F(points[2].x, points[2].y)
                }
            );
            pCompatibleRenderTarget->DrawGeometry(geometry.Get(), brushes[i], 1.0f, 0);
            pCompatibleRenderTarget->FillGeometry(geometry.Get(), brushes[i]);
            pCompatibleRenderTarget->EndDraw();

            // Retrieve the bitmap from the render target.
            hr = pCompatibleRenderTarget->GetBitmap(m_pIconButtonArrow[i].GetAddressOf());
            hr = pCompatibleRenderTarget.Reset();
        }
    }

    return hr;
}

HRESULT UI::Caching::CreateIconFlip(const std::shared_ptr<UI::D11DeviceResources>& pDevice)
{
    const auto& deviceContext = pDevice->GetID2D1DeviceContext();
    const auto& factory = pDevice->GetID2D1Factory().Get();

    const float& width = static_cast<FLOAT>(gFlipPlace);
    const float& height = static_cast<FLOAT>(gFlipPlace);
    const D2D1_POINT_2F& center{ width / 2.0f, height / 2.0f };

    D2D1_POINT_2F points[3]{};
    float anglesDegrees[] = { 90.0f, 180.0f, 270.0f };
    float radius[] = { 6.0f, 7.0f, 6.0f };

    for (size_t i = 0; i < 3; i++)
    {
        // Convert degrees to radians.
        const float angleRadians = anglesDegrees[i] * (gPI / 180.0f);
        // 
        points[i].x = (center.x + 2.0f) + radius[i] * std::cos(angleRadians);
        points[i].y = center.y + radius[i] * std::sin(angleRadians);
    }

    // Create a compatible render target.
    Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> pCompatibleRenderTarget;
    Microsoft::WRL::ComPtr<ID2D1PathGeometry> geometry;
    Primitives2D primitives;

    HRESULT hr = S_OK;

    for (size_t i{ 0 }; i < 2; i++)
    {
        hr = deviceContext->CreateCompatibleRenderTarget(
            D2D1::SizeF(width, height),
            D2D1::SizeU(gFlipPlace, gFlipPlace),
            m_pixelFormat,
            pCompatibleRenderTarget.GetAddressOf()
        );

        if (SUCCEEDED(hr))
        {
            pCompatibleRenderTarget->BeginDraw();
            pCompatibleRenderTarget->FillEllipse(D2D1::Ellipse(center, 14.0f, 14.0f), GetFlipAreaEllipseBrush());

            D2D1_MATRIX_3X2_F transform = D2D1::Matrix3x2F::Scale(1.0f - (0.15f * i), 1.0f - (0.15f * i), center);
            pCompatibleRenderTarget->SetTransform(transform);

            // Drawing an arrow.
            geometry = primitives.DrawPathGeometry(
                factory,
                {
                    D2D1::Point2F(points[0].x, points[0].y),
                    D2D1::Point2F(points[1].x, points[1].y),
                    D2D1::Point2F(points[2].x, points[2].y)
                }
            );
            pCompatibleRenderTarget->DrawGeometry(geometry.Get(), GetFlipAreaTriangleBrush(), 1.0f, 0);
            pCompatibleRenderTarget->FillGeometry(geometry.Get(), GetFlipAreaTriangleBrush());

            pCompatibleRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
            pCompatibleRenderTarget->EndDraw();

            // Retrieve the bitmap from the render target.
            hr = pCompatibleRenderTarget->GetBitmap(m_pIconButtonFlip[i].GetAddressOf());
            hr = pCompatibleRenderTarget.Reset();
        }
    }

    return hr;
}
