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

#include "PolygonPrimitives.h"
#include "HelperUtilities.h"
#include <DirectXCollision.h>
#include "Globals.h"

LisaApp::PolygonPrimitives::PolygonPrimitives() noexcept(false)
{
}

LisaApp::PolygonPrimitives::~PolygonPrimitives()
{
	if (m_uploadBuffer != nullptr)
		m_uploadBuffer->Unmap(0, nullptr);

	m_mappedData = nullptr;
}

// Create a grid that is located at the center of coordinates.
std::unique_ptr<LisaApp::PolygonPrimitives> LisaApp::PolygonPrimitives::CreateGrid(
	_In_ ID3D12Device3* device,
	_In_ ID3D12GraphicsCommandList* commandList,
	float lengthAndWidth, float gridLines, size_t subdivisions)
{
	VertexCollection vertices;
	IndexCollection indices;

	GeometryCalculation::ComputeGrid(vertices, indices, lengthAndWidth, gridLines, subdivisions);
	std::unique_ptr<PolygonPrimitives> primitive(new PolygonPrimitives());

	using VertexType = VertexStructs::VertexPositionNormalTextureTangentU;
	primitive->Initialize<VertexType>(vertices, indices, {}, {}, D3D_PRIMITIVE_TOPOLOGY_LINELIST, device, commandList, 3);

	return primitive;
}

std::unique_ptr<LisaApp::PolygonPrimitives> LisaApp::PolygonPrimitives::CreateSphere(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
	float radius, UINT subdivisionsAxis, UINT subdivisionsHeight)
{
	VertexCollection vertices;
	IndexCollection indices;
	DirectX::BoundingBox bounds;
	ComponentCollection components;

	GeometryCalculation::ComputeSphere(vertices, indices, bounds, components, radius, subdivisionsAxis, subdivisionsHeight);
	std::unique_ptr<PolygonPrimitives> primitive(new PolygonPrimitives());

	using VertexType = VertexStructs::VertexPositionNormalTextureTangentU;
	primitive->Initialize<VertexType>(vertices, indices, bounds, components, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, device, commandList, 3);
	primitive->OnRender[0].Name = L"pSphere";

	return primitive;
}

std::unique_ptr<LisaApp::PolygonPrimitives> LisaApp::PolygonPrimitives::CreateGeoSphere(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
	float radius, UINT subdivisions, bool rhcoords)
{
	VertexCollection vertices;
	IndexCollection indices;
	DirectX::BoundingBox bounds;
	ComponentCollection components;

	GeometryCalculation::ComputeGeoSphere(vertices, indices, bounds, components, radius, subdivisions, rhcoords);
	std::unique_ptr<PolygonPrimitives> primitive(new PolygonPrimitives());

	using VertexType = VertexStructs::VertexPositionNormalTextureTangentU;
	primitive->Initialize<VertexType>(vertices, indices, bounds, components, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, device, commandList, 3);
	primitive->OnRender[0].Name = L"pGeoSphere";

	return primitive;
}

std::unique_ptr<LisaApp::PolygonPrimitives> LisaApp::PolygonPrimitives::CreateGeoSphereCubeTex(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
	float radius, UINT subdivisions)
{
	VertexCollection vertices;
	IndexCollection indices;
	DirectX::BoundingBox bounds;

	GeometryCalculation::ComputeGeoSphereCubeTex(vertices, indices, bounds, radius, subdivisions);
	std::unique_ptr<PolygonPrimitives> primitive(new PolygonPrimitives());

	using VertexType = VertexStructs::VertexPositionNormalTextureTangentU;
	primitive->Initialize<VertexType>(vertices, indices, bounds, {}, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, device, commandList, 1);
	primitive->OnRender[0].Name = L"pGeoSphereCubeTex";

	return primitive;
}

std::unique_ptr<LisaApp::PolygonPrimitives> LisaApp::PolygonPrimitives::CreateGeoSphereEasy(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
	float radius, UINT subdivisions, const DirectX::XMFLOAT4& color)
{
	VertexColor vertices;
	IndexCollection indices;
	DirectX::BoundingBox bounds;

	GeometryCalculation::ComputeGeoSphereEasy(vertices, indices, bounds, radius, subdivisions, color);
	std::unique_ptr<PolygonPrimitives> primitive(new PolygonPrimitives());

	using VertexType = VertexStructs::VertexPositionColor;
	primitive->Initialize<VertexType>(vertices, indices, bounds, {}, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, device, commandList, 1);
	primitive->OnRender[0].Name = L"pGeoSphereEasy";

	return primitive;
}

