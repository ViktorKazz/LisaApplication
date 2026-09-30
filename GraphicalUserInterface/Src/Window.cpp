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

#include "..//Inc/Window.h"

// In this function, the parent window sends its RECT dimensions to its children. Invalidate
// 
// Regardless of the number of children, the parent window is split into parts 
// (if the SPLIT_X / SPLIT_Y flags are used) and a unique RECT is sent to each child.
void UI::Window::StretchingChildrenElements(const HWND& hwnd)
{
    RECT rc;
    GetClientRect(hwnd, &rc);

    // Finding children windows
    std::vector<HWND> child{ FindChild(hwnd) };

    if (!child.empty())
    {
        RECT rect{};

        if (m_transform & ui_transform::stretching_x)
        {
            if (m_transform & ui_transform::split_x)
            {
                LONG difference{ rc.right };
                LONG average{ difference / static_cast<LONG>(child.size()) };

                for (LONG g = 0; g < child.size(); g++)
                {
                    rect.top = rc.top;
                    rect.bottom = rc.bottom;

                    rect.left = rect.left + average * (g ? 1 : 0);
                    rect.right = rect.left + average;

                    SendMessageW(child[g], WM_COMMAND, RESIZING_ELEMENTS, reinterpret_cast<LPARAM>(&rect));
                }
            }
            else if (m_transform & ui_transform::split_y)
            {

            }
        }
    }

}

