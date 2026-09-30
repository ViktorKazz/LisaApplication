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

#ifndef VERTEX_STRUCTS_H
#define VERTEX_STRUCTS_H

#include <d3d12.h>
#include <DirectXMath.h>
#include <vector>

namespace VertexStructs
{	
	struct Triangle
	{
		uint32_t Index[3]{};
		std::vector<Triangle> Friend{};
	};

	// Vertex struct holding position and Size information.
	struct VertexPositionColor
	{
		VertexPositionColor() = default;

		VertexPositionColor(const VertexPositionColor&) = default;
		VertexPositionColor& operator=(const VertexPositionColor&) = default;

		VertexPositionColor(VertexPositionColor&&) = default;
		VertexPositionColor& operator=(VertexPositionColor&&) = default;

		VertexPositionColor(
			DirectX::XMFLOAT3 const& iposition, DirectX::XMFLOAT4 const& icolor) noexcept
			: position(iposition), color(icolor)
		{
		}

		VertexPositionColor(
			DirectX::FXMVECTOR iposition, DirectX::FXMVECTOR icolor) noexcept
		{
			DirectX::XMStoreFloat3(&this->position, iposition);
			DirectX::XMStoreFloat4(&this->color, icolor);
		}

		DirectX::XMFLOAT3 position{};
		DirectX::XMFLOAT4 color{};

		static const D3D12_INPUT_LAYOUT_DESC InputLayout;

	private:
		static constexpr unsigned int InputElementCount = 2;
		static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};

	// Vertex struct holding position, normal vector, and texture mapping information.
	struct VertexPositionNormalTexture
	{
		VertexPositionNormalTexture() = default;

		VertexPositionNormalTexture(const VertexPositionNormalTexture&) = default;
		VertexPositionNormalTexture& operator=(const VertexPositionNormalTexture&) = default;

		VertexPositionNormalTexture(VertexPositionNormalTexture&&) = default;
		VertexPositionNormalTexture& operator=(VertexPositionNormalTexture&&) = default;

		VertexPositionNormalTexture(
			DirectX::XMFLOAT3 const& iposition, 
			DirectX::XMFLOAT3 const& inormal, 
			DirectX::XMFLOAT2 const& iuv) noexcept
			: position(iposition),
			normal(inormal),
			uv(iuv)
		{ }

		VertexPositionNormalTexture(
			DirectX::FXMVECTOR iposition, 
			DirectX::FXMVECTOR inormal, 
			DirectX::FXMVECTOR iuv) noexcept
		{
			XMStoreFloat3(&this->position, iposition);
			XMStoreFloat3(&this->normal, inormal);
			XMStoreFloat2(&this->uv, iuv);
		}

		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;

		static const D3D12_INPUT_LAYOUT_DESC InputLayout;

	private:
		static constexpr unsigned int InputElementCount = 3;
		static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};

	// Vertex struct holding position, normal vector, texture mapping and TangentU information.
	struct VertexPositionNormalTextureTangentU
	{
		VertexPositionNormalTextureTangentU() = default;

		VertexPositionNormalTextureTangentU(const VertexPositionNormalTextureTangentU&) = default;
		VertexPositionNormalTextureTangentU& operator=(const VertexPositionNormalTextureTangentU&) = default;

		VertexPositionNormalTextureTangentU(VertexPositionNormalTextureTangentU&&) = default;
		VertexPositionNormalTextureTangentU& operator=(VertexPositionNormalTextureTangentU&&) = default;

		VertexPositionNormalTextureTangentU(
			DirectX::XMFLOAT3 const& iposition,
			DirectX::XMFLOAT3 const& inormal,
			DirectX::XMFLOAT2 const& iuv,
			DirectX::XMFLOAT3 const& itangentU) noexcept
			: position(iposition), normal(inormal), uv(iuv), tangentU(itangentU)
		{
		}

		VertexPositionNormalTextureTangentU(
			DirectX::FXMVECTOR iposition,
			DirectX::FXMVECTOR inormal,
			DirectX::FXMVECTOR iuv,
			DirectX::FXMVECTOR itangentU) noexcept
		{
			DirectX::XMStoreFloat3(&this->position, iposition);
			DirectX::XMStoreFloat3(&this->normal, inormal);
			DirectX::XMStoreFloat2(&this->uv, iuv);
			DirectX::XMStoreFloat3(&this->tangentU, itangentU);
		}

		DirectX::XMFLOAT3 position{};
		DirectX::XMFLOAT3 normal{};
		DirectX::XMFLOAT2 uv{};
		DirectX::XMFLOAT3 tangentU{};

		static const D3D12_INPUT_LAYOUT_DESC InputLayout;

	private:
		static constexpr unsigned int InputElementCount = 4;
		static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};

	// Vertex struct holding position, normal vector, texture mapping, tangentU and barycentric information.
	/*struct VertexPositionNormalTextureTangentUBar
	{
		VertexPositionNormalTextureTangentUBar() = default;

		VertexPositionNormalTextureTangentUBar(const VertexPositionNormalTextureTangentUBar&) = default;
		VertexPositionNormalTextureTangentUBar& operator=(const VertexPositionNormalTextureTangentUBar&) = default;

		VertexPositionNormalTextureTangentUBar(VertexPositionNormalTextureTangentUBar&&) = default;
		VertexPositionNormalTextureTangentUBar& operator=(VertexPositionNormalTextureTangentUBar&&) = default;

		VertexPositionNormalTextureTangentUBar(
			DirectX::XMFLOAT3 const& iposition,
			DirectX::XMFLOAT3 const& inormal,
			DirectX::XMFLOAT2 const& iuv,
			DirectX::XMFLOAT3 const& itangentU,
			DirectX::XMFLOAT3 const& ibar) noexcept
			: 
			position(iposition), 
			normal(inormal), 
			uv(iuv), 
			tangentU(itangentU), 
			bar(ibar)
		{
		}

		VertexPositionNormalTextureTangentUBar(
			DirectX::FXMVECTOR const& iposition,
			DirectX::FXMVECTOR const& inormal,
			DirectX::FXMVECTOR const& iuv,
			DirectX::FXMVECTOR const& itangentU,
			DirectX::FXMVECTOR const& ibar) noexcept
		{
			DirectX::XMStoreFloat3(&this->position, iposition);
			DirectX::XMStoreFloat3(&this->normal, inormal);
			DirectX::XMStoreFloat2(&this->uv, iuv);
			DirectX::XMStoreFloat3(&this->tangentU, itangentU);
			DirectX::XMStoreFloat3(&this->bar, ibar);
		}

		DirectX::XMFLOAT3 position{};
		DirectX::XMFLOAT3 normal{};
		DirectX::XMFLOAT2 uv{};
		DirectX::XMFLOAT3 tangentU{};
		DirectX::XMFLOAT3 bar{};

		static const D3D12_INPUT_LAYOUT_DESC InputLayout;

	private:
		static constexpr unsigned int InputElementCount = 5;
		static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};*/
}

#endif // !VERTEX_STRUCTS_H