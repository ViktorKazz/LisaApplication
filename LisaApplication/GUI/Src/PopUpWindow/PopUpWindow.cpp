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

#include "..//GUI/Inc/PopUpWindow/PopUpWindow.h"
#include "LisaGui.h"

HWND LisaApp::PopUpWindow::More2(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice, 
    const std::shared_ptr<UI::Caching>& pCaching, 
    HWND root, 
    HINSTANCE hInstance
)
{
    if (!pDevice)
        throw std::runtime_error("Device resources not specified.");

    if (!root)
        throw std::runtime_error("Root window not specified.");

    if (!hInstance)
        throw std::runtime_error("hInstance not specified.");

    const std::vector<LisaGui::PopUpConfig>& popUpConfig
    {
        {L"More...More...", {}, UI::ui_draw::button::pup_simple, {}}
    };

    HWND hwnd = LisaGui::CreatePopUpWindow(pDevice, pCaching, root, hInstance, L"more2", popUpConfig);

    return hwnd;
}

HWND LisaApp::PopUpWindow::More(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice, 
    const std::shared_ptr<UI::Caching>& pCaching, 
    HWND root, 
    HINSTANCE hInstance
)
{
    if (!pDevice)
        throw std::runtime_error("Device resources not specified.");

    if (!root)
        throw std::runtime_error("Root window not specified.");

    if (!hInstance)
        throw std::runtime_error("hInstance not specified.");

    const std::vector<LisaGui::PopUpConfig>& popUpConfig
    {
        {L"More...", {}, UI::ui_draw::button::pup_transition, m_callWnd.GetMeaning(wnd_keys::testMore2)}
    };

    HWND hwnd = LisaGui::CreatePopUpWindow(pDevice, pCaching, root, hInstance, L"more", popUpConfig);

    return hwnd;
}

HWND LisaApp::PopUpWindow::HelpFeedback(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice,
    const std::shared_ptr<UI::Caching>& pCaching, 
    HWND root, 
    HINSTANCE hInstance
)
{
    if (!pDevice)
        throw std::runtime_error("Device resources not specified.");

    if (!root)
        throw std::runtime_error("Root window not specified.");

    if (!hInstance)
        throw std::runtime_error("hInstance not specified.");

    const std::vector<LisaGui::PopUpConfig>& popUpConfig
    {
        {L"Report a Problem...", {}, UI::ui_draw::button::pup_simple, {}}
    };

    HWND hwnd = LisaGui::CreatePopUpWindow(pDevice, pCaching, root, hInstance, L"ReportProblem", popUpConfig);

    return hwnd;
}

HWND LisaApp::PopUpWindow::SelectAllType(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice,
    const std::shared_ptr<UI::Caching>& pCaching, 
    HWND root,
    HINSTANCE hInstance
)
{
    if (!pDevice)
        throw std::runtime_error("Device resources not specified.");

    if (!root)
        throw std::runtime_error("Root window not specified.");

    if (!hInstance)
        throw std::runtime_error("hInstance not specified.");

    const std::vector<LisaGui::PopUpConfig>& popUpConfig
    {
        {L"Geometry", {}, UI::ui_draw::button::pup_simple, {}},
        {L"Polygon Geometry", {}, UI::ui_draw::button::pup_simple, {}},
        {{}, {}, UI::ui_draw::separator::popUp, {}},
        {L"Cameras", {}, UI::ui_draw::button::pup_simple, {}},
        {L"Lights", {}, UI::ui_draw::button::pup_simple, {}}
    };

    HWND hwnd = LisaGui::CreatePopUpWindow(pDevice, pCaching, root, hInstance, L"selectAllType", popUpConfig);

    return hwnd;
}

