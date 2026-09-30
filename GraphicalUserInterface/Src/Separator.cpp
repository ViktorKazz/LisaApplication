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

#include "..//Inc/Separator.h"

LRESULT UI::DrawnSeparator::MessageHandled(UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result{};
    PAINTSTRUCT ps{};

    switch (message)
    {
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

void UI::DrawnSeparator::PopUpSeparator(const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget, const D2D1_SIZE_F& size)
{
    // Draw the background
    D2D1_ROUNDED_RECT roundedRectButton{};
    roundedRectButton = D2D1::RoundedRect(
        D2D1::RectF(0.0f, 0.0f, size.width, size.height), 0.0f, 0.0f);

    pRenderTarget->FillRoundedRectangle(roundedRectButton, GetCaching()->GetPopUpBrush());

    // Draw the separator
    const float left{ gIconPlace + 4.0f };
    const float right{ size.width - 2.0f };

    pRenderTarget->DrawLine({ left, 1.0f }, { right, size.height - 2.0f }, GetCaching()->GetPopUpBrush(), 0.5f);
    pRenderTarget->DrawLine({ left, 1.5f }, { right, size.height - 1.5f }, GetCaching()->GetPopUpFrameBrush(), 0.75f);
    pRenderTarget->DrawLine({ left, 2.0f }, { right, size.height - 1.0f }, GetCaching()->GetPopUpBrush(), 0.5f);
}

HRESULT UI::DrawnSeparator::Draw()
{
    HRESULT hr{ S_OK };

    // D2D1DeviceContext needs to be updated.
    GetComposition()->ConfigureSwapChain(GetHwnd());

    auto deviceContext = GetComposition()->GetID2D1DeviceContext();
    auto swapChain = GetComposition()->GetIDXGISwapChain();

    if (deviceContext && swapChain)
    {
        D2D1_SIZE_F size = deviceContext->GetSize();

        deviceContext->BeginDraw();
        deviceContext->Clear();

        switch (m_draw)
        {
        case ui_draw::separator::popUp:
            PopUpSeparator(deviceContext, size);
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

//const wchar_t* SYSTEM_CURSORS[] = {
//    IDC_ARROW,    // Standard arrow
//    IDC_IBEAM,    // Text cursor
//    IDC_WAIT,     // Hourglass
//    IDC_HAND,     // Hand (available since Windows 2000)
//    IDC_CROSS,    // Cross
//    IDC_SIZEALL   // Four-way arrow
//};
//
//static HCURSOR LoadSystemCursor(const wchar_t* cursorId)
//{
//    return (HCURSOR)::LoadImageW(
//        NULL,               // Using system resources
//        cursorId,           // Cursor ID
//        IMAGE_CURSOR,       // Resource type
//        0,                  // Width (0 = system size)
//        0,                  // Height (0 = system size)
//        LR_SHARED           // Flag for shared system resources
//    );
//}

void UI::Separator::MovingSubstrate()
{
    const HWND& parent = HWNDError(GetAncestor(GetHwnd(), GA_PARENT));

    RECT parentRect{}; Error(GetWindowRect(parent, &parentRect));
    RECT clientRect{}; Error(GetClientRect(parent, &clientRect));

    // Process separators only if they exist
    const auto& [previousSeparator, nextSeparator, previousSubstrate, nextSubstrate] { m_getHwndAdjacentElements };

    // No error checking is performed since previousSeparator may be NULL.
    RECT prevSeparatorRect{}; GetWindowRect(previousSeparator, &prevSeparatorRect);
    // No error checking is performed since nextSeparator may be NULL.
    RECT nextSeparatorRect{}; GetWindowRect(nextSeparator, &nextSeparatorRect);
    RECT separatorRect{}; Error(GetWindowRect(GetHwnd(), &separatorRect));

    Error(SetWindowPos(previousSubstrate, nullptr,
        previousSeparator
        ? (m_config.SplitX ? prevSeparatorRect.right - parentRect.left : 0)
        : 0,
        previousSeparator
        ? (m_config.SplitX ? 0 : prevSeparatorRect.bottom - parentRect.top)
        : 0,
        m_config.SplitX
        ? (previousSeparator ? separatorRect.left - prevSeparatorRect.right : separatorRect.left - parentRect.left)
        : clientRect.right,
        m_config.SplitX
        ? clientRect.bottom
        : (previousSeparator ? separatorRect.top - prevSeparatorRect.bottom : separatorRect.top - parentRect.top),
        SWP_NOZORDER | SWP_NOACTIVATE));

    Error(SetWindowPos(nextSubstrate, nullptr,
        m_config.SplitX ? separatorRect.right - parentRect.left : 0,
        m_config.SplitX ? 0 : separatorRect.bottom - parentRect.top,
        m_config.SplitX
        ? (nextSeparator ? nextSeparatorRect.left - separatorRect.right : parentRect.right - separatorRect.right)
        : clientRect.right,
        m_config.SplitX
        ? clientRect.bottom
        : nextSeparator ? nextSeparatorRect.top - separatorRect.bottom : parentRect.bottom - separatorRect.bottom,
        SWP_NOZORDER | SWP_NOACTIVATE));
}

LRESULT UI::Separator::MessageHandled(UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result{};
    PAINTSTRUCT ps{};
    MouseTrackEvents track;

    switch (message)
    {
    case WM_SETCURSOR:
    {
        if (m_currentCursor)
        {
            ::SetCursor(m_currentCursor);
            return TRUE;
        }
    }
    break;
    case WM_WINDOWPOSCHANGING:
    {
        WINDOWPOS* wpos = reinterpret_cast<WINDOWPOS*>(lParam);

        const HWND& parent = HWNDError(GetAncestor(GetHwnd(), GA_PARENT));

        RECT parentRect{}; Error(GetWindowRect(parent, &parentRect));
        RECT clientRect{}; Error(GetClientRect(parent, &clientRect));

        // DPI processing.
        const UINT dpi = GetDpiForWindow(parent);

        auto calculateScaledOffset = [&](INT value) { return MulDiv(value, dpi, gBaseDPI); };

        const INT scaledOffsetFirst = calculateScaledOffset(m_config.FirstLimitation);
        const INT scaledOffsetSecond = calculateScaledOffset(m_config.SecondLimitation);

        // Define the boundaries of the acceptable region
        INT min{};
        INT max{};

        // If only one separator.
        if (m_config.SplitX)
        {
            min = clientRect.left + scaledOffsetFirst;
            max = clientRect.right - scaledOffsetSecond;
        }
        else if (m_config.SplitY)
        {
            min = clientRect.top + scaledOffsetFirst;
            max = clientRect.bottom - scaledOffsetSecond;
        }

        // Process separators only if they exist
        const auto& [previousSeparator, nextSeparator, previousSubstrate, nextSubstrate] { m_getHwndAdjacentElements };

        // Processing of separators
        const auto processSeparator = [&](HWND hSep, bool isPrevious)
            {
                RECT sepRect;
                if (!hSep || !Error(IsWindow(hSep)) || !Error(GetWindowRect(hSep, &sepRect))) return;

                const bool isX = m_config.SplitX;
                const LONG sepPos = isX ? sepRect.left : sepRect.top;

                if (isPrevious)
                {
                    min = (sepPos - (isX ? parentRect.left : parentRect.top)) + scaledOffsetFirst;
                }
                else
                {
                    const LONG clientSize = isX ? clientRect.right : clientRect.bottom;
                    const LONG parentSize = isX ? parentRect.right : parentRect.bottom;
                    max = (clientSize - (parentSize - sepPos)) - scaledOffsetSecond;
                }
            };

        processSeparator(previousSeparator, true);
        processSeparator(nextSeparator, false);


        auto& axisVar = m_config.SplitX ? wpos->x : wpos->y;
        auto& sizeVar = m_config.SplitX ? wpos->cy : wpos->cx;
        auto& fixedPos = m_config.SplitX ? wpos->y : wpos->x;

        if (axisVar && max >= min)
        {
            // Limit the window coordinates to X or Y axis
            axisVar = std::clamp(axisVar, min, max);

            // Limit movement only vertically or horizontally
            fixedPos = m_config.SplitX ? clientRect.top : clientRect.left;
            sizeVar = m_config.SplitX ?
                (clientRect.bottom - clientRect.top) :
                (clientRect.right - clientRect.left);

            // This will keep the dividers in the new position when the parent window changes.
            if (m_rewrite)
            {
                m_config.SplitX ? SetLeft(axisVar) : SetTop(axisVar);
                // So that in RESIZING_CHILD_WINDOW_FROM_INBUILT the *m_difference variable always starts from zero.
                // Otherwise, the separator will "jump" because the GetLeft value will be changed here.
                m_difference = m_config.SplitX
                    ? (clientRect.right - m_parentWindowWidth)
                    : (clientRect.bottom - m_parentWindowHeight);
            }
        }
    }
    break;
    case WM_MOVING:
    {
        // Allow overwriting GetLeft and GetTop values when separator is moved.
        m_rewrite = true;
    }
    break;

    case WM_NCLBUTTONDOWN:
    {
        // !Whatever is written according to the WM_NCLBUTTONDOWN message before calling DefWindowProc 
        // will be executed when the mouse is pressed, and after DefWindowProc - when the mouse is released.

        m_pressing = true;
        SendMessageW(GetHwnd(), WM_DISPLAYCHANGE, NULL, lParam);

        // Separators must be first in the Z order.
        Error(SetWindowPos(GetHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE));

        DefWindowProc(GetHwnd(), message, wParam, lParam);
        Beep(1000, 10);

        MovingSubstrate();

        m_pressing = false;
        SendMessageW(GetHwnd(), WM_DISPLAYCHANGE, NULL, lParam);

        ReleaseCapture();
    }
    break;
    case WM_NCLBUTTONUP:
    {
        DefWindowProc(GetHwnd(), message, wParam, lParam);
    }
    break;
    case WM_NCHITTEST:
    {
        RECT rc; Error(GetClientRect(GetHwnd(), &rc));
        HelperWTools tools;

        return tools.NCHitTest(GetHwnd(), lParam, 0, rc.bottom, false);
    }
    break;
    case WM_COMMAND:
    {
        UINT wmId = LOWORD(wParam);

        switch (wmId)
        {
        case GETTING_DATA_DURING_INITIALIZATION:
        {
            // Get the dimensions of the parent window at the time of creation.
            const HWND parent{ HWNDError(GetAncestor(GetHwnd(), GA_PARENT)) };
            RECT rc; Error(GetClientRect(parent, &rc));

            m_parentWindowWidth = rc.right;
            m_parentWindowHeight = rc.bottom;
        }
        break;
        case RESIZING_CHILD_WINDOW_FROM_INBUILT:
        {
            // We remove the ability to overwrite GetLeft and GetTop values 
            // to avoid conflict with WM_WINDOWPOSCHANGING
            m_rewrite = false;

            HWND parent = HWNDError(GetAncestor(GetHwnd(), GA_PARENT));
            RECT rc; Error(GetClientRect(parent, &rc));

            INT delta{};

            if (!rc.right || !rc.bottom)
            {
                // When the window is minimized, restore the value from the buffer.
                delta = m_deltaBuffer;
            }
            else
            {
                delta = (m_transform & ui_transform::restore_lx)
                    ? (rc.right - m_parentWindowWidth)
                    : (rc.bottom - m_parentWindowHeight);
                m_deltaBuffer = delta;
            }

            const LONG differenceX = (m_transform & ui_transform::restore_lx)
                ? delta - m_difference
                : 0;

            const LONG differenceY = (m_transform & ui_transform::restore_ty)
                ? delta - m_difference
                : 0;

            Error(SetWindowPos(GetHwnd(), nullptr,
                GetLeft() + differenceX,
                GetTop() + differenceY,
                GetRight(),
                GetBottom(),
                SWP_NOZORDER | SWP_NOACTIVATE));

            MovingSubstrate();
        }
        break;
        case OBTAIN_HWND_OF_NEIGHBORING_SEPARATORS:
        {
            m_getHwndAdjacentElements = *reinterpret_cast<std::tuple<HWND, HWND, HWND, HWND>*>(lParam);
            m_rewrite = true;
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

void UI::Separator::DrawSeparator(
    const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget,
    const D2D1_SIZE_F& size,
    UINT pressing
)
{
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pBrush{};
    D2D1_COLOR_F color{ D2D1::ColorF(UI::Colors::InternalBacking, pressing ? 1.0f : 0.0f) };

    UI::ThrowIfFailed(pRenderTarget->CreateSolidColorBrush(color, &pBrush));

    D2D1_ROUNDED_RECT roundedRectButton{};
    roundedRectButton = D2D1::RoundedRect(D2D1::RectF(0.0f, 0.0f, size.width, size.height),
        4.0f,
        4.0f
    );
    pRenderTarget->FillRoundedRectangle(roundedRectButton, pBrush.Get());

    D2D1_POINT_2F center{ size.width / 2.0f, size.height / 2.0f };
    pRenderTarget->FillEllipse(D2D1::Ellipse(center, 1.75f, 1.75f), GetCaching()->GetPopUpBrush());

    for (size_t i = 1; i <= 2; i++)
        pRenderTarget->FillEllipse(D2D1::Ellipse(
            m_config.SplitX ? D2D1_POINT_2F{ center.x, center.y + 8.0f * i } : D2D1_POINT_2F{ center.x + 8.0f * i, center.y },
            1.75f, 1.75f), GetCaching()->GetPopUpBrush());
        
    
    for (size_t i = 1; i <= 2; i++)
        pRenderTarget->FillEllipse(D2D1::Ellipse(
            m_config.SplitX ? D2D1_POINT_2F{ center.x, center.y - 8.0f * i } : D2D1_POINT_2F{ center.x - 8.0f * i, center.y },
            1.75f, 1.75f), GetCaching()->GetPopUpBrush());
}

HRESULT UI::Separator::Draw()
{
    HRESULT hr{ S_OK };

    // D2D1DeviceContext needs to be updated.
    GetComposition()->ConfigureSwapChain(GetHwnd());

    auto deviceContext = GetComposition()->GetID2D1DeviceContext();
    auto swapChain = GetComposition()->GetIDXGISwapChain();

    if (deviceContext && swapChain)
    {
        D2D1_SIZE_F size = deviceContext->GetSize();

        deviceContext->BeginDraw();
        deviceContext->Clear();

        switch (m_draw)
        {
        case ui_draw::separator::window:
            DrawSeparator(deviceContext, size, m_pressing);
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