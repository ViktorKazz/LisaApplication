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

#include "ApplicationContent.h"
#include "SettingsWindow.h"
#include "..//IconResource/resource.h"
#include "..//GUI/Inc/CommandFunc.h"

#include "Globals.h"

void LisaApp::AppContent::Content(HINSTANCE hInstance)
{
    using namespace UI;
    using namespace LisaGui;


    CommandFunc commandFunc;

    // Commands for creating objects.
    CallCommand createSphere = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::CreateSphere);
    CallCommand createGeoSphere = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::CreateGeoSphere);
    CallCommand createCube = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::CreateCube);
    CallCommand createCylinder = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::CreateCylinder);
    CallCommand createCone = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::CreateCone);
    CallCommand createTorus = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::CreateTorus);
    CallCommand createPlane = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::CreatePlane);

    // Commands for setting values from input fields to global variables.
    CallCommand sphereDataRadius = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetSphereDataRadius);
    CallCommand sphereDataSubdivAxis = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetSphereDataSubdivAxis);
    CallCommand sphereDataSubdivHeight = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetSphereDataSubdivHeight);

    CallCommand geoSphereDataRadius = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetGeoSphereDataRadius);
    CallCommand geoSphereDataSubdivAxis = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetGeoSphereDataSubdiv);

    CallCommand cubeDataWidth = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetCubeDataWidth);
    CallCommand cubeDataHeight = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetCubeDataHeight);
    CallCommand cubeDataDepth = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetCubeDataDepth);
    CallCommand cubeDataSubdivWidth = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetCubeDataSubdivWidth);
    CallCommand cubeDataSubdivHeight = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetCubeDataSubdivHeight);
    CallCommand cubeDataSubdivDepth = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetCubeDataSubdivDepth);

    CallCommand cylinderDataRadius = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetCylinderDataRadius);
    CallCommand cylinderDataHeight = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetCylinderDataHeight);
    CallCommand cylinderDataSubdivAxis = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetCylinderDataSubdivAxis);
    CallCommand cylinderDataSubdivHeight = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetCylinderDataSubdivHeight);
    CallCommand cylinderDataSubdivCaps = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetCylinderDataSubdivCaps);
    
    CallCommand coneDataRadius = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetConeDataRadius);
    CallCommand coneDataHeight = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetConeDataHeight);
    CallCommand coneDataSubdivAxis = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetConeDataSubdivAxis);
    CallCommand coneDataSubdivHeight = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetConeDataSubdivHeight);
    CallCommand coneDataSubdivCaps = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetConeDataSubdivCaps);
    
    CallCommand torusDataRadius = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetTorusDataRadius);
    CallCommand torusDataSectionRadius = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetTorusDataSectionRadius);
    CallCommand torusDataSubdivAxis = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetTorusDataSubdivAxis);
    CallCommand torusDataSubdivHeight = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetTorusDataSubdivHeight);

    CallCommand planeDataWidth = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetPlaneDataWidth);
    CallCommand planeDataDepth = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetPlaneDataDepth);
    CallCommand planeDataSubdivWidth = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetPlaneDataSubdivWidth);
    CallCommand planeDataSubdivDepth = commandFunc.m_callCmd.SetFunc(&commandFunc, &CommandFunc::SetPlaneDataSubdivDepth);


    SettingsWindow settingsWindow(600, 440, L"SettingsWindow", m_icon);

    // Binding commands to input fields and buttons.
    settingsWindow.m_callCmd.SetFunc(
        &settingsWindow, 
        &SettingsWindow::CreateSphere, 
        std::vector{ 
            std::pair{cmd_keys::sphere, createSphere},
            std::pair{cmd_keys::sphereRadius, sphereDataRadius},
            std::pair{cmd_keys::sphereSubdivAxis, sphereDataSubdivAxis},
            std::pair{cmd_keys::sphereSubdivHeight, sphereDataSubdivHeight}
        }
    );

    settingsWindow.m_callCmd.SetFunc(
        &settingsWindow, 
        &SettingsWindow::CreateGeoSphere, 
        std::vector{ 
            std::pair{cmd_keys::geoSphere, createGeoSphere},
            std::pair{cmd_keys::geoSphereRadius, geoSphereDataRadius},
            std::pair{cmd_keys::geoSphereSubdiv, geoSphereDataSubdivAxis},
        }
    );

    settingsWindow.m_callCmd.SetFunc(
        &settingsWindow, 
        &SettingsWindow::CreateCube, 
        std::vector{ 
            std::pair{cmd_keys::cube, createCube},
            std::pair{cmd_keys::cubeWidth, cubeDataWidth},
            std::pair{cmd_keys::cubeHeight, cubeDataHeight},
            std::pair{cmd_keys::cubeDepth, cubeDataDepth},
            std::pair{cmd_keys::cubeSubdivWidth, cubeDataSubdivWidth},
            std::pair{cmd_keys::cubeSubdivHeight, cubeDataSubdivHeight},
            std::pair{cmd_keys::cubeSubdivDepth, cubeDataSubdivDepth},
        }
    );

    settingsWindow.m_callCmd.SetFunc(
        &settingsWindow, 
        &SettingsWindow::CreateCylinder, 
        std::vector{ 
            std::pair{cmd_keys::cylinder, createCylinder},
            std::pair{cmd_keys::cylinderRadius, cylinderDataRadius},
            std::pair{cmd_keys::cylinderHeight, cylinderDataHeight},
            std::pair{cmd_keys::cylinderSubdivAxis, cylinderDataSubdivAxis},
            std::pair{cmd_keys::cylinderSubdivHeight, cylinderDataSubdivHeight},
            std::pair{cmd_keys::cylinderSubdivCaps, cylinderDataSubdivCaps}
        }
    );

    settingsWindow.m_callCmd.SetFunc(
        &settingsWindow,
        &SettingsWindow::CreateCone, 
        std::vector{ 
            std::pair{cmd_keys::cone, createCone},
            std::pair{cmd_keys::coneRadius, coneDataRadius},
            std::pair{cmd_keys::coneHeight, coneDataHeight},
            std::pair{cmd_keys::coneSubdivAxis, coneDataSubdivAxis},
            std::pair{cmd_keys::coneSubdivHeight, coneDataSubdivHeight},
            std::pair{cmd_keys::coneSubdivCaps, coneDataSubdivCaps}
        }
    );

    settingsWindow.m_callCmd.SetFunc(
        &settingsWindow,
        &SettingsWindow::CreateTorus, 
        std::vector{ 
            std::pair{cmd_keys::torus, createTorus},
            std::pair{cmd_keys::torusRadius, torusDataRadius},
            std::pair{cmd_keys::torusSectionRadius, torusDataSectionRadius},
            std::pair{cmd_keys::torusSubdivAxis, torusDataSubdivAxis},
            std::pair{cmd_keys::torusSubdivHeight, torusDataSubdivHeight}
        }
    );

    settingsWindow.m_callCmd.SetFunc(
        &settingsWindow, 
        &SettingsWindow::CreatePlane, 
        std::vector{ 
            std::pair{cmd_keys::plane, createPlane},
            std::pair{cmd_keys::planeWidth, planeDataWidth},
            std::pair{cmd_keys::planeDepth, planeDataDepth},
            std::pair{cmd_keys::planeSubdivWidth, planeDataSubdivWidth},
            std::pair{cmd_keys::planeSubdivDepth, planeDataSubdivDepth}
        }
    );

    // Commands for calling windows with preliminary object settings.
    CallWindow createSphereWnd = settingsWindow.m_callWnd.SetFunc(&settingsWindow, &SettingsWindow::CreateSphere);
    CallWindow createGeoSphereWnd = settingsWindow.m_callWnd.SetFunc(&settingsWindow, &SettingsWindow::CreateGeoSphere);
    CallWindow createCubeWnd = settingsWindow.m_callWnd.SetFunc(&settingsWindow, &SettingsWindow::CreateCube);
    CallWindow createCylinderWnd = settingsWindow.m_callWnd.SetFunc(&settingsWindow, &SettingsWindow::CreateCylinder);
    CallWindow createConeWnd = settingsWindow.m_callWnd.SetFunc(&settingsWindow, &SettingsWindow::CreateCone);
    CallWindow createTorusWnd = settingsWindow.m_callWnd.SetFunc(&settingsWindow, &SettingsWindow::CreateTorus);
    CallWindow createPlaneWnd = settingsWindow.m_callWnd.SetFunc(&settingsWindow, &SettingsWindow::CreatePlane);

    
    PopUpWindow popUp;

    // Linking commands and buttons.
    popUp.m_callCmd.SetFunc(
        &popUp, 
        &PopUpWindow::CreatePolyPrimitives, 
        std::vector{ 
            std::pair{cmd_keys::sphere, createSphere},
            std::pair{cmd_keys::geoSphere, createGeoSphere},
            std::pair{cmd_keys::cube, createCube},
            std::pair{cmd_keys::cylinder, createCylinder},
            std::pair{cmd_keys::cone, createCone},
            std::pair{cmd_keys::torus, createTorus},
            std::pair{cmd_keys::plane, createPlane}
        }
    );

    CallWindow mainCreatePolyPrimitives = popUp.m_callWnd.SetFunc(
        &popUp,
        &PopUpWindow::CreatePolyPrimitives,
        std::vector{
            std::pair{wnd_keys::sphere, createSphereWnd},
            std::pair{wnd_keys::geoSphere, createGeoSphereWnd},
            std::pair{wnd_keys::cube, createCubeWnd},
            std::pair{wnd_keys::cylinder, createCylinderWnd},
            std::pair{wnd_keys::cone, createConeWnd},
            std::pair{wnd_keys::torus, createTorusWnd},
            std::pair{wnd_keys::plane, createPlaneWnd}
        }
    );

    // Linking pop-up windows to each other.

    CallWindow mainCreateLights = popUp.m_callWnd.SetFunc(&popUp, &PopUpWindow::CreateLights);
    CallWindow mainCreateCamera = popUp.m_callWnd.SetFunc(&popUp, &PopUpWindow::CreateCamera);

    CallWindow mainPopUpFile = popUp.m_callWnd.SetFunc(&popUp, &PopUpWindow::MainTopBarFile);
    CallWindow mainPopUpEdit = popUp.m_callWnd.SetFunc(&popUp, &PopUpWindow::MainTopBarEdit);
    CallWindow mainPopUpCreate = popUp.m_callWnd.SetFunc(
        &popUp, 
        &PopUpWindow::MainTopBarCreate, 
        std::vector{
            std::pair{wnd_keys::polyPrimitives, mainCreatePolyPrimitives},
            std::pair{wnd_keys::lights, mainCreateLights},
            std::pair{wnd_keys::cameras, mainCreateCamera}
        }
    );
    CallWindow mainSelectMore2 = popUp.m_callWnd.SetFunc(&popUp, &PopUpWindow::More2);
    CallWindow mainSelectMore = popUp.m_callWnd.SetFunc(
        &popUp,
        &PopUpWindow::More,
        std::vector{
            std::pair{wnd_keys::testMore2, mainSelectMore2}
        }
    );
    CallWindow mainSelectAllType = popUp.m_callWnd.SetFunc(&popUp, &PopUpWindow::SelectAllType);
    CallWindow mainSelectAllComponents = popUp.m_callWnd.SetFunc(
        &popUp, 
        &PopUpWindow::SelectAllComponents,
        std::vector{
            std::pair{wnd_keys::testMore1, mainSelectMore}
        }
    );
    CallWindow mainPopUpSelect = popUp.m_callWnd.SetFunc(
        &popUp,
        &PopUpWindow::MainTopBarSelect,
        std::vector{
            std::pair{wnd_keys::allByType, mainSelectAllType},
            std::pair{wnd_keys::components, mainSelectAllComponents}
        }
    );    
    CallWindow mainHelpFeedback = popUp.m_callWnd.SetFunc(&popUp, &PopUpWindow::HelpFeedback);

    CallWindow createAboutWnd = settingsWindow.m_callWnd.SetFunc(&settingsWindow, &SettingsWindow::CreateAbout);

    CallWindow mainPopUpHelp = popUp.m_callWnd.SetFunc(
        &popUp,
        &PopUpWindow::MainTopBarHelp,
        std::vector{
            std::pair{wnd_keys::feedback, mainHelpFeedback},
            std::pair{wnd_keys::about, createAboutWnd}
        }
    );
    const std::initializer_list<std::tuple<std::wstring, UI::CallWindow>>& noFrameButtons
    {
        {L"File", mainPopUpFile},
        {L"Edit", mainPopUpEdit},
        {L"Create", mainPopUpCreate},
        {L"Select", mainPopUpSelect},
        {L"Help", mainPopUpHelp}
    };

    // Create the main application window.

    HWND mainWindow = LisaGui::CreateSimpleWindow(m_pD11Device, m_pCaching, nullptr, hInstance, m_class, m_title,
        m_icon, m_width, m_height,
        ui_draw::button::minimize | ui_draw::button::restore | ui_draw::button::close, ui_draw::window::simple, noFrameButtons, false);
    
    // Set main icon.
    HICON appIcon = LoadIconW(hInstance, (LPCWSTR)MAIN_ICON);
    SendMessageW(mainWindow, WM_SETICON, 1, (LPARAM)appIcon);


    // Create main substrate.
    const ElementConfig<WindowConfig> mainSubstrateConfig{
        .hInstance = hInstance,
        .ClassName = L"mainSubstrate",
        .Rect = {
            gNonClientAreaSize, 
            gNonClientAreaSize + gMainMenuBarHeight, 
            m_width, 
            m_height - (gMainMenuBarHeight * 2)
    },
        .Parent = mainWindow,
        .Flags = {
            .Type = ui_type::inbuilt,
            .Modes = ui_modes::customposition,
            .Draw = ui_draw::window::inbuilt,
            .Transform = ui_transform::stretching_x | ui_transform::stretching_y | ui_transform::restore_lx | ui_transform::restore_ty
    },
    };
    HWND mainSubstrate = Element<UI::Window>::Create(m_pD11Device, m_pCaching, mainSubstrateConfig);


    // Create left bar.
    RECT mainSubstrateRect; GetClientRect(mainSubstrate, &mainSubstrateRect);

    RECT leftBarRect{ 0, 0, 40, mainSubstrateRect.bottom };
    LisaGui::CreateLeftButtonBar(m_pD11Device, m_pCaching, mainSubstrate, hInstance, L"leftBar", leftBarRect);

    // Create channel box / layer editor

    RECT channelBoxRect{ mainSubstrateRect.right - 280, 0, 280, mainSubstrateRect.bottom };
    LisaGui::CreateChannelBoxLayerEditor(m_pD11Device, m_pCaching, mainSubstrate, hInstance, L"channelBox", channelBoxRect);

    // Create main separators
    const RECT substrateRect1{ leftBarRect.right, 0, channelBoxRect.left - leftBarRect.right, mainSubstrateRect.bottom };

    /*LONG difference{substrateRect1.right - substrateRect1.left};
    std::vector<UI::SeparatorConfig> separatorConfig1
    {
        {.Indent = difference / 2, .FirstLimitation = 200, .SecondLimitation = 200, .SplitX = true},
    };

    std::vector<HWND> substrate1 = LisaGui::CreateWindowSeparators(
        m_pD11Device, m_pCaching, mainSubstrate, hInstance, L"mainSeparator1", { substrateRect1 }, separatorConfig1);


    RECT rc;
    GetClientRect(substrate1[0], &rc);
    std::vector<UI::SeparatorConfig> separatorConfig2{
        {.Indent = rc.bottom / 2, .FirstLimitation = 100, .SecondLimitation = 100, .SplitY = true} };

    std::vector<HWND> substrate2 = LisaGui::CreateWindowSeparators(
        m_pD11Device, m_pCaching, substrate1[0], hInstance, L"mainSeparator2", { rc }, separatorConfig2);


    GetClientRect(substrate1[1], &rc);
    std::vector<UI::SeparatorConfig> separatorConfig3{
        {.Indent = rc.bottom / 2, .FirstLimitation = 100, .SecondLimitation = 100, .SplitY = true} };

    std::vector<HWND> substrate3 = LisaGui::CreateWindowSeparators(
        m_pD11Device, m_pCaching, substrate1[1], hInstance, L"mainSeparator3", { rc }, separatorConfig3);*/






    

    // Create perspective

    //GetClientRect(substrate3[0], &rc);
    m_perspective = m_perspective->Perspective(
        nullptr, 
        substrateRect1/*{ 0, 0, rc.right, rc.bottom }*/,
        L"perspective",
        ui_type::inbuilt,
        ui_modes::customposition,
        ui_transform::stretching_x | ui_transform::stretching_y, 
        mainSubstrate/*substrate3[0]*/
    );

    commandFunc.SetHwnd(m_perspective->GetHwnd());

    LisaGui::SettingData(mainSubstrate);
    //LisaGui::SettingData(substrate1[0]);
    //LisaGui::SettingData(substrate1[1]);

    //LisaGui::SettingData(substrate2[0]);
    //LisaGui::SettingData(substrate2[1]);
    //LisaGui::SettingData(substrate3[0]);
    //LisaGui::SettingData(substrate3[1]);

    //LisaGui::SettingData(substrate3[0]);

    LisaGui::SettingData(mainWindow);

    LisaGui::RunMessageLoop();
}