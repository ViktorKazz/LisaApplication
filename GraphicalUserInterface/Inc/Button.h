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

#ifndef BUTTON_CLASS_H
#define BUTTON_CLASS_H

#include "WndInitialize.h"
#include "WindowTransformation.h"

namespace UI
{
	struct ButtonConfig
	{
		HWND Root{ nullptr };
		varimtx ImgTxt;		
		std::wstring MiddleName{};
		MessageConfig Message;
		CallWindow Window;
		CallCommand Command;
	};

	class Button : public WndInitialize, public WindowTransformation
	{
	public:
		using WndInitialize::WndInitialize;
		~Button() {};

		// Accessors.

		template<typename T>
		void GetConfigure(const T& t)
		{
			m_parent = t.Parent;
			m_type = t.Flags.Type;
			m_modes = t.Flags.Modes;
			m_transform = t.Flags.Transform;
			m_command = t.Flags.Command;

			if (std::holds_alternative<ui_draw::button>(t.Flags.Draw))
				m_draw = std::get<ui_draw::button>(t.Flags.Draw);

			m_config = t.Extra;
		};

		LRESULT SimpleCommand(HWND hwnd, LPARAM lParam, UI::ui_command cmd);
		bool ChangeIcon();
		INT DualPopUpBt(HWND hwnd, LPARAM lParam, ui_draw::button draw);

		void OpenPopUpAndMove(HWND hwnd, HWND root, ui_draw::button draw);
		void ClosePopUpWindow(LPARAM lParam, UINT msg, bool selectReset);

		LRESULT CALLBACK MessageHandled(UINT message, WPARAM wParam, LPARAM lParam);

		void Close(const Microsoft::WRL::ComPtr<ID2D1DeviceContext6>& pDeviceContext,
			const D2D1_SIZE_F& size, UINT selection, UINT pressing
		);
		void MaximizeRestore(const Microsoft::WRL::ComPtr<ID2D1DeviceContext6>& pDeviceContext,
			const D2D1_SIZE_F& size, UINT selection, UINT pressing
		);
		void Minimize(const Microsoft::WRL::ComPtr<ID2D1DeviceContext6>& pDeviceContext,
			const D2D1_SIZE_F& size, UINT selection, UINT pressing
		);
		void Frame(const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget,
			const std::wstring& name, const D2D1_SIZE_F& size, UINT selection, UINT pressing
		);
		void NoFrame(const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget,
			const std::wstring& name, const D2D1_SIZE_F& size, UINT selection, UINT pressing
		);
		void PopUp(
			const Microsoft::WRL::ComPtr<ID2D1DeviceContext6>& pRenderTarget,
			const Microsoft::WRL::ComPtr<IWICImagingFactory2>& pWICFactory,
			const varimtx& imagePath,
			const D2D1_SIZE_F& size,
			UINT selection,
			UINT pressing,
			bool isTransition = false,
			bool isDual = false
		);

		HRESULT Draw();

	private:
		HelperWTools* m_tools{};
		std::vector<HWND> m_child{};

		bool m_select{};
		bool m_pressing{};
		bool m_changeIcon{};

		// To express the intent to close pop-up windows by clicking the NOFRAME button again.
		// And also to prevent conflicts in the methods for closing pop-up windows.
		bool m_intentionToClose{};		

		HWND m_callHwnd{};

		HWND m_parent{ nullptr };
		
		ui_type m_type;
		ui_modes m_modes;
		ui_draw::button m_draw;
		ui_transform m_transform;
		ui_command m_command;
		
		ButtonConfig m_config{};
	};
}

#endif // !BUTTON_CLASS_H