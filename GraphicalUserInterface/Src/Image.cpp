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

#include "..//Inc/Image.h"

LRESULT CALLBACK UI::Image::MessageHandled(UINT message, WPARAM wParam, LPARAM lParam)
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
        case GETTING_DATA_DURING_INITIALIZATION:
        {
            // Get the dimensions of the parent window at the time of creation.
            RECT rc;
            Error(GetClientRect(GetAncestor(GetHwnd(), GA_PARENT), &rc));

            m_parentWindowWidth = rc.right;
            m_parentWindowHeight = rc.bottom;
        }
        break;
        case RESIZING_CHILD_WINDOW_FROM_INBUILT:
        {
            const HWND& parent = HWNDError(GetAncestor(GetHwnd(), GA_PARENT));

            RECT rc{}; Error(GetClientRect(parent, &rc));

            INT l{ GetLeft() }, t{ GetTop() }, r{ GetRight() }, b{ GetBottom() };

            if (m_transform & ui_transform::stretching_x)
                r = rc.right;

            if (m_transform & ui_transform::stretching_y)
                b = rc.bottom;

            /*if (m_flags.Transform & TRANSFORM::RESTORE_RX)
                l += (rc.right - m_parentWindowWidth);*/

            if (m_transform & ui_transform::restore_lx)
                l = rc.left;

            /*if (m_flags.Transform & TRANSFORM::RESTORE_TY)
                t += (rc.bottom - m_parentWindowHeight);*/

            if (m_transform & ui_transform::restore_by)
                t += (rc.bottom - m_parentWindowHeight);

            // Check for missing flag to avoid conflicts.
            if (m_transform != ui_transform::none)
            {
                Error(SetWindowPos(GetHwnd(), nullptr, l, t, r, b, SWP_NOZORDER | SWP_NOACTIVATE), 
                    "Window position update failed, RESIZING_CHILD_WINDOW_FROM_INBUILT");
            }
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

void UI::Image::DrawingImage(
    const Microsoft::WRL::ComPtr<ID2D1DeviceContext6>& pDeviceContext,
    const Microsoft::WRL::ComPtr<IWICImagingFactory2>& pWICFactory,
    const varimtx& inputImages,
    const D2D1_SIZE_F& size,
    size_t index
)
{
    // Drawing an image.
    
    // Using the helper function we will get the text.
    std::wstring img = UI::ReadVar<std::wstring>(inputImages, index);

    if (!img.empty())
    {
        HRESULT hr{ S_OK };
        
        hr = m_tools->LoadBitmapFromFile(pDeviceContext, pWICFactory, m_pBitmap.GetAddressOf(), img.c_str(), 0, 0);

        if (SUCCEEDED(hr))
        {
            D2D1_SIZE_F sizeBitmap{ m_pBitmap->GetSize() };

            float scale{};
            float indentX{}, indentY{};

            float narrownessRect1 = size.width / size.height;
            float narrownessRect2 = sizeBitmap.width / sizeBitmap.height;

            // Determine which rectangle is narrower.
            if (narrownessRect1 > narrownessRect2)
            {
                scale = size.height / sizeBitmap.height;
                indentX = (size.width - sizeBitmap.width * scale) / 2;
            }
            else
            {
                scale = size.width / sizeBitmap.width;
                indentY = (size.height - sizeBitmap.height * scale) / 2;
            }

            if(!m_translationEffect)
                pDeviceContext->CreateEffect(CLSID_D2D12DAffineTransform, &m_translationEffect);
            if(!m_rotationEffect)
                pDeviceContext->CreateEffect(CLSID_D2D12DAffineTransform, &m_rotationEffect);
            if(!m_scaleEffect)
                pDeviceContext->CreateEffect(CLSID_D2D1Scale, &m_scaleEffect);

            // Scale the image to fit the client area.
            m_scaleEffect->SetInput(0, m_pBitmap.Get());
            m_scaleEffect->SetValue(D2D1_SCALE_PROP_CENTER_POINT, D2D1::Vector2F(0.0f, 0.0f));
            m_scaleEffect->SetValue(D2D1_SCALE_PROP_SCALE, D2D1::Vector2F(scale, scale));

            
            // Rotate if necessary.
            /*rotationEffect->SetInputEffect(0, scaleEffect.Get());
            D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Rotation(45, D2D1::Point2F(rc.left, rc.top));
            rotationEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, matrix);*/

            // Move the image to the center of the client area.
            m_translationEffect->SetInputEffect(0, m_scaleEffect.Get());
            D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Translation({ indentX, indentY });
            m_translationEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, matrix);

            pDeviceContext->DrawImage(m_translationEffect.Get());

            m_pBitmap.Reset();
        }
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

HRESULT UI::Image::Draw()
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

        switch (m_draw)
        {
        case ui_draw::image::simple:
            DrawingImage(deviceContext, WICFactory, m_config.ImgTxt, size, m_index);
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