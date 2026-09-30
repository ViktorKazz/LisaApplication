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

#ifndef IMAGE_CLASS_H
#define IMAGE_CLASS_H

#include "WndInitialize.h"
#include "WindowTransformation.h"

namespace UI
{
	struct ImageConfig
	{
		HWND Root{ nullptr };
		varimtx ImgTxt;
	};

	class Image : public WndInitialize, public WindowTransformation
	{
	public:
		using WndInitialize::WndInitialize;
		virtual ~Image() = default;

		// Accessors.

		template<typename T>
		void GetConfigure(const T& t)
		{
			m_parent = t.Parent;
			m_type = t.Flags.Type;
			m_modes = t.Flags.Modes;
			m_transform = t.Flags.Transform;

			if (std::holds_alternative<ui_draw::image>(t.Flags.Draw))
				m_draw = std::get<ui_draw::image>(t.Flags.Draw);

			m_config = t.Extra;
		};

		LRESULT CALLBACK MessageHandled(UINT message, WPARAM wParam, LPARAM lParam);

		void DrawingImage(
			const Microsoft::WRL::ComPtr<ID2D1DeviceContext6>& pDeviceContext,
			const Microsoft::WRL::ComPtr<IWICImagingFactory2>& pWICFactory,
			const varimtx& inputImages,
			const D2D1_SIZE_F& size,
			size_t index
		);

		HRESULT Draw();

	private:
		HelperWTools* m_tools{};

		LONG m_parentWindowHeight{};
		LONG m_parentWindowWidth{};

		bool m_select{};
		bool m_pressing{};

		HWND m_parent{ nullptr };
		
		ui_type m_type;
		ui_modes m_modes;
		ui_draw::image m_draw;
		ui_transform m_transform;
		
		ImageConfig m_config{};

		// To flipping the image.
		// May be less than zero, hence signed type.
		INT m_index{};
		// 
		INT m_flipPlaceSelect{};
		size_t m_numberOfLayers{};

		Microsoft::WRL::ComPtr<ID2D1Bitmap> m_pBitmap{ nullptr };
		Microsoft::WRL::ComPtr<ID2D1Effect> m_translationEffect{ nullptr };
		Microsoft::WRL::ComPtr<ID2D1Effect> m_rotationEffect{ nullptr };
		Microsoft::WRL::ComPtr<ID2D1Effect> m_scaleEffect{ nullptr };
	};
}

#endif // !IMAGE_CLASS_H
