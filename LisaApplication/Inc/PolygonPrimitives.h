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

#ifndef POLYGON_PRIMITIVES_CLASS_H
#define POLYGON_PRIMITIVES_CLASS_H

#include "pch.h"
#include "UploadBuffer.h"
#include "Geometry.h"

namespace LisaApp
{
	class PolygonPrimitives;
	using SceneObjects = std::unordered_map<std::uint32_t, std::unique_ptr<PolygonPrimitives>>;

	class PolygonPrimitives
	{
	public:
		PolygonPrimitives(PolygonPrimitives&&) = default;
		PolygonPrimitives& operator= (PolygonPrimitives&&) = default;

		PolygonPrimitives(PolygonPrimitives const&) = delete;
		PolygonPrimitives& operator= (PolygonPrimitives const&) = delete;

		virtual ~PolygonPrimitives();


		template<typename Self>
		size_t& GetNumberOfLayers(this Self&& self) { return self.m_numberOfLayers; }

		template<typename Self>
		auto begin(this Self&& self) { return std::span<Item::OnRender>(self.OnRender.get(), self.m_numberOfLayers).begin(); }

		template<typename Self>
		auto end(this Self&& self) { return std::span<Item::OnRender>(self.OnRender.get(), self.m_numberOfLayers).end(); }

		void TurnOnLayer(size_t index) const
		{
			if (index < 0 || index >= m_numberOfLayers)
				throw std::out_of_range("The input index is too large.");

			OnRender[index].IndexCount = OnRender[0].IndexCount;
			OnRender[index].StartIndexLocation = OnRender[0].StartIndexLocation;
			OnRender[index].BaseVertexLocation = OnRender[0].BaseVertexLocation;
		}

		void TurnOffLayer(size_t index) const
		{
			if (index < 0 || index >= m_numberOfLayers)
				throw std::out_of_range("The input index is too large.");

			OnRender[index].IndexCount = 0;
			OnRender[index].StartIndexLocation = 0;
			OnRender[index].BaseVertexLocation = 0;
		}

		auto GetUploadBuffer() { return m_uploadBuffer.Get(); };

		void MappedData(const void* src, size_t size)
		{
			memcpy(&m_mappedData[0], src, size);
		}

		// Create a grid that is located at the center of coordinates.
		static std::unique_ptr<PolygonPrimitives> __cdecl CreateGrid(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
			float lengthAndWidth = 12.0f, float gridLines = 5.0f, size_t subdivisions = 5);

		// Create 3D primitives 

		static std::unique_ptr<PolygonPrimitives> __cdecl CreateSphere(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
			float radius = 1.0f, UINT subdivisionsAxis = 20, UINT subdivisionsHeight = 20);

		// Creating a Geosphere. Code taken from the following resources:
		// http://go.microsoft.com/fwlink/?LinkId=248929
		// http://go.microsoft.com/fwlink/?LinkID=615561
		// Function change: The geosphere now consists of individual triangles.
		static std::unique_ptr<PolygonPrimitives> __cdecl CreateGeoSphere(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
			float radius = 1.0f, UINT subdivisions = 3, bool rhcoords = true);

		// Creating a Geosphere. The code is taken from Frank Luna's book and slightly modified.
		static std::unique_ptr<PolygonPrimitives> __cdecl CreateGeoSphereCubeTex(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
			float radius = 1.0f, UINT subdivisions = 3);

