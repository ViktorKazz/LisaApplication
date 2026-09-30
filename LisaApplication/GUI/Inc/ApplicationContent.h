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

#pragma once

#include "..//GUI/Inc/PopUpWindow/PopUpWindow.h"

#include "LisaGui.h"
#include "Viewport.h"

namespace LisaApp
{
	class AppContent
	{
	public:
		AppContent() = default;
		AppContent(
			std::wstring wclass,
			std::wstring title,
			std::wstring icon,
			LONG width,
			LONG height
		) : m_class{ wclass }, 
			m_title{ title }, 
			m_icon{ icon }, 
			m_width{ width }, 
			m_height{ height } 
		{};

		~AppContent() = default;

		// Accessors.

		bool Initialize()
		{
			bool init{};

			m_pD11Device = LisaGui::CreateD3D11Device();

			if (m_pD11Device){
				init = true;
			}
			else{
				throw std::runtime_error("D3D11Device resources not initialized.");
			}

			if (init){
				m_pCaching = LisaGui::CreateCachingResources(m_pD11Device);
			}
			else{
				init = false;
				throw std::runtime_error("Caching resources not initialized."); 
			}

			return init;
		};

		void Content(HINSTANCE hInstance);

	private:
		std::wstring m_class{};
		std::wstring m_title{};
		std::wstring m_icon{};

		LONG m_width{};
		LONG m_height{};

		std::shared_ptr<UI::D11DeviceResources> m_pD11Device = std::make_shared<UI::D11DeviceResources>();
		std::shared_ptr<UI::Caching> m_pCaching = std::make_shared<UI::Caching>();

		std::unique_ptr<Viewport> m_perspective = std::make_unique<Viewport>();
		HWND m_perspectiveHwnd{};
	};

}