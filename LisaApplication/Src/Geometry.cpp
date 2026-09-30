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

#include "Geometry.h"
#include <MathHelper.h>
#include <numeric>

DirectX::XMFLOAT3 GeometryCalculation::RotationOfPointAAroundPointB2D(float aX, float aY, float bX, float bY, float angle, std::uint32_t definePlane)
{
	DirectX::XMFLOAT3 pos{};

	float degrees = DirectX::XM_PI * angle / 180;

	float a{}, b{}, c{};

	a = DirectX::XMScalarCos(degrees) * (aX - bX) - DirectX::XMScalarSin(degrees) * (bY - aY) + bX;
	b = DirectX::XMScalarSin(degrees) * (aX - bX) + DirectX::XMScalarCos(degrees) * (aY - bY) + bY;
	c = 0.0f;

	if (definePlane == 0)
	{
		pos = { a, b, c };
	}
	else if (definePlane == 1)
	{
		pos = { c, b, a };
	}
	else if (definePlane == 2)
	{
		pos = { a, c, b };
	}

	return pos;
}

DirectX::BoundingBox GeometryCalculation::BoundingBox(const float& width, const float& height, const float& depth)
{
	DirectX::XMVECTOR Center = DirectX::XMVectorSet(
		(-width + width) * 0.5f,
		(-height + height) * 0.5f,
		(-depth + depth) * 0.5f,
		0.0f
	);

	DirectX::XMVECTOR Extents = DirectX::XMVectorSet(width, height, depth, 0.0f);

	DirectX::BoundingBox bounds;

	DirectX::XMStoreFloat3(&bounds.Center, Center);
	DirectX::XMStoreFloat3(&bounds.Extents, Extents);

	return bounds;
}

void GeometryCalculation::DuplicatingVertices(_Inout_ VertexCollection& vertices, _Inout_ IndexCollection& indices, std::uint32_t& ind)
{
	size_t vtxSize = vertices.size();

	const auto& v0 = vertices[vtxSize - 1];
	vertices.push_back({ v0.position, v0.normal, v0.uv, v0.tangentU });
	indices.push_back(ind); ind++;

	const auto& v1 = vertices[vtxSize - 2];
	vertices.push_back({ v1.position, v1.normal, v1.uv, v1.tangentU });
	indices.push_back(ind); ind++;
}

/*DirectX::BoundingBox Geometry::BoundingBox(_In_ const std::vector<DirectX::XMVECTOR>& vertices)
{
	if (vertices.empty()) {
		// Handling the empty input case
		throw std::invalid_argument("Vertices collection is empty");
	}

	auto minX = std::min_element(vertices.begin(), vertices.end(),
		[](const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b) {
		return a.m128_f32[0] < b.m128_f32[0];
		})->m128_f32[0];

	auto maxX = std::max_element(vertices.begin(), vertices.end(),
		[](const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b) {
			return a.m128_f32[0] < b.m128_f32[0];
		})->m128_f32[0];


	auto minY = std::min_element(vertices.begin(), vertices.end(),
		[](const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b) {
			return a.m128_f32[1] < b.m128_f32[1];
		})->m128_f32[1];

	auto maxY = std::max_element(vertices.begin(), vertices.end(),
		[](const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b) {
			return a.m128_f32[1] < b.m128_f32[1];
		})->m128_f32[1];


	auto minZ = std::min_element(vertices.begin(), vertices.end(),
		[](const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b) {
			return a.m128_f32[2] < b.m128_f32[2];
		})->m128_f32[2];

	auto maxZ = std::max_element(vertices.begin(), vertices.end(),
		[](const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b) {
			return a.m128_f32[2] < b.m128_f32[2];
		})->m128_f32[2];


	DirectX::XMVECTOR Center = DirectX::XMVectorSet(
		(minX + maxX) * 0.5f,
		(minY + maxY) * 0.5f,
		(minZ + maxZ) * 0.5f,
		0.0f
	);

	float sideX = (maxX - minX) * 0.5f;
	float sideY = (maxY - minY) * 0.5f;
	float sideZ = (maxZ - minZ) * 0.5f;

	DirectX::XMVECTOR Extents = DirectX::XMVectorSet(sideX, sideY, sideZ, 0.0f);

	DirectX::BoundingBox bounds;

	DirectX::XMStoreFloat3(&bounds.Center, Center);
	DirectX::XMStoreFloat3(&bounds.Extents, Extents);

	return bounds;
}*/

GeometryCalculation::ComponentCollection GeometryCalculation::OrderingOfIndexes(const VertexCollection& vertices)
{
	std::unordered_multimap<
		std::tuple<INT, INT, INT>,
		std::pair<std::uint32_t, std::uint32_t>,
		HashTriple> storage;

	std::uint32_t triangle{};

	INT x{};
	INT y{};
	INT z{};

	for (std::uint32_t i = 0; i < vertices.size(); i += 3)
	{
		x = static_cast<INT>(vertices[i].position.x * 1e4f);
		y = static_cast<INT>(vertices[i].position.y * 1e4f);
		z = static_cast<INT>(vertices[i].position.z * 1e4f);

		storage.insert({ {x, y, z}, {i, triangle} });

		x = static_cast<INT>(vertices[i + 1uz].position.x * 1e4f);
		y = static_cast<INT>(vertices[i + 1uz].position.y * 1e4f);
		z = static_cast<INT>(vertices[i + 1uz].position.z * 1e4f);

		storage.insert({ {x, y, z}, {i + 1u, triangle} });

		x = static_cast<INT>(vertices[i + 2uz].position.x * 1e4f);
		y = static_cast<INT>(vertices[i + 2uz].position.y * 1e4f);
		z = static_cast<INT>(vertices[i + 2uz].position.z * 1e4f);

		storage.insert({ {x, y, z}, {i + 2u, triangle} });
		triangle++;
	}

	// triangle, { { index triangle } }

	GeometryCalculation::ComponentCollection umm;

	for (auto it = storage.begin(); it != storage.end();)
	{
		const std::tuple<INT, INT, INT>& currentKey = it->first;

		auto range = storage.equal_range(currentKey);
		size_t count = std::distance(range.first, range.second);

		std::vector<std::pair<std::uint32_t, std::uint32_t>> vec;
		vec.reserve(count);

		for (size_t i = 0; i < count; i++)
		{
			for (auto& j = range.first; j != range.second; ++j)
			{
				vec.emplace_back(j->second);
			}

			if (i != 0)
			{
				std::swap(vec[0], vec[i]);
			}

			umm.emplace(vec.begin()->second, vec);
		}

		it = range.second;
	}

	return std::move(umm);
}

// Helper function for creating a plane.
// The *side* parameter - 1 is front, top, back and bottom side.
// The *side* parameter - 0 is right and left side.
void GeometryCalculation::Plane(
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
)
{
	using namespace DirectX;
	using VertexType = VertexStructs::VertexPositionNormalTextureTangentU;

	XMVECTOR pos{};
	XMVECTOR uv{};
	uint32_t ind{ static_cast<uint32_t>(indices.size()) };

	const float segmentX{ XMVectorGetX(segment) };
	const float segmentY{ XMVectorGetY(segment) };
	const float segmentZ{ XMVectorGetZ(segment) };

	const float segmentUVX{ XMVectorGetX(segmentUV) };
	const float segmentUVY{ XMVectorGetY(segmentUV) };

	for (size_t h{ 0 }; h < row; h++)
	{
		const float heightPos{ segmentY * h };
		const float heightPlusPos{ segmentY * (1 + h) };

		const float heightUV{ segmentUVY * h };
		const float heightPlusUV{ segmentUVY * (1 + h) };

		for (size_t w{ 0 }; w < column; w++)
		{
			// We create two triangles that form a polygon.

			const float widthPos{ segmentX * w };
			const float widthPlusPos{ segmentX * (1 + w) };

			const float widthUV{ segmentUVX * w };
			const float widthPlusUV{ segmentUVX * (1 + w) };

			pos = XMVectorSet(widthPos, heightPos, segmentZ * (side ? h : w), 0.0f);
			uv = XMVectorSet(widthUV, heightUV, 0.0f, 1.0f);

			vertices.push_back(VertexType(XMVectorAdd(position, pos), nrm, XMVectorAdd(startUV, uv), tU ));
			indices.push_back(ind); ind++;

			pos = XMVectorSet(widthPlusPos, heightPos, segmentZ * (side ? h : (1 + w)), 0.0f);
			uv = XMVectorSet(widthPlusUV, heightUV, 0.0f, 1.0f);

			vertices.push_back(VertexType(XMVectorAdd(position, pos), nrm, XMVectorAdd(startUV, uv), tU ));
			indices.push_back(ind); ind++;

			pos = XMVectorSet(widthPos, heightPlusPos, segmentZ * (side ? (1 + h) : w), 0.0f);
			uv = XMVectorSet(widthUV, heightPlusUV, 0.0f, 1.0f);

			vertices.push_back(VertexType(XMVectorAdd(position, pos), nrm, XMVectorAdd(startUV, uv), tU ));
			indices.push_back(ind); ind++;

			size_t numVerticesInTriangle{ 3 };
			size_t vtxSize{ vertices.size() - numVerticesInTriangle };

			vertices.push_back(vertices[vtxSize + 2]);
			indices.push_back(ind); ind++;
			vertices.push_back(vertices[vtxSize + 1]);
			indices.push_back(ind); ind++;

			pos = XMVectorSet(widthPlusPos, heightPlusPos, segmentZ * (side ? (1 + h) : (1 + w)), 0.0f);
			uv = XMVectorSet(widthPlusUV, heightPlusUV, 0.0f, 1.0f);

			vertices.push_back(VertexType(XMVectorAdd(position, pos), nrm, XMVectorAdd(startUV, uv), tU ));
			indices.push_back(ind); ind++;
		}
	}
};

