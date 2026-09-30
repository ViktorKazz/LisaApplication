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

#ifndef DPI_SCALE_CLASS_H
#define DPI_SCALE_CLASS_H

#include <d2d1.h>

// Mouse coordinates are given in physical pixels, 
// but Direct2D expects device-independent pixels (DIPs). 
// To handle high-DPI settings correctly,
// you must translate the pixel coordinates into DIPs. 
// The following code shows a helper class that converts pixels into DIPs.

class DPIScale
{
    
public:
    float Initialize(HWND hwnd)
    {
        float scale{};

        UINT dpi = GetDpiForWindow(hwnd);
        
        return scale = dpi / 96.0f;
    }

    template <typename T>
    D2D1_POINT_2F PixelsToDips(float setScale, T x, T y)
    {
        return D2D1::Point2F(static_cast<float>(x) / setScale, static_cast<float>(y) / setScale);
    }

    template <typename T>
    float PixelsToDipsX(float setScale, T x)
    {
        return static_cast<float>(x) / setScale;
    }

    template <typename T>
    float PixelsToDipsY(float setScale, T y)
    {
        return static_cast<float>(y) / setScale;
    }

};

#endif //!DPI_SCALE_CLASS_H