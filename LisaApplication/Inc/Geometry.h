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

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <stdexcept>
#include <algorithm> 
#include <DirectXCollision.h>
#include "Globals.h"

namespace GeometryCalculation
{
	using namespace LisaApp;

	// The circle is created in the plane:
	// 0 - XY, 1 - ZY, 2 - XZ
	DirectX::XMFLOAT3 RotationOfPointAAroundPointB2D(float aX, float aY, float bX, float bY, float angle, std::uint32_t definePlane = 0);
	DirectX::BoundingBox BoundingBox(const float& width, const float& height, const float& depth);

	// Helper function for duplicating vertices for the second triangle in a pair.
	void DuplicatingVertices(_Inout_ VertexCollection& vertices, _Inout_ IndexCollection& indices, std::uint32_t& ind);

	// The function sorts the input container as follows: triangle, { { index triangle } }.
	// The first pair in the vector is { index, triangle } of the native triangle.
	ComponentCollection OrderingOfIndexes(const VertexCollection& vertices);

	void Plane(
		_Inout_ VertexCollection& vertices,
		_Inout_ IndexCollection& indices,
		const DirectX::XMVECTOR& position,
		const DirectX::XMVECTOR& segment,
		const DirectX::XMVECTOR& nrm,
		const DirectX::XMVECTOR& tU,
		const DirectX::XMVECTOR& startUV,
		const DirectX::XMVECTOR& segmentUV,
		UINT row,
		UINT column,
		UINT side
	);

	inline void CheckIndexOverflow(size_t value)
	{
		// Use >=, not > comparison, because some D3D level 9_x hardware does not support 0xFFFF index values.
		if (value >= USHRT_MAX)
			throw std::out_of_range("Index value out of range: cannot tesselate primitive so finely");
	}

	// Collection types used when generating the geometry.
	inline void index_push_back(IndexCollection& indices, size_t value)
	{
		CheckIndexOverflow(value);
		indices.push_back(static_cast<uint16_t>(value));
	}

	// Helper for flipping winding of geometric primitives for LH vs. RH coords
	inline void ReverseWinding(VertexCollection& vertices, IndexCollection& indices)
	{
		assert((indices.size() % 3) == 0);
		for (auto it = indices.begin(); it != indices.end(); it += 3)
		{
			std::swap(*it, *(it + 2));
		}

		for (auto& it : vertices)
		{
			it.uv.x = (1.f - it.uv.x);
		}
	}

	// The function calculates a grid that is located at the center of coordinates.
	void ComputeGrid(_Inout_ VertexCollection& vertices, _Inout_ IndexCollection& indices,
		float lengthAndWidth, float gridLines, size_t subdivisions);

	// Creates a sphere centered at the origin with the given radius.
    // The slices and stacks parameters control the degree of subdivisions.
	void ComputeSphere(_Inout_ VertexCollection& vertices, _Inout_ IndexCollection& indices, _Inout_ DirectX::BoundingBox& bounds,
		_Inout_ ComponentCollection& components, float radius, UINT subdivisionsAxis, UINT subdivisionsHeight);

	void ComputeGeoSphere(_Inout_ VertexCollection& vertices, _Inout_ IndexCollection& indices, _Inout_ DirectX::BoundingBox& bounds,
		_Inout_ ComponentCollection& components, float radius, UINT subdivisions, bool rhcoords);

	// Creating a Geosphere. The code is taken from Frank Luna's book and slightly modified.
	void ComputeGeoSphereCubeTex(_Inout_ VertexCollection& vertices, _Inout_ IndexCollection& indices, _Inout_ DirectX::BoundingBox& bounds,
		float radius, UINT subdivisions);

	// Based on the ComputeGeoSphereCubeTex function
	void ComputeGeoSphereEasy(_Inout_ VertexColor& vertices, _Inout_ IndexCollection& indices, _Inout_ DirectX::BoundingBox& bounds,
		float radius, UINT subdivisions, const DirectX::XMFLOAT4& color);