// The function calculates a grid that is located at the center of coordinates.
void GeometryCalculation::ComputeGrid(_Inout_ VertexCollection& vertices, _Inout_ IndexCollection& indices,
	float lengthAndWidth, float gridLines, size_t subdivisions
)
{
	using VertexType = VertexStructs::VertexPositionNormalTextureTangentU;

	vertices.clear();
	indices.clear();

	DirectX::XMVECTOR pos{};
	DirectX::XMVECTOR uv{};

	size_t numGridLines{ static_cast<size_t>(std::floorf(lengthAndWidth / gridLines)) };

	// Function for creating lines.
	auto const Lines = [&](size_t numLines, float lineSpacing, float lengthWidth, int xOrZAxis)
		{
			DirectX::XMFLOAT3 p{};
			DirectX::XMFLOAT2 uvf{};

			for (size_t i{ 1 }; i <= numLines; i++)
			{
				if (!xOrZAxis)
					p = { lineSpacing * i, 0.0f, lengthWidth };
				else
					p = { lengthWidth, 0.0f, lineSpacing * i };

				pos = DirectX::XMLoadFloat3(&p);

				uvf = { 0.0f, 0.0f };
				uv = DirectX::XMLoadFloat2(&uvf);

				vertices.push_back(VertexType(pos, DirectX::XMVECTOR{}, uv, DirectX::XMVECTOR{}));

				if (!xOrZAxis)
					p = { lineSpacing * i, 0.0f, -lengthWidth };
				else
					p = { -lengthWidth, 0.0f, lineSpacing * i };

				pos = DirectX::XMLoadFloat3(&p);

				uvf = { 1.0f, 1.0f };
				uv = DirectX::XMLoadFloat2(&uvf);

				vertices.push_back(VertexType(pos, DirectX::XMVECTOR{}, uv, DirectX::XMVECTOR{}));
			}
		};

	// Creating an Axis Intersection x and z.
	Lines(1, 0, lengthAndWidth, 0);
	Lines(1, 0, lengthAndWidth, 1);

	// Creating a contour +x.
	Lines(1, lengthAndWidth, lengthAndWidth, 0);
	// Creating a contour -x.
	Lines(1, -lengthAndWidth, lengthAndWidth, 0);
	// Creating a contour +z.
	Lines(1, lengthAndWidth, lengthAndWidth, 1);
	// Creating a contour -z.
	Lines(1, -lengthAndWidth, lengthAndWidth, 1);

	// Create grid lines every +x
	Lines(numGridLines, gridLines, lengthAndWidth, 0);
	// Create grid lines every -x
	Lines(numGridLines, -gridLines, lengthAndWidth, 0);
	// Create grid lines every +z
	Lines(numGridLines, gridLines, lengthAndWidth, 1);
	// Create grid lines every -z
	Lines(numGridLines, -gridLines, lengthAndWidth, 1);

	// Create subdivisions.
	float subSegment{ gridLines / subdivisions };
	float remainder{ lengthAndWidth - (gridLines * numGridLines) };

	size_t numRemainderLines{ static_cast<size_t>(std::floorf(remainder / subSegment)) };

	// Function for subdivisions.
	auto const SubLines = [&](size_t numLines, int negativePositiveAxis, float lengthWidth, int xOrZAxis)
		{
			for (size_t i{ 0 }; i <= numLines; i++)
			{
				if (i != numLines)
				{
					for (size_t s{ 1 }; s < subdivisions; s++)
					{
						Lines(1, ((subSegment * s) + (gridLines * i)) * negativePositiveAxis, lengthWidth, xOrZAxis);
					}
				}
				if (i == numLines)
				{
					for (size_t s{ 1 }; s < numRemainderLines; s++)
					{
						Lines(1, ((subSegment * s) + (gridLines * i)) * negativePositiveAxis, lengthWidth, xOrZAxis);
					}
				}
			}
		};

	// Create subdivisions +x
	SubLines(numGridLines, 1, lengthAndWidth, 0);
	// Create subdivisions -x
	SubLines(numGridLines, -1, lengthAndWidth, 0);
	// Create subdivisions +z
	SubLines(numGridLines, 1, lengthAndWidth, 1);
	// Create subdivisions -z
	SubLines(numGridLines, -1, lengthAndWidth, 1);


	// indices.
	for (uint32_t i{ 0 }; i < vertices.size() / 2; i++)
	{
		indices.push_back(i * 2 + 0);
		indices.push_back(i * 2 + 1);
	}
}

void GeometryCalculation::ComputeSphere(
	_Inout_ VertexCollection& vertices,
	_Inout_ IndexCollection& indices,
	_Inout_ DirectX::BoundingBox& bounds,
	_Inout_ ComponentCollection& components,
	float radius, 
	UINT subdivisionsAxis, 
	UINT subdivisionsHeight
)
{
	using namespace DirectX;
	using VertexType = VertexStructs::VertexPositionNormalTextureTangentU;

	vertices.clear();
	indices.clear();

	uint32_t ind{};
	VertexType v;

	// Compute the vertices stating at the top pole and moving down the stacks.
	//
	// Poles: note that there will be texture coordinate distortion as there is
	// not a unique point on the texture map to assign to the pole when mapping
	// a rectangular texture onto a sphere.
	const VertexType topVertex(
		XMFLOAT3{ 0.0f, radius, 0.0f },
		XMFLOAT3{ 0.0f, 1.0f, 0.0f },
		XMFLOAT2{ 0.0f, 0.0f },
		XMFLOAT3{ 1.0f, 0.0f, 0.0f }
	);
	const VertexType bottomVertex(
		XMFLOAT3{ 0.0f, -radius, 0.0f },
		XMFLOAT3{ 0.0f, -1.0f, 0.0f },
		XMFLOAT2{ 0.0f, 1.0f },
		XMFLOAT3{ 1.0f, 0.0f, 0.0f }
	);

	const float phiStep = XM_PI / subdivisionsHeight;
	const float thetaStep = 2.0f * XM_PI / subdivisionsAxis;

	auto const compute = [&](VertexType& v, const float& phi, const float& theta)
		{
			const auto& q = radius * sinf(phi) * cosf(theta);
			const auto& e = sinf(phi) * sinf(theta);

			// spherical to cartesian
			v.position.x = q;
			v.position.y = radius * cosf(phi);
			v.position.z = radius * e;

			// Partial derivative of P with respect to theta
			v.tangentU.x = -radius * e;
			v.tangentU.y = 0.0f;
			v.tangentU.z = q;

			XMVECTOR T = XMLoadFloat3(&v.tangentU);
			DirectX::XMStoreFloat3(&v.tangentU, XMVector3Normalize(T));

			XMVECTOR p = XMLoadFloat3(&v.position);
			DirectX::XMStoreFloat3(&v.normal, XMVector3Normalize(p));

			v.uv.x = theta / XM_2PI;
			v.uv.y = phi / XM_PI;
		};

	// Compute vertices for each stack ring (do not count the poles as rings).

	//Compute body

	for (std::uint32_t sH = 1; sH < subdivisionsHeight - 1; ++sH)
	{
		for (std::uint32_t sA = 0; sA < subdivisionsAxis; ++sA)
		{
			for (std::uint32_t i = 0; i < 2; ++i)
			{
				const float phi = ((subdivisionsHeight - sH) - i) * phiStep;

				// Vertices of ring.
				for (std::uint32_t j = 0; j < 2; ++j)
				{
					const float theta = (j + sA) * thetaStep;

					compute(v, phi, theta);

					vertices.push_back(v);
					indices.push_back(ind); ind++;

					if (i == 1 && j == 0)
					{
						DuplicatingVertices(vertices, indices, ind);
					}
				}
			}
		}
	}

	//Compute bottom cap

	for (std::uint32_t sA = 0; sA < subdivisionsAxis; ++sA)
	{
		const float phi = (1 + subdivisionsHeight) * phiStep;

		// Vertices of ring.
		for (std::int32_t j = 1; j >= 0; --j)
		{
			const float theta = (j + sA) * thetaStep;

			compute(v, phi, theta);

			vertices.push_back(v);
			indices.push_back(ind); ind++;
		}

		vertices.push_back(bottomVertex);
		indices.push_back(ind); ind++;
	}

	//Compute top cap

	for (std::uint32_t a = 0; a < subdivisionsAxis; ++a)
	{
		const float phi = 1 * phiStep;

		// Vertices of ring.
		for (std::uint32_t j = 0; j < 2; ++j)
		{
			const float theta = (j + a) * thetaStep;

			compute(v, phi, theta);

			vertices.push_back(v);
			indices.push_back(ind); ind++;
		}

		vertices.push_back(topVertex);
		indices.push_back(ind); ind++;
	}

	ComponentCollection umm = OrderingOfIndexes(vertices);
	
	// Find the bounding box of the geometry.
	bounds = BoundingBox(radius, radius, radius);
	components = std::move(umm);
}

void GeometryCalculation::ComputeGeoSphere(
	_Inout_ VertexCollection& vertices,
	_Inout_ IndexCollection& indices, 
	_Inout_ DirectX::BoundingBox& bounds,
	_Inout_ ComponentCollection& components,
	float radius, 
	UINT subdivisions,
	bool rhcoords
)
{
	size_t iTessellation{};

	if (subdivisions > 5)
		iTessellation = 5;
	else
		iTessellation = subdivisions;

	using namespace DirectX;
	using namespace VertexStructs;
	using VertexType = VertexStructs::VertexPositionNormalTextureTangentU;

	vertices.clear();
	indices.clear();

	// Create eight separate triangles.

	const std::vector<XMVECTOR> OctahedronVertices
	{
		// when looking down the negative z-axis (into the screen)
		{0.0f,  1.0f,  0.0f}, {0.0f,  0.0f, -1.0f}, {1.0f,  0.0f,  0.0f},
		{0.0f,  1.0f,  0.0f}, {1.0f,  0.0f,  0.0f}, {0.0f,  0.0f,  1.0f},
		{0.0f,  1.0f,  0.0f}, {0.0f,  0.0f,  1.0f}, {-1.0f,  0.0f,  0.0f},
		{0.0f,  1.0f,  0.0f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f, -1.0f},
		{0.0f,  -1.0f,  0.0f}, {1.0f,  0.0f,  0.0f}, {0.0f,  0.0f, -1.0f},
		{0.0f,  -1.0f,  0.0f}, {0.0f,  0.0f,  1.0f}, {1.0f,  0.0f,  0.0f},
		{0.0f,  -1.0f,  0.0f}, {-1.0f,  0.0f,  0.0f}, {0.0f,  0.0f,  1.0f},
		{0.0f,  -1.0f,  0.0f}, {0.0f,  0.0f,  -1.0f}, {-1.0f,  0.0f,  0.0f},
	};

	// Start with an octahedron; copy the data into the vertex collection.

	std::vector<XMVECTOR> vertexPositions;

	for (const auto& i : OctahedronVertices)
	{
		vertexPositions.push_back(i);
	}

	//       v0
	//       o
	//      / \
	//     / a \
	//tmp0o-----otmp2
	//   / \ b / \
	//  / c \ / d \
	// o-----o-----o
	// v1    tmp1  v2

	auto const generatePos = [](const std::vector<XMVECTOR>& inVertex, std::vector<XMVECTOR>& outVertex)
		{
			std::vector<XMVECTOR> tempVertex;
			tempVertex.reserve(inVertex.size());

			size_t n = inVertex.size();
			assert(n % 3 == 0);

			// Find new vertices on the edges of each triangle.

			for (size_t i = 0; i < n; i += 3)
			{
				tempVertex.push_back(XMVectorScale(XMVectorAdd(inVertex[i], inVertex[i + 1]), 0.5f));
				tempVertex.push_back(XMVectorScale(XMVectorAdd(inVertex[i + 1], inVertex[i + 2]), 0.5f));
				tempVertex.push_back(XMVectorScale(XMVectorAdd(inVertex[i + 2], inVertex[i]), 0.5f));
			}

			// Divide each triangle into four new triangles.

			for (size_t i = 0; i < n; i += 3)
			{
				// a
				outVertex.push_back(inVertex[i + 0]);
				outVertex.push_back(tempVertex[i]);
				outVertex.push_back(tempVertex[i + 2]);

				// b
				outVertex.push_back(tempVertex[i + 2]);
				outVertex.push_back(tempVertex[i]);
				outVertex.push_back(tempVertex[i + 1]);

				// c
				outVertex.push_back(tempVertex[i + 1]);
				outVertex.push_back(tempVertex[i + 0]);
				outVertex.push_back(inVertex[i + 1]);

				// d
				outVertex.push_back(tempVertex[i + 2]);
				outVertex.push_back(tempVertex[i + 1]);
				outVertex.push_back(inVertex[i + 2]);
			}
		};

	// Generating triangles.

	for (size_t i = 0; i < iTessellation; ++i)
	{
		std::vector<XMVECTOR> outVertex;
		outVertex.reserve(4 * vertexPositions.size());

		generatePos(vertexPositions, outVertex);

		std::swap(vertexPositions, outVertex);
	}

	// The number of indices is the same as the number of vertices.
	indices.reserve(vertexPositions.size());
	indices = std::ranges::to<std::vector<std::uint32_t>>(std::views::iota(0u, vertexPositions.size()));

	// Now that we've completed subdivision, fill in the final vertex collection
	vertices.reserve(vertexPositions.size());

	for (const auto& vPositions : vertexPositions)
	{
		const XMVECTOR normal = XMVector3Normalize(vPositions);
		const XMVECTOR pos = XMVectorScale(normal, radius);

		XMFLOAT3 normalFloat3;
		DirectX::XMStoreFloat3(&normalFloat3, normal);

		// calculate texture coordinates for this vertex
		const float longitude = atan2f(normalFloat3.x, -normalFloat3.z);
		const float latitude = acosf(normalFloat3.y);

		const float u = longitude / XM_2PI + 0.5f;
		const float v = latitude / XM_PI;

		auto const texcoord = XMVectorSet(1.0f - u, v, 0.0f, 0.0f);
		auto const tangentU = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

		vertices.emplace_back(pos, normal, texcoord, tangentU);
	}

	// Build RH above
	if (!rhcoords)
		ReverseWinding(vertices, indices);

	ComponentCollection umm = OrderingOfIndexes(vertices);

	// Find the bounding box of the geometry.
	bounds = BoundingBox(radius, radius, radius);
	components = std::move(umm);
}

