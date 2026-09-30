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

#ifndef LISA_GUI_CLASS_H
#define LISA_GUI_CLASS_H

#include "Elements.h"

namespace LisaGui
{
	using CallFunc = std::variant<UI::CallWindow, UI::CallCommand, std::pair<UI::CallWindow, UI::CallCommand>>;

	using PopUpConfig = std::tuple<
		std::wstring, std::wstring, 
		std::variant<UI::ui_draw::button, UI::ui_draw::separator>, 
		CallFunc
	>;

	template <typename T>
	T ReadFuncVar(const CallFunc& func) {

		// Get the type of the argument
		//using U = std::decay_t<decltype(type)>;
		T type{};

		if constexpr (std::is_same_v<T, UI::CallWindow>)
		{
			if (std::holds_alternative<UI::CallWindow>(func))
				type = std::get<UI::CallWindow>(func);
			if (std::holds_alternative<std::pair<UI::CallWindow, UI::CallCommand>>(func))
				type = std::get<std::pair<UI::CallWindow, UI::CallCommand>>(func).first;
		}
		else if constexpr (std::is_same_v<T, UI::CallCommand>)
		{
			if (std::holds_alternative<UI::CallCommand>(func))
				type = std::get<UI::CallCommand>(func);
			if (std::holds_alternative<std::pair<UI::CallWindow, UI::CallCommand>>(func))
				type = std::get<std::pair<UI::CallWindow, UI::CallCommand>>(func).second;
		}

		return type;
	}

	// Concept for checking type callability.
	template<typename T>
	concept CallableWindow = requires(T t) {
		// Option for window call function.
		{
			t(
				std::shared_ptr<UI::D11DeviceResources>(),
				std::shared_ptr<UI::Caching>(),
				HWND(),
				HINSTANCE()
			)
		} -> std::same_as<HWND>;
	} || requires(T t) {
		// Option for the command call function.
		{ t(std::wstring()) } -> std::same_as<void>;
	};

	// Ensures that Key supports comparison.
	// Checks that Meaning is a callable object

	// Class for storing callable functions.
	template<std::equality_comparable Key, CallableWindow Meaning>
	class CallableFunctions
	{
	public:
		CallableFunctions() = default;
		~CallableFunctions() = default;

		void SetMeaning(const Key& key, const Meaning& meaning)
		{
			// insert_or_assign instead of try_emplace to update values.
			m_unMap.insert_or_assign(key, meaning);
		};

		const Meaning& GetMeaning(const Key& key)
		{
			if (m_unMap.contains(key))
				return m_unMap[key];
			else
				throw std::out_of_range("Key not found");
		}


		// A function for linking functions to each other.
		template<typename Obj, typename Func, std::equality_comparable Key, CallableWindow Meaning>
		auto SetFunc(Obj* obj, Func&& destinationFunc, const std::vector<std::pair<Key, Meaning>>& range)
		{
			for (const auto& f : range)
				SetMeaning(f.first, f.second);

			auto func = std::forward<Func>(destinationFunc);

			// Preserving the value category (lvalue/rvalue) use std::forward.
			return [obj, func](auto&&... args)
				noexcept(
					noexcept((obj->*func)(std::forward<decltype(args)>(args)...))) // Automatic checking for exceptions
				-> decltype(auto) //Preserving the category of the return value.
				{
					return (obj->*func)(std::forward<decltype(args)>(args)...);
				};
		}

		template<typename Obj, typename Func>
		auto SetFunc(Obj* obj, Func&& destinationFunc)
		{
			auto func = std::forward<Func>(destinationFunc);
			// Preserving the value category (lvalue/rvalue) use std::forward.
			return [obj, func](auto&&... args)
				noexcept(
					noexcept((obj->*func)(std::forward<decltype(args)>(args)...))) // Automatic checking for exceptions
				-> decltype(auto) //Preserving the category of the return value.
				{
					return (obj->*func)(std::forward<decltype(args)>(args)...);
				};
		}