HWND LisaApp::PopUpWindow::SelectAllComponents(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice, 
    const std::shared_ptr<UI::Caching>& pCaching, 
    HWND root,
    HINSTANCE hInstance
)
{
    if (!pDevice)
        throw std::runtime_error("Device resources not specified.");

    if (!root)
        throw std::runtime_error("Root window not specified.");

    if (!hInstance)
        throw std::runtime_error("hInstance not specified.");

    const std::vector<LisaGui::PopUpConfig>& popUpConfig
    {
        {L"Vertex", L"F9", UI::ui_draw::button::pup_simple, {}},
        {L"Edge", L"F10", UI::ui_draw::button::pup_simple, {}},
        {L"Face", L"F11", UI::ui_draw::button::pup_simple, {}},
        {L"More", L"     ", UI::ui_draw::button::pup_transition, m_callWnd.GetMeaning(wnd_keys::testMore1)}
    };

    HWND hwnd = LisaGui::CreatePopUpWindow(pDevice, pCaching, root, hInstance, L"selectAllComponents", popUpConfig);

    return hwnd;
}

HWND LisaApp::PopUpWindow::CreatePolyPrimitives(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice,
    const std::shared_ptr<UI::Caching>& pCaching, 
    HWND root, 
    HINSTANCE hInstance
)
{
    if (!pDevice)
        throw std::runtime_error("Device resources not specified.");

    if (!root)
        throw std::runtime_error("Root window not specified.");

    if (!hInstance)
        throw std::runtime_error("hInstance not specified.");

    const std::vector<LisaGui::PopUpConfig>& popUpConfig
    {
        {L"Sphere", {}, UI::ui_draw::button::pup_dual, std::pair{m_callWnd.GetMeaning(wnd_keys::sphere), m_callCmd.GetMeaning(cmd_keys::sphere)} },
        {L"GeoSphere", {}, UI::ui_draw::button::pup_dual, std::pair{m_callWnd.GetMeaning(wnd_keys::geoSphere), m_callCmd.GetMeaning(cmd_keys::geoSphere)}},
        {L"Cube", {}, UI::ui_draw::button::pup_dual, std::pair{m_callWnd.GetMeaning(wnd_keys::cube), m_callCmd.GetMeaning(cmd_keys::cube)}},
        {L"Cylinder", {}, UI::ui_draw::button::pup_dual, std::pair{m_callWnd.GetMeaning(wnd_keys::cylinder), m_callCmd.GetMeaning(cmd_keys::cylinder)}},
        {L"Cone", {}, UI::ui_draw::button::pup_dual, std::pair{m_callWnd.GetMeaning(wnd_keys::cone), m_callCmd.GetMeaning(cmd_keys::cone)}},
        {L"Torus", {}, UI::ui_draw::button::pup_dual, std::pair{m_callWnd.GetMeaning(wnd_keys::torus), m_callCmd.GetMeaning(cmd_keys::torus)}},
        {L"Plane", {}, UI::ui_draw::button::pup_dual, std::pair{m_callWnd.GetMeaning(wnd_keys::plane), m_callCmd.GetMeaning(cmd_keys::plane)}}
    };

    HWND hwnd = LisaGui::CreatePopUpWindow(pDevice, pCaching, root, hInstance, L"createPolyPrimitives", popUpConfig);

    return hwnd;
}

HWND LisaApp::PopUpWindow::CreateLights(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice, 
    const std::shared_ptr<UI::Caching>& pCaching, 
    HWND root, 
    HINSTANCE hInstance
)
{
    if (!pDevice)
        throw std::runtime_error("Device resources not specified.");

    if (!root)
        throw std::runtime_error("Root window not specified.");

    if (!hInstance)
        throw std::runtime_error("hInstance not specified.");

    const std::vector<LisaGui::PopUpConfig>& popUpConfig
    {
        {L"Ambient Light", {}, UI::ui_draw::button::pup_dual, {}},
        {L"Directional Light", {}, UI::ui_draw::button::pup_dual, {}},
        {L"Point Light", {}, UI::ui_draw::button::pup_dual, {}},
        {L"Spot Light", {}, UI::ui_draw::button::pup_dual, {}},
        {L"Area Light", {}, UI::ui_draw::button::pup_dual, {}}
    };

    HWND hwnd = LisaGui::CreatePopUpWindow(pDevice, pCaching, root, hInstance, L"createLights", popUpConfig);

    return hwnd;
}

