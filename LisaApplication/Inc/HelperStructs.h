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

#ifndef HELPER_STRUCTS_H
#define HELPER_STRUCTS_H

#include "pch.h"

namespace Light
{
	struct Simple
	{
		DirectX::XMFLOAT3 Strength{ 0.5f, 0.5f, 0.5f };
		float FalloffStart{ 1.0f };                          // point/spot light only
		DirectX::XMFLOAT3 Direction{ 0.0f, -1.0f, 0.0f };    // directional/spot light only
		float FalloffEnd{ 10.0f };                           // point/spot light only
		DirectX::XMFLOAT3 Position{ 0.0f, 0.0f, 0.0f };      // point/spot light only
		float SpotPower{ 64.0f };                            // spot light only
	};
}

namespace Constants
{
	struct Object
	{
		DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
		UINT     MaterialIndex{};
		UINT     ObjPad0{};
		UINT     ObjPad1{};
		UINT     ObjPad2{};
	};

	struct InstanceData
	{
		// World matrix of the shape that describes the object's local space
		// relative to the world space, which defines the position, orientation,
		// and scale of the object in the world.
		DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();

		INT MaterialIndex{ -1 };
		UINT DrawTriangle{};
		UINT TriangleNumber{};
		UINT ComponentTriangle{};
		UINT HoldDataSize{};
		// Select off = 0, mesh = 1, face = 2, edge = 3, vertex = 4, already allocated = 5.
		UINT Mode{};
		// 0 - Visible, 1 - Hidden.
		UINT Hide{};
		DirectX::XMFLOAT4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
	};

	struct Material
	{
		DirectX::XMVECTORF32 DiffuseAlbedo{ { { 1.000000000f, 1.000000000f, 1.000000000f, 1.f } } };//XMVECTORF32
		DirectX::XMFLOAT3 FresnelR0{ 0.01f, 0.01f, 0.01f };
		float Roughness{ 0.25f };

		// Used in texture mapping.
		DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
		UINT DiffuseMapIndex{};
		UINT MaterialPad0{};
		UINT MaterialPad1{};
		UINT MaterialPad2{};
	};

	struct MaterialData
	{
		DirectX::XMVECTORF32 DiffuseAlbedo{ { { 1.000000000f, 1.000000000f, 1.000000000f, 1.f } } };
		DirectX::XMFLOAT3 FresnelR0{ 0.01f, 0.01f, 0.01f };
		float Roughness{ 0.5f };

		// Used in texture mapping.
		DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();

		UINT DiffuseMapIndex{};
		UINT NormalMapIndex{};
		UINT MaterialPad1{};
		UINT MaterialPad2{};
	};

	struct HoldData
	{
		UINT Triangle{};
		UINT Component{};
	};

	struct Ssao
	{
		DirectX::XMFLOAT4X4 Proj{};
		DirectX::XMFLOAT4X4 InvProj{};
		DirectX::XMFLOAT4X4 ProjTex{};
		DirectX::XMFLOAT4   OffsetVectors[14]{};

		// For SsaoBlur.hlsl
		DirectX::XMFLOAT4 BlurWeights[3]{};

		DirectX::XMFLOAT2 InvRenderTargetSize{ 0.0f, 0.0f };

		// Coordinates given in view space.
		float OcclusionRadius{ 0.5f };
		float OcclusionFadeStart{ 0.2f };
		float OcclusionFadeEnd{ 2.0f };
		float SurfaceEpsilon{ 0.05f };
	};

	struct Scene
	{
		DirectX::XMFLOAT4X4 View = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 Proj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 ViewProjTex = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 ShadowTransform = MathHelper::Identity4x4();
		DirectX::XMFLOAT3 EyePosW{ 0.0f, 0.0f, 0.0f };
		float cbPerObjectPad1{ 0.0f };
		DirectX::XMFLOAT2 RenderTargetSize{ 0.0f, 0.0f };
		DirectX::XMFLOAT2 InvRenderTargetSize{ 0.0f, 0.0f };
		float NearZ{ 0.0f };
		float FarZ{ 0.0f };
		float TotalTime{ 0.0f };
		float DeltaTime{ 0.0f };
		DirectX::XMFLOAT4 AmbientLight{ 0.0f, 0.0f, 0.0f, 1.0f };