void GeometryCalculation::ComputeGeoSphereCubeTex(_Inout_ VertexCollection& vertices, _Inout_ IndexCollection& indices, _Inout_ DirectX::BoundingBox& bounds,
	float radius, UINT subdivisions)
{
	using namespace DirectX;
	using namespace VertexStructs;

	vertices.clear();
	indices.clear();

	// Put a cap on the number of subdivisions.
	subdivisions = std::min<UINT>(subdivisions, 6u);

	// Approximate a sphere by tessellating an icosahedron.

	const float X = 0.525731f;
	const float Z = 0.850651f;

	XMFLOAT3 pos[12] =
	{
		XMFLOAT3(-X, 0.0f, Z),  XMFLOAT3(X, 0.0f, Z),
		XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
		XMFLOAT3(0.0f, Z, X),   XMFLOAT3(0.0f, Z, -X),
		XMFLOAT3(0.0f, -Z, X),  XMFLOAT3(0.0f, -Z, -X),
		XMFLOAT3(Z, X, 0.0f),   XMFLOAT3(-Z, X, 0.0f),
		XMFLOAT3(Z, -X, 0.0f),  XMFLOAT3(-Z, -X, 0.0f)
	};

	uint32_t k[60] =
	{
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
	};

	vertices.resize(12);
	indices.assign(&k[0], &k[60]);

	for (uint32_t i = 0; i < 12; ++i)
		vertices[i].position = pos[i];

	for (uint32_t i = 0; i < subdivisions; ++i)
	{
		//auto const divideEdge = [&](uint16_t i0, uint16_t i1, XMFLOAT3& outVertex, uint16_t& outIndex)
		auto const Subdivide = [&](VertexCollection& vtx, IndexCollection& ind)
			{
				// Save a copy of the input geometry.
				//MeshData inputCopy = meshData;
				using VertexType = VertexStructs::VertexPositionNormalTextureTangentU;

				VertexCollection vertexCopy = vtx;
				IndexCollection indexCopy = ind;

				vtx.resize(0);
				ind.resize(0);

				//       v1
				//       *
				//      / \
				//     /   \
				//  m0*-----*m1
				//   / \   / \
				//  /   \ /   \
				// *-----*-----*
				// v0    m2     v2

				uint32_t numTris = static_cast<uint32_t>(indexCopy.size() / 3);
				for (uint32_t i = 0; i < numTris; ++i)
				{
					VertexType v0 = vertexCopy[indexCopy[static_cast<size_t>(i) * 3 + 0]];
					VertexType v1 = vertexCopy[indexCopy[static_cast<size_t>(i) * 3 + 1]];
					VertexType v2 = vertexCopy[indexCopy[static_cast<size_t>(i) * 3 + 2]];

					// Generate the midpoints.

					auto const MidPoint = [&](const VertexType& v0, const VertexType& v1)
						{
							XMVECTOR p0 = XMLoadFloat3(&v0.position);
							XMVECTOR p1 = XMLoadFloat3(&v1.position);

							XMVECTOR n0 = XMLoadFloat3(&v0.normal);
							XMVECTOR n1 = XMLoadFloat3(&v1.normal);

							XMVECTOR tan0 = XMLoadFloat3(&v0.tangentU);
							XMVECTOR tan1 = XMLoadFloat3(&v1.tangentU);

							XMVECTOR tex0 = XMLoadFloat2(&v0.uv);
							XMVECTOR tex1 = XMLoadFloat2(&v1.uv);

							// Compute the midpoints of all the attributes.  Vectors need to be normalized
							// since linear interpolating can make them not unit length.  
							XMVECTOR pos = 0.5f * (p0 + p1);
							XMVECTOR normal = XMVector3Normalize(0.5f * (n0 + n1));
							XMVECTOR tangent = XMVector3Normalize(0.5f * (tan0 + tan1));
							XMVECTOR tex = 0.5f * (tex0 + tex1);

							VertexType v;
							DirectX::XMStoreFloat3(&v.position, pos);
							DirectX::XMStoreFloat3(&v.normal, normal);
							DirectX::XMStoreFloat3(&v.tangentU, tangent);
							DirectX::XMStoreFloat2(&v.uv, tex);

							return v;
						};

					VertexType m0 = MidPoint(v0, v1);
					VertexType m1 = MidPoint(v1, v2);
					VertexType m2 = MidPoint(v0, v2);

					// Add new geometry.

					vtx.push_back(v0); // 0
					vtx.push_back(v1); // 1
					vtx.push_back(v2); // 2
					vtx.push_back(m0); // 3
					vtx.push_back(m1); // 4
					vtx.push_back(m2); // 5

					ind.push_back(i * 6 + 0);
					ind.push_back(i * 6 + 3);
					ind.push_back(i * 6 + 5);

					ind.push_back(i * 6 + 3);
					ind.push_back(i * 6 + 4);
					ind.push_back(i * 6 + 5);

					ind.push_back(i * 6 + 5);
					ind.push_back(i * 6 + 4);
					ind.push_back(i * 6 + 2);

					ind.push_back(i * 6 + 3);
					ind.push_back(i * 6 + 1);
					ind.push_back(i * 6 + 4);
				}
			};

		Subdivide(vertices, indices);
	}

	// Project vertices onto sphere and scale.
	for (uint32_t i = 0; i < vertices.size(); ++i)
	{
		// Project onto unit sphere.
		XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&vertices[i].position));

		// Project onto sphere.
		XMVECTOR p = radius * n;

		DirectX::XMStoreFloat3(&vertices[i].position, p);
		DirectX::XMStoreFloat3(&vertices[i].normal, n);

		// Derive texture coordinates from spherical coordinates.
		float theta = atan2f(vertices[i].position.z, vertices[i].position.x);

		// Put in [0, 2pi].
		if (theta < 0.0f)
			theta += XM_2PI;

		float phi = acosf(vertices[i].position.y / radius);

		vertices[i].uv.x = theta / XM_2PI;
		vertices[i].uv.y = phi / XM_PI;

		// Partial derivative of P with respect to theta
		vertices[i].tangentU.x = -radius * sinf(phi) * sinf(theta);
		vertices[i].tangentU.y = 0.0f;
		vertices[i].tangentU.z = +radius * sinf(phi) * cosf(theta);

		XMVECTOR T = XMLoadFloat3(&vertices[i].tangentU);
		DirectX::XMStoreFloat3(&vertices[i].tangentU, XMVector3Normalize(T));
	}

	// Find the bounding box of the geometry.
	bounds = BoundingBox(radius, radius, radius);
}

