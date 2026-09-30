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

#ifndef FLAGS_UI_H
#define FLAGS_UI_H

#include <cstdint>
#include <type_traits>

namespace UI
{
	enum class ui_type : std::uint16_t
	{
		none = 0x000000,
		simple = 0x000001,
		popUp = 0x000002,
		inbuilt = 0x000004,
		message = 0x000008,
		separator = 0x000010,
	};

	enum class ui_modes : std::uint16_t
	{
		none = 0x000000,
		centerscreen = 0x000001,
		customposition = 0x000002,
		pup_simple = 0x000004,
		pup_transition = 0x000008,
		mrmc1 = 0x000010,
		mrmc2 = 0x000020,
		mrmc3 = 0x000040,
		topbar = 0x000080,
		bottombar_simple = 0x000100,
		bottombar_inbuilt = 0x000200,
		dblclks = 0x000400,
		hide = 0x000800,
	};

	enum class ui_transform : std::uint16_t
	{
		none = 0x000000,
		stretching_x = 0x000001,
	    stretching_y = 0x000002,
	    restore_lx = 0x000004,
	    restore_rx = 0x000008,
	    restore_ty = 0x000010,
	    restore_by = 0x000020,
	    modifiable_x = 0x000040,
	    pup_modifiable = 0x000080,
	    split_x = 0x000100,
	    split_y = 0x000200,
	};

	enum class ui_command : std::uint16_t
	{
		none = 0x000000,
        up = 0x000001,
	    down = 0x000002,
	    close = 0x000004,
	    restore = 0x000008,
	    minimize = 0x000010,
	};

	class ui_draw
	{
	public:
		enum class window : std::uint16_t
		{
			none = 0x000000,
			simple = 0x000001,
			popUp = 0x000002,
			inbuilt = 0x000004,
			inbuilt_in = 0x00008, // inbuilt internal color
			inbuilt_border_l = 0x000010,
			inbuilt_border_t = 0x000020,
			inbuilt_border_r = 0x000040,
			inbuilt_border_b = 0x000080,
		};

		enum class button : std::uint16_t
		{
			none = 0x000000,
			close = 0x000001,
			restore = 0x000002,
			minimize = 0x000004,
			noframe = 0x000008,
			frame = 0x000010,
			pup_simple = 0x000020,
			pup_transition = 0x000040,
			pup_dual = 0x000080
		};

		enum class separator : std::uint16_t
		{
			none = 0x000000,
			simple = 0x000001,
			popUp = 0x000002,
			window = 0x000004
		};

		enum class image : std::uint16_t
		{
			none = 0x000000,
			simple = 0x000001
		};

		enum class text : std::uint16_t
		{
			none = 0x000000,
			simple = 0x000001,
			label = 0x000002
		};

		enum class field : std::uint16_t
		{
			none = 0x000000,
			simple = 0x000001,
			hide = 0x000002,
			dubleclick = 0x000004,
		};
	};


	template <typename T>
	concept Enum = std::disjunction_v<
		std::is_same<T, ui_type>,
		std::is_same<T, ui_modes>,
		std::is_same<T, ui_transform>,
		std::is_same<T, ui_command>,
		std::is_same<T, ui_draw::window>,
		std::is_same<T, ui_draw::button>,
		std::is_same<T, ui_draw::separator>,
		std::is_same<T, ui_draw::image>,
		std::is_same<T, ui_draw::text>,
		std::is_same<T, ui_draw::field>
	>;

	template <Enum E>
	inline E operator|(const E& lhs, const E& rhs) {
		return static_cast<E>(static_cast<std::uint16_t>(lhs) | static_cast<std::uint16_t>(rhs));
	}

	template <Enum E>
	inline bool operator&(const E& flags, const E& value) {
		return (static_cast<std::uint16_t>(flags) & static_cast<std::uint16_t>(value));
	}
}

#endif // !FLAGS_UI_H