// Cube (aka a Hexahedron) or Box
std::unique_ptr<LisaApp::PolygonPrimitives> LisaApp::PolygonPrimitives::CreateBox(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
	float width, float height, float depth, UINT subdivisionsWidth, UINT subdivisionsHeight, UINT subdivisionsDepth)
{
	VertexCollection vertices;
	IndexCollection indices;
	DirectX::BoundingBox bounds;
	ComponentCollection components;

	// Create a Box.
	std::unique_ptr<PolygonPrimitives> primitive{ new PolygonPrimitives{} };
	GeometryCalculation::ComputeBox(vertices, indices, bounds, components, width, height, depth, subdivisionsWidth, subdivisionsHeight, subdivisionsDepth);

	using VertexType = VertexStructs::VertexPositionNormalTextureTangentU;
	primitive->Initialize<VertexType>(vertices, indices, bounds, components, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, device, commandList, 3);
	primitive->OnRender[0].Name = L"pCube";

	return primitive;
}

std::unique_ptr<LisaApp::PolygonPrimitives> LisaApp::PolygonPrimitives::CreateBoxEasy(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
	float width, float height, float depth, const DirectX::XMFLOAT4& color)
{
	VertexColor vertices;
	IndexCollection indices;
	DirectX::BoundingBox bounds;

	// Create a Box.
	std::unique_ptr<PolygonPrimitives> primitive{ new PolygonPrimitives{} };
	GeometryCalculation::ComputeBoxEasy(vertices, indices, bounds, width, height, depth, color);

	using VertexType = VertexStructs::VertexPositionColor;
	primitive->Initialize<VertexType>(vertices, indices, bounds, {}, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, device, commandList, 1);
	primitive->OnRender[0].Name = L"pCubeEasy";

	return primitive;
}

std::unique_ptr<LisaApp::PolygonPrimitives> LisaApp::PolygonPrimitives::CreateCylinder(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
	float radius, float height, UINT subdivisionsAxis, UINT subdivisionsHeight, UINT subdivisionsCaps)
{
	VertexCollection vertices;
	IndexCollection indices;
	DirectX::BoundingBox bounds;
	ComponentCollection components;

	GeometryCalculation::ComputeCylinder(vertices, indices, bounds, components, radius, height, subdivisionsAxis, subdivisionsHeight, subdivisionsCaps);
	std::unique_ptr<PolygonPrimitives> primitive(new PolygonPrimitives());

	using VertexType = VertexStructs::VertexPositionNormalTextureTangentU;
	primitive->Initialize<VertexType>(vertices, indices, bounds, components, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, device, commandList, 3);
	primitive->OnRender[0].Name = L"pCylinder";

	return primitive;
}

std::unique_ptr<LisaApp::PolygonPrimitives> LisaApp::PolygonPrimitives::CreateDisc(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
	float radius, UINT subdivisionsAxis, const DirectX::XMFLOAT4& color)
{
	VertexColor vertices;
	IndexCollection indices;
	DirectX::BoundingBox bounds;

	GeometryCalculation::ComputeDisc(vertices, indices, bounds, radius, subdivisionsAxis, color);
	std::unique_ptr<PolygonPrimitives> primitive(new PolygonPrimitives());

	using VertexType = VertexStructs::VertexPositionColor;
	primitive->Initialize<VertexType>(vertices, indices, bounds, {}, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, device, commandList, 3);
	primitive->OnRender[0].Name = L"pDisc";

	return primitive;
}

std::unique_ptr<LisaApp::PolygonPrimitives> LisaApp::PolygonPrimitives::CreateCone(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
	float radius, float height, UINT subdivisionsAxis, UINT subdivisionsHeight, UINT subdivisionsCaps)
{
	VertexCollection vertices;
	IndexCollection indices;
	DirectX::BoundingBox bounds;
	ComponentCollection components;

	GeometryCalculation::ComputeCone(vertices, indices, bounds, components, radius, height, subdivisionsAxis, subdivisionsHeight, subdivisionsCaps);
	std::unique_ptr<PolygonPrimitives> primitive(new PolygonPrimitives());

	using VertexType = VertexStructs::VertexPositionNormalTextureTangentU;
	primitive->Initialize<VertexType>(vertices, indices, bounds, components, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, device, commandList, 3);
	primitive->OnRender[0].Name = L"pCone";

	return primitive;
}