		// Indices [0, NUM_DIR_LIGHTS) are directional lights;
		// indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
		// indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
		// are spot lights for a maximum of MaxLights per object.
		Light::Simple Lights[16];
	};
}

namespace Geometry
{
	//struct ComponentsInfo
	//{
	//	using triangle = std::uint32_t;
	//	using index0 = std::uint32_t;
	//	using index1 = std::uint32_t;
	//	using index2 = std::uint32_t;

	//	std::vector<triangle> Triangle;

	//	std::vector<std::pair<index0, triangle>> Vertex0;
	//	std::vector<std::pair<index1, triangle>> Vertex1;
	//	std::vector<std::pair<index2, triangle>> Vertex2;

	//	std::vector<std::tuple<index0, index1, triangle>> Edge0;
	//	std::vector<std::tuple<index1, index2, triangle>> Edge1;
	//	std::vector<std::tuple<index2, index0, triangle>> Edge2;
	//};

	struct InstTest
	{
		bool Visible{ true };
		
		UINT IndexCount{};
		UINT StartIndexLocation{};
		UINT BaseVertexLocation{};
	};
	
	// Defines a subrange of geometry in a MeshGeometry.  This is for when multiple
	// geometries are stored in one vertex and index buffer.  It provides the offsets
	// and data needed to draw a subset of geometry stores in the vertex and index 
	// buffers so that we can implement the technique described by Figure 6.3.
	struct Submesh
	{		
		UINT IndexCount{};
		UINT StartIndexLocation{};
		UINT BaseVertexLocation{};

		// Bounding box of the geometry defined by this submesh. 
		// This is used in later chapters of the book.
		DirectX::BoundingBox Bounds;
	};

	struct Mesh
	{
		// Give it a name so we can look it up by name.
		std::string Name;

		// System memory copies.  Use Blobs because the vertex/index format can be generic.
		// It is up to the client to cast appropriately.  
		Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU{ nullptr };
		Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU{ nullptr };

		Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU{ nullptr };
		Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU{ nullptr };

		Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader{ nullptr };
		Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader{ nullptr };

		D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology{};

		// Data about the buffers.
		UINT VertexByteStride{};
		UINT VertexBufferByteSize{};
		DXGI_FORMAT IndexFormat{ DXGI_FORMAT_R16_UINT };
		UINT IndexBufferByteSize{};

		// A MeshGeometry may store multiple geometries in one vertex/index buffer.
		// Use this container to define the Submesh geometries so we can draw
		// the Submeshes individually.
		//std::unordered_map<std::string, Geometry::Submesh> drawArgs; <- old
		UINT IndexCount{};
		UINT StartIndexLocation{};
		INT BaseVertexLocation{};

		// Bounding box of the geometry defined by this submesh. 
		// This is used in later chapters of the book.
		DirectX::BoundingBox Bounds;

		D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
		{
			D3D12_VERTEX_BUFFER_VIEW vbv{};
			vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
			vbv.StrideInBytes = VertexByteStride;
			vbv.SizeInBytes = VertexBufferByteSize;

			return vbv;
		}
		
		D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
		{
			D3D12_INDEX_BUFFER_VIEW ibv{};
			ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
			ibv.Format = IndexFormat;
			ibv.SizeInBytes = IndexBufferByteSize;

			return ibv;
		}

		// We can free this memory after we finish upload to the GPU.
		void DisposeUploaders()
		{
			VertexBufferUploader = nullptr;
			IndexBufferUploader = nullptr;
		}

		std::unordered_multimap<std::uint32_t, std::vector<std::pair<std::uint32_t, std::uint32_t>>> ComponentsInfo;
	};
}

namespace Item
{
	// Simple struct to represent a material for our demos.  A production 3D engine
	// would likely create a class hierarchy of Materials.
	struct Material
	{
		// Unique material name for lookup.
		std::wstring Name;

		// Index into constant buffer corresponding to this material.
		INT MatCBIndex{ -1 };

		// Index into SRV heap for diffuse texture.
		INT DiffuseSrvHeapIndex{ -1 };

		// Index into SRV heap for normal texture.
		INT NormalSrvHeapIndex{ -1 };

