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

#include "Textures.h"

Textures::Textures() noexcept(false)
{
}

Textures::~Textures()
{
}

std::unique_ptr<Textures> Textures::LoadTexture(
	_In_ ID3D12Device3* device,
	_In_ ID3D12GraphicsCommandList* commandList,
	const std::wstring& imagePath,
	D3D12_SRV_DIMENSION srvDimension,
	const std::vector<UINT8>& textures
)
{
	std::unique_ptr<Textures> texture(new Textures());

	texture->InitializeTexture(device, commandList, imagePath.c_str(), textures);

	texture->m_srvDimension = srvDimension;

	return texture;
}

// Load a custom texture or a texture from a file.
void Textures::InitializeTexture(
	_In_ ID3D12Device3* device,
	_In_ ID3D12GraphicsCommandList* commandList,
	const std::wstring& imagePath,
	const std::vector<UINT8> textures
)
{
	if (device && commandList)
	{
		if (!imagePath.empty())
		{
			HRESULT hr{ S_OK };
			
			hr = App::DirectX::CreateDDSTextureFromFile12(device, commandList,
				imagePath.c_str(), m_resource, m_uploadHeap);

			DX::ThrowIfFailed(hr);
		}
		else
		{
			CreateTextureResource(device, commandList, textures);
		}
	}
}

// Creating resources for custom textures.
void Textures::CreateTextureResource(
	_In_ ID3D12Device3* device,
	_In_ ID3D12GraphicsCommandList* commandList,
	std::vector<UINT8> textures
)
{
	// Describe and create a material.
	D3D12_RESOURCE_DESC materialDesc = {};
	materialDesc.MipLevels = 1;
	materialDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
	materialDesc.Width = 256;
	materialDesc.Height = 256;
	materialDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	materialDesc.DepthOrArraySize = 1;
	materialDesc.SampleDesc.Count = 1;
	materialDesc.SampleDesc.Quality = 0;
	materialDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	const CD3DX12_HEAP_PROPERTIES heapDefault{ D3D12_HEAP_TYPE_DEFAULT };
	DX::ThrowIfFailed(device->CreateCommittedResource(
		&heapDefault,
		D3D12_HEAP_FLAG_NONE,
		&materialDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(m_resource.ReleaseAndGetAddressOf())));

	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_resource.Get(), 0, 1);

	// Create the GPU upload buffer.
	const CD3DX12_HEAP_PROPERTIES heapUpload{ D3D12_HEAP_TYPE_UPLOAD };
	const CD3DX12_RESOURCE_DESC resDesc{ resDesc.Buffer(uploadBufferSize) };
	DX::ThrowIfFailed(device->CreateCommittedResource(
		&heapUpload,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_uploadHeap.GetAddressOf())));

	// Copy data to the intermediate upload heap and then schedule a copy 
	// from the upload heap to the Texture2D. 
	auto& getTexture = textures;

	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = &getTexture[0];
	textureData.RowPitch = 256 * sizeof(UINT);
	textureData.SlicePitch = textureData.RowPitch * 256;

	UpdateSubresources(commandList, m_resource.Get(), m_uploadHeap.Get(), 0, 0, 1, &textureData);

	const CD3DX12_RESOURCE_BARRIER resBarrier{ resBarrier.Transition(
		m_resource.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	) };
	commandList->ResourceBarrier(1, &resBarrier);
}