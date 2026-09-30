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

#include "..//Inc/Primitives2D.h"

Microsoft::WRL::ComPtr<ID2D1PathGeometry> UI::Primitives2D::DrawPathGeometry(
    const Microsoft::WRL::ComPtr<ID2D1Factory7>& pD2DFactory,
    const std::initializer_list<D2D1_POINT_2F>& pt
)
{
    ID2D1GeometrySink* pSink{ NULL };
    HRESULT hr{ S_OK };

    Microsoft::WRL::ComPtr<ID2D1PathGeometry> m_pPathGeometry;

    // Create a path geometry.
    if (SUCCEEDED(hr))
    {
        UI::ThrowIfFailed(pD2DFactory->CreatePathGeometry(m_pPathGeometry.ReleaseAndGetAddressOf()));

        if (SUCCEEDED(hr))
        {
            // Write to the path geometry using the geometry sink.
            UI::ThrowIfFailed(m_pPathGeometry->Open(&pSink));

            if (SUCCEEDED(hr))
            { 
                pSink->BeginFigure(pt.begin()[0], D2D1_FIGURE_BEGIN_FILLED);

                for (const auto& i : pt)
                {
                    pSink->AddLine(i);
                }

                pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

                UI::ThrowIfFailed(pSink->Close());
            }
            SafeRelease(&pSink);
        }
    }
    return m_pPathGeometry;
}