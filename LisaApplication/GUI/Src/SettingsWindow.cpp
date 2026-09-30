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

#include "SettingsWindow.h"
#include <HelperUtilities.h>
#include "DefaultElementsData.h"

//#include "..//Inc/HelperWindowTools.h"

extern struct LisaApp::Default::SphereData gSphereData;
extern struct LisaApp::Default::GeoSphereData gGeoSphereData;
extern struct LisaApp::Default::CubeData gCubeData;
extern struct LisaApp::Default::CylinderData gCylinderData;
extern struct LisaApp::Default::ConeData gConeData;
extern struct LisaApp::Default::TorusData gTorusData;
extern struct LisaApp::Default::PlaneData gPlaneData;

void LisaApp::SettingsWindow::CreateLabelText(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice,
    const std::shared_ptr<UI::Caching>& pCaching,
    const std::vector<UI::FontConfig>& fontConfig,
    HWND parent,
    HINSTANCE hInstance
) const
{
    std::wstring className{ m_windowClass + L"_Text" };
    LONG right{ (m_windowWidth / 2) - m_lenghtTextField };

    for (auto&& [indx, config] : fontConfig | std::views::enumerate)
    {
        const UI::ElementConfig<UI::TextConfig> configure{
            .hInstance = hInstance,
            .ClassName = className + std::to_wstring(indx),
            .Rect = {right, m_topIndent + UI::gHeightField * static_cast<LONG>(indx), m_lenghtTextField, UI::gHeightField},
            .Parent = parent,
            .Flags = {.Type = UI::ui_type::inbuilt, .Modes = UI::ui_modes::customposition, .Draw = UI::ui_draw::text::label },
            .Extra = {.ImgTxt = config}
        };
        UI::Element<UI::Text>::Create(pDevice, pCaching, configure);
    }
}

void LisaApp::SettingsWindow::CreateSimpleField(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice,
    const std::shared_ptr<UI::Caching>& pCaching,
    const std::vector<UI::FieldConfig>& fieldConfig,
    HWND parent,
    HINSTANCE hInstance
) const
{
    std::wstring className{ m_windowClass + L"_Field" };
    LONG right{ m_windowWidth / 2 };

    for (auto&& [indx, config] : fieldConfig | std::views::enumerate)
    {
        const UI::ElementConfig<UI::FieldConfig> configure{
                .hInstance = hInstance,
                .ClassName = className + std::to_wstring(indx),
                .Rect = {right, m_topIndent + UI::gHeightField * static_cast<LONG>(indx), m_lenghtFloatField, UI::gHeightField},
                .Parent = parent,
                .Flags = {.Type = UI::ui_type::inbuilt, .Modes = UI::ui_modes::customposition, .Draw = UI::ui_draw::field::simple },
                .Extra = config
        };
        UI::Element<UI::Field>::Create(pDevice, pCaching, configure);
    }
}