		template<typename Obj, typename Func, typename... Args>
			requires (sizeof...(Args) == 0 ||
		(std::ranges::input_range<Args> && ...)) 
			auto SetFuncA(Obj* obj, Func&& destinationFunc, Args&&... args)
		{
			
			if constexpr (sizeof...(Args) > 0) {
				auto process_range = [obj](const auto& range) {
					for (const auto& f : range) {
						/*obj->*/SetMeaning(f.first, f.second);
					}
					};

				(process_range(std::forward<Args>(args)), ...);
			}

			
			return [obj, func = std::forward<Func>(destinationFunc)](auto&&... params)
				noexcept(noexcept((obj->*func)(std::forward<decltype(params)>(params)...)))
				-> decltype(auto)
				{
					return (obj->*func)(std::forward<decltype(params)>(params)...);
				};
		}

	private:
		std::unordered_map<Key, Meaning> m_unMap{};
	};


	static std::shared_ptr<UI::D11DeviceResources> CreateD3D11Device()
	{
		std::shared_ptr<UI::D11DeviceResources> pD11Device = std::make_shared<UI::D11DeviceResources>();
		pD11Device->CreateDevice();

		return pD11Device;
	};

	static std::shared_ptr<UI::Caching> CreateCachingResources(const std::shared_ptr<UI::D11DeviceResources>& pDevice)
	{
		std::shared_ptr<UI::Caching> pCaching = std::make_shared<UI::Caching>();
		pCaching->VirtualKeyCodes();
		pCaching->Brushes(pDevice);
		pCaching->FlipGradientStopCollection(pDevice);
		pCaching->CreateTextFormat(pDevice->GetIDWriteFactory());
		pCaching->CreateIconBitmap(pDevice);

		return pCaching;
	};

	void CreateLabelText(
		const std::shared_ptr<UI::D11DeviceResources>& pDevice,
		const std::shared_ptr<UI::Caching>& pCaching,
		const std::vector<UI::FontConfig>& titles,
		HWND parent,
		HINSTANCE hInstance,
		const std::wstring& className,
		const RECT& rect
	);

	void CreateSimpleField(
		const std::shared_ptr<UI::D11DeviceResources>& pDevice,
		const std::shared_ptr<UI::Caching>& pCaching,
		const std::vector<UI::FieldConfig>& fieldConfig,
		HWND parent,
		HINSTANCE hInstance,
		const std::wstring& className,
		const RECT& rect
	);

	void CreateTopLeftButtonBar(
		_In_ const std::shared_ptr<UI::D11DeviceResources>& pDevice,
		_In_ const std::shared_ptr<UI::Caching>& pCaching,
		_In_ HWND parent,
		_In_opt_ HINSTANCE hInstance,
		_In_ const std::wstring& className,
		_In_ const std::initializer_list<std::tuple<std::wstring, UI::CallWindow>>& noFrameButtons
	);

	void CreateTopRightButtonBar(
		_In_ const std::shared_ptr<UI::D11DeviceResources>& pDevice,
		_In_ const std::shared_ptr<UI::Caching>& pCaching,
		_In_ HWND parent,
		_In_opt_ HINSTANCE hInstance,
		_In_ const std::wstring& className,
		_In_ UI::ui_draw::button button
	);

	HWND CreateLeftButtonBar(
		_In_ const std::shared_ptr<UI::D11DeviceResources>& pDevice,
		_In_ const std::shared_ptr<UI::Caching>& pCaching,
		_In_ HWND parent,
		_In_opt_ HINSTANCE hInstance,
		_In_ const std::wstring& className,
		_In_ const RECT& rect
	);

	void CreateBottomButtonBar(
		_In_ const std::shared_ptr<UI::D11DeviceResources>& pDevice,
		_In_ const std::shared_ptr<UI::Caching>& pCaching,
		_In_ HWND parent,
		_In_opt_ HINSTANCE hInstance,
		_In_ const std::wstring& className,
		_In_ const std::initializer_list<std::tuple<std::wstring, UI::ui_command, UI::CallCommand>>& buttons
	);