void GeometryCalculation::ComputeGeoSphereEasy(_Inout_ VertexColor& vertices, _Inout_ IndexCollection& indices, _Inout_ DirectX::BoundingBox& bounds,
	float radius, UINT subdivisions, const DirectX::XMFLOAT4& color)
{
	using namespace DirectX;
	using namespace VertexStructs;

	vertices.clear();
	indices.clear();

	// Put a cap on the number of subdivisions.
	subdivisions = std::min<UINT>(subdivisions, 6u);

	// Approximate a sphere by tessellating an icosahedron.

	const float X = 0.525731f;
	const float Z = 0.850651f;

	XMFLOAT3 pos[12] =
	{
		XMFLOAT3(-X, 0.0f, Z),  XMFLOAT3(X, 0.0f, Z),
		XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
		XMFLOAT3(0.0f, Z, X),   XMFLOAT3(0.0f, Z, -X),
		XMFLOAT3(0.0f, -Z, X),  XMFLOAT3(0.0f, -Z, -X),
		XMFLOAT3(Z, X, 0.0f),   XMFLOAT3(-Z, X, 0.0f),
		XMFLOAT3(Z, -X, 0.0f),  XMFLOAT3(-Z, -X, 0.0f)
	};

	uint32_t k[60] =
	{
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
	};

	vertices.resize(12);
	indices.assign(&k[0], &k[60]);

	for (uint32_t i = 0; i < 12; ++i)
		vertices[i].position = pos[i];

	for (uint32_t i = 0; i < subdivisions; ++i)
	{
		//auto const divideEdge = [&](uint16_t i0, uint16_t i1, XMFLOAT3& outVertex, uint16_t& outIndex)
		auto const Subdivide = [&](VertexColor& vtx, IndexCollection& ind)
			{
				// Save a copy of the input geometry.
				//MeshData inputCopy = meshData;
				using VertexType = VertexStructs::VertexPositionColor;

				VertexColor vertexCopy = vtx;
				IndexCollection indexCopy = ind;

				vtx.resize(0);
				ind.resize(0);

				//       v1
				//       *
				//      / \
				//     /   \
				//  m0*-----*m1
				//   / \   / \
				//  /   \ /   \
				// *-----*-----*
				// v0    m2     v2

				uint32_t numTris = static_cast<uint32_t>(indexCopy.size() / 3);
				for (uint32_t i = 0; i < numTris; ++i)
				{
					VertexType v0 = vertexCopy[indexCopy[static_cast<size_t>(i) * 3 + 0]];
					VertexType v1 = vertexCopy[indexCopy[static_cast<size_t>(i) * 3 + 1]];
					VertexType v2 = vertexCopy[indexCopy[static_cast<size_t>(i) * 3 + 2]];

					// Generate the midpoints.

					auto const MidPoint = [&](const VertexType& v0, const VertexType& v1)
						{
							XMVECTOR p0 = XMLoadFloat3(&v0.position);
							XMVECTOR p1 = XMLoadFloat3(&v1.position);

							// Compute the midpoints of all the attributes.  Vectors need to be normalized
							// since linear interpolating can make them not unit length.  
							XMVECTOR pos = 0.5f * (p0 + p1);

							VertexType v;
							DirectX::XMStoreFloat3(&v.position, pos);
							v.color = color;

							return v;
						};

					VertexType m0 = MidPoint(v0, v1);
					VertexType m1 = MidPoint(v1, v2);
					VertexType m2 = MidPoint(v0, v2);

					// Add new geometry.

					vtx.push_back(v0); // 0
					vtx.push_back(v1); // 1
					vtx.push_back(v2); // 2
					vtx.push_back(m0); // 3
					vtx.push_back(m1); // 4
					vtx.push_back(m2); // 5

					ind.push_back(i * 6 + 0);
					ind.push_back(i * 6 + 3);
					ind.push_back(i * 6 + 5);

					ind.push_back(i * 6 + 3);
					ind.push_back(i * 6 + 4);
					ind.push_back(i * 6 + 5);

					ind.push_back(i * 6 + 5);
					ind.push_back(i * 6 + 4);
					ind.push_back(i * 6 + 2);

					ind.push_back(i * 6 + 3);
					ind.push_back(i * 6 + 1);
					ind.push_back(i * 6 + 4);
				}
			};

		Subdivide(vertices, indices);
	}

	// Project vertices onto sphere and scale.
	for (uint32_t i = 0; i < vertices.size(); ++i)
	{
		// Project onto unit sphere.
		XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&vertices[i].position));

		// Project onto sphere.
		XMVECTOR p = radius * n;

		DirectX::XMStoreFloat3(&vertices[i].position, p);
		vertices[i].color = color;
	}

	// Find the bounding box of the geometry.
	bounds = BoundingBox(radius, radius, radius);
}

void GeometryCalculation::ComputeBox(
	_Inout_ VertexCollection& vertices,
	_Inout_ IndexCollection& indices, 
	_Inout_ DirectX::BoundingBox& bounds,
	_Inout_ ComponentCollection& components,
	float width, 
	float height, 
	float depth, 
	UINT subdivisionsWidth, 
	UINT subdivisionsHeight,
	UINT subdivisionsDepth
)
{
	using namespace DirectX;
	using VertexType = VertexStructs::VertexPositionNormalTextureTangentU;

	vertices.clear();
	indices.clear();

	const float w2{ 0.5f * width };
	const float h2{ 0.5f * height };
	const float d2{ 0.5f * depth };

	// Size of one side in "UV" coordinates by height and width. 
	// Taken from the program "Autodesk Maya".
	const float fullSegmentUV{ 0.25 };

	const float wSegment{ width / subdivisionsWidth };
	const float hSegment{ height / subdivisionsHeight };
	const float dSegment{ depth / subdivisionsDepth };

	const float wSegmentUV{ fullSegmentUV / subdivisionsWidth };
	const float hSegmentUV{ fullSegmentUV / subdivisionsHeight };
	const float dSegmentUV{ fullSegmentUV / subdivisionsDepth };

	// The values for the UV coordinates for the variable *startUV* are also taken from the Autodesk Maya program.

	// Fill in the front face vertex data.
	Plane(vertices, indices,
		{ -w2, -h2, -d2 }, { wSegment, hSegment, 0.0f },
		{ 0.0f, 0.0f, -1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f },
		{ 0.375, 0.0f }, { wSegmentUV, hSegmentUV },
		subdivisionsHeight, subdivisionsWidth, 1);

	// Fill in the top face vertex data.
	Plane(vertices, indices,
		{ -w2, h2, -d2 }, { wSegment, 0.0f, dSegment },
		{ 0.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f },
		{ 0.375, 0.25f }, { wSegmentUV, dSegmentUV },
		subdivisionsDepth, subdivisionsWidth, 1);

	// Fill in the back face vertex data.
	Plane(vertices, indices,
		{ -w2, h2, d2 }, { wSegment, -hSegment, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 1.0f }, { -1.0f, 0.0f, 0.0f, 1.0f },
		{ 0.375f, 0.50f }, { wSegmentUV, hSegmentUV },
		subdivisionsHeight, subdivisionsWidth, 1);

	// Fill in the bottom face vertex data.
	Plane(vertices, indices,
		{ -w2, -h2, d2 }, { wSegment, 0.0f, -dSegment },
		{ 0.0f, -1.0f, 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f, 1.0f },
		{ 0.375f, 0.75f }, { wSegmentUV, dSegmentUV },
		subdivisionsDepth, subdivisionsWidth, 1);

	// Fill in the right face vertex data.
	Plane(vertices, indices,
		{ w2, -h2, -d2 }, { 0.0f, hSegment, dSegment },
		{ 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f },
		{ 0.625f, 0.0f }, { dSegmentUV, hSegmentUV },
		subdivisionsHeight, subdivisionsDepth, 0);

	// Fill in the left face vertex data.
	Plane(vertices, indices,
		{ -w2, -h2, d2 }, { 0.0f, hSegment, -dSegment },
		{ -1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f, 1.0f },
		{ 0.125f, 0.0f }, { dSegmentUV, hSegmentUV },
		subdivisionsHeight, subdivisionsDepth, 0);

	ComponentCollection umm = OrderingOfIndexes(vertices);

	// Find the bounding box of the geometry.
	bounds = BoundingBox(w2, h2, d2);
	components = std::move(umm);
}

void GeometryCalculation::ComputeBoxEasy(_Inout_ VertexColor& vertices, _Inout_ IndexCollection& indices, _Inout_ DirectX::BoundingBox& bounds,
	float width, float height, float depth, const DirectX::XMFLOAT4& color
)
{
	using namespace DirectX;
	using VertexType = VertexStructs::VertexPositionColor;

	vertices.clear();
	indices.clear();

	const float w2{ 0.5f * width };
	const float h2{ 0.5f * height };
	const float d2{ 0.5f * depth };

	// Fill in the front face vertex data.

	vertices.push_back(VertexType({ -w2, -h2, -d2 }, color));
	vertices.push_back(VertexType({ w2, -h2, -d2 }, color));
	vertices.push_back(VertexType({ -w2, h2, -d2 }, color));
	vertices.push_back(VertexType({ w2, h2, -d2 }, color));

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	indices.push_back(2);
	indices.push_back(1);
	indices.push_back(3);


	// Fill in the top face vertex data.

	vertices.push_back(VertexType({ -w2, h2, d2 }, color));
	vertices.push_back(VertexType({ w2, h2, d2 }, color));

	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(4);

	indices.push_back(4);
	indices.push_back(3);
	indices.push_back(5);

	// Fill in the back face vertex data.

	vertices.push_back(VertexType({ -w2, -h2, d2 }, color));
	vertices.push_back(VertexType({ w2, -h2, d2 }, color));

	indices.push_back(4);
	indices.push_back(5);
	indices.push_back(6);

	indices.push_back(6);
	indices.push_back(5);
	indices.push_back(7);

	// Fill in the bottom face vertex data.

	indices.push_back(6);
	indices.push_back(7);
	indices.push_back(0);

	indices.push_back(0);
	indices.push_back(7);
	indices.push_back(1);

	// Fill in the left face vertex data.

	indices.push_back(2);
	indices.push_back(4);
	indices.push_back(6);

	indices.push_back(2);
	indices.push_back(6);
	indices.push_back(0);

	// Fill in the right face vertex data.

	indices.push_back(5);
	indices.push_back(1);
	indices.push_back(3);

	indices.push_back(5);
	indices.push_back(1);
	indices.push_back(7);

	// Find the bounding box of the geometry.
	bounds = BoundingBox(w2, h2, d2);
}

