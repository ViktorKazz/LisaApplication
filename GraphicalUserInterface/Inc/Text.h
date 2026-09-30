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

#ifndef TEXT_CLASS_H
#define TEXT_CLASS_H

#include "WndInitialize.h"
#include "WindowTransformation.h"

namespace UI
{
	struct TextConfig
	{
		HWND Root{ nullptr };
		varimtx ImgTxt;
	};

	class Text : public WndInitialize, public WindowTransformation
	{
	public:
		using WndInitialize::WndInitialize;
		virtual ~Text() = default;

		// Accessors.

		template<typename T>
		void GetConfigure(const T& t)
		{
			m_parent = t.Parent;
			m_type = t.Flags.Type;
			m_modes = t.Flags.Modes;
			m_transform = t.Flags.Transform;

			if (std::holds_alternative<ui_draw::text>(t.Flags.Draw))
				m_draw = std::get<ui_draw::text>(t.Flags.Draw);

			m_config = t.Extra;
		};

		LRESULT CALLBACK MessageHandled(UINT message, WPARAM wParam, LPARAM lParam);

		void DrawingSimpleText(
			const Microsoft::WRL::ComPtr<ID2D1DeviceContext6>& pDeviceContext,
			const Microsoft::WRL::ComPtr<IDWriteFactory7>& pDWriteFactory,
			const varimtx& inputText,
			const D2D1_SIZE_F& size,
			size_t index
		);

		void DrawingLabelText(
			const Microsoft::WRL::ComPtr<ID2D1RenderTarget>& pRenderTarget,
			const varimtx& inputText,
			const D2D1_SIZE_F& size,
			size_t index
		);

		HRESULT Draw();

	private:
		HelperWTools* m_tools{};

		bool m_select{};
		bool m_pressing{};

		HWND m_parent{ nullptr };

		ui_type m_type;
		ui_modes m_modes;
		ui_draw::text m_draw;
		ui_transform m_transform;
		ui_command m_command;

		TextConfig m_config{};

		// To flipping the text.
		// May be less than zero, hence signed type.
		INT m_index{};
		// 
		INT m_flipPlaceSelect{};
		size_t m_numberOfLayers{};

	};
}

#endif // !TEXT_CLASS_H