	HWND CreateFullResizablePlane(
		_In_ const std::shared_ptr<UI::D11DeviceResources>& pDevice,
		_In_ const std::shared_ptr<UI::Caching>& pCaching,
		_In_ HWND parent,
		_In_opt_ HINSTANCE hInstance,
		_In_ const std::wstring& className,
		_In_ LONG width,
		_In_ LONG height
	);

	HWND CreateSimpleWindow(
		_In_ const std::shared_ptr<UI::D11DeviceResources>& pDevice,
		_In_ const std::shared_ptr<UI::Caching>& pCaching,
		_In_opt_ HWND parent,
		_In_opt_ HINSTANCE hInstance,
		_In_ const std::wstring& className,
		_In_ const std::wstring& titleName,
		_In_ const std::wstring& icon,
		_In_ LONG width,
		_In_ LONG height,
		_In_ UI::ui_draw::button topRightButton,
		_In_ UI::ui_draw::window windowDraw = UI::ui_draw::window::simple,
		_In_ const std::initializer_list<std::tuple<std::wstring, UI::CallWindow>>& noFrameButtons = {},
		_In_opt_ bool showTitle = true,
		_In_opt_ bool resizable = true
	);

	void CreatePopUpElement(
		_In_ const std::shared_ptr<UI::D11DeviceResources>& pDevice,
		_In_ const std::shared_ptr<UI::Caching>& pCaching,
		_In_ const CallFunc& callFunc,
		_In_opt_ HINSTANCE hInstance,
		_In_ const std::wstring& className,
		_In_ const std::wstring& titleName,
		_In_ const std::wstring& middleName,
		_In_ const std::variant<UI::ui_draw::button, UI::ui_draw::separator>& draw,
		_In_ HWND parent,
		_In_ HWND root,
		_In_ LONG windowWidth,
		_Inout_ LONG& addTopValue
	);

	LONG GetWindowMaxWidth(
		_In_ const std::shared_ptr<UI::D11DeviceResources>& pDevice,
		_In_ const std::shared_ptr<UI::Caching>& pCaching,
		_In_ const std::vector<PopUpConfig>& buttonElements
	);

	LONG CalculateWindowHeight(
		_In_ const std::vector<PopUpConfig>& buttonElements
	);

	HWND CreatePopUpWindow(
		_In_ const std::shared_ptr<UI::D11DeviceResources>& pDevice,
		_In_ const std::shared_ptr<UI::Caching>& pCaching,
		_In_ const HWND& root,
		_In_opt_ const HINSTANCE& hInstance,
		_In_ const std::wstring& className,
		_In_ const std::vector<PopUpConfig>& buttonElements
	);

	std::vector<HWND> CreateWindowSeparators(
		_In_ const std::shared_ptr<UI::D11DeviceResources>& pDevice,
		_In_ const std::shared_ptr<UI::Caching>& pCaching,
		_In_ HWND parent,
		_In_opt_ HINSTANCE hInstance,
		_In_ const std::wstring& className,
		_In_opt_ const RECT& rect,
		_In_ const std::vector<UI::SeparatorConfig>& separators
	);

	// Create channel box / layer editor
	void CreateChannelBoxLayerEditor(
		_In_ const std::shared_ptr<UI::D11DeviceResources>& pDevice,
		_In_ const std::shared_ptr<UI::Caching>& pCaching,
		_In_ HWND parent,
		_In_opt_ HINSTANCE hInstance,
		_In_ const std::wstring& className,
		_In_ const RECT& rect
	);

	// The parent window collects information about its children 
	// and also transmits some of its data to them.
	// This feature is especially useful if child windows 
	// need to be modified when the parent window changes.
	void SettingData(HWND hwnd);

	void RunMessageLoop();
}

#endif // !LISA_GUI_CLASS_H
