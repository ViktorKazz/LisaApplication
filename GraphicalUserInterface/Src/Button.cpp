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

#include "..//Inc/Button.h"

LRESULT UI::Button::SimpleCommand(HWND hwnd, LPARAM lParam, UI::ui_command cmd)
{
    // Finding the parent window.
    HWND parent = GetAncestor(hwnd, GA_ROOT);

    if (cmd & ui_command::close)
        DestroyWindow(parent);
    if (cmd & ui_command::restore)
        SendMessageW(parent, WM_COMMAND, WINDOW_RESTORE_MAXIMIZE, lParam);
    if (cmd & ui_command::minimize)
        SendMessageW(parent, WM_COMMAND, WINDOW_MINIMIZE, lParam);

    return 0;
}

bool UI::Button::ChangeIcon()
{
    RECT workAreaSize{};
    Error(SystemParametersInfoW(SPI_GETWORKAREA, 0, &workAreaSize, 0));
    RECT clientRc;
    Error(GetClientRect(GetAncestor(GetHwnd(), GA_PARENT), &clientRc));

    bool changeIcon{};

    if (clientRc.right == workAreaSize.right && clientRc.bottom == workAreaSize.bottom)
        changeIcon = true;
    else
        changeIcon = false;

    return changeIcon;
}

INT UI::Button::DualPopUpBt(HWND hwnd, LPARAM lParam, ui_draw::button draw)
{
    MouseTrackEvents track;
    INT dual{};

    if (draw & ui_draw::button::pup_dual)
    {
        RECT rcD; Error(GetClientRect(hwnd, &rcD));

        RECT dualA{ rcD }, dualB{ rcD };

        dualA.right = ((dualA.right - dualA.left) - gImagePlace) + dualA.left;
        dualB.left = ((dualA.right - dualA.left) - gImagePlace) + dualB.left;

        if (track.ElementSelected(hwnd, dualA, lParam))
            dual = -1;
        else if (track.ElementSelected(hwnd, dualB, lParam))
            dual = 1;
    }
    else
    {
        dual = 0;
    }

    return dual;
}

// The function opens a pop-up window and moves it to the desired position.
// NOFRAME or PUP_TRANSITION
void UI::Button::OpenPopUpAndMove(HWND hwnd, HWND root, ui_draw::button draw)
{
    RECT buttonRc{}, windowRc{};
    Error(GetWindowRect(hwnd, &buttonRc));

    m_callHwnd = m_config.Window(GetD11Device(), GetCaching(), root, GetHInstance());

    Error(GetWindowRect(m_callHwnd, &windowRc));

    if (draw & ui_draw::button::noframe)
    {
        Error(SetWindowPos(m_callHwnd, 0, buttonRc.left, buttonRc.bottom, windowRc.right, windowRc.bottom,
            SWP_NOZORDER | SWP_NOACTIVATE));
    }
    else if (draw & ui_draw::button::pup_transition)
    {
        Error(SetWindowPos(m_callHwnd, 0, buttonRc.right, buttonRc.top, windowRc.right, windowRc.bottom,
            SWP_NOZORDER | SWP_NOACTIVATE));
    }

    ShowWindow(m_callHwnd, SW_SHOWNORMAL);
}

// Function for sending messages to children's windows to close 
// those windows that could have been called by one of the children.
void UI::Button::ClosePopUpWindow(LPARAM lParam, UINT msg, bool selectReset)
{
    std::vector<HWND> child{ FindChild(m_callHwnd) };

    // Ask other children of the parent window if any of them have a pop-up window running.
    for (auto i : child)
        SendMessageW(i, WM_COMMAND, msg, lParam);

    // A button that has an open window moves focus to the parent window before closing it.
    // If this is not done, then after the command to close the window, 
    // the focus is transferred to the main window, 
    // thereby closing the entire chain of windows when this is not necessary.
    SetFocus(GetAncestor(GetHwnd(), GA_PARENT));
    DestroyWindow(m_callHwnd);

    // Destroy the object safely.
    m_callHwnd = nullptr;
    m_select = selectReset;
}

