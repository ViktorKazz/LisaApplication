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

#ifndef APP_COLORS_H
#define APP_COLORS_H

#include <DirectXMath.h>

namespace AppColors
{
    namespace Color
    {
        XMGLOBALCONST DirectX::XMVECTORF32 CarouselPink = { { { 1.000000000f, 0.890196078f, 0.933333333f, 1.f } } }; // #FEE3EE
        XMGLOBALCONST DirectX::XMVECTORF32 Carissma = { { { 0.862745098f, 0.501960784f, 0.584313725f, 1.f } } };     // #DC8095
        XMGLOBALCONST DirectX::XMVECTORF32 DarkBlue = { { { 0.192156862f, 0.356862745f, 0.588235294f, 1.f } } };     // #315B96
        XMGLOBALCONST DirectX::XMVECTORF32 CatalinaBlue = { { { 0.137254901f, 0.207843137f, 0.403921568f, 1.f } } }; // #233567
        XMGLOBALCONST DirectX::XMVECTORF32 Blue = { { { 0.258823529f, 0.392156862f, 0.760784313f, 1.f } } };         // #4264C2
        XMGLOBALCONST DirectX::XMVECTORF32 VeryDarkGray = { { { 0.243137254f, 0.243137254f, 0.243137254f, 1.f } } }; // #3E3E3E
  
        XMGLOBALCONST DirectX::XMVECTORF32 OffWhite = { { { 0.635294139f, 0.635294139f, 0.635294139f, 1.f } } };     // #A2A2A2
        XMGLOBALCONST DirectX::XMVECTORF32 White = { { { 1.000000000f, 1.000000000f, 1.000000000f, 1.f } } };        // #FFFFFF
        XMGLOBALCONST DirectX::XMVECTORF32 CornflowerBlue = { { { 0.192156862f, 0.584313725f, 0.929411764f, 1.f } } };

        XMGLOBALCONST DirectX::XMVECTORF32 PivotPoint = { { { 0.750000000f, 0.750000000f, 0.500000000f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 PivotFrame = { { { 0.625000000f, 0.625000000f, 0.500000000f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 PivotXAxis = { { { 1.00000000f, 0.349999994f, 0.349999994f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 PivotYAxis = { { { 0.349999994f, 1.00000000f, 0.349999994f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 PivotZAxis = { { { 0.349999994f, 0.349999994f, 1.00000000f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 PivotOuterCircle = { { { 1.00000000f, 1.00000000f, 0.500000000f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 PivotInnerCircle = { { { 0.500000000f, 0.500000000f, 0.500000000f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 PivotSphere = { { { 0.000000f, 0.000000f, 0.0000000f, 0.0f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 PivotAuxiliaryLine = { { { 1.000000000f, 1.000000000f, 1.000000000f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 PivotAuxiliaryTriangle = { { { 0.2000000f, 0.0000000f, 0.0000000f, 0.5f } } };
    };

    namespace Linear
    {
        XMGLOBALCONST DirectX::XMVECTORF32 Background = { { { 0.052860655f, 0.052860655f, 0.052860655f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 Green = { { { 0.005181516f, 0.201556236f, 0.005181516f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 Blue = { { { 0.001517635f, 0.114435382f, 0.610495627f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 Orange = { { { 0.545724571f, 0.026241219f, 0.001517635f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 DarkGrey = { { { 0.033104762f, 0.033104762f, 0.033104762f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 LightGrey = { { { 0.194617808f, 0.194617808f, 0.194617808f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 OffWhite = { { { 0.361306787f, 0.361306787f, 0.361306787f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 White = { { { 0.955973506f, 0.955973506f, 0.955973506f, 1.f } } };
    };

    namespace HDR
    {
        XMGLOBALCONST DirectX::XMVECTORF32 Background = { { { 0.052860655f * 2.f, 0.052860655f * 2.f, 0.052860655f * 2.f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 Green = { { { 0.005181516f * 2.f, 0.201556236f * 2.f, 0.005181516f * 2.f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 Blue = { { { 0.001517635f * 2.f, 0.114435382f * 2.f, 0.610495627f * 2.f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 Orange = { { { 0.545724571f * 2.f, 0.026241219f * 2.f, 0.001517635f * 2.f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 DarkGrey = { { { 0.033104762f * 2.f, 0.033104762f * 2.f, 0.033104762f * 2.f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 LightGrey = { { { 0.394617808f * 2.f, 0.394617808f * 2.f, 0.394617808f * 2.f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 OffWhite = { { { 0.361306787f * 2.f, 0.361306787f * 2.f, 0.361306787f * 2.f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 White = { { { 0.955973506f * 2.f, 0.955973506f * 2.f, 0.955973506f * 2.f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 LemonYellow = { { { 1.000000000f * 2.f, 0.952941176f * 2.f, 0.427450980f * 2.f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 Milano = { { { 0.850980392f * 2.f, 0.364705882f * 2.f, 0.403921568f * 2.f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 FrostySky = { { { 0.000000000f * 2.f, 0.749019607f * 2.f, 1.000000000f * 2.f, 1.f } } };

        XMGLOBALCONST DirectX::XMVECTORF32 CornflowerBlue = { { { 0.392156862f * 2.f, 0.584313725f * 2.f, 0.929411764f * 2.f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 CobaltBlue = { { { 0.000000000f * 2.f, 0.278431372f * 2.f, 0.670588235f * 2.f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 AlizarinRed = { { { 0.890196078f * 2.f, 0.149019607f * 2.f, 0.211764705f * 2.f, 1.f } } };
        XMGLOBALCONST DirectX::XMVECTORF32 TurquoiseBlue = { { { 0.147058823f * 2.f, 0.333333333f * 2.f, 0.360784313f * 2.f, 1.f } } };
    };
}

#endif // !APP_COLORS_H