HWND LisaApp::SettingsWindow::CreateSphere(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice, 
    const std::shared_ptr<UI::Caching>& pCaching, 
    HWND root, 
    HINSTANCE hInstance
)
{
    using namespace UI;

    HWND window = LisaGui::CreateSimpleWindow(
        pDevice, pCaching, root, hInstance, m_windowClass, L"Polygon Sphere Options",
        m_mainIcon, m_windowWidth, m_windowHeight,
        ui_draw::button::minimize | ui_draw::button::restore | ui_draw::button::close);

    if (!window)
    {
        // Assume that the window exists, so we will find its handler.
        window = FindWindowW(m_windowClassBuffer.c_str(), NULL);

        // Then we'll replace the title.
        SendMessageW(window, WM_COMMAND, UPDATE_TITLE, (LPARAM)(LPWSTR(L"Polygon Sphere Options")));

        if (!window)
            throw std::runtime_error("The desired window was not found.");
    }

    // If the window did not exist before, 
    // it will be created and therefore we create elements for it.
    // Otherwise, there is no need for elements.
    if (window)
    {
        // Save the current window title to the buffer for future use.
        if (wchar_t winClass[256]{}; GetClassNameW(window, winClass, 256))
            m_windowClassBuffer = winClass;
        else
            throw std::runtime_error("Failed to get window title.");

        RECT rc; GetClientRect(window, &rc);
        HWND plane = LisaGui::CreateFullResizablePlane(pDevice, pCaching, window, hInstance, m_windowClass + L"_" + L"Plane",
            rc.right - (gNonClientAreaSize * 2), rc.bottom - (gNonClientAreaSize * 2));

        // Create text.

        std::vector<FontConfig> titles
        {
            {.Input = L"Radius:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
            {.Input = L"Axis divisions:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
            {.Input = L"Height divisions:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING}
        };
        CreateLabelText(pDevice, pCaching, titles, plane, hInstance);

        // Create field.

        std::vector<FieldConfig> fc
        { 
            {.Value = gSphereData.Radius, .Min = 0.0, .Max = 10.0, .DecimalPlaces = 3, .Command = m_callCmd.GetMeaning(cmd_keys::sphereRadius)},
            {.Value = gSphereData.SubdivisionsAxis, .Min = 3u, .Max = 50u, .Command = m_callCmd.GetMeaning(cmd_keys::sphereSubdivAxis)},
            {.Value = gSphereData.SubdivisionsHeight, .Min = 3u, .Max = 50u, .Command = m_callCmd.GetMeaning(cmd_keys::sphereSubdivHeight)}
        };
        CreateSimpleField(pDevice, pCaching, fc, plane, hInstance);

        std::initializer_list<std::tuple<std::wstring, ui_command, CallCommand>> bottomButton
        {
            {L"Apply and Close", ui_command::close | ui_command::up, m_callCmd.GetMeaning(cmd_keys::sphere)},
            {L"Apply", ui_command::up, m_callCmd.GetMeaning(cmd_keys::sphere)},
            {L"Close", ui_command::close | ui_command::up, nullptr}
        };
        LisaGui::CreateBottomButtonBar(pDevice, pCaching, plane, hInstance, m_windowClass, bottomButton);

        LisaGui::SettingData(plane);
        LisaGui::SettingData(window);

        ClearingPreviousContents(window, plane);
    }

    return window;
}

HWND LisaApp::SettingsWindow::CreateGeoSphere(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice, const std::shared_ptr<UI::Caching>& pCaching, 
    HWND root, 
    HINSTANCE hInstance
)
{
    using namespace UI;

    HWND window = LisaGui::CreateSimpleWindow(
        pDevice, pCaching, root, hInstance, m_windowClass, L"Polygon GeoSphere Options",
        m_mainIcon, m_windowWidth, m_windowHeight,
        ui_draw::button::minimize | ui_draw::button::restore | ui_draw::button::close);

    if (!window)
    {
        // Assume that the window exists, so we will find its handler.
        window = FindWindowW(m_windowClassBuffer.c_str(), NULL);

        // Then we'll replace the title.
        SendMessageW(window, WM_COMMAND, UPDATE_TITLE, (LPARAM)(LPWSTR(L"Polygon GeoSphere Options")));

        if (!window)
            throw std::runtime_error("The desired window was not found.");
    }

    // If the window did not exist before, 
    // it will be created and therefore we create elements for it.
    // Otherwise, there is no need for elements.
    if (window)
    {
        // Save the current window title to the buffer for future use.
        if (wchar_t winClass[256]{}; GetClassNameW(window, winClass, 256))
            m_windowClassBuffer = winClass;
        else
            throw std::runtime_error("Failed to get window title.");

        RECT rc; GetClientRect(window, &rc);
        HWND plane = LisaGui::CreateFullResizablePlane(pDevice, pCaching, window, hInstance, m_windowClass + L"_" + L"Plane",
            rc.right - (gNonClientAreaSize * 2), rc.bottom - (gNonClientAreaSize * 2));

        // Create text.

        std::vector<FontConfig> titles
        {
            {.Input = L"Radius:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
            {.Input = L"Axis divisions:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING}
        };
        CreateLabelText(pDevice, pCaching, titles, plane, hInstance);

        // Create field.

        std::vector<FieldConfig> fc
        {
            {.Value = gGeoSphereData.Radius, .Min = 0.0, .Max = 10.0, .DecimalPlaces = 3, .Command = m_callCmd.GetMeaning(cmd_keys::geoSphereRadius)},
            {.Value = gGeoSphereData.Subdivisions, .Min = 1u, .Max = 5u, .Command = m_callCmd.GetMeaning(cmd_keys::geoSphereSubdiv)}
        };
        CreateSimpleField(pDevice, pCaching, fc, plane, hInstance);

        std::initializer_list<std::tuple<std::wstring, ui_command, CallCommand>> bottomButton
        {
            {L"Apply and Close", ui_command::close | ui_command::up, m_callCmd.GetMeaning(cmd_keys::geoSphere)},
            {L"Apply", ui_command::up, m_callCmd.GetMeaning(cmd_keys::geoSphere)},
            {L"Close", ui_command::close | ui_command::up, nullptr}
        };
        LisaGui::CreateBottomButtonBar(pDevice, pCaching, plane, hInstance, m_windowClass, bottomButton);

        LisaGui::SettingData(plane);
        LisaGui::SettingData(window);

        ClearingPreviousContents(window, plane);
    }

    return window;
}

HWND LisaApp::SettingsWindow::CreateCube(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice, 
    const std::shared_ptr<UI::Caching>& pCaching, 
    HWND root, 
    HINSTANCE hInstance
)
{
    using namespace UI;

    HWND window = LisaGui::CreateSimpleWindow(
        pDevice, pCaching, root, hInstance, m_windowClass, L"Polygon Cube Options",
        m_mainIcon, m_windowWidth, m_windowHeight,
        ui_draw::button::minimize | ui_draw::button::restore | ui_draw::button::close);

    if (!window)
    {
        // Assume that the window exists, so we will find its handler.
        window = FindWindowW(m_windowClassBuffer.c_str(), NULL);

        // Then we'll replace the title.
        SendMessageW(window, WM_COMMAND, UPDATE_TITLE, (LPARAM)(LPWSTR(L"Polygon Cube Options")));

        if (!window)
            throw std::runtime_error("The desired window was not found.");
    }

    // If the window did not exist before, 
    // it will be created and therefore we create elements for it.
    // Otherwise, there is no need for elements.
    if (window)
    {
        // Save the current window title to the buffer for future use.
        if (wchar_t winClass[256]{}; GetClassNameW(window, winClass, 256))
            m_windowClassBuffer = winClass;
        else
            throw std::runtime_error("Failed to get window title.");

        RECT rc; GetClientRect(window, &rc);
        HWND plane = LisaGui::CreateFullResizablePlane(pDevice, pCaching, window, hInstance, m_windowClass + L"_" + L"Plane",
            rc.right - (gNonClientAreaSize * 2), rc.bottom - (gNonClientAreaSize * 2));

        // Create text.

        std::vector<FontConfig> titles
        {
            {.Input = L"Width:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
            {.Input = L"Height:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
            {.Input = L"Depth:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
            {.Input = L"Width divisions:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
            {.Input = L"Height divisions:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
            {.Input = L"Depth divisions:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING}
        };
        CreateLabelText(pDevice, pCaching, titles, plane, hInstance);

        // Create field.

        std::vector<FieldConfig> fc
        {
            {.Value = gCubeData.Width, .Min = 0.0, .Max = 10.0, .DecimalPlaces = 3, .Command = m_callCmd.GetMeaning(cmd_keys::cubeWidth)},
            {.Value = gCubeData.Height, .Min = 0.0, .Max = 10.0, .DecimalPlaces = 3, .Command = m_callCmd.GetMeaning(cmd_keys::cubeHeight)},
            {.Value = gCubeData.Depth, .Min = 0.0, .Max = 10.0, .DecimalPlaces = 3, .Command = m_callCmd.GetMeaning(cmd_keys::cubeDepth)},
            {.Value = gCubeData.SubdivisionsWidth, .Min = 1u, .Max = 100u, .Command = m_callCmd.GetMeaning(cmd_keys::cubeSubdivWidth)},
            {.Value = gCubeData.SubdivisionsHeight, .Min = 1u, .Max = 100u, .Command = m_callCmd.GetMeaning(cmd_keys::cubeSubdivHeight)},
            {.Value = gCubeData.SubdivisionsDepth, .Min = 1u, .Max = 100u, .Command = m_callCmd.GetMeaning(cmd_keys::cubeSubdivDepth)}
        };
        CreateSimpleField(pDevice, pCaching, fc, plane, hInstance);

        std::initializer_list<std::tuple<std::wstring, ui_command, CallCommand>> bottomButton
        {
            {L"Apply and Close", ui_command::close | ui_command::up, m_callCmd.GetMeaning(cmd_keys::cube)},
            {L"Apply", ui_command::up, m_callCmd.GetMeaning(cmd_keys::cube)},
            {L"Close", ui_command::close | ui_command::up, nullptr}
        };
        LisaGui::CreateBottomButtonBar(pDevice, pCaching, plane, hInstance, m_windowClass, bottomButton);

        LisaGui::SettingData(plane);
        LisaGui::SettingData(window);

        ClearingPreviousContents(window, plane);
    }

    return window;
}

HWND LisaApp::SettingsWindow::CreateCylinder(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice, 
    const std::shared_ptr<UI::Caching>& pCaching, 
    HWND root, 
    HINSTANCE hInstance
)
{
    using namespace UI;

    HWND window = LisaGui::CreateSimpleWindow(
        pDevice, pCaching, root, hInstance, m_windowClass, L"Polygon Cylinder Options",
        m_mainIcon, m_windowWidth, m_windowHeight,
        ui_draw::button::minimize | ui_draw::button::restore | ui_draw::button::close);

    if (!window)
    {
        // Assume that the window exists, so we will find its handler.
        window = FindWindowW(m_windowClassBuffer.c_str(), NULL);

        // Then we'll replace the title.
        SendMessageW(window, WM_COMMAND, UPDATE_TITLE, (LPARAM)(LPWSTR(L"Polygon Cylinder Options")));

        if (!window)
            throw std::runtime_error("The desired window was not found.");
    }

    // If the window did not exist before, 
    // it will be created and therefore we create elements for it.
    // Otherwise, there is no need for elements.
    if (window)
    {
        // Save the current window title to the buffer for future use.
        if (wchar_t winClass[256]{}; GetClassNameW(window, winClass, 256))
            m_windowClassBuffer = winClass;
        else
            throw std::runtime_error("Failed to get window title.");

        RECT rc; GetClientRect(window, &rc);
        HWND plane = LisaGui::CreateFullResizablePlane(pDevice, pCaching, window, hInstance, m_windowClass + L"_" + L"Plane",
            rc.right - (gNonClientAreaSize * 2), rc.bottom - (gNonClientAreaSize * 2));

        // Create text.

        std::vector<FontConfig> titles
        {
            {.Input = L"Radius:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
            {.Input = L"Height:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
            {.Input = L"Axis divisions:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
            {.Input = L"Height divisions:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
            {.Input = L"Caps divisions:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING}
        };
        CreateLabelText(pDevice, pCaching, titles, plane, hInstance);

        // Create field.

        std::vector<FieldConfig> fc
        {
            {.Value = gCylinderData.Radius, .Min = 0.0, .Max = 10.0, .DecimalPlaces = 3, .Command = m_callCmd.GetMeaning(cmd_keys::cylinderRadius)},
            {.Value = gCylinderData.Height, .Min = 0.0, .Max = 10.0, .DecimalPlaces = 3, .Command = m_callCmd.GetMeaning(cmd_keys::cylinderHeight)},
            {.Value = gCylinderData.SubdivisionsAxis, .Min = 3u, .Max = 50u, .Command = m_callCmd.GetMeaning(cmd_keys::cylinderSubdivAxis)},
            {.Value = gCylinderData.SubdivisionsHeight, .Min = 1u, .Max = 50u, .Command = m_callCmd.GetMeaning(cmd_keys::cylinderSubdivHeight)},
            {.Value = gCylinderData.SubdivisionsCaps, .Min = 1u, .Max = 50u, .Command = m_callCmd.GetMeaning(cmd_keys::cylinderSubdivCaps)}
        };
        CreateSimpleField(pDevice, pCaching, fc, plane, hInstance);

        std::initializer_list<std::tuple<std::wstring, ui_command, CallCommand>> bottomButton
        {
            {L"Apply and Close", ui_command::close | ui_command::up, m_callCmd.GetMeaning(cmd_keys::cylinder)},
            {L"Apply", ui_command::up, m_callCmd.GetMeaning(cmd_keys::cylinder)},
            {L"Close", ui_command::close | ui_command::up, nullptr}
        };
        LisaGui::CreateBottomButtonBar(pDevice, pCaching, plane, hInstance, m_windowClass, bottomButton);

        LisaGui::SettingData(plane);
        LisaGui::SettingData(window);

        ClearingPreviousContents(window, plane);
    }

    return window;
}

HWND LisaApp::SettingsWindow::CreateCone(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice, 
    const std::shared_ptr<UI::Caching>& pCaching, 
    HWND root, 
    HINSTANCE hInstance
)
{
    using namespace UI;

    HWND window = LisaGui::CreateSimpleWindow(
        pDevice, pCaching, root, hInstance, m_windowClass, L"Polygon Cone Options",
        m_mainIcon, m_windowWidth, m_windowHeight,
        ui_draw::button::minimize | ui_draw::button::restore | ui_draw::button::close);

    if (!window)
    {
        // Assume that the window exists, so we will find its handler.
        window = FindWindowW(m_windowClassBuffer.c_str(), NULL);

        // Then we'll replace the title.
        SendMessageW(window, WM_COMMAND, UPDATE_TITLE, (LPARAM)(LPWSTR(L"Polygon Cone Options")));

        if (!window)
            throw std::runtime_error("The desired window was not found.");
    }

    // If the window did not exist before, 
    // it will be created and therefore we create elements for it.
    // Otherwise, there is no need for elements.
    if (window)
    {
        // Save the current window title to the buffer for future use.
        if (wchar_t winClass[256]{}; GetClassNameW(window, winClass, 256))
            m_windowClassBuffer = winClass;
        else
            throw std::runtime_error("Failed to get window title.");

        RECT rc; GetClientRect(window, &rc);
        HWND plane = LisaGui::CreateFullResizablePlane(pDevice, pCaching, window, hInstance, m_windowClass + L"_" + L"Plane",
            rc.right - (gNonClientAreaSize * 2), rc.bottom - (gNonClientAreaSize * 2));

        // Create text.

        std::vector<FontConfig> titles
        {
            {.Input = L"Radius:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
            {.Input = L"Height:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
            {.Input = L"Axis divisions:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
            {.Input = L"Height divisions:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
            {.Input = L"Caps divisions:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING}
        };
        CreateLabelText(pDevice, pCaching, titles, plane, hInstance);

        // Create field.

        std::vector<FieldConfig> fc
        {
            {.Value = gConeData.Radius, .Min = 0.0, .Max = 10.0, .DecimalPlaces = 3, .Command = m_callCmd.GetMeaning(cmd_keys::coneRadius)},
            {.Value = gConeData.Height, .Min = 0.0, .Max = 10.0, .DecimalPlaces = 3, .Command = m_callCmd.GetMeaning(cmd_keys::coneHeight)},
            {.Value = gConeData.SubdivisionsAxis, .Min = 3u, .Max = 50u, .Command = m_callCmd.GetMeaning(cmd_keys::coneSubdivAxis)},
            {.Value = gConeData.SubdivisionsHeight, .Min = 1u, .Max = 50u, .Command = m_callCmd.GetMeaning(cmd_keys::coneSubdivHeight)},
            {.Value = gConeData.SubdivisionsCaps, .Min = 1u, .Max = 50u, .Command = m_callCmd.GetMeaning(cmd_keys::coneSubdivCaps)}
        };
        CreateSimpleField(pDevice, pCaching, fc, plane, hInstance);

        std::initializer_list<std::tuple<std::wstring, ui_command, CallCommand>> bottomButton
        {
            {L"Apply and Close", ui_command::close | ui_command::up, m_callCmd.GetMeaning(cmd_keys::cone)},
            {L"Apply", ui_command::up, m_callCmd.GetMeaning(cmd_keys::cone)},
            {L"Close", ui_command::close | ui_command::up, nullptr}
        };
        LisaGui::CreateBottomButtonBar(pDevice, pCaching, plane, hInstance, m_windowClass, bottomButton);

        LisaGui::SettingData(plane);
        LisaGui::SettingData(window);

        ClearingPreviousContents(window, plane);
    }

    return window;
}

HWND LisaApp::SettingsWindow::CreateTorus(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice, 
    const std::shared_ptr<UI::Caching>& pCaching, 
    HWND root, 
    HINSTANCE hInstance
)
{
    using namespace UI;

    HWND window = LisaGui::CreateSimpleWindow(
        pDevice, pCaching, root, hInstance, m_windowClass, L"Polygon Torus Options",
        m_mainIcon, m_windowWidth, m_windowHeight,
        ui_draw::button::minimize | ui_draw::button::restore | ui_draw::button::close);

    if (!window)
    {
        // Assume that the window exists, so we will find its handler.
        window = FindWindowW(m_windowClassBuffer.c_str(), NULL);

        // Then we'll replace the title.
        SendMessageW(window, WM_COMMAND, UPDATE_TITLE, (LPARAM)(LPWSTR(L"Polygon Torus Options")));

        if (!window)
            throw std::runtime_error("The desired window was not found.");
    }

    // If the window did not exist before, 
    // it will be created and therefore we create elements for it.
    // Otherwise, there is no need for elements.
    if (window)
    {
        // Save the current window title to the buffer for future use.
        if (wchar_t winClass[256]{}; GetClassNameW(window, winClass, 256))
            m_windowClassBuffer = winClass;
        else
            throw std::runtime_error("Failed to get window title.");

        RECT rc; GetClientRect(window, &rc);
        HWND plane = LisaGui::CreateFullResizablePlane(pDevice, pCaching, window, hInstance, m_windowClass + L"_" + L"Plane",
            rc.right - (gNonClientAreaSize * 2), rc.bottom - (gNonClientAreaSize * 2));

        // Create text.

        std::vector<FontConfig> titles
        {
            {.Input = L"Radius:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
            {.Input = L"Section radius:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
            {.Input = L"Axis divisions:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
            {.Input = L"Height divisions:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING}
        };
        CreateLabelText(pDevice, pCaching, titles, plane, hInstance);

        // Create field.

        std::vector<FieldConfig> fc
        {
            {.Value = gTorusData.Radius, .Min = 0.0, .Max = 10.0, .DecimalPlaces = 3, .Command = m_callCmd.GetMeaning(cmd_keys::torusRadius)},
            {.Value = gTorusData.SectionRadius, .Min = 0.0, .Max = 10.0, .DecimalPlaces = 3, .Command = m_callCmd.GetMeaning(cmd_keys::torusSectionRadius)},
            {.Value = gTorusData.SubdivisionsAxis, .Min = 1u, .Max = 50u, .Command = m_callCmd.GetMeaning(cmd_keys::torusSubdivAxis)},
            {.Value = gTorusData.SubdivisionsHeight, .Min = 1u, .Max = 50u, .Command = m_callCmd.GetMeaning(cmd_keys::torusSubdivHeight)}
        };
        CreateSimpleField(pDevice, pCaching, fc, plane, hInstance);

        std::initializer_list<std::tuple<std::wstring, ui_command, CallCommand>> bottomButton
        {
            {L"Apply and Close", ui_command::close | ui_command::up, m_callCmd.GetMeaning(cmd_keys::torus)},
            {L"Apply", ui_command::up, m_callCmd.GetMeaning(cmd_keys::torus)},
            {L"Close", ui_command::close | ui_command::up, nullptr}
        };
        LisaGui::CreateBottomButtonBar(pDevice, pCaching, plane, hInstance, m_windowClass, bottomButton);

        LisaGui::SettingData(plane);
        LisaGui::SettingData(window);

        ClearingPreviousContents(window, plane);
    }

    return window;
}

HWND LisaApp::SettingsWindow::CreatePlane(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice, 
    const std::shared_ptr<UI::Caching>& pCaching, 
    HWND root, 
    HINSTANCE hInstance
)
{
    using namespace UI;

    HWND window = LisaGui::CreateSimpleWindow(
        pDevice, pCaching, root, hInstance, m_windowClass, L"Polygon Plane Options",
        m_mainIcon, m_windowWidth, m_windowHeight,
        ui_draw::button::minimize | ui_draw::button::restore | ui_draw::button::close);

    if (!window)
    {
        // Assume that the window exists, so we will find its handler.
        window = FindWindowW(m_windowClassBuffer.c_str(), NULL);

        // Then we'll replace the title.
        SendMessageW(window, WM_COMMAND, UPDATE_TITLE, (LPARAM)(LPWSTR(L"Polygon Plane Options")));

        if (!window)
            throw std::runtime_error("The desired window was not found.");
    }

    // If the window did not exist before, 
    // it will be created and therefore we create elements for it.
    // Otherwise, there is no need for elements.
    if (window)
    {
        // Save the current window title to the buffer for future use.
        if (wchar_t winClass[256]{}; GetClassNameW(window, winClass, 256))
            m_windowClassBuffer = winClass;
        else
            throw std::runtime_error("Failed to get window title.");

        RECT rc; GetClientRect(window, &rc);
        HWND plane = LisaGui::CreateFullResizablePlane(pDevice, pCaching, window, hInstance, m_windowClass + L"_" + L"Plane",
            rc.right - (gNonClientAreaSize * 2), rc.bottom - (gNonClientAreaSize * 2));

        // Create text.

        std::vector<FontConfig> titles
        {
            {.Input = L"Width:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
            {.Input = L"Depth:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
            {.Input = L"Width divisions:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING},
            {.Input = L"Depth divisions:", .TextAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING}
        };
        CreateLabelText(pDevice, pCaching, titles, plane, hInstance);

        // Create field.

        std::vector<FieldConfig> fc
        {
            {.Value = gPlaneData.Width, .Min = 0.0, .Max = 100.0, .DecimalPlaces = 3, .Command = m_callCmd.GetMeaning(cmd_keys::planeWidth)},
            {.Value = gPlaneData.Depth, .Min = 0.0, .Max = 100.0, .DecimalPlaces = 3, .Command = m_callCmd.GetMeaning(cmd_keys::planeDepth)},
            {.Value = gPlaneData.SubdivisionsWidth, .Min = 1u, .Max = 100000u, .Command = m_callCmd.GetMeaning(cmd_keys::planeSubdivWidth)},
            {.Value = gPlaneData.SubdivisionsDepth, .Min = 1u, .Max = 100000u, .Command = m_callCmd.GetMeaning(cmd_keys::planeSubdivDepth)}
        };
        CreateSimpleField(pDevice, pCaching, fc, plane, hInstance);

        std::initializer_list<std::tuple<std::wstring, ui_command, CallCommand>> bottomButton
        {
            {L"Apply and Close", ui_command::close | ui_command::up, m_callCmd.GetMeaning(cmd_keys::plane)},
            {L"Apply", ui_command::up, m_callCmd.GetMeaning(cmd_keys::plane)},
            {L"Close", ui_command::close | ui_command::up, nullptr}
        };
        LisaGui::CreateBottomButtonBar(pDevice, pCaching, plane, hInstance, m_windowClass, bottomButton);

        LisaGui::SettingData(plane);
        LisaGui::SettingData(window);

        ClearingPreviousContents(window, plane);
    }

    return window;
}

HWND LisaApp::SettingsWindow::CreateAbout(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice, 
    const std::shared_ptr<UI::Caching>& pCaching,
    HWND root, 
    HINSTANCE hInstance
)
{
    using namespace UI;

    HWND window = LisaGui::CreateSimpleWindow(
        pDevice, pCaching, root, hInstance, m_windowClass, L"About Lisa",
        m_mainIcon, m_windowWidth, m_windowHeight,
        ui_draw::button::minimize | ui_draw::button::restore | ui_draw::button::close);

    if (!window)
    {
        // Assume that the window exists, so we will find its handler.
        window = FindWindowW(m_windowClassBuffer.c_str(), NULL);

        // Then we'll replace the title.
        SendMessageW(window, WM_COMMAND, UPDATE_TITLE, (LPARAM)(LPWSTR(L"About Lisa")));

        if (!window)
            throw std::runtime_error("The desired window was not found.");
    }

    // If the window did not exist before, 
    // it will be created and therefore we create elements for it.
    // Otherwise, there is no need for elements.
    if (window)
    {
        // Save the current window title to the buffer for future use.
        if (wchar_t winClass[256]{}; GetClassNameW(window, winClass, 256))
            m_windowClassBuffer = winClass;
        else
            throw std::runtime_error("Failed to get window title.");

        /*wchar_t winClass[256]{};
        GetClassNameW(window, winClass, 256);
        m_windowClassBuffer = winClass;*/

        RECT rc; GetClientRect(window, &rc);
        HWND plane = LisaGui::CreateFullResizablePlane(pDevice, pCaching, window, hInstance, m_windowClass + L"_" + L"Plane",
            rc.right - (gNonClientAreaSize * 2), rc.bottom - (gNonClientAreaSize * 2));

        // Create text.

        std::vector<FontConfig> fontConfig
        {
            {
                .Input = L"A program for simulation physical properties of objects and animation.", 
                .TextAlignment = DWRITE_TEXT_ALIGNMENT_CENTER
            },
        };

        const ElementConfig<TextConfig> textConfig{
            .hInstance = hInstance,
            .ClassName = L"text",
            .TitleName = L"text",
            .Rect = {  (m_windowWidth / 2) - 260, m_topIndent + 260, 260 * 2, 24 },
            .Parent = plane,
            .Flags = {.Type = ui_type::inbuilt, .Modes = ui_modes::customposition, .Draw = ui_draw::text::simple },
            .Extra =
            {
                .ImgTxt = fontConfig
            }
        };
        Element<UI::Text>::Create(pDevice, pCaching, textConfig);

        // Create images.

        std::vector<std::wstring> images
        {
            LisaApp::HelperPath(L"Resources\\AboutImage\\about_lisa_image_3.png"),
            LisaApp::HelperPath(L"Resources\\AboutImage\\about_lisa_image_2.png")
        };

        const ElementConfig<ImageConfig> imageConfig{
            .hInstance = hInstance,
            .ClassName = L"image",
            .Rect = { (m_windowWidth / 2) - 260, m_topIndent, 260 * 2, 260 },
            .Parent = plane,
            .Flags = {.Type = ui_type::inbuilt, .Modes = ui_modes::customposition, .Draw = ui_draw::image::simple },
            .Extra =
            {
                .ImgTxt = images
            }
        };
        Element<UI::Image>::Create(pDevice, pCaching, imageConfig);

        std::initializer_list<std::tuple<std::wstring, ui_command, CallCommand>> bottomButton
        {
            {L"Ok", ui_command::close | ui_command::up, nullptr}
        };
        LisaGui::CreateBottomButtonBar(pDevice, pCaching, plane, hInstance, m_windowClass, bottomButton);

        LisaGui::SettingData(plane);
        LisaGui::SettingData(window);

        ClearingPreviousContents(window, plane);
    }

    return window;
}
