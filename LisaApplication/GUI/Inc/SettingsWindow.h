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

#ifndef SETTINGS_WINDOW_CLASS_H
#define SETTINGS_WINDOW_CLASS_H

#include "LisaGui.h"
#include "KeysBindingMap.h"

namespace LisaApp
{
	class SettingsWindow
	{
	public:
		SettingsWindow() = default;
		SettingsWindow(
			LONG width,
			LONG height,
			std::wstring windowClass,
			std::wstring mainIcon
		) :
			m_windowWidth{ width },
			m_windowHeight{ height },
			m_windowClass{ windowClass },
			m_mainIcon{ mainIcon }
		{
		}

		~SettingsWindow() = default;

		// Delete the previous contents of the window.
		void ClearingPreviousContents(HWND currentWindow, HWND plane)
		{
			DestroyWindow(m_plane);
			UpdateWindow(currentWindow);
			m_plane = nullptr;
			m_plane = plane;
		};

		void CreateLabelText(
			const std::shared_ptr<UI::D11DeviceResources>& pDevice, 
			const std::shared_ptr<UI::Caching>& pCaching,
			const std::vector<UI::FontConfig>& titles,
			HWND parent, 
			HINSTANCE hInstance
		) const;

		void CreateSimpleField(
			const std::shared_ptr<UI::D11DeviceResources>& pDevice,
			const std::shared_ptr<UI::Caching>& pCaching,
			const std::vector<UI::FieldConfig>& fieldConfig,
			HWND parent,
			HINSTANCE hInstance
		) const;

		HWND CreateSphere(const std::shared_ptr<UI::D11DeviceResources>& pDevice, const std::shared_ptr<UI::Caching>& pCaching, HWND root, HINSTANCE hInstance);
		HWND CreateGeoSphere(const std::shared_ptr<UI::D11DeviceResources>& pDevice, const std::shared_ptr<UI::Caching>& pCaching, HWND root, HINSTANCE hInstance);
		HWND CreateCube(const std::shared_ptr<UI::D11DeviceResources>& pDevice, const std::shared_ptr<UI::Caching>& pCaching, HWND root, HINSTANCE hInstance);
		HWND CreateCylinder(const std::shared_ptr<UI::D11DeviceResources>& pDevice, const std::shared_ptr<UI::Caching>& pCaching, HWND root, HINSTANCE hInstance);
		HWND CreateCone(const std::shared_ptr<UI::D11DeviceResources>& pDevice, const std::shared_ptr<UI::Caching>& pCaching, HWND root, HINSTANCE hInstance);
		HWND CreateTorus(const std::shared_ptr<UI::D11DeviceResources>& pDevice, const std::shared_ptr<UI::Caching>& pCaching, HWND root, HINSTANCE hInstance);
		HWND CreatePlane(const std::shared_ptr<UI::D11DeviceResources>& pDevice, const std::shared_ptr<UI::Caching>& pCaching, HWND root, HINSTANCE hInstance);

		HWND CreateAbout(const std::shared_ptr<UI::D11DeviceResources>& pDevice, const std::shared_ptr<UI::Caching>& pCaching, HWND root, HINSTANCE hInstance);

		LisaGui::CallableFunctions<wnd_keys, UI::CallWindow> m_callWnd{};
		LisaGui::CallableFunctions<cmd_keys, UI::CallCommand> m_callCmd{};

	private:
		LONG m_windowWidth{};
		LONG m_windowHeight{};
		std::wstring m_windowClass{};
		std::wstring m_windowClassBuffer{};
		std::wstring m_mainIcon{};

		HWND m_plane{ nullptr };

		LONG m_lenghtTextField{ 120 };
		LONG m_lenghtFloatField{ 100 };
		LONG m_topIndent{ 20 };
	};
}

#endif // !SETTINGS_WINDOWS_CLASS_H