void GeometryCalculation::ComputeCylinder(
	_Inout_ VertexCollection& vertices,
	_Inout_ IndexCollection& indices, 
	_Inout_ DirectX::BoundingBox& bounds,
	_Inout_ ComponentCollection& components,
	float radius, 
	float height, 
	UINT subdivisionsAxis, 
	UINT subdivisionsHeight,
	UINT subdivisionsCaps
)
{
	using namespace DirectX;

	using VertexType = VertexStructs::VertexPositionNormalTextureTangentU;
	vertices.clear();
	indices.clear();

	std::uint32_t ind{};
	VertexType vt;

	const float h2{ height * 0.5f };
	const float hSegment{ height / subdivisionsHeight };
	const float wSegmentUV{ 1.0f / subdivisionsAxis };
	const float hSegmentUV{ 1.0f / subdivisionsHeight };
	const float dTheta{ 2.0f * XM_PI / subdivisionsAxis };
	const float capsStep{ radius / subdivisionsCaps };
	const float y{ 0.5f * height };

	const VertexType topVertex(
		XMFLOAT3{ 0.0f, y, 0.0f },
		XMFLOAT3{ 0.0f, 1.0f, 0.0f },
		XMFLOAT2{ 0.5f, 0.5f },
		XMFLOAT3{ 1.0f, 0.0f, 0.0f }
	);
	const VertexType bottomVertex(
		XMFLOAT3{ 0.0f, -y, 0.0f },
		XMFLOAT3{ 0.0f, -1.0f, 0.0f },
		XMFLOAT2{ 0.5f, 0.5f },
		XMFLOAT3{ 1.0f, 0.0f, 0.0f }
	);

	// Build cylinder bottom cap

	for (size_t sC{ 1 }; sC < subdivisionsCaps; ++sC)
	{
		for (size_t sA{ 0 }; sA < subdivisionsAxis; ++sA)
		{
			// Duplicate cap ring vertices because the texture coordinates and normals differ.
			for (size_t i{ 0 }; i < 2; ++i)
			{
				for (size_t j{ 0 }; j < 2; j++)
				{
					const float x{ (capsStep * (i + sC)) * cosf((j + sA) * dTheta) };
					const float z{ (capsStep * (i + sC)) * sinf((j + sA) * dTheta) };

					// Scale down by the height to try and make top cap texture coord area
					// proportional to base.
					const float u{ x / height + 0.5f };
					const float v{ z / height + 0.5f };

					vt.position = { x, -y, z };
					vt.normal = { 0.0f, -1.0f, 0.0f };
					vt.uv = { u, v };
					vt.tangentU = { 1.0f, 0.0f, 0.0f };

					vertices.push_back(vt);
					indices.push_back(ind); ind++;

					if (i == 1 && j == 0)
					{
						DuplicatingVertices(vertices, indices, ind);
					}
				}
			}
		}
	}

	// Compute vertices for each stack ring starting at the bottom and moving up.

	for (size_t sH{ 0 }; sH < subdivisionsHeight; ++sH)
	{
		for (size_t sA{ 0 }; sA < subdivisionsAxis; sA++)
		{
			// Create two triangles.
			VertexType vBuffer[2]{};

			// First triangle.

			for (size_t j{ 0 }; j < 2; ++j)
			{
				const float c{ cosf((j + sA) * dTheta) };
				const float s{ sinf((j + sA) * dTheta) };

				vt.position = { radius * c, -h2 + (hSegment * sH), radius * s };
				vt.uv = { wSegmentUV * (j + sA), hSegmentUV * sH };
				// This is unit length.
				vt.tangentU = { -s, 0.0f, c };

				XMVECTOR bitangent{ XMVectorSet(radius * c, -height, radius * s, 0.0f) };

				XMVECTOR T{ vt.tangentU.x, vt.tangentU.y, vt.tangentU.z };
				XMVECTOR B{ bitangent };

				DirectX::XMStoreFloat3(&vt.normal, XMVector3Normalize(XMVector3Cross(T, B)));

				vertices.push_back(vt);
				vBuffer[j] = vt;

				indices.push_back(ind); ind++;
			}

			vt.position.x = vBuffer[0].position.x;
			vt.position.y = vBuffer[0].position.y + hSegment;
			vt.position.z = vBuffer[0].position.z;

			vt.normal.x = vBuffer[0].normal.x;
			vt.normal.y = vBuffer[0].normal.y + hSegment;
			vt.normal.z = vBuffer[0].normal.z;

			vt.uv.x = vBuffer[0].uv.x;
			vt.uv.y = vBuffer[0].uv.y + hSegmentUV;

			vertices.push_back(vt);
			indices.push_back(ind); ind++;

			// Second triangle.

			vertices.push_back({ vt.position, vt.normal, vt.uv, vt.tangentU });
			indices.push_back(ind); ind++;

			vertices.push_back({ vBuffer[1].position, vBuffer[1].normal, vBuffer[1].uv, vBuffer[1].tangentU });
			indices.push_back(ind); ind++;

			vt.position.x = vBuffer[1].position.x;
			vt.position.y = vBuffer[1].position.y + hSegment;
			vt.position.z = vBuffer[1].position.z;

			vt.normal.x = vBuffer[1].normal.x;
			vt.normal.y = vBuffer[1].normal.y + hSegment;
			vt.normal.z = vBuffer[1].normal.z;

			vt.uv.x = vBuffer[1].uv.x;
			vt.uv.y = vBuffer[1].uv.y + hSegmentUV;

			vertices.push_back(vt);
			indices.push_back(ind); ind++;
		}
	}

	// Build cylinder top cap

	for (std::uint32_t sC{ 0 }; sC < subdivisionsCaps - 1; ++sC)
	{
		for (std::uint32_t sA{ 0 }; sA < subdivisionsAxis; ++sA)
		{
			// Duplicate cap ring vertices because the texture coordinates and normals differ.
			for (std::uint32_t i{ 0 }; i < 2; ++i)
			{
				for (std::uint32_t j{ 0 }; j < 2; j++)
				{
					const float x{ (capsStep * ((subdivisionsCaps - sC) - i)) * cosf((j + sA) * dTheta) };
					const float z{ (capsStep * ((subdivisionsCaps - sC) - i)) * sinf((j + sA) * dTheta) };

					// Scale down by the height to try and make top cap texture coord area
					// proportional to base.
					const float u{ x / height + 0.5f };
					const float v{ z / height + 0.5f };


					vt.position = { x, y, z };
					vt.normal = { 0.0f, 1.0f, 0.0f };
					vt.uv = { u, v };
					vt.tangentU = { 1.0f, 0.0f, 0.0f };

					vertices.push_back(vt);
					indices.push_back(ind); ind++;

					if (i == 1 && j == 0)
					{
						DuplicatingVertices(vertices, indices, ind);
					}
				}
			}
		}
	}

	// Bottom cap center vertex.

	for (std::uint32_t sA{ 0 }; sA < subdivisionsAxis; ++sA)
	{
		for (std::int32_t j = 1; j >= 0; --j)
		{
			const float x{ capsStep * cosf((j + sA) * dTheta) };
			const float z{ capsStep * sinf((j + sA) * dTheta) };

			// Scale down by the height to try and make top cap texture coord area
			// proportional to base.
			const float u{ x / height + 0.5f };
			const float v{ z / height + 0.5f };


			vt.position = { x, -y, z };
			vt.normal = { 0.0f, -1.0f, 0.0f };
			vt.uv = { u, v };
			vt.tangentU = { 1.0f, 0.0f, 0.0f };

			vertices.push_back(vt);
			indices.push_back(ind); ind++;
		}

		vertices.push_back(bottomVertex);
		indices.push_back(ind); ind++;
	}

	// Top cap center vertex.

	for (std::uint32_t sA{ 0 }; sA < subdivisionsAxis; ++sA)
	{
		for (std::uint32_t j = 0; j < 2; ++j)
		{
			const float x{ capsStep * cosf((j + sA) * dTheta) };
			const float z{ capsStep * sinf((j + sA) * dTheta) };

			// Scale down by the height to try and make top cap texture coord area
			// proportional to base.
			const float u{ x / height + 0.5f };
			const float v{ z / height + 0.5f };


			vt.position = { x, y, z };
			vt.normal = { 0.0f, 1.0f, 0.0f };
			vt.uv = { u, v };
			vt.tangentU = { 1.0f, 0.0f, 0.0f };

			vertices.push_back(vt);
			indices.push_back(ind); ind++;
		}

		vertices.push_back(topVertex);
		indices.push_back(ind); ind++;
	}

	ComponentCollection umm = OrderingOfIndexes(vertices);

	// Find the bounding box of the geometry.
	bounds = BoundingBox(radius, h2, radius);
	components = std::move(umm);
}

void GeometryCalculation::ComputeDisc(_Inout_ VertexColor& vertices, _Inout_ IndexCollection& indices, _Inout_ DirectX::BoundingBox& bounds,
	float radius, UINT subdivisionsAxis, const DirectX::XMFLOAT4& color)
{
	using namespace DirectX;

	using VertexType = VertexStructs::VertexPositionColor;
	vertices.clear();
	indices.clear();

	std::uint32_t ind{};
	VertexType vt;

	const float dTheta{ 2.0f * XM_PI / subdivisionsAxis };
	const VertexType topVertex(XMFLOAT3{ 0.0f, 0.0f, 0.0f }, color);

	for (std::uint32_t sA{ 0 }; sA < subdivisionsAxis; ++sA)
	{
		for (std::uint32_t j = 0; j < 2; ++j)
		{
			const float x{ radius * cosf((j + sA) * dTheta) };
			const float z{ radius * sinf((j + sA) * dTheta) };

			vt.position = { x, 0.0f, z };
			vt.color = color;

			vertices.push_back(vt);
			indices.push_back(ind); ind++;
		}

		vertices.push_back(topVertex);
		indices.push_back(ind); ind++;
	}

	// Find the bounding box of the geometry.
	bounds = BoundingBox(radius, 0.0f, radius);
}

