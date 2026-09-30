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

#include "MessageOnlyWindow.h"
#include "Elements.h"
#include "DefaultElementsData.h"
#include "Globals.h"

extern struct LisaApp::Default::SphereData gSphereData;
extern struct LisaApp::Default::GeoSphereData gGeoSphereData;
extern struct LisaApp::Default::CubeData gCubeData;
extern struct LisaApp::Default::CylinderData gCylinderData;
extern struct LisaApp::Default::ConeData gConeData;
extern struct LisaApp::Default::TorusData gTorusData;
extern struct LisaApp::Default::PlaneData gPlaneData;

// Creating a message-only window.
std::unique_ptr<LisaApp::MessageOnlyWindow> LisaApp::MessageOnlyWindow::MessageWindow(HINSTANCE hInstance,
    const std::variant<std::wstring, std::pair<std::wstring, std::wstring>>& varName, UI::ui_type type
)
{
    HelperWTools tools;

    std::pair<std::wstring, std::wstring> wstr{};

    if (std::holds_alternative<std::wstring>(varName))
        wstr.first = std::get<std::wstring>(varName);

    else if (std::holds_alternative<std::pair<std::wstring, std::wstring>>(varName))
        wstr = std::get<std::pair<std::wstring, std::wstring>>(varName);

    std::wstring wClass{ tools.CreateClass(type, wstr.first) };


    // Using WS_CLIPCHILDREN.
    // Excludes the area occupied by child windows when drawing occurs within the parent window. 
    // This style is used when creating the parent window.
    std::unique_ptr<LisaApp::MessageOnlyWindow> element(new LisaApp::MessageOnlyWindow(0, 0, 0, 0, hInstance, wClass.c_str(), wstr.second.c_str(),
        CS_HREDRAW | CS_VREDRAW, WS_EX_NOREDIRECTIONBITMAP | WS_EX_APPWINDOW, WS_POPUP | WS_CLIPCHILDREN));


    element->Initialize(*element, element->MessageOnlyWindowProc, HWND_MESSAGE);

    return element;
}