std::unique_ptr<LisaApp::PolygonPrimitives> LisaApp::PolygonPrimitives::CreateConeEasy(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
	float radius, float height, UINT subdivisionsAxis, const DirectX::XMFLOAT4& color)
{
	VertexColor vertices;
	IndexCollection indices;
	DirectX::BoundingBox bounds;

	GeometryCalculation::ComputeConeEasy(vertices, indices, bounds, radius, height, subdivisionsAxis, color);
	std::unique_ptr<PolygonPrimitives> primitive(new PolygonPrimitives());

	using VertexType = VertexStructs::VertexPositionColor;
	primitive->Initialize<VertexType>(vertices, indices, bounds, {}, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, device, commandList, 1);
	primitive->OnRender[0].Name = L"pConeEasy";

	return primitive;
}

std::unique_ptr<LisaApp::PolygonPrimitives> LisaApp::PolygonPrimitives::CreateTorus(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
	float radius, float sectionRadius, UINT subdivisionsAxis, UINT subdivisionsHeight, bool rhcoords)
{
	VertexCollection vertices;
	IndexCollection indices;
	DirectX::BoundingBox bounds;
	ComponentCollection components;

	GeometryCalculation::ComputeTorus(vertices, indices, bounds, components, radius, sectionRadius, subdivisionsAxis, subdivisionsHeight, rhcoords);
	std::unique_ptr<PolygonPrimitives> primitive(new PolygonPrimitives());

	using VertexType = VertexStructs::VertexPositionNormalTextureTangentU;
	primitive->Initialize<VertexType>(vertices, indices, bounds, components, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, device, commandList, 3);
	primitive->OnRender[0].Name = L"pTorus";

	return primitive;
}

std::unique_ptr<LisaApp::PolygonPrimitives> __cdecl LisaApp::PolygonPrimitives::CreatePlane(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
	float width, float depth, UINT subdivisionsWidth, UINT subdivisionsDepth)
{
	VertexCollection vertices;
	IndexCollection indices;
	DirectX::BoundingBox bounds;
	ComponentCollection components;

	std::unique_ptr<PolygonPrimitives> primitive(new PolygonPrimitives());
	GeometryCalculation::ComputePlane(vertices, indices, bounds, components, width, depth, subdivisionsWidth, subdivisionsDepth);

	using VertexType = VertexStructs::VertexPositionNormalTextureTangentU;
	primitive->Initialize<VertexType>(vertices, indices, bounds, components, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, device, commandList, 3);
	primitive->OnRender[0].Name = L"pPlane";

	return primitive;
}

std::unique_ptr<LisaApp::PolygonPrimitives> __cdecl LisaApp::PolygonPrimitives::CreatePlaneEasy(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
	float width, float depth, const DirectX::XMFLOAT4& color)
{
	VertexColor vertices;
	IndexCollection indices;
	DirectX::BoundingBox bounds;

	std::unique_ptr<PolygonPrimitives> primitive(new PolygonPrimitives());
	GeometryCalculation::ComputePlaneEasy(vertices, indices, bounds, width, depth, color);

	using VertexType = VertexStructs::VertexPositionColor;
	primitive->Initialize<VertexType>(vertices, indices, bounds, {}, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, device, commandList, 1);
	primitive->OnRender[0].Name = L"pPlaneEasy";

	return primitive;
}

std::unique_ptr<LisaApp::PolygonPrimitives> __cdecl LisaApp::PolygonPrimitives::CreateTriangle(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
	const DirectX::XMFLOAT3& pointA, const DirectX::XMFLOAT3& pointB, const DirectX::XMFLOAT3& pointC, const DirectX::XMFLOAT4& color)
{
	VertexColor vertices;
	IndexCollection indices;
	DirectX::BoundingBox bounds;

	std::unique_ptr<PolygonPrimitives> primitive(new PolygonPrimitives());
	GeometryCalculation::ComputeTriangle(vertices, indices, bounds, pointA, pointB, pointC, color);

	using VertexType = VertexStructs::VertexPositionColor;
	primitive->Initialize<VertexType>(vertices, indices, bounds, {}, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, device, commandList, 1);
	primitive->OnRender[0].Name = L"pTriangle";

	return primitive;
}