void GeometryCalculation::ComputeCone(
	_Inout_ VertexCollection& vertices,
	_Inout_ IndexCollection& indices, 
	_Inout_ DirectX::BoundingBox& bounds,
	_Inout_ ComponentCollection& components,
	float radius, 
	float height, 
	UINT subdivisionsAxis,
	UINT subdivisionsHeight, 
	UINT subdivisionsCaps
)
{
	using namespace DirectX;

	using VertexType = VertexStructs::VertexPositionNormalTextureTangentU;
	vertices.clear();
	indices.clear();

	uint32_t ind{};
	VertexType vt;

	float hSegment = height / subdivisionsHeight;
	// Amount to increment radius as we move up each stack level from bottom to top.
	float radiusStep = (0.0f - radius) / subdivisionsHeight;
	float dTheta{ 2.0f * XM_PI / subdivisionsAxis };
	const float capsStep{ radius / subdivisionsCaps };

	const VertexType bottomVertex(
		XMFLOAT3{ 0.0f, -0.5f * height, 0.0f },
		XMFLOAT3{ 0.0f, -1.0f, 0.0f },
		XMFLOAT2{ 0.5f, 0.5f },
		XMFLOAT3{ 1.0f, 0.0f, 0.0f }
	);

	// Build cylinder bottom cap

	for (size_t sC{ 1 }; sC < subdivisionsCaps; ++sC)
	{
		for (size_t sA{ 0 }; sA < subdivisionsAxis; ++sA)
		{
			// Duplicate cap ring vertices because the texture coordinates and normals differ.
			for (size_t i{ 0 }; i < 2; ++i)
			{
				for (size_t j{ 0 }; j < 2; j++)
				{
					const float x{ (capsStep * (i + sC)) * cosf((j + sA) * dTheta) };
					const float z{ (capsStep * (i + sC)) * sinf((j + sA) * dTheta) };

					// Scale down by the height to try and make top cap texture coord area
					// proportional to base.
					const float u{ x / height + 0.5f };
					const float v{ z / height + 0.5f };


					vt.position = { x, -0.5f * height, z };
					vt.normal = { 0.0f, -1.0f, 0.0f };
					vt.uv = { u, v };
					vt.tangentU = { 1.0f, 0.0f, 0.0f };

					vertices.push_back(vt);
					indices.push_back(ind); ind++;

					if (i == 1 && j == 0)
					{
						DuplicatingVertices(vertices, indices, ind);
					}
				}
			}
		}
	}

	// Construction of the body of a cone.

	for (std::uint32_t sH{ 0 }; sH < subdivisionsHeight - 1; ++sH)
	{
		for (std::uint32_t sA{ 0 }; sA < subdivisionsAxis; ++sA)
		{
			// Compute vertices for each stack ring starting at the bottom and moving up.
			for (std::uint32_t i{ 0 }; i < 2; ++i)
			{
				float y{ -0.5f * height + (i + sH) * hSegment };
				float r{ radius + (i + sH) * radiusStep };

				for (std::uint32_t j{ 0 }; j < 2; ++j)
				{
					float c{ cosf((j + sA) * dTheta) };
					float s{ sinf((j + sA) * dTheta) };


					vt.position = { r * c, y, r * s };
					vt.uv = { (float)(j + sA) / subdivisionsAxis, 1.0f - (float)(i + sH) / subdivisionsHeight };
					vt.tangentU = { -s, 0.0f, c };

					XMVECTOR bitangent = XMVectorSet(radius * c, -height, radius * s, 0.0f);
					XMVECTOR T = { vt.tangentU.x, vt.tangentU.y, vt.tangentU.z };
					XMVECTOR B = bitangent;

					DirectX::XMStoreFloat3(&vt.normal, XMVector3Normalize(XMVector3Cross(T, B)));

					vertices.push_back(vt);
					indices.push_back(ind); ind++;

					if (i == 1 && j == 0)
					{
						DuplicatingVertices(vertices, indices, ind);
					}
				}
			}
		}
	}

	// Bottom cap center vertex.

	for (std::uint32_t sA{ 0 }; sA < subdivisionsAxis; ++sA)
	{
		for (std::int32_t j = 1; j >= 0; --j)
		{
			const float x{ capsStep * cosf((j + sA) * dTheta) };
			const float z{ capsStep * sinf((j + sA) * dTheta) };

			// Scale down by the height to try and make top cap texture coord area
			// proportional to base.
			const float u{ x / height + 0.5f };
			const float v{ z / height + 0.5f };


			vt.position = { x, -0.5f * height, z };
			vt.normal = { 0.0f, -1.0f, 0.0f };
			vt.uv = { u, v };
			vt.tangentU = { 1.0f, 0.0f, 0.0f };

			vertices.push_back(vt);
			indices.push_back(ind); ind++;
		}

		vertices.push_back(bottomVertex);
		indices.push_back(ind); ind++;
	}

	// Create center cone vertex.

	for (std::uint32_t sA{ 0 }; sA < subdivisionsAxis; ++sA)
	{
		// Compute vertices for each stack ring starting at the bottom and moving up.

		float y{ -0.5f * height + (subdivisionsHeight - 1) * hSegment };
		float r{ radius + (subdivisionsHeight - 1) * radiusStep };

		for (std::uint32_t j{ 0 }; j < 2; ++j)
		{
			float c{ cosf((j + sA) * dTheta) };
			float s{ sinf((j + sA) * dTheta) };

			vt.position = { r * c, y, r * s };
			vt.uv = { (float)(j + sA) / subdivisionsAxis, 1.0f - (float)(subdivisionsHeight - 1) / subdivisionsHeight };
			vt.tangentU = { -s, 0.0f, c };

			// This is unit length.
			XMVECTOR bitangent = XMVectorSet(radius * c, -height, radius * s, 0.0f);
			XMVECTOR T = { vt.tangentU.x, vt.tangentU.y, vt.tangentU.z };
			XMVECTOR B = bitangent;

			DirectX::XMStoreFloat3(&vt.normal, XMVector3Normalize(XMVector3Cross(T, B)));

			vertices.push_back(vt);
			indices.push_back(ind); ind++;
		}

		vt.position = { 0.0f, 0.5f * height, 0.0f };
		vt.normal = { 0.0f, 1.0f, 0.0f };
		vt.uv = { (float)sA / subdivisionsAxis, 1.0f - (float)subdivisionsHeight / subdivisionsHeight };
		vt.tangentU = { 1.0f, 0.0f, 0.0f };

		vertices.push_back(vt);
		indices.push_back(ind); ind++;
	}

	ComponentCollection umm = OrderingOfIndexes(vertices);

	// Find the bounding box of the geometry.
	bounds = BoundingBox(radius, height * 0.5f, radius);
	components = std::move(umm);
}

void GeometryCalculation::ComputeConeEasy(_Inout_ VertexColor& vertices, _Inout_ IndexCollection& indices, _Inout_ DirectX::BoundingBox& bounds,
	float radius, float height, UINT subdivisionsAxis, const DirectX::XMFLOAT4& color)
{
	using namespace DirectX;

	using VertexType = VertexStructs::VertexPositionColor;
	vertices.clear();
	indices.clear();

	uint32_t ind{};
	VertexType vt;

	// Amount to increment radius as we move up each stack level from bottom to top.
	float dTheta{ 2.0f * XM_PI / subdivisionsAxis };

	const VertexType bottomVertex(XMFLOAT3{ 0.0f, -0.5f * height, 0.0f }, color);

	// Bottom cap center vertex.

	for (std::uint32_t sA{ 0 }; sA < subdivisionsAxis; ++sA)
	{
		for (std::int32_t j = 1; j >= 0; --j)
		{
			const float x{ radius * cosf((j + sA) * dTheta) };
			const float z{ radius * sinf((j + sA) * dTheta) };

			vt.position = { x, -0.5f * height, z };
			vt.color = color;

			vertices.push_back(vt);
			indices.push_back(ind); ind++;
		}

		vertices.push_back(bottomVertex);
		indices.push_back(ind); ind++;
	}

	// Create center cone vertex.

	for (std::uint32_t sA{ 0 }; sA < subdivisionsAxis; ++sA)
	{
		// Compute vertices for each stack ring starting at the bottom and moving up.

		float y{ -0.5f * height };

		for (std::uint32_t j{ 0 }; j < 2; ++j)
		{
			float c{ cosf((j + sA) * dTheta) };
			float s{ sinf((j + sA) * dTheta) };

			vt.position = { radius * c, y, radius * s };
			vt.color = color;

			vertices.push_back(vt);
			indices.push_back(ind); ind++;
		}

		vt.position = { 0.0f, 0.5f * height, 0.0f };
		vt.color = color;

		vertices.push_back(vt);
		indices.push_back(ind); ind++;
	}

	// Find the bounding box of the geometry.
	bounds = BoundingBox(radius, height * 0.5f, radius);
}

void GeometryCalculation::ComputeTorus(
	_Inout_ VertexCollection& vertices,
	_Inout_ IndexCollection& indices, 
	_Inout_ DirectX::BoundingBox& bounds,
	_Inout_ ComponentCollection& components,
	float radius, 
	float sectionRadius,
	UINT subdivisionsAxis, 
	UINT subdivisionsHeight, 
	bool rhcoords
)
{
	using VertexType = VertexStructs::VertexPositionNormalTextureTangentU;
	vertices.clear();
	indices.clear();

	uint32_t ind{};

	if (subdivisionsAxis < 3 && subdivisionsHeight < 3)
		throw std::invalid_argument("tesselation parameter must be at least 3");

	const float halfSectionRadius{ sectionRadius / 2 };

	DirectX::XMVECTOR position{};
	DirectX::XMVECTOR normal{};
	DirectX::XMVECTOR uv{};
	DirectX::XMVECTOR tangentU{};
	DirectX::XMVECTOR bar{};

	for (size_t sH = 0; sH < subdivisionsHeight; sH++)
	{
		for (size_t sA = 0; sA < subdivisionsAxis; sA++)
		{
			// First we loop around the main ring of the torus.
			for (size_t i = 0; i < 2; i++)
			{
				const float u = (i + sA) / static_cast<float>(subdivisionsAxis);

				const float outerAngle = (i + sA) * DirectX::XM_2PI / static_cast<float>(subdivisionsAxis) - DirectX::XM_PIDIV2;

				// Create a transform matrix that will align geometry to
				// slice perpendicularly though the current ring position.
				const DirectX::XMMATRIX transform = DirectX::XMMatrixTranslation(radius, 0, 0) * DirectX::XMMatrixRotationY(outerAngle);

				// Now we loop along the other axis, around the side of the tube.
				for (size_t j = 0; j < 2; j++)
				{
					const float v = 1 - (j + sH) / static_cast<float>(subdivisionsHeight);

					const float innerAngle = (j + sH) * DirectX::XM_2PI / static_cast<float>(subdivisionsHeight) + DirectX::XM_PI;
					float dx, dy;

					DirectX::XMScalarSinCos(&dy, &dx, innerAngle);

					// Create a vertex.
					normal = DirectX::XMVectorSet(dx, dy, 0, 0);
					position = DirectX::XMVectorScale(normal, halfSectionRadius);
					uv = DirectX::XMVectorSet(u, v, 0.0f, 0.0f);
					position = DirectX::XMVector3Transform(position, transform);
					normal = DirectX::XMVector3TransformNormal(normal, transform);
					tangentU = { 0.0f, 1.0f, 0.0f };

					vertices.push_back(VertexType(position, normal, uv, tangentU));
					indices.push_back(ind); ind++;

					if (i == 1 && j == 0)
					{
						DuplicatingVertices(vertices, indices, ind);
					}
				}
			}
		}
	}

	// Build RH above
	if (!rhcoords)
		ReverseWinding(vertices, indices);

	ComponentCollection umm = OrderingOfIndexes(vertices);

	// Find the bounding box of the geometry.
	float sr = sectionRadius * 0.5f;
	bounds = BoundingBox(radius + sr, sectionRadius * 0.5f, radius + sr);
	components = std::move(umm);
}