// The window message handler.
LRESULT CALLBACK LisaApp::MessageOnlyWindow::MessageOnlyWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result{};
    static LPWSTR pszMem{};

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        MessageOnlyWindow* pWindow = reinterpret_cast<LisaApp::MessageOnlyWindow*>(pcs->lpCreateParams);
        // The "SetWindowLongPtrW" function allows you to create several windows
        // using one "WindowProc" function for all.
        //
        ::SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));

        UI::ThrowIfFailed(RoInitialize(RO_INIT_MULTITHREADED));

        result = 1;
    }
    else
    {
        MessageOnlyWindow* pWindow = reinterpret_cast<MessageOnlyWindow*>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA));

        bool wasHandled = false;

        if (pWindow)
        {
            switch (message)
            {
            case WM_COMMAND:
            {
                UINT wmId = LOWORD(wParam);

                switch (wmId)
                {
                case UI::GET_DATA_FROM_INPUT_FIELD:
                {
                    UI::GetFieldData* data = reinterpret_cast<UI::GetFieldData*>(lParam);

                    // Filling pre-prepared global structures with data from input fields.


                    using namespace LisaApp::Default;
                    using namespace LisaApp::Global;
                    
                    // Sphere

                    gSphereData.Radius = pWindow->GetFieldData<float>(
                        gSphereData.Radius, 
                        *data, 
                        PRIMITIVES::SPHERE, 
                        PRIMITIVES_MEMBERS::RADIUS
                    );
                    gSphereData.SubdivisionsAxis = pWindow->GetFieldData<UINT>(
                        gSphereData.SubdivisionsAxis, 
                        *data, 
                        PRIMITIVES::SPHERE, 
                        PRIMITIVES_MEMBERS::SUBDIVISIONS_AXIS
                    );
                    gSphereData.SubdivisionsHeight = pWindow->GetFieldData<UINT>(
                        gSphereData.SubdivisionsHeight, 
                        *data, 
                        PRIMITIVES::SPHERE, 
                        PRIMITIVES_MEMBERS::SUBDIVISIONS_HEIGHT
                    );

                    // GeoSphere

                    gGeoSphereData.Radius = pWindow->GetFieldData<float>(
                        gGeoSphereData.Radius, 
                        *data, 
                        PRIMITIVES::GEO_SPHERE, 
                        PRIMITIVES_MEMBERS::RADIUS
                    );
                    gGeoSphereData.Subdivisions = pWindow->GetFieldData<UINT>(
                        gGeoSphereData.Subdivisions, 
                        *data, 
                        PRIMITIVES::GEO_SPHERE, 
                        PRIMITIVES_MEMBERS::SUBDIVISIONS
                    );

                    // Cube

                    gCubeData.Width = pWindow->GetFieldData<float>(
                        gCubeData.Width, 
                        *data, 
                        PRIMITIVES::CUBE, 
                        PRIMITIVES_MEMBERS::WIDTH
                    );
                    gCubeData.Height = pWindow->GetFieldData<float>(
                        gCubeData.Height, 
                        *data, 
                        PRIMITIVES::CUBE, 
                        PRIMITIVES_MEMBERS::HEIGHT
                    );
                    gCubeData.Depth = pWindow->GetFieldData<float>(
                        gCubeData.Depth, 
                        *data, 
                        PRIMITIVES::CUBE, 
                        PRIMITIVES_MEMBERS::DEPTH
                    );
                    gCubeData.SubdivisionsWidth = pWindow->GetFieldData<UINT>(
                        gCubeData.SubdivisionsWidth, 
                        *data, 
                        PRIMITIVES::CUBE, 
                        PRIMITIVES_MEMBERS::SUBDIVISIONS_WIDTH
                    );
                    gCubeData.SubdivisionsHeight = pWindow->GetFieldData<UINT>(
                        gCubeData.SubdivisionsHeight, 
                        *data, 
                        PRIMITIVES::CUBE, 
                        PRIMITIVES_MEMBERS::SUBDIVISIONS_HEIGHT
                    );
                    gCubeData.SubdivisionsDepth = pWindow->GetFieldData<UINT>(
                        gCubeData.SubdivisionsDepth, 
                        *data, 
                        PRIMITIVES::CUBE, 
                        PRIMITIVES_MEMBERS::SUBDIVISIONS_DEPTH
                    );

                    // Cylinder

                    gCylinderData.Radius = pWindow->GetFieldData<float>(
                        gCylinderData.Radius, 
                        *data, 
                        PRIMITIVES::CYLINDER, 
                        PRIMITIVES_MEMBERS::RADIUS
                    );
                    gCylinderData.Height = pWindow->GetFieldData<float>(
                        gCylinderData.Height, 
                        *data, 
                        PRIMITIVES::CYLINDER, 
                        PRIMITIVES_MEMBERS::HEIGHT
                    );
                    gCylinderData.SubdivisionsAxis = pWindow->GetFieldData<UINT>(
                        gCylinderData.SubdivisionsAxis, 
                        *data, 
                        PRIMITIVES::CYLINDER, 
                        PRIMITIVES_MEMBERS::SUBDIVISIONS_AXIS
                    );
                    gCylinderData.SubdivisionsHeight = pWindow->GetFieldData<UINT>(
                        gCylinderData.SubdivisionsHeight, 
                        *data, 
                        PRIMITIVES::CYLINDER, 
                        PRIMITIVES_MEMBERS::SUBDIVISIONS_HEIGHT
                    );
                    gCylinderData.SubdivisionsCaps = pWindow->GetFieldData<UINT>(
                        gCylinderData.SubdivisionsCaps, 
                        *data, 
                        PRIMITIVES::CYLINDER, 
                        PRIMITIVES_MEMBERS::SUBDIVISIONS_CAPS
                    );

                    // Cone

                    gConeData.Radius = pWindow->GetFieldData<float>(
                        gConeData.Radius, 
                        *data, 
                        PRIMITIVES::CONE, 
                        PRIMITIVES_MEMBERS::RADIUS
                    );
                    gConeData.Height = pWindow->GetFieldData<float>(
                        gConeData.Height, 
                        *data, 
                        PRIMITIVES::CONE, 
                        PRIMITIVES_MEMBERS::HEIGHT
                    );
                    gConeData.SubdivisionsAxis = pWindow->GetFieldData<UINT>(
                        gConeData.SubdivisionsAxis, 
                        *data, 
                        PRIMITIVES::CONE, 
                        PRIMITIVES_MEMBERS::SUBDIVISIONS_AXIS
                    );
                    gConeData.SubdivisionsHeight = pWindow->GetFieldData<UINT>(
                        gConeData.SubdivisionsHeight, 
                        *data, 
                        PRIMITIVES::CONE, 
                        PRIMITIVES_MEMBERS::SUBDIVISIONS_HEIGHT
                    );
                    gConeData.SubdivisionsCaps = pWindow->GetFieldData<UINT>(
                        gConeData.SubdivisionsCaps, 
                        *data, 
                        PRIMITIVES::CONE, 
                        PRIMITIVES_MEMBERS::SUBDIVISIONS_CAPS
                    );

                    // Torus

                    gTorusData.Radius = pWindow->GetFieldData<float>(
                        gTorusData.Radius, 
                        *data, 
                        PRIMITIVES::TORUS, 
                        PRIMITIVES_MEMBERS::RADIUS
                    );
                    gTorusData.SectionRadius = pWindow->GetFieldData<float>(
                        gTorusData.SectionRadius, 
                        *data, 
                        PRIMITIVES::TORUS, 
                        PRIMITIVES_MEMBERS::SECTION_RADIUS
                    );
                    gTorusData.SubdivisionsAxis = pWindow->GetFieldData<UINT>(
                        gTorusData.SubdivisionsAxis, 
                        *data, 
                        PRIMITIVES::TORUS, 
                        PRIMITIVES_MEMBERS::SUBDIVISIONS_AXIS
                    );
                    gTorusData.SubdivisionsHeight = pWindow->GetFieldData<UINT>(
                        gTorusData.SubdivisionsHeight, 
                        *data, 
                        PRIMITIVES::TORUS, 
                        PRIMITIVES_MEMBERS::SUBDIVISIONS_HEIGHT
                    );

                    // Plane

                    gPlaneData.Width = pWindow->GetFieldData<float>(
                        gPlaneData.Width, 
                        *data, 
                        PRIMITIVES::PLANE, 
                        PRIMITIVES_MEMBERS::WIDTH
                    );
                    gPlaneData.Depth = pWindow->GetFieldData<float>(
                        gPlaneData.Depth,
                        *data, 
                        PRIMITIVES::PLANE, 
                        PRIMITIVES_MEMBERS::DEPTH
                    );
                    gPlaneData.SubdivisionsWidth = pWindow->GetFieldData<UINT>(
                        gPlaneData.SubdivisionsWidth, 
                        *data, 
                        PRIMITIVES::PLANE, 
                        PRIMITIVES_MEMBERS::SUBDIVISIONS_WIDTH
                    );
                    gPlaneData.SubdivisionsDepth = pWindow->GetFieldData<UINT>(
                        gPlaneData.SubdivisionsDepth, 
                        *data, 
                        PRIMITIVES::PLANE, 
                        PRIMITIVES_MEMBERS::SUBDIVISIONS_DEPTH
                    );


                }
                break;               
                default:
                {
                    return DefWindowProc(hwnd, message, wParam, lParam);
                }
                break;
                }
            }
            break;
            case WM_DESTROY:
            {
                RoUninitialize();
                DestroyWindow(hwnd);
                PostQuitMessage(0);
                return 0;
            }
            break;
            }
        }

        if (!wasHandled)
        {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }
    return result;
}
