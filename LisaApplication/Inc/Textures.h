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

#ifndef TEXTURES_CLASS_H
#define TEXTURES_CLASS_H

#include "pch.h"
#include "HelperStructs.h"
#include "CustomTextures.h"

class Textures
{
public:
	Textures(Textures&&) = default;
	Textures& operator= (Textures&&) = default;

	Textures(Textures const&) = delete;
	Textures& operator= (Textures const&) = delete;

	virtual ~Textures();

	// Textures Accessors.

	auto GetSrvDimension(this Textures& object) noexcept { return object.m_srvDimension; };

	ID3D12Resource* GetResource() const noexcept { return m_resource.Get(); }
	
	// Call InitializeTexture.
	static std::unique_ptr<Textures> __cdecl LoadTexture(
		_In_ ID3D12Device3* device,
		_In_ ID3D12GraphicsCommandList* commandList,
		const std::wstring& imagePath = {},
		D3D12_SRV_DIMENSION srvDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS,
		const std::vector<UINT8>& textures = CustomTextures::Base()
	);

private:
	// Load a custom texture or a texture from a file.
	void InitializeTexture(
		_In_ ID3D12Device3* device,
		_In_ ID3D12GraphicsCommandList* commandList,
		const std::wstring& imagePath,
		const std::vector<UINT8> textures
	);

	// Creating resources for custom textures.
	void CreateTextureResource(
		_In_ ID3D12Device3* device,
		_In_ ID3D12GraphicsCommandList* commandList,
		const std::vector<UINT8> textures
	);

private:
	Textures() noexcept(false);

	D3D12_SRV_DIMENSION m_srvDimension{};

	Microsoft::WRL::ComPtr<ID3D12Resource>                              m_resource{ nullptr };
	//std::unordered_map<std::wstring, Item::Texture>                     m_textures;

	Microsoft::WRL::ComPtr<ID3D12Resource>                              m_uploadHeap{ nullptr };
};

#endif // !TEXTURES_CLASS_H