	// Cube (aka a Hexahedron) or Box
	void ComputeBox(_Inout_ VertexCollection& vertices, _Inout_ IndexCollection& indices, _Inout_ DirectX::BoundingBox& bounds,
		_Inout_ ComponentCollection& components, float width, float height, float depth, UINT subdivisionsWidth, UINT subdivisionsHeight, UINT subdivisionsDepth);

	void ComputeBoxEasy(_Inout_ VertexColor& vertices, _Inout_ IndexCollection& indices, _Inout_ DirectX::BoundingBox& bounds,
		float width, float height, float depth, const DirectX::XMFLOAT4& color);

	// Creates a cylinder parallel to the y-axis, and centered about the origin.  
	// The bottom and top radius can vary to form various cone shapes rather than true cylinders.
	// The slices and stacks parameters control the degree of subdivisions.
	void ComputeCylinder(_Inout_ VertexCollection& vertices, _Inout_ IndexCollection& indices, _Inout_ DirectX::BoundingBox& bounds,
		_Inout_ ComponentCollection& components, float radius, float height, UINT subdivisionsAxis, UINT subdivisionsHeight, UINT subdivisionsCaps);

	void ComputeDisc(_Inout_ VertexColor& vertices, _Inout_ IndexCollection& indices, _Inout_ DirectX::BoundingBox& bounds,
		float radius, UINT subdivisionsAxis, const DirectX::XMFLOAT4& color);

	// Creates a cone primitive.
	void ComputeCone(_Inout_ VertexCollection& vertices, _Inout_ IndexCollection& indices, _Inout_ DirectX::BoundingBox& bounds,
		_Inout_ ComponentCollection& components, float radius, float height, UINT subdivisionsAxis, UINT subdivisionsHeight, UINT subdivisionsCaps);

	void ComputeConeEasy(_Inout_ VertexColor& vertices, _Inout_ IndexCollection& indices, _Inout_ DirectX::BoundingBox& bounds,
		float radius, float height, UINT subdivisionsAxis, const DirectX::XMFLOAT4& color);

	// Creates a torus primitive.
	void ComputeTorus(_Inout_ VertexCollection& vertices, _Inout_ IndexCollection& indices, _Inout_ DirectX::BoundingBox& bounds,
		_Inout_ ComponentCollection& components, float radius, float sectionRadius, UINT subdivisionsAxis, UINT subdivisionsHeight, bool rhcoords);

	// Creates an m x n grid in the xz-plane with m rows and n columns, centered.
	void ComputePlane(_Inout_ VertexCollection& vertices, _Inout_ IndexCollection& indices, _Inout_ DirectX::BoundingBox& bounds,
		_Inout_ ComponentCollection& components, float width, float depth, UINT subdivisionsWidth, UINT subdivisionsDepth);

	void ComputePlaneEasy(_Inout_ VertexColor& vertices, _Inout_ IndexCollection& indices, _Inout_ DirectX::BoundingBox& bounds,
		float width, float depth, const DirectX::XMFLOAT4& color);

	void ComputeTriangle(_Inout_ VertexColor& vertices, _Inout_ IndexCollection& indices, _Inout_ DirectX::BoundingBox& bounds,
		const DirectX::XMFLOAT3& pointA, const DirectX::XMFLOAT3& pointB, const DirectX::XMFLOAT3& pointC, const DirectX::XMFLOAT4& color);

	void ComputeLineCircle(_Inout_ VertexColor& vertices, _Inout_ IndexCollection& indices,
		float radius, UINT quality, const DirectX::XMFLOAT4& color);

	void ComputeLine(_Inout_ VertexColor& vertices, _Inout_ IndexCollection& indices, 
		const DirectX::XMFLOAT3& pointA, const DirectX::XMFLOAT3& pointB, const DirectX::XMFLOAT4& color);
	

	void ComputePoint(_Inout_ VertexColor& vertices, _Inout_ IndexCollection& indices, 
		const DirectX::XMFLOAT3& point, const DirectX::XMFLOAT4& color);
}

#endif // !GEOMETRY_H