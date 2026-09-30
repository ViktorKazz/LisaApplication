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

#ifndef MATERIALS_CLASS_H
#define MATERIALS_CLASS_H

#include "pch.h"
#include "HelperStructs.h"
#include "UploadBuffer.h"

namespace LisaApp
{	
	class Materials
	{
	public:
		Materials(Materials&&) = default;
		Materials& operator= (Materials&&) = default;

		Materials(Materials const&) = delete;
		Materials& operator= (Materials const&) = delete;

		Materials() = default;
		virtual ~Materials() = default;

		// Materials Accessors.

		INT GetMaterialIndex(this Materials& object, const std::wstring& name) noexcept
		{
			return object.m_materials[name]->MatCBIndex;
		}

		auto GetMaterialBuffer(this Materials& object) noexcept { return object.m_materialBuffer->Resource(); };

		void SetMaterialBuffer(
			this Materials& object, _In_ ID3D12Device3* device, UINT elementCount, bool isConstantBuffer) noexcept
		{
			object.m_materialBuffer = std::make_unique<UploadBuffer<Constants::MaterialData>>(device, elementCount, isConstantBuffer);
		}


		void Create(this Materials& object, const std::wstring& name, UINT material);

		void UpdateMaterial(this Materials& object, const std::wstring& name, const Item::Material& updateMat);

		void UpdateBuffer(this Materials& object, const std::wstring& name, bool newMaterial);

		static std::unique_ptr<Item::Material> __cdecl Base();

	private:
		INT m_matIndex{};

		std::unordered_map<std::wstring, std::unique_ptr<Item::Material>> m_materials{};

		std::unique_ptr<UploadBuffer<Constants::MaterialData>> m_materialBuffer{ nullptr };
	};
}



#endif // !MATERIALS_CLASS_H