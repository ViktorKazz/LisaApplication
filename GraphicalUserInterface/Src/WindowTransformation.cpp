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

#include "..//Inc/WindowTransformation.h"
#include "..//Inc/HelperWindowTools.h"
#include "..//Inc/GlobalValue.h"

INT UI::WindowTransformation::RestoreX(
    HWND hwnd, 
    const RECT& rc, 
    LONG inLeft,
    LONG nonClientAreaSize,
    ui_transform transform,
    LONG parentWndWidth
)
{   
    INT left{ inLeft };
    HelperWTools tools;

    if (transform & ui_transform::restore_rx)
    {
        left = left + (rc.right - parentWndWidth);

        if (tools.ComparisonWindowSizes(GetAncestor(hwnd, GA_PARENT)))
            left += nonClientAreaSize;
    }
    if (transform & ui_transform::restore_lx)
    {
        if (tools.ComparisonWindowSizes(GetAncestor(hwnd, GA_PARENT)))
            left -= nonClientAreaSize;
    }

    return left;
}

INT UI::WindowTransformation::RestoreY(
    HWND hwnd, 
    const RECT& rc, 
    LONG inTop,
    LONG nonClientAreaSize,
    ui_transform transform,
    LONG parentWndHeight
)
{
    LONG top{ inTop };
    HelperWTools tools;

    if (transform & ui_transform::restore_ty)
    {
        if (tools.ComparisonWindowSizes(GetAncestor(hwnd, GA_PARENT)))
            top -= nonClientAreaSize;
    }
    if (transform & ui_transform::restore_by)
    {
        top = top + (rc.bottom - parentWndHeight);

        if (tools.ComparisonWindowSizes(GetAncestor(hwnd, GA_PARENT)))
            top += nonClientAreaSize;
    }

    return top;
}

INT UI::WindowTransformation::StretchingX(
    HWND hwnd, 
    const RECT& rc, 
    LONG inRight,
    LONG nonClientAreaSize,
    ui_transform transform, 
    LONG parentWndWidth
)
{
    INT right{};
    HelperWTools tools;

    if (transform & ui_transform::stretching_x)
    {       
        right = inRight + (rc.right - parentWndWidth);
        if (tools.ComparisonWindowSizes(GetAncestor(hwnd, GA_PARENT)))
            right += nonClientAreaSize * 2;
    }

    return right;
}

INT UI::WindowTransformation::StretchingY(
    HWND hwnd, 
    const RECT& rc,
    LONG inBottom,
    LONG nonClientAreaSize,
    ui_transform transform,
    LONG parentWndHeight
)
{
    LONG bottom{};
    HelperWTools tools;

    if (transform & ui_transform::stretching_y)
    {
        bottom = inBottom + (rc.bottom - parentWndHeight);

        if (tools.ComparisonWindowSizes(GetAncestor(hwnd, GA_PARENT)))
            bottom += nonClientAreaSize * 2;
    }

    return bottom;
}

void UI::WindowTransformation::RestorePosition(
    this WindowTransformation& object,
    const HWND& hwnd,
    const UINT& parentWndWidth,
    const UINT& parentWndHeight,
    ui_transform transform,
    LONG nonClientAreaSize,
    LONG inLeft,
    LONG inTop,
    LONG inRight,
    LONG inBottom,
    INT& left,
    INT& top,
    INT& right,
    INT& bottom
)
{
    RECT rc;
    GetClientRect(GetAncestor(hwnd, GA_PARENT), &rc);

    HelperWTools tools{};

    if (transform & ui_transform::restore_lx || transform & ui_transform::restore_rx)
        left = object.RestoreX(hwnd, rc, inLeft, nonClientAreaSize, transform,  parentWndWidth);

    if (transform & ui_transform::restore_ty || transform & ui_transform::restore_by)
        top = object.RestoreY(hwnd, rc, inTop, nonClientAreaSize, transform,  parentWndHeight);

    if (transform & ui_transform::stretching_x)
        right = object.StretchingX(hwnd, rc, inRight, nonClientAreaSize, transform, parentWndWidth);

    if (transform & ui_transform::stretching_y)
        bottom = object.StretchingY(hwnd, rc, inBottom, nonClientAreaSize, transform, parentWndHeight);
}