void GeometryCalculation::ComputePlane(
	_Inout_ VertexCollection& vertices,
	_Inout_ IndexCollection& indices,
	_Inout_ DirectX::BoundingBox& bounds,
	_Inout_ ComponentCollection& components,
	float width, float depth, UINT subdivisionsWidth, UINT subdivisionsDepth
)
{
	using namespace DirectX;
	using VertexType = VertexStructs::VertexPositionNormalTextureTangentU;
	using TriangleType = VertexStructs::Triangle;

	vertices.clear();
	indices.clear();

	const float w2{ 0.5f * width };
	const float d2{ 0.5f * depth };

	// Size of one side in "UV" coordinates by height and width. 
	// Taken from the program "Autodesk Maya".
	const float fullSegmentUV{ 1.0f };

	const float wSegment{ width / subdivisionsWidth };
	const float dSegment{ depth / subdivisionsDepth };

	const float wSegmentUV{ fullSegmentUV / subdivisionsWidth };
	const float dSegmentUV{ fullSegmentUV / subdivisionsDepth };

	// The values for the UV coordinates for the variable *startUV* are also taken from the Autodesk Maya program.

	XMFLOAT3 nrml{ 0.0f, 1.0f, 0.0f };
	XMFLOAT3 tU{ 1.0f, 0.0f, 0.0f };

	size_t reserve = 6 * static_cast<size_t>(subdivisionsWidth * subdivisionsDepth);

	vertices.reserve(reserve);
	indices.reserve(reserve);

	indices = std::ranges::to<std::vector<std::uint32_t>>(std::views::iota(0u, reserve));

	for (uint32_t sd{ 0 }; sd < subdivisionsDepth; sd++)
	{
		const float depthPos{ -d2 + dSegment * sd };
		const float depthPlusPos{ -d2 + dSegment * (sd + 1) };

		const float depthUV{ dSegmentUV * sd };
		const float depthPlusUV{ dSegmentUV * (sd + 1) };

		for (uint32_t sw{ 0 }; sw < subdivisionsWidth; sw++)
		{
			// We create two triangles that form a polygon.

			const float widthPos{ -w2 + wSegment * sw };
			const float widthPlusPos{ -w2 + wSegment * (sw + 1) };

			const float widthUV{ wSegmentUV * sw };
			const float widthPlusUV{ wSegmentUV * (sw + 1) };

			vertices.push_back({ { widthPos, 0.0f, depthPos }, nrml, { widthUV, depthUV }, tU });
			vertices.push_back({ { widthPlusPos, 0.0f, depthPos }, nrml, { widthPlusUV, depthUV }, tU });
			vertices.push_back({ { widthPos, 0.0f, depthPlusPos }, nrml, { widthUV, depthPlusUV }, tU });

			vertices.push_back({ { widthPos, 0.0f, depthPlusPos }, nrml, { widthUV, depthPlusUV }, tU });
			vertices.push_back({ { widthPlusPos, 0.0f, depthPos }, nrml, { widthPlusUV, depthUV }, tU });
			vertices.push_back({ { widthPlusPos, 0.0f, depthPlusPos }, nrml, { widthPlusUV, depthPlusUV }, tU });
		}
	}

	//	
	ComponentCollection umm = OrderingOfIndexes(vertices);

	// Find the bounding box of the geometry.
	bounds = BoundingBox(w2, 0.0f, d2);
	components = std::move(umm);
}

void GeometryCalculation::ComputePlaneEasy(
	_Inout_ VertexColor& vertices,
	_Inout_ IndexCollection& indices,
	_Inout_ DirectX::BoundingBox& bounds,
	float width, float depth,
	const DirectX::XMFLOAT4& color
)
{
	using namespace DirectX;
	using VertexType = VertexStructs::VertexPositionColor;
	using TriangleType = VertexStructs::Triangle;

	vertices.clear();
	indices.clear();

	const float w2{ 0.5f * width };
	const float d2{ 0.5f * depth };

	vertices.reserve(6);
	indices.reserve(6);
	indices.resize(6);
	uint32_t x{};
	std::generate(indices.begin(), indices.end(), [&] { return x++; });

	// We create two triangles that form a polygon.

	const float depthPos{ -d2 + depth };
	const float widthPos{ -w2 + width };

	vertices.push_back({ { -w2, 0.0f, -d2 }, color });
	vertices.push_back({ { widthPos, 0.0f, -d2 }, color });
	vertices.push_back({ { -w2, 0.0f, depthPos }, color });

	vertices.push_back({ { -w2, 0.0f, depthPos }, color });
	vertices.push_back({ { widthPos, 0.0f, -d2 }, color });
	vertices.push_back({ { widthPos, 0.0f, depthPos }, color });

	// Find the bounding box of the geometry.
	bounds = BoundingBox(w2, 0.0f, d2);
}

void GeometryCalculation::ComputeTriangle(
	_Inout_ VertexColor& vertices,
	_Inout_ IndexCollection& indices,
	_Inout_ DirectX::BoundingBox& bounds,
	const DirectX::XMFLOAT3& pointA,
	const DirectX::XMFLOAT3& pointB,
	const DirectX::XMFLOAT3& pointC,
	const DirectX::XMFLOAT4& color
)
{
	using namespace DirectX;
	using VertexType = VertexStructs::VertexPositionColor;
	using TriangleType = VertexStructs::Triangle;

	vertices.clear();
	indices.clear();

	vertices.reserve(3);
	indices.reserve(3);
	indices.resize(3);
	uint32_t x{};
	std::generate(indices.begin(), indices.end(), [&] { return x++; });

	float left = (std::min)((std::min)(pointA.x, pointB.x), pointC.x);
	float right = (std::max)((std::max)(pointA.x, pointB.x), pointC.x);
	float bottom = (std::min)((std::min)(pointA.y, pointB.y), pointC.y);
	float top = (std::max)((std::max)(pointA.y, pointB.y), pointC.y);
	float closer = (std::min)((std::min)(pointA.z, pointB.z), pointC.z);
	float further = (std::max)((std::max)(pointA.z, pointB.z), pointC.z);

	const float width = right - left;
	const float height = top - bottom;
	const float depth = further - closer;


	vertices.push_back({ pointA, color });
	vertices.push_back({ pointB, color });
	vertices.push_back({ pointC, color });


	// Find the bounding box of the geometry.
	bounds = BoundingBox(width, height, depth);
}

void GeometryCalculation::ComputeLineCircle(_Inout_ VertexColor& vertices, _Inout_ IndexCollection& indices,
	float radius, UINT quality, const DirectX::XMFLOAT4& color)
{
	// Tested only with type D3D_PRIMITIVE_TOPOLOGY_LINELIST
	// together with D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE for PSO.	
	using VertexType = VertexStructs::VertexPositionColor;

	vertices.clear();
	indices.clear();

	int vNumDivisionsCircle{ 0 };

	if (quality == 3)
		vNumDivisionsCircle = 144;
	if (quality == 2)
		vNumDivisionsCircle = 120;
	if (quality == 1)
		vNumDivisionsCircle = 72;
	if (quality == 0)
		vNumDivisionsCircle = 40;
	if (quality < 0 || quality > 3)
		vNumDivisionsCircle = 40;

	DirectX::XMFLOAT3 position{};

	// point A
	position = { radius, 0.0f, 0.0f };
	vertices.push_back(VertexType(position, color));

	DirectX::XMFLOAT3 pointA{ radius, 0.0f, 0.0f };
	DirectX::XMFLOAT3 pointB{ 0.0f, 0.0f, 0.0f };

	for (size_t i = 0; i < vNumDivisionsCircle; ++i)
	{
		for (size_t a = 0; a < 1; ++a)
		{
			position = RotationOfPointAAroundPointB2D(pointA.x, pointA.y, pointB.x, pointB.y,
				(360.0f / vNumDivisionsCircle) * (i + 1.0f), 2
			);

			vertices.push_back(VertexType(position, color));
		}
	}

	// index
	uint32_t iNumDivisionsCircle{ static_cast<uint32_t>(vNumDivisionsCircle) };

	for (uint32_t i = 0; i < iNumDivisionsCircle; ++i)
	{
		indices.push_back(0 + i);
		indices.push_back(1 + i);
		indices.push_back(0 + i);
	}
}

void GeometryCalculation::ComputeLine(_Inout_ VertexColor& vertices, _Inout_ IndexCollection& indices,
	const DirectX::XMFLOAT3& pointA, const DirectX::XMFLOAT3& pointB, const DirectX::XMFLOAT4& color
)
{
	// Tested only with type D3D_PRIMITIVE_TOPOLOGY_LINELIST
	// together with D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE for PSO.
	using VertexType = VertexStructs::VertexPositionColor;

	vertices.clear();
	indices.clear();

	vertices.emplace_back(pointA, color);
	vertices.emplace_back(pointB, color);

	indices.push_back(0);
	indices.push_back(1);
}

void GeometryCalculation::ComputePoint(_Inout_ VertexColor& vertices, _Inout_ IndexCollection& indices,
	const DirectX::XMFLOAT3& point, const DirectX::XMFLOAT4& color)
{
	using namespace DirectX;
	using VertexType = VertexStructs::VertexPositionColor;

	vertices.clear();
	indices.clear();

	vertices.emplace_back(point, color);

	// Create index.
	indices.push_back(0);
}