// Create a line circle primitive. By type of edging.
std::unique_ptr<LisaApp::PolygonPrimitives> LisaApp::PolygonPrimitives::CreateLineCircle(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
	float radius, UINT quality, const DirectX::XMFLOAT4& color)
{
	VertexColor vertices;
	IndexCollection indices;

	GeometryCalculation::ComputeLineCircle(vertices, indices, radius, quality, color);
	std::unique_ptr<PolygonPrimitives> primitive(new PolygonPrimitives());

	using VertexType = VertexStructs::VertexPositionColor;
	primitive->Initialize<VertexType>(vertices, indices, {}, {}, D3D_PRIMITIVE_TOPOLOGY_LINELIST, device, commandList, 1);
	primitive->OnRender[0].Name = L"lineCircle";

	return primitive;
}

// Create a line.

std::unique_ptr<LisaApp::PolygonPrimitives> LisaApp::PolygonPrimitives::CreateLine(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
	const DirectX::XMFLOAT3& pointA, const DirectX::XMFLOAT3& pointB, const DirectX::XMFLOAT4& color)
{
	VertexColor vertices;
	IndexCollection indices;

	GeometryCalculation::ComputeLine(vertices, indices, pointA, pointB, color);
	std::unique_ptr<PolygonPrimitives> primitive(new PolygonPrimitives());

	using VertexType = VertexStructs::VertexPositionColor;
	primitive->Initialize<VertexType>(vertices, indices, {}, {}, D3D_PRIMITIVE_TOPOLOGY_LINELIST, device, commandList, 1);
	primitive->OnRender[0].Name = L"nurbsLine";

	return primitive;
}

std::unique_ptr<LisaApp::PolygonPrimitives> LisaApp::PolygonPrimitives::CreatePoint(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList,
	const DirectX::XMFLOAT3& point, const DirectX::XMFLOAT4& color)
{
	VertexColor vertices;
	IndexCollection indices;

	GeometryCalculation::ComputePoint(vertices, indices, point, color);
	std::unique_ptr<PolygonPrimitives> primitive(new PolygonPrimitives());

	using VertexType = VertexStructs::VertexPositionColor;
	primitive->Initialize<VertexType>(vertices, indices, {}, {}, D3D_PRIMITIVE_TOPOLOGY_POINTLIST, device, commandList, 1);
	primitive->OnRender[0].Name = L"point";

	return primitive;
}

void LisaApp::PolygonPrimitives::Parameters()
{
	for (size_t i = 0; i < m_numberOfLayers; i++)
	{
		OnRender[i].ObjCBIndex = 0;
		OnRender[i].Mat = 0;
		OnRender[i].Geo = m_geometry.get();
		OnRender[i].PrimitiveType = m_geometry->PrimitiveTopology;
		OnRender[i].InstanceCount = 0;
		OnRender[i].IndexCount = i ? 0 : OnRender[0].Geo->IndexCount;
		OnRender[i].StartIndexLocation = i ? 0 : OnRender[0].Geo->StartIndexLocation;
		OnRender[i].BaseVertexLocation = i ? 0 : OnRender[0].Geo->BaseVertexLocation;
		OnRender[i].BoundsOriginal = OnRender[0].Geo->Bounds;
		OnRender[i].Bounds = OnRender[0].Geo->Bounds;
		OnRender[i].Instances.resize(1);
		OnRender[i].Attributes.resize(1);
	}
}

void LisaApp::PolygonPrimitives::ConnectMaterial(size_t object, size_t instances, UINT matIndex) const
{
	OnRender[object].Instances[instances].MaterialIndex = matIndex;
}

void LisaApp::PolygonPrimitives::AddInstances(size_t n) const
{	
	for (size_t i = 0; i < m_numberOfLayers; i++)
	{
		OnRender[i].Instances.resize(n + 1);
		OnRender[i].Attributes.resize(n + 1);
	}
}