LRESULT UI::Button::MessageHandled(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result{};
	PAINTSTRUCT ps{};
    MouseTrackEvents track;

    switch (message)
    {
    case WM_LBUTTONDOWN:
    {
        RECT rc; Error(GetClientRect(GetHwnd(), &rc));
        m_pressing = track.OnButtonDown(GetHwnd(), rc);

        if (m_config.Window && m_draw & ui_draw::button::noframe)
        {
            if (!m_callHwnd)
            {
                OpenPopUpAndMove(GetHwnd(), GetHwnd(), m_draw);
            }
            else
            {
                DestroyWindow(m_callHwnd);
                m_callHwnd = nullptr;
            }
        }

        if (m_config.Window && m_draw & ui_draw::button::pup_transition)
            if (m_callHwnd)
                SetFocus(m_callHwnd);

        if (m_pressing && m_select)
            if (m_command & ui_command::down)
                return SimpleCommand(GetHwnd(), lParam, m_command);
    }
    return 0;
    break;
    case WM_LBUTTONUP:
    {       
        RECT rc;
        Error(GetClientRect(GetHwnd(), &rc));
        m_pressing = track.OnButtonUp(GetHwnd(), rc);
    
        // Define additional rectangles for the double command button.
        INT dual{ DualPopUpBt(GetHwnd(),lParam, m_draw) };


        if (m_config.Window && dual == 1 && m_draw & ui_draw::button::pup_dual)
            m_config.Window(GetD11Device(), GetCaching(), GetAncestor(GetHwnd(), GA_ROOTOWNER), GetHInstance());

        else if (m_config.Command && dual == -1 && m_draw & ui_draw::button::pup_dual)
            m_config.Command({});

        else if (m_config.Window && m_draw & ui_draw::button::pup_simple)
            m_config.Window(GetD11Device(), GetCaching(), GetAncestor(GetHwnd(), GA_ROOTOWNER), GetHInstance());

        else if (m_config.Command && m_draw & ui_draw::button::frame && m_select)
            m_config.Command({});

        if (!m_pressing && m_select)
            if (m_command & ui_command::up)
                return SimpleCommand(GetHwnd(), lParam, m_command);                    
    }
    return 0;
    break;
    case WM_MOUSEMOVE:
    {
        // Define additional rectangles for the double command button.
        //bool dual{ DualPopUpBt(GetHwnd(),lParam, m_flags.Draw)};

        if (m_draw & ui_draw::button::noframe ||
            m_draw & ui_draw::button::pup_transition ||
            m_draw & ui_draw::button::pup_simple ||
            m_draw & ui_draw::button::pup_dual
            )
        {
            // Finding children of a parent window.
            // Except for the children of the button that is highlighted.
            HWND hwndChild{};

            if (m_child.empty())
            {
                do
                {
                    hwndChild = FindWindowExA(GetAncestor(GetHwnd(), GA_PARENT), hwndChild, NULL, NULL);

                    if (hwndChild && hwndChild != GetHwnd())
                        m_child.push_back(hwndChild);

                } while (hwndChild);
            }

            // Ask other children of the parent window if any of them have a pop-up window running.
            if (!m_callHwnd)
            {
                for (auto i : m_child)
                    SendMessageW(i, WM_COMMAND, CHILDRENS_WINDOW_SURVEY, LPARAM(GetHwnd()));
            }
            // To close the pop-up window that was opened by the previous button.
            else
            {
                std::vector<HWND> child{ FindChild(m_callHwnd) };

                // Ask other children of the parent window if any of them have a pop-up window running.
                for (auto i : child)
                    SendMessageW(i, WM_COMMAND, CHILDRENS_WINDOW_SURVEY, LPARAM(GetHwnd()));
            }

            if (m_config.Window && m_draw & ui_draw::button::pup_transition)
                if (!m_callHwnd)
                    OpenPopUpAndMove(GetHwnd(), m_config.Root, m_draw);
        }


        RECT rc; Error(GetClientRect(GetHwnd(), &rc));
        m_select = track.OnMouseMove(GetHwnd(), rc, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        m_intentionToClose = m_select;
    }
    return 0;
    break;
    case WM_MOUSELEAVE:
    {
        if (m_draw & ui_draw::button::noframe || m_draw & ui_draw::button::pup_transition)
        {
            if (!m_callHwnd)
                m_select = track.Reset(GetHwnd());

            m_intentionToClose = track.Reset(GetHwnd());           
        }
        else
        {
            m_select = track.Reset(GetHwnd());
        }
    }
    return 0;
    break;
    case WM_COMMAND:
    {
        UINT wmId = LOWORD(wParam);

        switch (wmId)
        {
        case RESIZING_ELEMENTS:
        {
            INT l{ GetLeft() }, t{ GetTop() }, r{ GetRight() }, b{ GetBottom() };

            // for bottom bar elements
            RECT* data = reinterpret_cast<RECT*>(lParam);

            if (m_transform & UI::ui_transform::stretching_x)
                l = { data->left }, t = { data->top }, r = { data->right - data->left }, b = { data->bottom };

            Error(SetWindowPos(GetHwnd(), 0, l, t, r, b, SWP_NOZORDER | SWP_NOACTIVATE));
        }
        break;
        case CHILDRENS_WINDOW_SURVEY:
        {
            // Accept the request - whether a pop-up window was launched from this button.

            HWND theApplicantsHWND = HWND(lParam);

            // If the pop-up window was launched, we send a response back.
            if (theApplicantsHWND && m_callHwnd)
            {
                // Send it along the chain to the following windows.
                ClosePopUpWindow(lParam, CHILDRENS_WINDOW_SURVEY, track.Reset(GetHwnd()));
                SendMessageW(theApplicantsHWND, WM_COMMAND, REVERSE_COMMAND_TO_OPEN_A_POP_UP_WINDOW, lParam);
            }
        }
        break;
        case REVERSE_COMMAND_TO_OPEN_A_POP_UP_WINDOW:
        {
            // Open a new pop-up window.
            if (m_config.Window)
            {
                if (!m_callHwnd)
                {
                    HWND root{};
                    if (m_draw & ui_draw::button::pup_transition)
                        root = m_config.Root;
                    else if (m_draw & ui_draw::button::noframe)
                        root = GetHwnd();

                    if (m_draw & ui_draw::button::noframe || m_draw & ui_draw::button::pup_transition)
                        OpenPopUpAndMove(GetHwnd(), root, m_draw);
                }

            }
        }
        break;
        case MESSAGE_TO_ROOT_NOFRAME_BUTTON:
        {
            // Accepting a message from a window that has lost focus
            // to close windows in a chain.
            if (m_callHwnd && !m_intentionToClose)
            {
                // Send it along the chain to the following windows.
                ClosePopUpWindow(lParam, MESSAGE_TO_ROOT_NOFRAME_BUTTON, track.Reset(GetHwnd()));
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

        // If the window is maximized, then change the Maximize/Restore icon.
        m_changeIcon = ChangeIcon();

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

void UI::Button::Close(
    const Microsoft::WRL::ComPtr<ID2D1DeviceContext6>& pDeviceContext,
    const D2D1_SIZE_F& size,
    UINT selection,
    UINT pressing
)
{
    // Drawing a close button.
    const D2D1_RECT_F& rect{ 0.0f, 0.0f, size.width, size.height };

    if (selection == 0 && pressing == 0)
    {
        pDeviceContext->DrawBitmap(GetCaching()->GetIconButtonClose(), rect);
    }

    if (selection == 1 && pressing == 0)
    {
        Microsoft::WRL::ComPtr<ID2D1Effect> highlightsAndShadowsEffect;
        pDeviceContext->CreateEffect(CLSID_D2D1HighlightsShadows, &highlightsAndShadowsEffect);

        highlightsAndShadowsEffect->SetInput(0, GetCaching()->GetIconButtonClose());
        highlightsAndShadowsEffect->SetValue(D2D1_HIGHLIGHTSANDSHADOWS_PROP_HIGHLIGHTS, 0.05f);
        highlightsAndShadowsEffect->SetValue(D2D1_HIGHLIGHTSANDSHADOWS_PROP_SHADOWS, 0.5f);
        highlightsAndShadowsEffect->SetValue(D2D1_HIGHLIGHTSANDSHADOWS_PROP_CLARITY, 0.2f);
        highlightsAndShadowsEffect->SetValue(D2D1_HIGHLIGHTSANDSHADOWS_PROP_INPUT_GAMMA, D2D1_HIGHLIGHTSANDSHADOWS_INPUT_GAMMA_LINEAR);
        highlightsAndShadowsEffect->SetValue(D2D1_HIGHLIGHTSANDSHADOWS_PROP_MASK_BLUR_RADIUS, 1.0f);
        
        pDeviceContext->DrawImage(highlightsAndShadowsEffect.Get());
    }

    if (pressing == 1)
    {
        pDeviceContext->DrawBitmap(GetCaching()->GetIconButtonClose(), rect);
    }
}

void UI::Button::MaximizeRestore(
    const Microsoft::WRL::ComPtr<ID2D1DeviceContext6>& pDeviceContext,
    const D2D1_SIZE_F& size,
    UINT selection,
    UINT pressing
)
{
    // Drawing a maximize/restore button.
    const D2D1_RECT_F& rect{ 0.0f, 0.0f, size.width, size.height };

    if (selection == 0 && pressing == 0)
    {
        pDeviceContext->DrawBitmap(GetCaching()->GetIconButtonRestore(), rect);
    }

    if (selection == 1 && pressing == 0)
    {
        Microsoft::WRL::ComPtr<ID2D1Effect> highlightsAndShadowsEffect;
        pDeviceContext->CreateEffect(CLSID_D2D1HighlightsShadows, &highlightsAndShadowsEffect);

        highlightsAndShadowsEffect->SetInput(0, GetCaching()->GetIconButtonRestore());
        highlightsAndShadowsEffect->SetValue(D2D1_HIGHLIGHTSANDSHADOWS_PROP_HIGHLIGHTS, 0.5f);
        highlightsAndShadowsEffect->SetValue(D2D1_HIGHLIGHTSANDSHADOWS_PROP_SHADOWS, 0.5f);
        highlightsAndShadowsEffect->SetValue(D2D1_HIGHLIGHTSANDSHADOWS_PROP_CLARITY, 0.2f);
        highlightsAndShadowsEffect->SetValue(D2D1_HIGHLIGHTSANDSHADOWS_PROP_INPUT_GAMMA, D2D1_HIGHLIGHTSANDSHADOWS_INPUT_GAMMA_LINEAR);
        highlightsAndShadowsEffect->SetValue(D2D1_HIGHLIGHTSANDSHADOWS_PROP_MASK_BLUR_RADIUS, 1.0f);

        pDeviceContext->DrawImage(highlightsAndShadowsEffect.Get());
    }

    if (pressing == 1)
    {
        pDeviceContext->DrawBitmap(GetCaching()->GetIconButtonRestore(), rect);
    }
}

void UI::Button::Minimize(
    const Microsoft::WRL::ComPtr<ID2D1DeviceContext6>& pDeviceContext,
    const D2D1_SIZE_F& size,
    UINT selection,
    UINT pressing
)
{
    // Drawing a minimize button.
    const D2D1_RECT_F& rect{ 0.0f, 0.0f, size.width, size.height };

    if (selection == 0 && pressing == 0)
    {
        pDeviceContext->DrawBitmap(GetCaching()->GetIconButtonMinimize(), rect);
    }

    if (selection == 1 && pressing == 0)
    {
        Microsoft::WRL::ComPtr<ID2D1Effect> highlightsAndShadowsEffect;
        pDeviceContext->CreateEffect(CLSID_D2D1HighlightsShadows, &highlightsAndShadowsEffect);

        highlightsAndShadowsEffect->SetInput(0, GetCaching()->GetIconButtonMinimize());
        highlightsAndShadowsEffect->SetValue(D2D1_HIGHLIGHTSANDSHADOWS_PROP_HIGHLIGHTS, 0.25f);
        highlightsAndShadowsEffect->SetValue(D2D1_HIGHLIGHTSANDSHADOWS_PROP_SHADOWS, 0.5f);
        highlightsAndShadowsEffect->SetValue(D2D1_HIGHLIGHTSANDSHADOWS_PROP_CLARITY, 0.2f);
        highlightsAndShadowsEffect->SetValue(D2D1_HIGHLIGHTSANDSHADOWS_PROP_INPUT_GAMMA, D2D1_HIGHLIGHTSANDSHADOWS_INPUT_GAMMA_LINEAR);
        highlightsAndShadowsEffect->SetValue(D2D1_HIGHLIGHTSANDSHADOWS_PROP_MASK_BLUR_RADIUS, 1.0f);

        pDeviceContext->DrawImage(highlightsAndShadowsEffect.Get());
    }

    if (pressing == 1)
    {
        pDeviceContext->DrawBitmap(GetCaching()->GetIconButtonMinimize(), rect);
    }
}

void UI::Button::Frame(
    const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget,
    const std::wstring& name,
    const D2D1_SIZE_F& size,
    UINT selection,
    UINT pressing
)
{
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pFillBrush;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pFrameBrush;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pTextBrush;

    // Drawing a button with rounded Corners.

    D2D1_ROUNDED_RECT roundedRect{};
    roundedRect = D2D1::RoundedRect(D2D1::RectF(2.0f, 2.0f, size.width - 2.0f, size.height - 2.0f),
        6.0f, 6.0f);

    if (!selection)
    {
        pFillBrush = GetCaching()->GetButtonNoActive();
        pFrameBrush = GetCaching()->GetButtonFrameOutlinerNoActive();
        pTextBrush = GetCaching()->GetButtonTxtNoActive();
    }
    if (selection && !pressing)
    {
        pFillBrush = GetCaching()->GetButtonPreActive();
        pFrameBrush = GetCaching()->GetButtonFrameOutlinerPreActive();
        pTextBrush = GetCaching()->GetButtonTxtPreActive();
    }
    if (pressing)
    {
        pFillBrush = GetCaching()->GetButtonActive();
        pFrameBrush = GetCaching()->GetButtonFrameOutlinerActive();
        pTextBrush = GetCaching()->GetButtonTxtActive();
    }

    // Draw a frame for the button.          
    pRenderTarget->DrawRoundedRectangle(roundedRect, pFrameBrush.Get(), 0.75f);

    // Draw a rectangle with rounded corners for the button.
    pRenderTarget->FillRoundedRectangle(roundedRect, pFillBrush.Get());

    // Drawing text in to button.
    pRenderTarget->DrawTextW(
        name.c_str(),
        lstrlen(name.c_str()),
        GetCaching()->GetButtonTextFormat(),
        D2D1::RectF(2.0f, 2.0f, size.width - 2.0f, size.height - 2.0f),
        pTextBrush.Get()
    );
}

void UI::Button::NoFrame(
    const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget,
    const std::wstring& name,
    const D2D1_SIZE_F& size,
    UINT selection,
    UINT pressing
)
{
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pFillBrush;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pFrameBrush;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pTextBrush;

    // Drawing a button with rounded Corners.

    D2D1_ROUNDED_RECT roundedRectButton{};
    roundedRectButton = D2D1::RoundedRect(D2D1::RectF(2.0f, 2.0f, size.width - 2.0f, size.height - 2.0f),
        6.0f, 6.0f);

    if (!selection)
    {
        pFillBrush = GetCaching()->GetButtonNoActiveNF();
        pFrameBrush = GetCaching()->GetButtonNFrameOutlinerNoActive();
        pTextBrush = GetCaching()->GetButtonTxtNFNoActive();
    }
    if (selection && !pressing)
    {
        pFillBrush = GetCaching()->GetButtonPreActiveNF();
        pFrameBrush = GetCaching()->GetButtonNFrameOutlinerPreActive();
        pTextBrush = GetCaching()->GetButtonTxtPreActive();
    }
    if (pressing)
    {
        pFillBrush = GetCaching()->GetButtonActiveNF();
        pFrameBrush = GetCaching()->GetButtonNFrameOutlinerActive();
        pTextBrush = GetCaching()->GetButtonTxtPreActive();
    }

    // Draw a frame for the button.          
    pRenderTarget->DrawRoundedRectangle(roundedRectButton, pFrameBrush.Get(), 0.75f);

    // Draw a rectangle with rounded corners for the button.
    pRenderTarget->FillRoundedRectangle(roundedRectButton, pFillBrush.Get());

    // Drawing text in to button.
    pRenderTarget->DrawTextW(
        name.c_str(),
        lstrlen(name.c_str()),
        GetCaching()->GetButtonTextFormat(),
        D2D1::RectF(2.0f, 2.0f, size.width - 2.0f, size.height - 2.0f),
        pTextBrush.Get()
    );
}

void UI::Button::PopUp(
    const Microsoft::WRL::ComPtr<ID2D1DeviceContext6>& pRenderTarget,
    const Microsoft::WRL::ComPtr<IWICImagingFactory2>& pWICFactory,
    const varimtx& imagePath,
    const D2D1_SIZE_F& size,
    UINT selection,
    UINT pressing,
    bool isTransition,
    bool isDual
)
{
    // Draw the background
    const Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>& pBackgroundBrush{ GetCaching()->GetPopUpBrush() };

    D2D1_ROUNDED_RECT roundedRectButton{};
    roundedRectButton = D2D1::RoundedRect(D2D1::RectF(0.0f, 0.0f, size.width, size.height), 0.0f, 0.0f);
    pRenderTarget->FillRoundedRectangle(roundedRectButton, pBackgroundBrush.Get());


    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pFillBrush;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pFrameBrush;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pTextBrush;

    // Drawing a button with rounded Corners.
    float iconPlace{ 26.0f };
    float imagePlace{ 26.0f };
    float nCl{ static_cast<float>(UI::gNonClientAreaSize) };

    const D2D1_RECT_F& rect = D2D1::RectF(iconPlace + (nCl / 3), 1.0f, size.width - 1.0f, size.height - 1.0f);

    const D2D1_ROUNDED_RECT& roundedRect = D2D1::RoundedRect(rect, 6.0f, 6.0f);


    if (!selection)
    {
        pFillBrush = GetCaching()->GetButtonNoActivePopUp();
        pFrameBrush = GetCaching()->GetButtonPFrameOutlinerNoActive();
        pTextBrush = GetCaching()->GetButtonTxtNoActive();
    }
    if (selection && !pressing)
    {
        pFillBrush = GetCaching()->GetButtonPreActivePopUp();
        pFrameBrush = GetCaching()->GetButtonPFrameOutlinerPreActive();
        pTextBrush = GetCaching()->GetButtonTxtPreActive();
    }
    if (pressing)
    {
        pFillBrush = GetCaching()->GetButtonActivePopUp();
        pFrameBrush = GetCaching()->GetButtonPFrameOutlinerActive();
        pTextBrush = GetCaching()->GetButtonTxtActive();
    }

    // Draw a frame for the button.          
    pRenderTarget->DrawRoundedRectangle(roundedRect, pFrameBrush.Get(), 0.75f);

    // Draw a rectangle with rounded corners for the button.
    pRenderTarget->FillRoundedRectangle(roundedRect, pFillBrush.Get());

    // Drawing text in to button.

    // Draw main text.
    if (!GetWindowTitle().empty())
    {
        pRenderTarget->DrawTextW(
            GetWindowTitle().c_str(),
            lstrlen(GetWindowTitle().c_str()),
            GetCaching()->GetButtonPopUpTextFormat(),
            D2D1::RectF(
                (iconPlace + (nCl / 3) * 2), 
                (nCl / 3) + 1.0f,
                size.width - nCl, 
                size.height - (nCl / 3)
            ),
            pTextBrush.Get()
        );
    }

    if (!m_config.MiddleName.empty())
    {
        // Draw hot key text.
        pRenderTarget->DrawTextW(
            m_config.MiddleName.c_str(),
            lstrlen(m_config.MiddleName.c_str()),
            GetCaching()->GetButtonPopUpExtraTextFormat(),
            D2D1::RectF(0.0f, (nCl / 3) + 1.0f, size.width - imagePlace, size.height - (nCl / 3)),
            pTextBrush.Get()
        );
    }

    if (isTransition)
    {
        // Drawing an arrow.

        Microsoft::WRL::ComPtr<ID2D1Effect> translationEffect{ nullptr };
        D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Translation({ size.width - gPopUpButtonHeight, 0.0f });

        pRenderTarget->CreateEffect(CLSID_D2D12DAffineTransform, &translationEffect);

        if (!selection && !pressing)
        {
            translationEffect->SetInput(0, GetCaching()->GetIconButtonArrowNoSelected());
            translationEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, matrix);
        }
        if (selection && !pressing)
        {
            translationEffect->SetInput(0, GetCaching()->GetIconButtonArrowSelected());
            translationEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, matrix);
        }
        if (pressing)
        {
            translationEffect->SetInput(0, GetCaching()->GetIconButtonArrowPressed());
            translationEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, matrix);

        }
        pRenderTarget->DrawImage(translationEffect.Get());
    }

    if (isDual)
    {
        // Draw a gear.        

        Microsoft::WRL::ComPtr<ID2D1Effect> translationEffect{ nullptr };
        D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Translation({ size.width - gPopUpButtonHeight, 0.0f });
        
        pRenderTarget->CreateEffect(CLSID_D2D12DAffineTransform, &translationEffect);

        if (!selection && !pressing)
        {
            translationEffect->SetInput(0, GetCaching()->GetIconButtonGearNoSelected());
            translationEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, matrix);
        }
        if (selection && !pressing)
        {
            translationEffect->SetInput(0, GetCaching()->GetIconButtonGearSelected());
            translationEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, matrix);
        }
        if (pressing)
        {
            translationEffect->SetInput(0, GetCaching()->GetIconButtonGearPressed());
            translationEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, matrix);
            
        }
        pRenderTarget->DrawImage(translationEffect.Get());
    }

    // Drawing an image.

    std::wstring img{};
    // Access the std::wstring
    if (std::holds_alternative<std::wstring>(imagePath))
        img = std::get<std::wstring>(imagePath);

    if (!img.empty())
    {
        HRESULT hr{ S_OK };
        Microsoft::WRL::ComPtr<ID2D1Bitmap> pBitmap{ nullptr };
        hr = m_tools->LoadBitmapFromFile(
            pRenderTarget, pWICFactory, pBitmap.GetAddressOf(), img.c_str(), 0, 0);

        if (SUCCEEDED(hr))
        {
            D2D1_SIZE_F sizeBitmap{ pBitmap->GetSize() };

            pRenderTarget->DrawBitmap(
                pBitmap.Get(),
                D2D1::RectF(
                    ((nCl / 2) - 1.0f),
                    0.0f,
                    sizeBitmap.width + ((nCl / 2) - 1.0f),
                    sizeBitmap.height
                )
            );

            pBitmap.Reset();
        }
    }
}

HRESULT UI::Button::Draw()
{
    HRESULT hr{ S_OK };

    // D2D1DeviceContext needs to be updated.
    GetComposition()->ConfigureSwapChain(GetHwnd());

    const auto& deviceContext = GetComposition()->GetID2D1DeviceContext().Get();
    const auto& WICFactory = GetComposition()->GetWICFactory().Get();
    const auto& swapChain = GetComposition()->GetIDXGISwapChain().Get();

    if (deviceContext && swapChain)
    {
        D2D1_SIZE_F size = deviceContext->GetSize();

        deviceContext->BeginDraw();
        deviceContext->Clear();

        switch (m_draw)
        {
        case ui_draw::button::close:
            Close(deviceContext, size, m_select, m_pressing);
            break;
        case ui_draw::button::restore:
            MaximizeRestore(deviceContext, size, m_select, m_pressing);
            break;
        case ui_draw::button::minimize:
            Minimize(deviceContext, size, m_select, m_pressing);
            break;
        case ui_draw::button::frame:
            Frame(deviceContext, GetWindowTitle(), size, m_select, m_pressing);
            break;
        case ui_draw::button::noframe:
            NoFrame(deviceContext, GetWindowTitle(), size, m_select, m_pressing);
            break;
        case ui_draw::button::pup_simple:
            PopUp(deviceContext, WICFactory, m_config.ImgTxt, size, m_select, m_pressing);
            break;
        case ui_draw::button::pup_transition:
            PopUp(deviceContext, WICFactory, m_config.ImgTxt, size, m_select, m_pressing, true);
            break;
        case ui_draw::button::pup_dual:
            PopUp(deviceContext, WICFactory, m_config.ImgTxt, size, m_select, m_pressing, false, true);
            break;
        default:
            throw std::runtime_error("Drawing function not found.");
            break;
        }

        //There was an error from the noframe button (index 8)

        hr = deviceContext->EndDraw();
        // Make the swap chain available to the composition engine.
        hr = swapChain->Present(0, 0);
    }

    return hr;
}