/*
void Geometry::ComputeGeoSphere(_Inout_ VertexCollection& vertices, _Inout_ IndexCollection& indices, _Inout_ DirectX::BoundingBox& bounds,
	float radius, UINT subdivisions, bool rhcoords)
{
	size_t iTessellation{};

	if (subdivisions > 6)
		iTessellation = 6;
	else
		iTessellation = subdivisions;

	using namespace DirectX;
	using namespace VertexStructs;
	using VertexType = VertexStructs::VertexPositionNormalTextureTangentU;

	vertices.clear();
	indices.clear();

	// An undirected edge between two vertices, represented by a pair of indexes into a vertex array.
	// Becuse this edge is undirected, (a,b) is the same as (b,a).
	using UndirectedEdge = std::pair<uint32_t, uint32_t>;

	// Makes an undirected edge. Rather than overloading comparison operators to give us the (a,b)==(b,a) property,
	// we'll just ensure that the larger of the two goes first. This'll simplify things greatly.
	auto makeUndirectedEdge = [](uint32_t a, uint32_t b) noexcept
		{
			return std::make_pair((std::max)(a, b), (std::min)(a, b));
		};

	// Key: an edge
	// Value: the index of the vertex which lies midway between the two vertices pointed to by the key value
	// This map is used to avoid duplicating vertices when subdividing triangles along edges.
	using EdgeSubdivisionMap = std::map<UndirectedEdge, uint32_t>;


	static const XMFLOAT3 OctahedronVertices[] =
	{
		// when looking down the negative z-axis (into the screen)
		XMFLOAT3(0,  1,  0), // 0 top
		XMFLOAT3(0,  0, -1), // 1 front
		XMFLOAT3(1,  0,  0), // 2 right
		XMFLOAT3(0,  0,  1), // 3 back
		XMFLOAT3(-1,  0,  0), // 4 left
		XMFLOAT3(0, -1,  0), // 5 bottom
	};
	static const uint32_t OctahedronIndices[] =
	{
		0, 1, 2, // top front-right face
		0, 2, 3, // top back-right face
		0, 3, 4, // top back-left face
		0, 4, 1, // top front-left face
		5, 1, 4, // bottom front-left face
		5, 4, 3, // bottom back-left face
		5, 3, 2, // bottom back-right face
		5, 2, 1, // bottom front-right face
	};

	// Start with an octahedron; copy the data into the vertex/index collection.

	std::vector<XMFLOAT3> vertexPositions(std::begin(OctahedronVertices), std::end(OctahedronVertices));

	indices.insert(indices.begin(), std::begin(OctahedronIndices), std::end(OctahedronIndices));

	// We know these values by looking at the above index list for the octahedron. Despite the subdivisions that are
	// about to go on, these values aren't ever going to change because the vertices don't move around in the array.
	// We'll need these values later on to fix the singularities that show up at the poles.
	constexpr uint32_t northPoleIndex = 0;
	constexpr uint32_t southPoleIndex = 5;

	for (size_t iSubdivision = 0; iSubdivision < iTessellation; ++iSubdivision)
	{
		assert(indices.size() % 3 == 0); // sanity

		// We use this to keep track of which edges have already been subdivided.
		EdgeSubdivisionMap subdividedEdges;

		// The new index collection after subdivision.
		IndexCollection newIndices;

		const size_t triangleCount = indices.size() / 3;
		for (size_t iTriangle = 0; iTriangle < triangleCount; ++iTriangle)
		{
			// For each edge on this triangle, create a new vertex in the middle of that edge.
			// The winding order of the triangles we output are the same as the winding order of the inputs.

			// Indices of the vertices making up this triangle
			const uint32_t iv0 = indices[iTriangle * 3 + 0];
			const uint32_t iv1 = indices[iTriangle * 3 + 1];
			const uint32_t iv2 = indices[iTriangle * 3 + 2];

			// Get the new vertices
			XMFLOAT3 v01; // vertex on the midpoint of v0 and v1
			XMFLOAT3 v12; // ditto v1 and v2
			XMFLOAT3 v20; // ditto v2 and v0
			uint32_t iv01; // index of v01
			uint32_t iv12; // index of v12
			uint32_t iv20; // index of v20

			// Function that, when given the index of two vertices, creates a new vertex at the midpoint of those vertices.
			auto const divideEdge = [&](uint32_t i0, uint32_t i1, XMFLOAT3& outVertex, uint32_t& outIndex)
				{
					const UndirectedEdge edge = makeUndirectedEdge(i0, i1);

					// Check to see if we've already generated this vertex
					auto it = subdividedEdges.find(edge);
					if (it != subdividedEdges.end())
					{
						// We've already generated this vertex before
						outIndex = it->second; // the index of this vertex
						outVertex = vertexPositions[outIndex]; // and the vertex itself
					}
					else
					{
						// Haven't generated this vertex before: so add it now

						// outVertex = (vertices[i0] + vertices[i1]) / 2
						DirectX::XMStoreFloat3(
							&outVertex,
							XMVectorScale(
								XMVectorAdd(XMLoadFloat3(&vertexPositions[i0]), XMLoadFloat3(&vertexPositions[i1])),
								0.5f
							)
						);

						outIndex = static_cast<uint32_t>(vertexPositions.size());
						CheckIndexOverflow(outIndex);
						vertexPositions.push_back(outVertex);

						// Now add it to the map.
						auto entry = std::make_pair(edge, outIndex);
						subdividedEdges.insert(entry);
					}
				};

			// Add/get new vertices and their indices
			divideEdge(iv0, iv1, v01, iv01);
			divideEdge(iv1, iv2, v12, iv12);
			divideEdge(iv0, iv2, v20, iv20);

			// Add the new indices. We have four new triangles from our original one:
			//        v0
			//        o
			//       /a\
			//  v20 o---o v01
			//     /b\c/d\
			// v2 o---o---o v1
			//       v12
			const uint32_t indicesToAdd[] =
			{
				 iv0, iv01, iv20, // a
				iv20, iv12,  iv2, // b
				iv20, iv01, iv12, // c
				iv01,  iv1, iv12, // d
			};
			newIndices.insert(newIndices.end(), std::begin(indicesToAdd), std::end(indicesToAdd));
		}

		indices = std::move(newIndices);
	}

	// Now that we've completed subdivision, fill in the final vertex collection
	vertices.reserve(vertexPositions.size());
	for (const auto& it : vertexPositions)
	{
		const XMVECTOR normal = XMVector3Normalize(XMLoadFloat3(&it));
		const XMVECTOR pos = XMVectorScale(normal, radius);

		XMFLOAT3 normalFloat3;
		DirectX::XMStoreFloat3(&normalFloat3, normal);

		// calculate texture coordinates for this vertex
		const float longitude = atan2f(normalFloat3.x, -normalFloat3.z);
		const float latitude = acosf(normalFloat3.y);

		const float u = longitude / XM_2PI + 0.5f;
		const float v = latitude / XM_PI;

		auto const texcoord = XMVectorSet(1.0f - u, v, 0.0f, 0.0f);
		auto const tangentU = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		vertices.push_back(VertexStructs::VertexPositionNormalTextureTangentU(pos, normal, texcoord, tangentU));
	}

	// There are a couple of fixes to do. One is a texture coordinate wraparound fixup. At some point, there will be
	// a set of triangles somewhere in the mesh with texture coordinates such that the wraparound across 0.0/1.0
	// occurs across that triangle. Eg. when the left hand side of the triangle has a U coordinate of 0.98 and the
	// right hand side has a U coordinate of 0.0. The intent is that such a triangle should render with a U of 0.98 to
	// 1.0, not 0.98 to 0.0. If we don't do this fixup, there will be a visible seam across one side of the sphere.
	//
	// Luckily this is relatively easy to fix. There is a straight edge which runs down the prime meridian of the
	// completed sphere. If you imagine the vertices along that edge, they circumscribe a semicircular arc starting at
	// y=1 and ending at y=-1, and sweeping across the range of z=0 to z=1. x stays zero. It's along this edge that we
	// need to duplicate our vertices - and provide the correct texture coordinates.
	const size_t preFixupVertexCount = vertices.size();
	for (size_t i = 0; i < preFixupVertexCount; ++i)
	{
		// This vertex is on the prime meridian if position.x and texcoord.u are both zero (allowing for small epsilon).
		const bool isOnPrimeMeridian = XMVector2NearEqual(
			XMVectorSet(vertices[i].position.x, vertices[i].uv.x, 0.0f, 0.0f),
			XMVectorZero(),
			XMVectorSplatEpsilon());

		if (isOnPrimeMeridian)
		{
			size_t newIndex = vertices.size(); // the index of this vertex that we're about to add
			CheckIndexOverflow(newIndex);

			// copy this vertex, correct the texture coordinate, and add the vertex
			VertexStructs::VertexPositionNormalTextureTangentU v = vertices[i];
			v.uv.x = 1.0f;
			vertices.push_back(v);

			// Now find all the triangles which contain this vertex and update them if necessary
			for (size_t j = 0; j < indices.size(); j += 3)
			{
				uint32_t* triIndex0 = &indices[j + 0];
				uint32_t* triIndex1 = &indices[j + 1];
				uint32_t* triIndex2 = &indices[j + 2];

				if (*triIndex0 == i)
				{
					// nothing; just keep going
				}
				else if (*triIndex1 == i)
				{
					std::swap(triIndex0, triIndex1); // swap the pointers (not the values)
				}
				else if (*triIndex2 == i)
				{
					std::swap(triIndex0, triIndex2); // swap the pointers (not the values)
				}
				else
				{
					// this triangle doesn't use the vertex we're interested in
					continue;
				}

				// If we got to this point then triIndex0 is the pointer to the index to the vertex we're looking at
				assert(*triIndex0 == i);
				assert(*triIndex1 != i && *triIndex2 != i); // assume no degenerate triangles

				const VertexStructs::VertexPositionNormalTextureTangentU& v0 = vertices[*triIndex0];
				const VertexStructs::VertexPositionNormalTextureTangentU& v1 = vertices[*triIndex1];
				const VertexStructs::VertexPositionNormalTextureTangentU& v2 = vertices[*triIndex2];

				// check the other two vertices to see if we might need to fix this triangle

				if (abs(v0.uv.x - v1.uv.x) > 0.5f ||
					abs(v0.uv.x - v2.uv.x) > 0.5f)
				{
					// yep; replace the specified index to point to the new, corrected vertex
					*triIndex0 = static_cast<uint32_t>(newIndex);
				}
			}
		}
	}

	// And one last fix we need to do: the poles. A common use-case of a sphere mesh is to map a rectangular texture onto
	// it. If that happens, then the poles become singularities which map the entire top and bottom rows of the texture
	// onto a single point. In general there's no real way to do that right. But to match the behavior of non-geodesic
	// spheres, we need to duplicate the pole vertex for every triangle that uses it. This will introduce seams near the
	// poles, but reduce stretching.
	auto const fixPole = [&](size_t poleIndex)
		{
			const auto& poleVertex = vertices[poleIndex];
			bool overwrittenPoleVertex = false; // overwriting the original pole vertex saves us one vertex

			for (size_t i = 0; i < indices.size(); i += 3)
			{
				// These pointers point to the three indices which make up this triangle. pPoleIndex is the pointer to the
				// entry in the index array which represents the pole index, and the other two pointers point to the other
				// two indices making up this triangle.
				uint32_t* pPoleIndex;
				uint32_t* pOtherIndex0;
				uint32_t* pOtherIndex1;
				if (indices[i + 0] == poleIndex)
				{
					pPoleIndex = &indices[i + 0];
					pOtherIndex0 = &indices[i + 1];
					pOtherIndex1 = &indices[i + 2];
				}
				else if (indices[i + 1] == poleIndex)
				{
					pPoleIndex = &indices[i + 1];
					pOtherIndex0 = &indices[i + 2];
					pOtherIndex1 = &indices[i + 0];
				}
				else if (indices[i + 2] == poleIndex)
				{
					pPoleIndex = &indices[i + 2];
					pOtherIndex0 = &indices[i + 0];
					pOtherIndex1 = &indices[i + 1];
				}
				else
				{
					continue;
				}

				const auto& otherVertex0 = vertices[*pOtherIndex0];
				const auto& otherVertex1 = vertices[*pOtherIndex1];

				// Calculate the texcoords for the new pole vertex, add it to the vertices and update the index
				VertexStructs::VertexPositionNormalTextureTangentU newPoleVertex = poleVertex;
				newPoleVertex.uv.x = (otherVertex0.uv.x + otherVertex1.uv.x) / 2;
				newPoleVertex.uv.y = poleVertex.uv.y;

				if (!overwrittenPoleVertex)
				{
					vertices[poleIndex] = newPoleVertex;
					overwrittenPoleVertex = true;
				}
				else
				{
					CheckIndexOverflow(vertices.size());

					*pPoleIndex = static_cast<uint32_t>(vertices.size());
					vertices.push_back(newPoleVertex);
				}
			}
		};

	fixPole(northPoleIndex);
	fixPole(southPoleIndex);

	// Build RH above
	if (!rhcoords)
		ReverseWinding(indices, vertices);

	// Find the bounding box of the geometry.
	BoundingBox(vertices, bounds);
}
*/