void LisaApp::PolygonPrimitives::UpdateBuffer(
	DirectX::XMMATRIX getViev,
	DirectX::BoundingFrustum camFrustum,
	bool frustumCullingEnabled
)
{
	using namespace LisaApp::Global;

	DirectX::XMMATRIX view = getViev;
	DirectX::XMVECTOR v{ XMMatrixDeterminant(view) };
	DirectX::XMMATRIX invView = XMMatrixInverse(&v, view);

	// We place the data for the shape and mesh objects in the buffer. 0 - Shape, 1 - Mesh, 2 - Face. 3 - Edge
	for (size_t j{ 0 }; j < m_instanceDataConstants.size(); j++)
	{
		auto& e = OnRender[j];

		const auto& instanceData = e.Instances;

		Constants::HoldData hdata;

		if (instanceData.begin()->Mode == selection::face)
		{
			for (auto&& [iter, hold] : e.HoldTriangle | std::views::enumerate)
			{
				hdata = hold.second;
				m_holdDataConstants[j]->CopyData(static_cast<std::int32_t>(iter), hdata);
			}
		}
		else if (instanceData.begin()->Mode == selection::edge)
		{
			for (auto&& [iter, hold] : e.HoldEdge | std::views::enumerate)
			{
				hdata = hold.second;
				m_holdDataConstants[j]->CopyData(static_cast<std::int32_t>(iter), hdata);				
			}
		}
		else if (instanceData.begin()->Mode == selection::vertex)
		{
			for (auto&& [iter, hold] : e.HoldVertex | std::views::enumerate)
			{
				hdata = hold.second;
				m_holdDataConstants[j]->CopyData(static_cast<std::int32_t>(iter), hdata);
			}
		}

		UINT visibleInstanceCount = 0;

		for (size_t i{ 0 }; i < instanceData.size(); ++i)
		{
			DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&instanceData[i].World);
			DirectX::XMMATRIX texTransform = DirectX::XMLoadFloat4x4(&instanceData[i].TexTransform);
			DirectX::XMVECTOR w{ XMMatrixDeterminant(world) };
			DirectX::XMMATRIX invWorld = DirectX::XMMatrixInverse(&w, world);

			// View space to the object's local space.
			DirectX::XMMATRIX viewToLocal = DirectX::XMMatrixMultiply(invView, invWorld);

			// Transform the camera frustum from view space to the object's local space.
			DirectX::BoundingFrustum localSpaceFrustum;
			camFrustum.Transform(localSpaceFrustum, viewToLocal);

			// Perform the box/frustum intersection test in local space.
			if ((localSpaceFrustum.Contains(e.Bounds) != DirectX::DISJOINT) || (frustumCullingEnabled == false))
			{
				Constants::InstanceData data;
				XMStoreFloat4x4(&data.World, XMMatrixTranspose(world));
				XMStoreFloat4x4(&data.TexTransform, XMMatrixTranspose(texTransform));
				
				data.MaterialIndex = instanceData[i].MaterialIndex;
				data.DrawTriangle = instanceData[i].DrawTriangle;
				data.TriangleNumber = instanceData[i].TriangleNumber;
				data.ComponentTriangle = instanceData[i].ComponentTriangle;
				data.HoldDataSize = instanceData[i].HoldDataSize;
				data.Mode = instanceData[i].Mode;
				data.Hide = instanceData[i].Hide;
				data.Color = instanceData[i].Color;

				// Write the instance data to structured buffer for the visible objects.
				m_instanceDataConstants[j]->CopyData(e.ObjCBIndex + visibleInstanceCount, data);
				
				visibleInstanceCount++;
			}
		}
		e.InstanceCount = visibleInstanceCount;		
	}
}

void LisaApp::PolygonPrimitives::Draw(_In_ ID3D12GraphicsCommandList* commandList, std::initializer_list<ID3D12PipelineState*> pso)
{
	// Draw a shape, mesh, face, edge and vertex object.

	Microsoft::WRL::ComPtr<ID3D12Resource> objConstant;
	Microsoft::WRL::ComPtr<ID3D12Resource> componentsBuffer;

	for (auto&& [i, ipso] : pso | std::views::enumerate)
	{
		const auto& ri = OnRender[i];
		
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{ ri.Geo->VertexBufferView() };
		commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

		D3D12_INDEX_BUFFER_VIEW indexBufferView{ ri.Geo->IndexBufferView() };
		commandList->IASetIndexBuffer(&indexBufferView);
		commandList->IASetPrimitiveTopology(ri.PrimitiveType);

		objConstant = m_instanceDataConstants[i]->Resource();
		commandList->SetGraphicsRootShaderResourceView(0, objConstant->GetGPUVirtualAddress());

		componentsBuffer = m_holdDataConstants[i]->Resource();
		commandList->SetGraphicsRootShaderResourceView(2, componentsBuffer->GetGPUVirtualAddress());

		commandList->SetPipelineState(ipso);
		commandList->DrawIndexedInstanced(ri.IndexCount, ri.InstanceCount, ri.StartIndexLocation, ri.BaseVertexLocation, 0);
	}
}