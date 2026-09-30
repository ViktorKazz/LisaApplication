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

//#include "ApplicationContent.h"
//
//std::vector<UI::Layer> LisaApp::CreateCameraLayers(INT windowWidth)
//{
//    using namespace UI;
//    using namespace UI::STYLE;
//    using namespace UI::TRANSFORM;
//    using namespace UI::DRAW;
//
//    std::vector<UI::Layer> vLayer{};
//
//    Layer bottom = Layer::Create(
//        L"CreateCameraBottom", RECT{ 0, 398, 600 - (gNonClientAreaSize / 3) * 2, 398 + gButtonHeight }, SPLIT_X, NULL,
//        {
//            { { L"Apply and Close", {} }, BUTTON::FRAME, { .Command = CMD::RNCOMMAND | CMD::CLOSE | CMD::UP } },
//            { { L"Apply", {} }, BUTTON::FRAME, { .Command = CMD::RNCOMMAND | CMD::UP } },
//            { { L"Close", {} }, BUTTON::FRAME, { .Command = CMD::CLOSE | CMD::UP } }
//        }
//    );
//    vLayer.push_back(bottom);
//
//    // Camera properties
//    UI::Text txt0{ .input = L"Center of interest:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//    // Lens properties
//    UI::Text txt1{ .input = L"Focal length:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//    UI::Text txt2{ .input = L"Lens squeeze ratio:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//    UI::Text txt3{ .input = L"Camera scale:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//    // Film back properties
//    UI::Text txt4{ .input = L"Horizontal film aperture:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//    UI::Text txt5{ .input = L"Vertical film aperture:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//    UI::Text txt6{ .input = L"Horizontal film offset:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//    UI::Text txt7{ .input = L"Vertical film offset:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//    UI::Text txt8{ .input = L"Film fit offset:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//    UI::Text txt9{ .input = L"Overscan:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//    // Motion blur properties
//    UI::Text txt10{ .input = L"Shutter angle:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//    // Clipping planes
//    UI::Text txt11{ .input = L"Near clip plane:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//    UI::Text txt12{ .input = L"Far clip plane:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//    // Orthographic views
//    UI::Text txt13{ .input = L"Orthographic width:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//    // 2D pan/zoom
//    UI::Text txt14{ .input = L"Horizontal pan:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//    UI::Text txt15{ .input = L"Vertical pan:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//    UI::Text txt16{ .input = L"Zoom:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//
//    INT l{ (gNonClientAreaSize / 3) * 4 };
//    INT r{ windowWidth - (gNonClientAreaSize / 3) * 2 }; 
//
//    INT nElements{ 17 };
//
//    Layer text = Layer::Create(
//        L"CreateCameraText", RECT{ 0, 20, (r - l) / 2, 20 + gHeightField * nElements }, SPLIT_Y, NULL,
//        {
//            { { L"CreateCameraText0", {} }, TXT::SIMPLE, {.ImgTxt = txt0} },
//            { { L"CreateCameraText1", {} }, TXT::SIMPLE, {.ImgTxt = txt1} },
//            { { L"CreateCameraText2", {} }, TXT::SIMPLE, {.ImgTxt = txt2} },
//            { { L"CreateCameraText3", {} }, TXT::SIMPLE, {.ImgTxt = txt3} },
//            { { L"CreateCameraText4", {} }, TXT::SIMPLE, {.ImgTxt = txt4} },
//            { { L"CreateCameraText5", {} }, TXT::SIMPLE, {.ImgTxt = txt5} },
//            { { L"CreateCameraText6", {} }, TXT::SIMPLE, {.ImgTxt = txt6} },
//            { { L"CreateCameraText7", {} }, TXT::SIMPLE, {.ImgTxt = txt7} },
//            { { L"CreateCameraText8", {} }, TXT::SIMPLE, {.ImgTxt = txt8} },
//            { { L"CreateCameraText9", {} }, TXT::SIMPLE, {.ImgTxt = txt9} },
//            { { L"CreateCameraText10", {} }, TXT::SIMPLE, {.ImgTxt = txt10} },
//            { { L"CreateCameraText11", {} }, TXT::SIMPLE, {.ImgTxt = txt11} },
//            { { L"CreateCameraText12", {} }, TXT::SIMPLE, {.ImgTxt = txt12} },
//            { { L"CreateCameraText13", {} }, TXT::SIMPLE, {.ImgTxt = txt13} },
//            { { L"CreateCameraText14", {} }, TXT::SIMPLE, {.ImgTxt = txt14} },
//            { { L"CreateCameraText15", {} }, TXT::SIMPLE, {.ImgTxt = txt15} },
//            { { L"CreateCameraText16", {} }, TXT::SIMPLE, {.ImgTxt = txt16} }
//        }
//    );
//    vLayer.push_back(text);
//
//    Layer field = Layer::Create(
//        L"CreateCameraField", RECT{ (r - l) / 2, 21, ((r - l) / 2) + 100, 21 + gHeightField * nElements }, SPLIT_Y, NULL,
//        {
//            { { L"CreateCameraField0", {} }, FIELD::SIMPLE, {} },
//            { { L"CreateCameraField1", {} }, FIELD::SIMPLE, {} },
//            { { L"CreateCameraField2", {} }, FIELD::SIMPLE, {} },
//            { { L"CreateCameraField3", {} }, FIELD::SIMPLE, {} },
//            { { L"CreateCameraField4", {} }, FIELD::SIMPLE, {} },
//            { { L"CreateCameraField5", {} }, FIELD::SIMPLE, {} },
//            { { L"CreateCameraField6", {} }, FIELD::SIMPLE, {} },
//            { { L"CreateCameraField7", {} }, FIELD::SIMPLE, {} },
//            { { L"CreateCameraField8", {} }, FIELD::SIMPLE, {} },
//            { { L"CreateCameraField9", {} }, FIELD::SIMPLE, {} },
//            { { L"CreateCameraField10", {} }, FIELD::SIMPLE, {} },
//            { { L"CreateCameraField11", {} }, FIELD::SIMPLE, {} },
//            { { L"CreateCameraField12", {} }, FIELD::SIMPLE, {} },
//            { { L"CreateCameraField13", {} }, FIELD::SIMPLE, {} },
//            { { L"CreateCameraField14", {} }, FIELD::SIMPLE, {} },
//            { { L"CreateCameraField15", {} }, FIELD::SIMPLE, {} },
//            { { L"CreateCameraField16", {} }, FIELD::SIMPLE, {} }
//        }
//    );
//    vLayer.push_back(field);
//
//    return vLayer;
//}