LRESULT CALLBACK UI::Window::MessageHandled(UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result{};

    switch (m_type)
    {
    case ui_type::simple:
        result = SimpleWindow(message, wParam, lParam);
    break;
    case ui_type::popUp:
        result = SimpleWindow(message, wParam, lParam);
        break;
    case ui_type::inbuilt:
        result = InbuiltWindow(message, wParam, lParam);
    break;   
    default:
        throw std::runtime_error("Message handling function not found.");
        break;
    }

    return result;
}
//
LRESULT CALLBACK UI::Window::SimpleWindow(UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result{};
    PAINTSTRUCT ps{};

    switch (message)
    {
    case WM_KILLFOCUS:
    {
        // If the window that received focus does not belong to the same pop-up chain,
        // then we close all pop-up windows in this chain.
        if (m_draw & ui_draw::window::popUp)
        {
            SendMessageW(GetFocus(), WM_COMMAND, WINDOW_IS_CHECKED_FOR_FRIEND_OR_FOE, LPARAM(GetHwnd()));

            if (m_friendliness)
                m_friendliness = 0;
            else
                SendMessageW(m_config.Root, WM_COMMAND, MESSAGE_TO_ROOT_NOFRAME_BUTTON, lParam);
        }
    }
    return 0;
    break;
    case WM_GETMINMAXINFO:
    {
        m_tools->MinMaxWindow(lParam, GetRight(), GetBottom());
    }
    return 0;
    break;
    case WM_NCLBUTTONDBLCLK:
    {
        m_tools->ButtonDoubleClick(
            GetHwnd(),
            lParam,
            GetNonClientAreaSize(),
            gMainMenuBarHeight,
            m_config.Resizable
        );
    }
    return 0;
    break;
    case WM_NCHITTEST:
    {
        // There is a gap between the button and the edge of the window.
        // And so that when you click on this gap, 
        // the focus is not transferred to the next pop-up window, 
        // we will increase the non-client area.
        // Now the window will not respond to mouse clicks on it.

        LONG nonClient{ 
            (m_draw & ui_draw::window::popUp) ? 
            GetNonClientAreaSize() + (GetNonClientAreaSize() / 3) : 
            GetNonClientAreaSize() 
        };
        LONG menuBar{ (m_draw & ui_draw::window::popUp) ? 0 : gMainMenuBarHeight };

        return m_tools->NCHitTest(GetHwnd(), lParam, nonClient, menuBar, m_config.Resizable);
    }
    case WM_COMMAND:
    {
        UINT wmId = LOWORD(wParam);

        switch (wmId)
        {
        case SETTING_DATA_DURING_INITIALIZATION:
        {
            // Finding children windows.
            for (const auto& i : FindChild(GetHwnd()))
                m_child.push_back(i);

            for (auto i : m_child)
                SendMessageW(i, WM_COMMAND, GETTING_DATA_DURING_INITIALIZATION, lParam);
        }
        break;
        case WINDOW_RESTORE_MAXIMIZE:
        {
            RECT workAreaSize{};
            Error(SystemParametersInfoW(SPI_GETWORKAREA, 0, &workAreaSize, 0));
            RECT rc; Error(GetClientRect(GetHwnd(), &rc));

            // If the window is already maximized, it returns to its previous size and vice versa.
            if (rc.right == workAreaSize.right && rc.bottom == workAreaSize.bottom)
                SendMessage(GetHwnd(), WM_SYSCOMMAND, SC_RESTORE, NULL);
            else
                SendMessage(GetHwnd(), WM_SYSCOMMAND, SC_MAXIMIZE, NULL);
        }
        break;
        case WINDOW_MINIMIZE:
        {
            SendMessage(GetHwnd(), WM_SYSCOMMAND, SC_MINIMIZE, NULL);
        }
        break;
        case UPDATE_TITLE:
        {
            m_updateTitle = LPWSTR(lParam);
            Error(InvalidateRect(GetHwnd(), FALSE, FALSE));
        }
        break;
        case WINDOW_IS_CHECKED_FOR_FRIEND_OR_FOE:
        {
            HWND theApplicantsHWND = HWND(lParam);

            if (m_draw & ui_draw::window::popUp)
                SendMessageW(theApplicantsHWND, WM_COMMAND, RESPONSE_CONFIRMATION_OF_FRIENDLINESS, LPARAM(GetHwnd()));
        }
        break;
        case RESPONSE_CONFIRMATION_OF_FRIENDLINESS:
        {
            // If the window being checked is friendly.
            m_friendliness = 1;
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
        if (!m_tools->ComparisonWindowSizes(GetHwnd()))
            SetNonClientAreaSize(gNonClientAreaSize);
        else
            SetNonClientAreaSize(0);

        for (auto i : m_child)
            SendMessageW(i, WM_COMMAND, RESIZING_CHILD_WINDOW_FROM_SIMPLE, lParam);

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
        ThrowIfFailed(Draw());
        EndPaint(GetHwnd(), &ps);
    }
    result = 0;
    break;
    case WM_DESTROY:
    {
        if (m_tools->SearchByOneKey(GetWindowClass().c_str(), {L"main"}))
        {
            PostQuitMessage(0);
        }
        
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

LRESULT CALLBACK UI::Window::InbuiltWindow(UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result{};
    PAINTSTRUCT ps{};

    switch (message)
    {
    case WM_SETCURSOR:
    {
        ::SetCursor((HCURSOR)::LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED));
        return TRUE;
    }
    break;
    case WM_COMMAND:
    {
        UINT wmId = LOWORD(wParam);

        switch (wmId)
        {
        case SETTING_DATA_DURING_INITIALIZATION:
        {
            std::vector<HWND> separators{};
            std::vector<HWND> substrate{};

            // Finding children windows.
            for (const auto& i : FindChild(GetHwnd()))
                m_child.push_back(i);

            for (const auto& i : m_child)
            {
                wchar_t winClass[256]{};
                Error(GetClassNameW(i, winClass, 256));

                if (HelperWTools tools; tools.SearchByOneKey(winClass, { L"separator" }))
                    separators.push_back(i);
                if (HelperWTools tools; tools.SearchByOneKey(winClass, { L"substrate" }))
                    substrate.push_back(i);
            }

            auto getAdjacentSeparators = [&](size_t index) -> std::pair<HWND, HWND>
                {
                    return
                    {
                        (index > 0) ? separators[index - 1] : nullptr,
                        (index < separators.size() - 1) ? separators[index + 1] : nullptr
                    };
                };

            // Transfer to each separator the HWND of the neighboring separators and neighboring substrates.
            for (size_t i = 0; i < separators.size(); ++i)
            {
                std::tuple<HWND, HWND, HWND, HWND> tplHwnd
                {
                    getAdjacentSeparators(i).first,
                    getAdjacentSeparators(i).second,
                    substrate[i],
                    substrate[i + 1]
                };

                SendMessageW(separators[i], WM_COMMAND, UI::OBTAIN_HWND_OF_NEIGHBORING_SEPARATORS,
                    reinterpret_cast<LPARAM>(&tplHwnd));
            }

            for (const auto& i : m_child)
                SendMessageW(i, WM_COMMAND, GETTING_DATA_DURING_INITIALIZATION, lParam);
        }
        break;
        case GETTING_DATA_DURING_INITIALIZATION:
        {
            // Get the dimensions of the parent window at the time of creation.
            const HWND parent = HWNDError(GetAncestor(GetHwnd(), GA_PARENT));
            RECT rc; Error(GetClientRect(parent, &rc));

            m_parentWindowWidth = rc.right;
            m_parentWindowHeight = rc.bottom;

            // For windows that are already open and their contents are changing, 
            // it is necessary to update the data for their children.
            if (m_modes & ui_modes::mrmc1 || m_modes & ui_modes::mrmc2 || m_modes & ui_modes::mrmc3)
            {
                LONG l{}, t{}, r{}, b{};

                INT nButton{};// close, restore, minimize -> 3

                if (m_modes & ui_modes::mrmc1)
                    nButton = 1;
                if (m_modes & ui_modes::mrmc2)
                    nButton = 2;
                if (m_modes & ui_modes::mrmc3)
                    nButton = 3;

                l = { m_parentWindowWidth - ((gButtonWidth * nButton) - (gNonClientAreaSize / 2) + (gNonClientAreaSize * 2)) };
                t = { gNonClientAreaSize + ((gMainMenuBarHeight - gButtonHeight) / 2) };
                r = { gButtonWidth * nButton };
                b = { gButtonHeight };

                SetLeft(l); SetTop(t); SetRight(r); SetBottom(b);
            }
        }
        break;
        case RESIZING_CHILD_WINDOW_FROM_SIMPLE:
        {
            INT l{ GetLeft() }, t{ GetTop() }, r{ GetRight() }, b{ GetBottom() };

            // This function is relevant when the parent is a regular, non-embedded window.
            RestorePosition(
                GetHwnd(),
                m_parentWindowWidth,
                m_parentWindowHeight,
                m_transform,
                GetNonClientAreaSize(),
                GetLeft(),
                GetTop(),
                GetRight(),
                GetBottom(),
                l, t, r, b
            );

            if (m_transform & ui_transform::modifiable_x)
            {
                r = { m_modifiableX };
            }

            Error(SetWindowPos(GetHwnd(), 0, l, t, r, b, SWP_NOZORDER | SWP_NOACTIVATE), 
                "Window position update failed, RESIZING_CHILD_WINDOW_FROM_SIMPLE");
        }
        break;
        case RESIZING_CHILD_WINDOW_FROM_INBUILT:
        {
            const HWND parent = HWNDError(GetAncestor(GetHwnd(), GA_PARENT));

            RECT parentRect{}; Error(GetClientRect(parent, &parentRect));

            INT l{ GetLeft() }, t{ GetTop() }, r{ GetRight() }, b{ GetBottom() };

            if (m_transform & ui_transform::stretching_x)
                r = parentRect.right - (m_parentWindowWidth - GetRight());

            if (m_transform & ui_transform::stretching_y)
                b = parentRect.bottom - (m_parentWindowHeight - GetBottom());

            //if (m_transform & ui_transform::restore_rx)
            //    l += (parentRect->right - m_parentWindowWidth);

            if (m_transform & ui_transform::restore_lx)
                l += parentRect.right - m_parentWindowWidth;

            /*if (m_transform & TRANSFORM::RESTORE_TY)
                t += (parentRect->bottom - m_parentWindowHeight);*/

            if (m_transform & ui_transform::restore_by)
                t += (parentRect.bottom - m_parentWindowHeight);

            // Check for missing flag to avoid conflicts.
            if (m_transform != ui_transform::none)
            {
                Error(SetWindowPos(GetHwnd(), nullptr, l, t, r, b, SWP_NOZORDER | SWP_NOACTIVATE), 
                    "Window position update failed, RESIZING_CHILD_WINDOW_FROM_INBUILT");
            }
        }
        break;
        case GET_MODIFIABLE_X:
        {
            LONG r{ GetRight() };

            // Get the new window width.
            if (m_transform & ui_transform::modifiable_x)
            {
                r = m_modifiableX = static_cast<LONG>(LOWORD(lParam));
            }

            Error(SetWindowPos(GetHwnd(), 0, GetLeft(), GetTop(), r, GetBottom(), SWP_NOZORDER | SWP_NOACTIVATE),
                "Window position update failed, GET_MODIFIABLE_X");

            // After resizing the window, it is necessary to redraw it.
            // This needs to be done in this way, 
            // since it is not the usual drawing function that is used.
            SendMessageW(GetHwnd(), WM_PAINT, 0, 0);
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

        // To change the size of children's elements
        for (auto i : m_child)
            SendMessageW(i, WM_COMMAND, RESIZING_CHILD_WINDOW_FROM_INBUILT, lParam);

        // Set data hwnd(elements/button) child
        StretchingChildrenElements(GetHwnd());
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

HRESULT UI::Window::CreateGridPatternBrush(
    ID2D1RenderTarget* pRenderTarget,
    ID2D1Bitmap** ppBitmapBrush
)
{
    // Create a compatible render target.
    ID2D1BitmapRenderTarget* pCompatibleRenderTarget = NULL;
    HRESULT hr = pRenderTarget->CreateCompatibleRenderTarget(
        D2D1::SizeF(10.0f, 10.0f),
        &pCompatibleRenderTarget
    );
    if (SUCCEEDED(hr))
    {
        // Draw a pattern.
        ID2D1SolidColorBrush* pGridBrush = NULL;
        hr = pCompatibleRenderTarget->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF(0.93f, 0.94f, 0.96f, 1.0f)),
            &pGridBrush
        );
        if (SUCCEEDED(hr))
        {
            pCompatibleRenderTarget->BeginDraw();


            pRenderTarget->DrawEllipse(
                D2D1::Ellipse(
                    D2D1::Point2F(
                        100.0f,
                        100.0f
                    ),
                    40.0f,
                    40.0f
                ),
                pGridBrush, 3
            );

            /*pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, 10.0f, 1.0f), pGridBrush);
            pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.1f, 1.0f, 10.0f), pGridBrush);*/
            pCompatibleRenderTarget->EndDraw();

            // Retrieve the bitmap from the render target.
            //ID2D1Bitmap* pGridBitmap = NULL;
            hr = pCompatibleRenderTarget->GetBitmap(ppBitmapBrush);
            //if (SUCCEEDED(hr))
            //{
            //    // Choose the tiling mode for the bitmap brush.
            //    D2D1_BITMAP_BRUSH_PROPERTIES brushProperties =
            //        D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP);

            //    // Create the bitmap brush.
            //    hr = pRenderTarget->CreateBitmapBrush(pGridBitmap, brushProperties, ppBitmapBrush);

            //    pGridBitmap->Release();
            //}

            pGridBrush->Release();
        }

        pCompatibleRenderTarget->Release();
    }

    return hr;
}

void UI::Window::DrawSimpleWindow(
    const Microsoft::WRL::ComPtr<ID2D1DeviceContext6>& pDeviceContext,
    const Microsoft::WRL::ComPtr<IWICImagingFactory2>& pWICFactory,
    const std::wstring& updateTitle,
    const D2D1_SIZE_F& size,
    LONG NonClientAreaSize
)
{
    // Drawing a main window shadow.
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pShadowBrush{ GetCaching()->GetShadowBrush() };

    float left = { static_cast<float>(NonClientAreaSize) };
    float top = { static_cast<float>(NonClientAreaSize) };
    float right = { size.width - static_cast<float>(NonClientAreaSize) };
    float bottom = { size.height - static_cast<float>(NonClientAreaSize) };

    float roundness{ 6.0f };
    float transparent{ 0.4f };

    for (int i = 0; i < 13; i++)
    {
        transparent *= 0.75f - (i * 0.01f);
        pShadowBrush->SetOpacity(transparent);

        pDeviceContext->DrawRoundedRectangle(
            D2D1::RoundedRect(D2D1::RectF(left - i, top - i, right + i, bottom + i),
                roundness,
                roundness
            ),
            pShadowBrush.Get(), 1
        );
    }

    // Drawing a main window background.      
    pDeviceContext->FillRoundedRectangle(
        D2D1::RoundedRect(D2D1::RectF(left, top, right, bottom), roundness, roundness), 
        GetCaching()->GetInternalBackingBrush());

    // Draw the outer rectangle.
    pDeviceContext->FillRoundedRectangle(
        D2D1::RoundedRect(D2D1::RectF(left, top + gMainMenuBarHeight, right, bottom),
            roundness, roundness
        ), GetCaching()->GetExternalBackingBrush());

    // Draw a small insert to hide the upper roundings.
    pDeviceContext->FillRoundedRectangle(
        D2D1::RoundedRect(D2D1::RectF(left, top + gMainMenuBarHeight, right, top + gMainMenuBarHeight + (roundness * 2)),
            0.0f, 0.0f
        ), GetCaching()->GetExternalBackingBrush());

    // Creating a window title.
    if (m_config.ShowTitle)
    {
        if (!GetWindowTitle().empty())
        {
            std::wstring t{};

            if (updateTitle.size())
                t = updateTitle;
            else
                t = GetWindowTitle();

            pDeviceContext->DrawTextW(
                t.c_str(),
                lstrlen(t.c_str()),
                GetCaching()->GetTitleTextFormat(),
                D2D1::RectF(
                    (left + gNonClientAreaSize) + gMainIconPlace,
                    static_cast<float>(NonClientAreaSize) * 2,
                    right,
                    UI::gMainIconPlace
                ),
                GetCaching()->GetTitleBrush()
            );
        }
    }

    // Drawing an image.
    std::wstring img{};
    // Access the std::wstring
    if (std::holds_alternative<std::wstring>(m_config.ImgTxt))
        img = std::get<std::wstring>(m_config.ImgTxt);

    if (!img.empty())
    {
        HRESULT hr{ S_OK };
        
        if(!m_pBitmap)
            hr = m_tools->LoadBitmapFromFile(pDeviceContext, pWICFactory, m_pBitmap.GetAddressOf(), img.c_str(), 0, 0);

        if (!m_translationEffect)
            pDeviceContext->CreateEffect(CLSID_D2D12DAffineTransform, &m_translationEffect);
        if (!m_scaleEffect)
            pDeviceContext->CreateEffect(CLSID_D2D1Scale, &m_scaleEffect);

        if (SUCCEEDED(hr))
        {
            // Scale the image to fit the client area.
            m_scaleEffect->SetInput(0, m_pBitmap.Get());
            m_scaleEffect->SetValue(D2D1_SCALE_PROP_CENTER_POINT, D2D1::Vector2F(0.0f, 0.0f));
            m_scaleEffect->SetValue(D2D1_SCALE_PROP_SCALE, D2D1::Vector2F(0.75f, 0.75f));

            // Move the image to the center of the client area.
            m_translationEffect->SetInputEffect(0, m_scaleEffect.Get());
            const D2D1_VECTOR_2F indent{ gNonClientAreaSize / 2.5f, gNonClientAreaSize / 2.5f };
            D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Translation({ left + indent.x, top + indent.y });
            m_translationEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, matrix);

            pDeviceContext->DrawImage(m_translationEffect.Get());

            m_pBitmap.Reset();
        }
    }

}

void UI::Window::DrawInbuiltWindow(
    const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget, 
    const D2D1_SIZE_F& size
)
{
    D2D1_ROUNDED_RECT roundedRectButton{};
    
    roundedRectButton = D2D1::RoundedRect(D2D1::RectF(0.0f, 0.0f, size.width, size.height), 0.0f, 0.0f);
    pRenderTarget->FillRoundedRectangle(
        roundedRectButton, 
        (m_draw & ui_draw::window::inbuilt_in) ? 
        GetCaching()->GetInternalBackingBrush() : 
        GetCaching()->GetExternalBackingBrush());

    if (m_draw & ui_draw::window::inbuilt_border_l)
        pRenderTarget->DrawLine({ 0.0f, 0.0f }, { 0.0f, size.height }, GetCaching()->GetBorder(), 1);
    if (m_draw & ui_draw::window::inbuilt_border_t)
        pRenderTarget->DrawLine({ 0.0f, 0.0f }, { size.width, 0.0f }, GetCaching()->GetBorder(), 1);
    if (m_draw & ui_draw::window::inbuilt_border_r)
        pRenderTarget->DrawLine({ size.width, 0.0f }, { size.width, size.height }, GetCaching()->GetBorder(), 1);
    if (m_draw & ui_draw::window::inbuilt_border_b)
        pRenderTarget->DrawLine({ 0.0f, size.height }, { size.width, size.height }, GetCaching()->GetBorder(), 1);

}

void UI::Window::DrawPopUpWindow(
    const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget, 
    const D2D1_SIZE_F& size
)
{
    // Drawing a main window shadow.
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pShadowBrush{ GetCaching()->GetShadowBrush() };

    float left = { static_cast<float>(gNonClientAreaSize) };
    float top = { static_cast<float>(gNonClientAreaSize) };
    float right = { size.width - static_cast<float>(gNonClientAreaSize) };
    float bottom = { size.height - static_cast<float>(gNonClientAreaSize) };

    float roundness{ 6.0f };
    float transparent{ 0.4f };

    for (int i = 0; i < 13; i++)
    {
        transparent *= 0.75f - (i * 0.01f);
        pShadowBrush->SetOpacity(transparent);

        pRenderTarget->DrawRoundedRectangle(
            D2D1::RoundedRect(D2D1::RectF(left - i, top - i, right + i, bottom + i),
                roundness,
                roundness
            ),
            pShadowBrush.Get(), 1
        );
    }

    left = { 1.0f };
    top = { 1.0f };

    // Draw a frame for the pop-up window.          
    pRenderTarget->DrawRoundedRectangle(
        D2D1::RoundedRect(D2D1::RectF(left, top, right, bottom), roundness, roundness), 
        GetCaching()->GetPopUpFrameBrush(), 0.4f);

    // Drawing a pop-up window background.       
    pRenderTarget->FillRoundedRectangle(
        D2D1::RoundedRect(D2D1::RectF(left, top, right, bottom), roundness, roundness), 
        GetCaching()->GetPopUpBrush());
}

HRESULT UI::Window::Draw()
{
    HRESULT hr{ S_OK };

    // D2D1DeviceContext needs to be updated.
    GetComposition()->ConfigureSwapChain(GetHwnd());

    auto deviceContext = GetComposition()->GetID2D1DeviceContext();
    auto WICFactory = GetComposition()->GetWICFactory();
    auto swapChain = GetComposition()->GetIDXGISwapChain();

    if (deviceContext && swapChain)
    {
        D2D1_SIZE_F size = deviceContext->GetSize();

        deviceContext->BeginDraw();
        deviceContext->Clear();

        if (m_draw & ui_draw::window::simple)
        {
            DrawSimpleWindow(deviceContext, WICFactory, m_updateTitle, size, GetNonClientAreaSize());
        }
        else if (m_draw & ui_draw::window::inbuilt)
        {
            DrawInbuiltWindow(deviceContext, size);
        }
        else if (m_draw & ui_draw::window::popUp)
        {
            DrawPopUpWindow(deviceContext, size);
        }

        hr = deviceContext->EndDraw();
        // Make the swap chain available to the composition engine.
        hr = swapChain->Present(0, 0);
    }

    return hr;
}