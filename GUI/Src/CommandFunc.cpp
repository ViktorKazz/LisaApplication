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

#include "..//GUI/Inc/CommandFunc.h"
#include "Globals.h"
#include "DefaultElementsData.h"

extern struct LisaApp::Default::SphereData gSphereData;
extern struct LisaApp::Default::GeoSphereData gGeoSphereData;
extern struct LisaApp::Default::CubeData gCubeData;
extern struct LisaApp::Default::CylinderData gCylinderData;
extern struct LisaApp::Default::ConeData gConeData;
extern struct LisaApp::Default::TorusData gTorusData;
extern struct LisaApp::Default::PlaneData gPlaneData;


void CommandFunc::CreateSphere(const std::wstring&) const
{
	SendMessageW(m_hwnd, WM_COMMAND, UI::CREATE_PRIMITIVES, (LPARAM)(UINT(LisaApp::Global::SPHERE)));
	SetFocus(m_hwnd);
}

void CommandFunc::CreateGeoSphere(const std::wstring&) const
{
	SendMessageW(m_hwnd, WM_COMMAND, UI::CREATE_PRIMITIVES, (LPARAM)(UINT(LisaApp::Global::GEO_SPHERE)));
	SetFocus(m_hwnd);
}

void CommandFunc::CreateCube(const std::wstring&) const
{
	SendMessageW(m_hwnd, WM_COMMAND, UI::CREATE_PRIMITIVES, (LPARAM)(UINT(LisaApp::Global::CUBE)));
	SetFocus(m_hwnd);
}

void CommandFunc::CreateCylinder(const std::wstring&) const
{
	SendMessageW(m_hwnd, WM_COMMAND, UI::CREATE_PRIMITIVES, (LPARAM)(UINT(LisaApp::Global::CYLINDER)));
	SetFocus(m_hwnd);
}

void CommandFunc::CreateCone(const std::wstring&) const
{
	SendMessageW(m_hwnd, WM_COMMAND, UI::CREATE_PRIMITIVES, (LPARAM)(UINT(LisaApp::Global::CONE)));
	SetFocus(m_hwnd);
}

void CommandFunc::CreateTorus(const std::wstring&) const
{
	SendMessageW(m_hwnd, WM_COMMAND, UI::CREATE_PRIMITIVES, (LPARAM)(UINT(LisaApp::Global::TORUS)));
	SetFocus(m_hwnd);
}

void CommandFunc::CreatePlane(const std::wstring&) const
{
	SendMessageW(m_hwnd, WM_COMMAND, UI::CREATE_PRIMITIVES, (LPARAM)(UINT(LisaApp::Global::PLANE)));
	SetFocus(m_hwnd);
}

void CommandFunc::SetSphereDataRadius(const std::wstring& txt)
{
	gSphereData.Radius = m_tools.WstringToFloat(txt);
}

void CommandFunc::SetSphereDataSubdivAxis(const std::wstring& txt)
{
	gSphereData.SubdivisionsAxis = std::stoul(txt);
}

void CommandFunc::SetSphereDataSubdivHeight(const std::wstring& txt)
{
	gSphereData.SubdivisionsHeight = std::stoul(txt);
}

void CommandFunc::SetGeoSphereDataRadius(const std::wstring& txt)
{
	gGeoSphereData.Radius = m_tools.WstringToFloat(txt);
}

void CommandFunc::SetGeoSphereDataSubdiv(const std::wstring& txt)
{
	gGeoSphereData.Subdivisions = std::stoul(txt);
}

void CommandFunc::SetCubeDataWidth(const std::wstring& txt)
{
	gCubeData.Width = m_tools.WstringToFloat(txt);
}

void CommandFunc::SetCubeDataHeight(const std::wstring& txt)
{
	gCubeData.Height = m_tools.WstringToFloat(txt);
}

void CommandFunc::SetCubeDataDepth(const std::wstring& txt)
{
	gCubeData.Depth = m_tools.WstringToFloat(txt);
}

void CommandFunc::SetCubeDataSubdivWidth(const std::wstring& txt)
{
	gCubeData.SubdivisionsWidth = std::stoul(txt);
}

void CommandFunc::SetCubeDataSubdivHeight(const std::wstring& txt)
{
	gCubeData.SubdivisionsHeight = std::stoul(txt);
}

void CommandFunc::SetCubeDataSubdivDepth(const std::wstring& txt)
{
	gCubeData.SubdivisionsDepth = std::stoul(txt);
}

void CommandFunc::SetCylinderDataRadius(const std::wstring& txt)
{
	gCylinderData.Radius = m_tools.WstringToFloat(txt);
}

void CommandFunc::SetCylinderDataHeight(const std::wstring& txt)
{
	gCylinderData.Height = m_tools.WstringToFloat(txt);
}

void CommandFunc::SetCylinderDataSubdivAxis(const std::wstring& txt)
{
	gCylinderData.SubdivisionsAxis = std::stoul(txt);
}

void CommandFunc::SetCylinderDataSubdivHeight(const std::wstring& txt)
{
	gCylinderData.SubdivisionsHeight = std::stoul(txt);
}

void CommandFunc::SetCylinderDataSubdivCaps(const std::wstring& txt)
{
	gCylinderData.SubdivisionsCaps = std::stoul(txt);
}

void CommandFunc::SetConeDataRadius(const std::wstring& txt)
{
	gConeData.Radius = m_tools.WstringToFloat(txt);
}

void CommandFunc::SetConeDataHeight(const std::wstring& txt)
{
	gConeData.Height = m_tools.WstringToFloat(txt);
}

void CommandFunc::SetConeDataSubdivAxis(const std::wstring& txt)
{
	gConeData.SubdivisionsAxis = std::stoul(txt);
}

void CommandFunc::SetConeDataSubdivHeight(const std::wstring& txt)
{
	gConeData.SubdivisionsHeight = std::stoul(txt);
}

void CommandFunc::SetConeDataSubdivCaps(const std::wstring& txt)
{
	gConeData.SubdivisionsCaps = std::stoul(txt);
}

void CommandFunc::SetTorusDataRadius(const std::wstring& txt)
{
	gTorusData.Radius = m_tools.WstringToFloat(txt);
}

void CommandFunc::SetTorusDataSectionRadius(const std::wstring& txt)
{
	gTorusData.SectionRadius = m_tools.WstringToFloat(txt);
}

void CommandFunc::SetTorusDataSubdivAxis(const std::wstring& txt)
{
	gTorusData.SubdivisionsAxis = std::stoul(txt);
}

void CommandFunc::SetTorusDataSubdivHeight(const std::wstring& txt)
{
	gTorusData.SubdivisionsHeight = std::stoul(txt);
}

void CommandFunc::SetPlaneDataWidth(const std::wstring& txt)
{
	gPlaneData.Width = m_tools.WstringToFloat(txt);
}

void CommandFunc::SetPlaneDataDepth(const std::wstring& txt)
{
	gPlaneData.Depth = m_tools.WstringToFloat(txt);
}

void CommandFunc::SetPlaneDataSubdivWidth(const std::wstring& txt)
{
	gPlaneData.SubdivisionsWidth = std::stoul(txt);
}

void CommandFunc::SetPlaneDataSubdivDepth(const std::wstring& txt)
{
	gPlaneData.SubdivisionsDepth = std::stoul(txt);
}