HWND LisaApp::PopUpWindow::CreateCamera(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice, 
    const std::shared_ptr<UI::Caching>& pCaching, 
    HWND root, 
    HINSTANCE hInstance
)
{
    if (!pDevice)
        throw std::runtime_error("Device resources not specified.");

    if (!root)
        throw std::runtime_error("Root window not specified.");

    if (!hInstance)
        throw std::runtime_error("hInstance not specified.");

    const std::vector<LisaGui::PopUpConfig>& popUpConfig
    {
        {L"Camera", {}, UI::ui_draw::button::pup_dual, {}},
    };

    HWND hwnd = LisaGui::CreatePopUpWindow(pDevice, pCaching, root, hInstance, L"createCamera", popUpConfig);

    return hwnd;
}


HWND LisaApp::PopUpWindow::MainTopBarFile(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice, 
    const std::shared_ptr<UI::Caching>& pCaching, 
    HWND root,
    HINSTANCE hInstance
)
{
    if (!pDevice)
        throw std::runtime_error("Device resources not specified.");

    if (!root)
        throw std::runtime_error("Root window not specified.");

    if (!hInstance)
        throw std::runtime_error("hInstance not specified.");

    const std::vector<LisaGui::PopUpConfig>& popUpConfig
    {
        {L"New Scene", L"Ctrl+N", UI::ui_draw::button::pup_simple, {}},
        {L"Open Scene..", L"Ctrl+O", UI::ui_draw::button::pup_simple, {}},
        {{}, {}, UI::ui_draw::separator::popUp, {}},
        {L"Save Scene..", L"Ctrl+S", UI::ui_draw::button::pup_simple, {}},
        {L"Save Scene As..", L"Ctrl+Shift+S", UI::ui_draw::button::pup_simple, {}},
        {{}, {}, UI::ui_draw::separator::popUp, {}},
        {L"Exit", L"Ctrl+Q", UI::ui_draw::button::pup_simple, {}}
    };

    HWND hwnd = LisaGui::CreatePopUpWindow(pDevice, pCaching, root, hInstance, L"mainFile", popUpConfig);

    return hwnd;
}

HWND LisaApp::PopUpWindow::MainTopBarEdit(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice, 
    const std::shared_ptr<UI::Caching>& pCaching, 
    HWND root,
    HINSTANCE hInstance
)
{
    if (!pDevice)
        throw std::runtime_error("Device resources not specified.");

    if (!root)
        throw std::runtime_error("Root window not specified.");

    if (!hInstance)
        throw std::runtime_error("hInstance not specified.");

    const std::vector<LisaGui::PopUpConfig>& popUpConfig
    {
        {L"Undo", L"Ctrl+Z", UI::ui_draw::button::pup_simple, {}},
        {L"Redo", L"Ctrl+Y", UI::ui_draw::button::pup_simple, {}},
        {{}, {}, UI::ui_draw::separator::popUp, {}},
        {L"Cut", L"Ctrl+X", UI::ui_draw::button::pup_simple, {}},
        {L"Copy", L"Ctrl+C", UI::ui_draw::button::pup_simple, {}},
        {L"Paste", L"Ctrl+V", UI::ui_draw::button::pup_simple, {}},
        {{}, {}, UI::ui_draw::separator::popUp, {}},
        {L"Delete", {} , UI::ui_draw::button::pup_simple, {}},
        {{}, {}, UI::ui_draw::separator::popUp, {}},
        {L"Duplicate", L"Ctrl+D", UI::ui_draw::button::pup_simple, {}}
    };

    HWND hwnd = LisaGui::CreatePopUpWindow(pDevice, pCaching, root, hInstance, L"mainEdit", popUpConfig);

    return hwnd;
}

