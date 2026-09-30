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

#ifndef COMMAND_FUNC_CLASS_H
#define COMMAND_FUNC_CLASS_H

#include "LisaGui.h"

class CommandFunc
{
public:
	CommandFunc() = default;
	~CommandFunc() = default;

	// Accessors.

	void SetHwnd(HWND hwnd) { m_hwnd = hwnd; };

	void CreateSphere(const std::wstring&) const;
	void CreateGeoSphere(const std::wstring&) const;
	void CreateCube(const std::wstring&) const;
	void CreateCylinder(const std::wstring&) const;
	void CreateCone(const std::wstring&) const;
	void CreateTorus(const std::wstring&) const;
	void CreatePlane(const std::wstring&) const;

	void SetSphereDataRadius(const std::wstring& txt);
	void SetSphereDataSubdivAxis(const std::wstring& txt);
	void SetSphereDataSubdivHeight(const std::wstring& txt);

	void SetGeoSphereDataRadius(const std::wstring& txt);
	void SetGeoSphereDataSubdiv(const std::wstring& txt);

	void SetCubeDataWidth(const std::wstring& txt);
	void SetCubeDataHeight(const std::wstring& txt);
	void SetCubeDataDepth(const std::wstring& txt);
	void SetCubeDataSubdivWidth(const std::wstring& txt);
	void SetCubeDataSubdivHeight(const std::wstring& txt);
	void SetCubeDataSubdivDepth(const std::wstring& txt);

	void SetCylinderDataRadius(const std::wstring& txt);
	void SetCylinderDataHeight(const std::wstring& txt);
	void SetCylinderDataSubdivAxis(const std::wstring& txt);
	void SetCylinderDataSubdivHeight(const std::wstring& txt);
	void SetCylinderDataSubdivCaps(const std::wstring& txt);

	void SetConeDataRadius(const std::wstring& txt);
	void SetConeDataHeight(const std::wstring& txt);
	void SetConeDataSubdivAxis(const std::wstring& txt);
	void SetConeDataSubdivHeight(const std::wstring& txt);
	void SetConeDataSubdivCaps(const std::wstring& txt);

	void SetTorusDataRadius(const std::wstring& txt);
	void SetTorusDataSectionRadius(const std::wstring& txt);
	void SetTorusDataSubdivAxis(const std::wstring& txt);
	void SetTorusDataSubdivHeight(const std::wstring& txt);

	void SetPlaneDataWidth(const std::wstring& txt);
	void SetPlaneDataDepth(const std::wstring& txt);
	void SetPlaneDataSubdivWidth(const std::wstring& txt);
	void SetPlaneDataSubdivDepth(const std::wstring& txt);

	LisaGui::CallableFunctions<UINT, UI::CallCommand> m_callCmd{};

private:
	HWND m_hwnd{};
	HelperWTools m_tools;
};

#endif // !COMMAND_FUNC_CLASS_H
