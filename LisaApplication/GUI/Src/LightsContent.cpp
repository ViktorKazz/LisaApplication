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
//std::vector<UI::Layer> LisaApp::CreateAmbientLightLayers(INT windowWidth)
//{
//    using namespace UI;
//    using namespace UI::STYLE;
//    using namespace UI::TRANSFORM;
//    using namespace UI::DRAW;
//
//    std::vector<UI::Layer> vLayer{};
//
//    Layer bottom = Layer::Create(
//        L"CreateAmbientLightBottom", RECT{ 0, 398, 600 - (gNonClientAreaSize / 3) * 2, 398 + gButtonHeight }, SPLIT_X, NULL,
//        {
//            { { L"Apply and Close", {} }, BUTTON::FRAME, { .Command = CMD::RNCOMMAND | CMD::CLOSE | CMD::UP } },
//            { { L"Apply", {} }, BUTTON::FRAME, { .Command = CMD::RNCOMMAND | CMD::UP } },
//            { { L"Close", {} }, BUTTON::FRAME, { .Command = CMD::CLOSE | CMD::UP } }
//        }
//    );
//    vLayer.push_back(bottom);
//
//    UI::Text txt0{ .input = L"Intensity:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//    UI::Text txt1{ .input = L"Ambient shade:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//
//    INT l{ (gNonClientAreaSize / 3) * 4 };
//    INT r{ windowWidth - (gNonClientAreaSize / 3) * 2 };
//
//    INT nElements{ 2 };
//
//    Layer text = Layer::Create(
//        L"CreateAmbientLightText", RECT{ 0, 20, (r - l) / 2, 20 + gHeightField * nElements }, SPLIT_Y, NULL,
//        {
//            { { L"CreateAmbientLightText0", {} }, TXT::SIMPLE, {.ImgTxt = txt0} },
//            { { L"CreateAmbientLightText1", {} }, TXT::SIMPLE, {.ImgTxt = txt1} }
//        }
//    );
//    vLayer.push_back(text);
//
//    Layer field = Layer::Create(
//        L"CreateAmbientLightField", RECT{ (r - l) / 2, 21, ((r - l) / 2) + 100, 21 + gHeightField * nElements }, SPLIT_Y, NULL,
//        {
//            { { L"CreateAmbientLightField0", {} }, FIELD::SIMPLE, {} },
//            { { L"CreateAmbientLightField1", {} }, FIELD::SIMPLE, {} }
//        }
//    );
//    vLayer.push_back(field);
//
//    return vLayer;
//}
//
//std::vector<UI::Layer> LisaApp::CreateDirectionalLightLayers(INT windowWidth)
//{
//    using namespace UI;
//    using namespace UI::STYLE;
//    using namespace UI::TRANSFORM;
//    using namespace UI::DRAW;
//
//    std::vector<UI::Layer> vLayer{};
//
//    Layer bottom = Layer::Create(
//        L"CreateDirectionalLightBottom", RECT{ 0, 398, 600 - (gNonClientAreaSize / 3) * 2, 398 + gButtonHeight }, SPLIT_X, NULL,
//        {
//            { { L"Apply and Close", {} }, BUTTON::FRAME, { .Command = CMD::RNCOMMAND | CMD::CLOSE | CMD::UP } },
//            { { L"Apply", {} }, BUTTON::FRAME, { .Command = CMD::RNCOMMAND | CMD::UP } },
//            { { L"Close", {} }, BUTTON::FRAME, { .Command = CMD::CLOSE | CMD::UP } }
//        }
//    );
//    vLayer.push_back(bottom);
//
//    UI::Text txt0{ .input = L"Intensity:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//
//    INT l{ (gNonClientAreaSize / 3) * 4 };
//    INT r{ windowWidth - (gNonClientAreaSize / 3) * 2 };
//
//    INT nElements{ 1 };
//
//    Layer text = Layer::Create(
//        L"CreateDirectionalLightText", RECT{ 0, 20, (r - l) / 2, 20 + gHeightField * nElements }, SPLIT_Y, NULL,
//        {
//            { { L"CreateDirectionalLightText0", {} }, TXT::SIMPLE, {.ImgTxt = txt0} }
//        }
//    );
//    vLayer.push_back(text);
//
//    Layer field = Layer::Create(
//        L"CreateDirectionalLightField", RECT{ (r - l) / 2, 21, ((r - l) / 2) + 100, 21 + gHeightField * nElements }, SPLIT_Y, NULL,
//        {
//            { { L"CreateDirectionalLightField0", {} }, FIELD::SIMPLE, {} }
//        }
//    );
//    vLayer.push_back(field);
//
//    return vLayer;
//}
//
//std::vector<UI::Layer> LisaApp::CreatePointLightLayers(INT windowWidth)
//{
//    using namespace UI;
//    using namespace UI::STYLE;
//    using namespace UI::TRANSFORM;
//    using namespace UI::DRAW;
//
//    std::vector<UI::Layer> vLayer{};
//
//    Layer bottom = Layer::Create(
//        L"CreateSphereBottom", RECT{ 0, 398, 600 - (gNonClientAreaSize / 3) * 2, 398 + gButtonHeight }, SPLIT_X, NULL,
//        {
//            { { L"Apply and Close", {} }, BUTTON::FRAME, { .Command = CMD::RNCOMMAND | CMD::CLOSE | CMD::UP } },
//            { { L"Apply", {} }, BUTTON::FRAME, { .Command = CMD::RNCOMMAND | CMD::UP } },
//            { { L"Close", {} }, BUTTON::FRAME, { .Command = CMD::CLOSE | CMD::UP } }
//        }
//    );
//    vLayer.push_back(bottom);
//
//    UI::Text txt0{ .input = L"Intensity:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//
//    INT l{ (gNonClientAreaSize / 3) * 4 };
//    INT r{ windowWidth - (gNonClientAreaSize / 3) * 2 };
//
//    INT nElements{ 1 };
//
//    Layer text = Layer::Create(
//        L"CreatePointLightText", RECT{ 0, 20, (r - l) / 2, 20 + gHeightField * nElements }, SPLIT_Y, NULL,
//        {
//            { { L"CreatePointLightText0", {} }, TXT::SIMPLE, {.ImgTxt = txt0} }
//        }
//    );
//    vLayer.push_back(text);
//
//    Layer field = Layer::Create(
//        L"CreatePointLightField", RECT{ (r - l) / 2, 21, ((r - l) / 2) + 100, 21 + gHeightField * nElements }, SPLIT_Y, NULL,
//        {
//            { { L"CreatePointLightField0", {} }, FIELD::SIMPLE, {} }
//        }
//    );
//    vLayer.push_back(field);
//
//    return vLayer;
//}
//
//std::vector<UI::Layer> LisaApp::CreateSpotLightLayers(INT windowWidth)
//{
//    using namespace UI;
//    using namespace UI::STYLE;
//    using namespace UI::TRANSFORM;
//    using namespace UI::DRAW;
//
//    std::vector<UI::Layer> vLayer{};
//
//    Layer bottom = Layer::Create(
//        L"CreateSpotLightBottom", RECT{ 0, 398, 600 - (gNonClientAreaSize / 3) * 2, 398 + gButtonHeight }, SPLIT_X, NULL,
//        {
//            { { L"Apply and Close", {} }, BUTTON::FRAME, { .Command = CMD::RNCOMMAND | CMD::CLOSE | CMD::UP } },
//            { { L"Apply", {} }, BUTTON::FRAME, { .Command = CMD::RNCOMMAND | CMD::UP } },
//            { { L"Close", {} }, BUTTON::FRAME, { .Command = CMD::CLOSE | CMD::UP } }
//        }
//    );
//    vLayer.push_back(bottom);
//
//    UI::Text txt0{ .input = L"Intensity:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//    UI::Text txt1{ .input = L"Cone angle:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//    UI::Text txt2{ .input = L"Penumbra angle:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//    UI::Text txt3{ .input = L"Dropoff:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//
//    INT l{ (gNonClientAreaSize / 3) * 4 };
//    INT r{ windowWidth - (gNonClientAreaSize / 3) * 2 };
//
//    INT nElements{ 4 };
//
//    Layer text = Layer::Create(
//        L"CreateSpotLightText", RECT{ 0, 20, (r - l) / 2, 20 + gHeightField * nElements }, SPLIT_Y, NULL,
//        {
//            { { L"CreateSpotLightText0", {} }, TXT::SIMPLE, {.ImgTxt = txt0} },
//            { { L"CreateSpotLightText1", {} }, TXT::SIMPLE, {.ImgTxt = txt1} },
//            { { L"CreateSpotLightText2", {} }, TXT::SIMPLE, {.ImgTxt = txt2} },
//            { { L"CreateSpotLightText3", {} }, TXT::SIMPLE, {.ImgTxt = txt3} }
//        }
//    );
//    vLayer.push_back(text);
//
//    Layer field = Layer::Create(
//        L"CreateSpotLightField", RECT{ (r - l) / 2, 21, ((r - l) / 2) + 100, 21 + gHeightField * nElements }, SPLIT_Y, NULL,
//        {
//            { { L"CreateSpotLightField0", {} }, FIELD::SIMPLE, {} },
//            { { L"CreateSpotLightField1", {} }, FIELD::SIMPLE, {} },
//            { { L"CreateSpotLightField2", {} }, FIELD::SIMPLE, {} },
//            { { L"CreateSpotLightField3", {} }, FIELD::SIMPLE, {} }
//        }
//    );
//    vLayer.push_back(field);
//
//    return vLayer;
//}
//
//std::vector<UI::Layer> LisaApp::CreateAreaLightLayers(INT windowWidth)
//{
//    using namespace UI;
//    using namespace UI::STYLE;
//    using namespace UI::TRANSFORM;
//    using namespace UI::DRAW;
//
//    std::vector<UI::Layer> vLayer{};
//
//    Layer bottom = Layer::Create(
//        L"CreateAreaLightBottom", RECT{ 0, 398, 600 - (gNonClientAreaSize / 3) * 2, 398 + gButtonHeight }, SPLIT_X, NULL,
//        {
//            { { L"Apply and Close", {} }, BUTTON::FRAME, { .Command = CMD::RNCOMMAND | CMD::CLOSE | CMD::UP } },
//            { { L"Apply", {} }, BUTTON::FRAME, { .Command = CMD::RNCOMMAND | CMD::UP } },
//            { { L"Close", {} }, BUTTON::FRAME, { .Command = CMD::CLOSE | CMD::UP } }
//        }
//    );
//    vLayer.push_back(bottom);
//
//    UI::Text txt0{ .input = L"Intensity:",.textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING };
//
//    INT l{ (gNonClientAreaSize / 3) * 4 };
//    INT r{ windowWidth - (gNonClientAreaSize / 3) * 2 };
//
//    INT nElements{ 1 };
//
//    Layer text = Layer::Create(
//        L"CreateAreaLightText", RECT{ 0, 20, (r - l) / 2, 20 + gHeightField * nElements }, SPLIT_Y, NULL,
//        {
//            { { L"CreateAreaLightText0", {} }, TXT::SIMPLE, {.ImgTxt = txt0} }
//        }
//    );
//    vLayer.push_back(text);SetDefaultValueToField
//
//    Layer field = Layer::Create(
//        L"CreateAreaLightField", RECT{ (r - l) / 2, 21, ((r - l) / 2) + 100, 21 + gHeightField * nElements }, SPLIT_Y, NULL,
//        {
//            { { L"CreateAreaLightField0", {} }, FIELD::SIMPLE, {} }
//        }
//    );
//    vLayer.push_back(field);
//
//    return vLayer;
//}