HWND LisaApp::PopUpWindow::MainTopBarCreate(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice, 
    const std::shared_ptr<UI::Caching>& pCaching,
    HWND root,
    HINSTANCE hInstance
)
{
    if (!pDevice)
        throw std::runtime_error("Device resources not specified.");

    if (!root)
        throw std::runtime_error("Root window not specified.");

    if (!hInstance)
        throw std::runtime_error("hInstance not specified.");

    const std::vector<LisaGui::PopUpConfig>& popUpConfig
    {
        {L"Polygon Primitives", {}, UI::ui_draw::button::pup_transition, m_callWnd.GetMeaning(wnd_keys::polyPrimitives)},
        {L"Lights", {}, UI::ui_draw::button::pup_transition, m_callWnd.GetMeaning(wnd_keys::lights)},
        {L"Cameras", {}, UI::ui_draw::button::pup_transition, m_callWnd.GetMeaning(wnd_keys::cameras)},
        {{}, {}, UI::ui_draw::separator::popUp, {}},
        {L"Locator", {}, UI::ui_draw::button::pup_simple, {}}
    };

    HWND hwnd = LisaGui::CreatePopUpWindow(pDevice, pCaching, root, hInstance, L"mainCreate", popUpConfig);

    return hwnd;
}

HWND LisaApp::PopUpWindow::MainTopBarSelect(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice, 
    const std::shared_ptr<UI::Caching>& pCaching,
    HWND root, 
    HINSTANCE hInstance
)
{
    if (!pDevice)
        throw std::runtime_error("Device resources not specified.");

    if (!root)
        throw std::runtime_error("Root window not specified.");

    if (!hInstance)
        throw std::runtime_error("hInstance not specified.");

    const std::vector<LisaGui::PopUpConfig>& popUpConfig
    {
        {L"All", L"Ctrl+Shift+A", UI::ui_draw::button::pup_simple, {}},
        {L"All by Type", {}, UI::ui_draw::button::pup_transition, m_callWnd.GetMeaning(wnd_keys::allByType)},
        {L"Deselect All", L"Alt+D", UI::ui_draw::button::pup_simple, {}},
        {{}, {}, UI::ui_draw::separator::popUp, {}},
        {L"Object/Component", L"F8", UI::ui_draw::button::pup_simple, {}},
        { {}, {}, UI::ui_draw::separator::popUp, {}},
        {L"Components", {}, UI::ui_draw::button::pup_transition, m_callWnd.GetMeaning(wnd_keys::components)}
    };

    HWND hwnd = LisaGui::CreatePopUpWindow(pDevice, pCaching, root, hInstance, L"mainSelect", popUpConfig);

    return hwnd;
}

HWND LisaApp::PopUpWindow::MainTopBarHelp(
    const std::shared_ptr<UI::D11DeviceResources>& pDevice, 
    const std::shared_ptr<UI::Caching>& pCaching, 
    HWND root,
    HINSTANCE hInstance
)
{
    if (!pDevice)
        throw std::runtime_error("Device resources not specified.");

    if (!root)
        throw std::runtime_error("Root window not specified.");

    if (!hInstance)
        throw std::runtime_error("hInstance not specified.");

    const std::vector<LisaGui::PopUpConfig>& popUpConfig
    {
        {L"Lisa Help", L"F1", UI::ui_draw::button::pup_simple, {}},
        {{}, {}, UI::ui_draw::separator::popUp, {}},
        {L"YouTube Learning Channel", {}, UI::ui_draw::button::pup_simple, {}},
        {{}, {}, UI::ui_draw::separator::popUp, {}},
        {L"Feedback", {}, UI::ui_draw::button::pup_transition, m_callWnd.GetMeaning(wnd_keys::feedback)},
        {{}, {}, UI::ui_draw::separator::popUp, {}},
        {L"About Lisa", {}, UI::ui_draw::button::pup_simple, m_callWnd.GetMeaning(wnd_keys::about)},
    };

    HWND hwnd = LisaGui::CreatePopUpWindow(pDevice, pCaching, root, hInstance, L"mainHelp", popUpConfig);

    return hwnd;
}