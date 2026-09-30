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
//#include "HelperUtilities.h"
//
//std::vector<UI::Layer> LisaApp::AboutLisaLayers(INT windowWidth)
//{
//    using namespace UI;
//    using namespace UI::STYLE;
//    using namespace UI::TRANSFORM;
//    using namespace UI::DRAW;
//
//    std::vector<UI::Layer> vLayer{};
//
//    INT l{ (gNonClientAreaSize / 3) * 4 };
//    INT r{ windowWidth - (gNonClientAreaSize / 3) * 2 };
//
//    Layer bottom = Layer::Create(
//        L"AboutLisaBottom", RECT{ ((r - l) / 2) - 44, 398, ((r - l) / 2) + 60, 398 + gButtonHeight }, NULL, NULL,
//        {
//            { { L"Ok", {} }, BUTTON::FRAME, { .Command = CMD::CLOSE | CMD::UP } }
//        }
//    );
//    vLayer.push_back(bottom);
//
//    std::vector<std::wstring> img
//    { 
//        LisaApp::HelperPath(L"Resources\\AboutImage\\about_lisa_image_0.png"),
//        LisaApp::HelperPath(L"Resources\\AboutImage\\about_lisa_image_1.png")
//    };
//
//    Layer image = Layer::Create(
//        L"AboutLisaImage", RECT{ 36, 0, 512 + 36, 256 }, NULL, NULL,
//        {
//            { { L"img0", {} }, IMG::SIMPLE, {.ImgTxt = img} }
//        }
//    );
//    vLayer.push_back(image);
//
//    UI::Text txt0
//    {
//        .input = L"A program for simulation physical properties of objects and animation.",
//        .textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER
//    };
//
//    UI::Text txt1
//    {
//        .input = L"Version 0.1 alpha",
//        .textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER
//    };
//
//    UI::Text txt2
//    {
//        .input = L"Author: Deputatov Victor.",
//        .textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER
//    };
//
//    Layer text = Layer::Create(
//        L"AboutLisaText", RECT{ 0, 256, 590, 328 }, SPLIT_Y, NULL,
//        {
//            { { L"AboutLisaText0", {} }, TXT::SIMPLE, {.ImgTxt = txt0} },
//            { { L"AboutLisaText1", {} }, TXT::SIMPLE, {.ImgTxt = txt1} },
//            { { L"AboutLisaText0", {} }, TXT::SIMPLE, {.ImgTxt = txt2} }
//        }
//    );
//    vLayer.push_back(text);
//
//    return vLayer;
//}