		// Dirty flag indicating the material has changed and we need to update the constant buffer.
		// Because we have a material constant buffer for each FrameResource, we have to apply the
		// update to each FrameResource.  Thus, when we modify a material we should set 
		// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
		INT NumFramesDirty{ 3 };// gNumFrameResources = 3;

		// Material constant buffer data used for shading.
		DirectX::XMVECTORF32 DiffuseAlbedo{ { { 1.000000000f, 1.000000000f, 1.000000000f, 1.f } } };
		DirectX::XMFLOAT3 FresnelR0{ 0.01f, 0.01f, 0.01f };
		float Roughness{ 0.25f };
		DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
	};

	struct Texture
	{
		// Unique material name for lookup.
		int Name{};

		std::wstring Filename;

		Microsoft::WRL::ComPtr<ID3D12Resource> Resource{ nullptr };
		Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap{ nullptr };
	};

	struct Attributes
	{
		DirectX::XMVECTOR Quaternion = DirectX::XMQuaternionIdentity();

		float TranslateX{};
		float TranslateY{};
		float TranslateZ{};

		//float RotateX{};
		//float RotateY{};
		//float RotateZ{};

		float ScaleX{ 1.0f };
		float ScaleY{ 1.0f };
		float ScaleZ{ 1.0f };
	};

	// Lightweight structure stores parameters to draw a shape.  This will
	// vary from app-to-app.
	struct OnRender
	{
		OnRender() = default;

		std::wstring Name{};
		std::uint32_t ID{};

		// Dirty flag indicating the object data has changed and we need to update the constant buffer.
		// Because we have an object cbuffer for each FrameResource, we have to apply the
		// update to each FrameResource.  Thus, when we modify obect data we should set 
		// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
		int NumFramesDirty{ 3 };//gNumFrameResources = 3

		// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
		UINT ObjCBIndex{};

		Item::Material* Mat{ nullptr };
		Geometry::Mesh* Geo{ nullptr };	
		
		// Primitive topology.
		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		DirectX::BoundingBox BoundsOriginal;
		DirectX::BoundingBox Bounds;
		std::vector<Constants::InstanceData> Instances;
		std::vector<Item::Attributes> Attributes;

		std::unordered_map<UINT, Constants::HoldData> HoldTriangle;
		std::unordered_map<std::wstring, Constants::HoldData> HoldEdge;
		std::unordered_map<UINT, Constants::HoldData> HoldVertex;

		// Draw Indexed Instanced parameters.
		UINT IndexCount{};
		UINT InstanceCount{};
		UINT StartIndexLocation{};
		UINT BaseVertexLocation{};

		void TurnOn(const OnRender& inOR)
		{
			IndexCount = inOR.IndexCount;
			StartIndexLocation = inOR.StartIndexLocation;
			BaseVertexLocation = inOR.BaseVertexLocation;
		}

		void TurnOff()
		{
			IndexCount = 0;
			StartIndexLocation = 0;
			BaseVertexLocation = 0;
		}
	};
}

namespace Shaders
{
	struct OutShaders
	{
		Microsoft::WRL::ComPtr<ID3DBlob> VS{ nullptr };
		Microsoft::WRL::ComPtr<ID3DBlob> PS{ nullptr };
		Microsoft::WRL::ComPtr<ID3DBlob> GS{ nullptr };
	};
}

namespace IPivot
{
	struct Attributes
	{
		DirectX::XMVECTOR Quaternion = DirectX::XMQuaternionIdentity();
		DirectX::XMVECTOR AxisNorm{};

		float Radian{};

		float TranslateX{};
		float TranslateY{};
		float TranslateZ{};

		float OutputTranslateX{};
		float OutputTranslateY{};
		float OutputTranslateZ{};

		float ScaleX{ 1.0f };
		float ScaleY{ 1.0f };
		float ScaleZ{ 1.0f };

		float OutputScaleX{ 1.0f };
		float OutputScaleY{ 1.0f };
		float OutputScaleZ{ 1.0f };
	};

	struct Axis
	{
		bool AxisX{};
		bool AxisY{};
		bool AxisZ{};
		bool OuterCircle{};
		bool Sphere{};
	};
}

#endif // !HELPER_STRUCTS_H