		// Based on the CreateGeoSphereCubeTex function
		static std::unique_ptr<PolygonPrimitives> __cdecl CreateGeoSphereEasy(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
			float radius = 1.0f, UINT subdivisions = 3, const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

		static std::unique_ptr<PolygonPrimitives> __cdecl CreateBox(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
			float width = 1.0f, float height = 1.0f, float depth = 1.0f, UINT subdivisionsWidth = 1, UINT subdivisionsHeight = 1, UINT subdivisionsDepth = 1);

		static std::unique_ptr<PolygonPrimitives> __cdecl CreateBoxEasy(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
			float width = 1.0f, float height = 1.0f, float depth = 1.0f, const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

		static std::unique_ptr<PolygonPrimitives> __cdecl CreateCylinder(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
			float radius = 1.0f, float height = 2.0f, UINT subdivisionsAxis = 20, UINT subdivisionsHeight = 1, UINT subdivisionsCaps = 1);

		static std::unique_ptr<PolygonPrimitives> __cdecl CreateDisc(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
			float radius = 1.0f, UINT subdivisionsAxis = 20, const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

		static std::unique_ptr<PolygonPrimitives> __cdecl CreateCone(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
			float radius = 1.0f, float height = 2.0f, UINT subdivisionsAxis = 20, UINT subdivisionsHeight = 1, UINT subdivisionsCaps = 1);

		static std::unique_ptr<PolygonPrimitives> __cdecl CreateConeEasy(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
			float radius = 1.0f, float height = 2.0f, UINT subdivisionsAxis = 20, const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

		static std::unique_ptr<PolygonPrimitives> __cdecl CreateTorus(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
			float radius = 1.0f, float sectionRadius = 1.0f, UINT subdivisionsAxis = 20, UINT subdivisionsHeight = 20, bool rhcoords = true);

		static std::unique_ptr<PolygonPrimitives> __cdecl CreatePlane(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
			float width = 1.0f, float depth = 1.0f, UINT subdivisionsWidth = 10, UINT subdivisionsDepth = 10);

		static std::unique_ptr<PolygonPrimitives> __cdecl CreatePlaneEasy(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
			float width = 1.0f, float depth = 1.0f, const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

		static std::unique_ptr<PolygonPrimitives> __cdecl CreateTriangle(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
			const DirectX::XMFLOAT3& pointA, const DirectX::XMFLOAT3& pointB, const DirectX::XMFLOAT3& pointC, const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

		// Create Line primitives

		static std::unique_ptr<PolygonPrimitives> __cdecl CreateLineCircle(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
			float radius = 1.0f, UINT quality = 1, const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

		static std::unique_ptr<PolygonPrimitives> __cdecl CreateLine(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
			const DirectX::XMFLOAT3& pointA, const DirectX::XMFLOAT3& pointB, const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

		// Create point primitives

		static std::unique_ptr<PolygonPrimitives> __cdecl CreatePoint(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
			const DirectX::XMFLOAT3& point, const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });


		void Parameters();

		void ConnectMaterial(size_t object, size_t instances, UINT matIndex) const;

		void AddInstances(size_t n) const;

		void UpdateBuffer(
			DirectX::XMMATRIX getViev,
			DirectX::BoundingFrustum camFrustum,
			bool frustumCullingEnabled
		);

		void Draw(_In_ ID3D12GraphicsCommandList* commandList, std::initializer_list<ID3D12PipelineState*> pso);

	public:
		std::unique_ptr<Item::OnRender[]> OnRender{ nullptr };

	private:
		// Initializes a geometric primitive instance that will draw the specified vertex and index data.
		template<typename VertexType, typename T>
		void Initialize(
			_In_ const T& vertices,
			_In_ const IndexCollection& indices,
			_In_ const DirectX::BoundingBox& bounds,
			_In_ const ComponentCollection& components,
			_In_ const D3D12_PRIMITIVE_TOPOLOGY& primitiveTopology,
			_In_ ID3D12Device3* device,
			_In_ ID3D12GraphicsCommandList* commandList,
			_In_ size_t numberOfLayers
		)
		{
			const CD3DX12_HEAP_PROPERTIES heapUpload{ D3D12_HEAP_TYPE_UPLOAD };
			const CD3DX12_RESOURCE_DESC resDesc{
				resDesc.Buffer(static_cast<UINT>(vertices.size()) * sizeof(VertexType))
			};

			device->CreateCommittedResource(
				&heapUpload,
				D3D12_HEAP_FLAG_NONE,
				&resDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_uploadBuffer));

			DX::ThrowIfFailed(m_uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedData)));


			const UINT vbByteSize = static_cast<UINT>(vertices.size()) * sizeof(VertexType);
			const UINT ibByteSize = static_cast<UINT>(indices.size()) * sizeof(std::uint32_t);

			auto geometry{ std::make_unique<Geometry::Mesh>() };
			geometry->PrimitiveTopology = primitiveTopology;

			DX::ThrowIfFailed(D3DCreateBlob(vbByteSize, &geometry->VertexBufferCPU));
			RtlCopyMemory(geometry->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

			DX::ThrowIfFailed(D3DCreateBlob(ibByteSize, &geometry->IndexBufferCPU));
			RtlCopyMemory(geometry->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);


			geometry->VertexBufferGPU = HelperUtilities::CreateDefaultBuffer(device,
				commandList, vertices.data(), vbByteSize, geometry->VertexBufferUploader);

			geometry->IndexBufferGPU = HelperUtilities::CreateDefaultBuffer(device,
				commandList, indices.data(), ibByteSize, geometry->IndexBufferUploader);

			geometry->VertexByteStride = sizeof(VertexType);
			geometry->VertexBufferByteSize = vbByteSize;
			geometry->IndexFormat = DXGI_FORMAT_R32_UINT;
			geometry->IndexBufferByteSize = ibByteSize;

			geometry->IndexCount = static_cast<UINT>(indices.size());
			geometry->BaseVertexLocation = 0;
			geometry->StartIndexLocation = 0;
			geometry->Bounds = bounds;

			geometry->ComponentsInfo = std::move(components);

			m_geometry = std::move(geometry);

			m_numberOfLayers = numberOfLayers;
			OnRender = std::make_unique<Item::OnRender[]>(m_numberOfLayers);

			Parameters();

			// A constant is needed for each layer.

			m_instanceDataConstants.reserve(numberOfLayers);
			m_holdDataConstants.reserve(numberOfLayers);

			for (size_t _ = 0; _ < numberOfLayers; _++)
				m_instanceDataConstants.emplace_back(std::make_unique<UploadBuffer<Constants::InstanceData>>(device, 1, false));

			for (size_t _ = 0; _ < numberOfLayers; _++)
				m_holdDataConstants.emplace_back(std::make_unique<UploadBuffer<Constants::HoldData>>(device, 1, false));
		};

	private:
		PolygonPrimitives() noexcept(false);

		size_t m_numberOfLayers{ 0 };

		std::vector<VertexStructs::Triangle>                                m_triangles{};
		std::unique_ptr<Geometry::Mesh>                                     m_geometry{ std::make_unique<Geometry::Mesh>() };

		Microsoft::WRL::ComPtr<ID3D12Resource>                              m_uploadBuffer{ nullptr };
		BYTE* m_mappedData{ nullptr };

		std::vector<std::unique_ptr<UploadBuffer<Constants::InstanceData>>> m_instanceDataConstants{};
		std::vector<std::unique_ptr<UploadBuffer<Constants::HoldData>>> m_holdDataConstants{};

	};
}

#endif // !POLYGON_PRIMITIVES_CLASS_H