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

#include "..//Inc/MouseTrackEvents.h"
#include <windowsx.h>

// Initialize members.
//
MouseTrackEvents::MouseTrackEvents() :
    m_bMouseTracking(false)
{
}
bool MouseTrackEvents::OnButtonDown(HWND hwnd, RECT rc)
{
    // Capturing the coordinates of a left mouse button click.
    SetCapture(hwnd);

    InvalidateRect(hwnd, &rc, FALSE);

    return true;
}

bool MouseTrackEvents::OnButtonUp(HWND hwnd, RECT rc)
{
    // For the left-button-up message, 
    // simply call ReleaseCapture to release the mouse capture.
    ReleaseCapture();

    InvalidateRect(hwnd, &rc, FALSE);

    return false;
}

// The function tracks the cursor only when it is inside the specified area.
//
bool MouseTrackEvents::OnMouseMove(this MouseTrackEvents& object, HWND hwnd, RECT rc, int pixelX, int pixelY)
{
    bool hit{};

    if (!object.m_bMouseTracking)
    {
        // Enable mouse tracking.
        TRACKMOUSEEVENT tme{};
        tme.cbSize = sizeof(tme);
        tme.hwndTrack = hwnd;
        tme.dwFlags = TME_LEAVE | TME_HOVER;
        tme.dwHoverTime = HOVER_DEFAULT;
        TrackMouseEvent(&tme);
        object.m_bMouseTracking = true;

        DPIScale dpi;

        float scale{ dpi.Initialize(hwnd) };

        const float dipX = dpi.PixelsToDipsX(scale, pixelX);
        const float dipY = dpi.PixelsToDipsY(scale, pixelY);

        // If the cursor is in the specified area(RECT), 
        // then "1" is returned and the area is updated.
        //
        if (dipX >= rc.left && dipX <= rc.right)
        {
            if (dipY >= rc.top && dipY <= rc.bottom)
            {
                InvalidateRect(hwnd, &rc, FALSE);
                hit = true;
            }
            else
            {
                InvalidateRect(hwnd, &rc, FALSE);
                hit = false;
            }
        }
        else
        {
            InvalidateRect(hwnd, &rc, FALSE);
            hit = false;
        }
    }

    return hit;
}

// The function tracks the cursor not only inside a given area but also outside it.
//
bool MouseTrackEvents::OnMouseMoveOut(this MouseTrackEvents& object, HWND hwnd)
{
    bool hit{};

    if (!object.m_bMouseTracking)
    {
        // Enable mouse tracking.
        TRACKMOUSEEVENT tme{};
        tme.cbSize = sizeof(tme);
        tme.hwndTrack = hwnd;
        tme.dwFlags = TME_LEAVE | TME_HOVER;
        tme.dwHoverTime = HOVER_DEFAULT;
        TrackMouseEvent(&tme);
        object.m_bMouseTracking = true;

        hit = true;
    }

    return hit;
}


bool MouseTrackEvents::Reset(this MouseTrackEvents& object, HWND hwnd)
{
    object.m_bMouseTracking = false;

    InvalidateRect(hwnd, NULL, FALSE);
    return false;
}

RECT MouseTrackEvents::controlElementArea(
    HWND hwnd,
    unsigned int positionX,
    unsigned int positionY,
    unsigned int widthControl,
    unsigned int heightControl
)
{
    // The control's position is calculated from the top left corner.
    RECT rc;

    if (widthControl == 0 || heightControl == 0)
    {
        GetClientRect(hwnd, &rc);
    }
    else
    {
        rc.left = positionX;
        rc.top = positionY;
        rc.right = widthControl;
        rc.bottom = heightControl;
    }

    return rc;
}

bool MouseTrackEvents::ElementSelected(const HWND& hwnd, const RECT& rect, const LPARAM& lParam)
{   
    MouseTrackEvents mouseEvents;
    return mouseEvents.OnMouseMove(hwnd, rect, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
}