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

#ifndef PIVOT_TOOLS_CLASS_H
#define PIVOT_TOOLS_CLASS_H

#include "pch.h"
#include "PolygonPrimitives.h"

namespace LisaApp
{
	class PivotTools
	{
	public:
		PivotTools() = default;
		~PivotTools() = default;

		// Accessors.

		DirectX::XMVECTOR GetPlaneXY() const noexcept { return m_planeXY; };
		DirectX::XMVECTOR GetPlaneYZ() const noexcept { return m_planeYZ; };
		DirectX::XMVECTOR GetPlaneXZ() const noexcept { return m_planeXZ; };
		DirectX::XMMATRIX GetMatrixFromStorage(std::uint32_t key) noexcept { return m_matrixStore[key]; };


		void PlanesParallelToTheAxes();
		void SetAttributes(std::uint32_t key, DirectX::XMFLOAT4X4& world, float tx, float ty, float tz, float rx, float ry, float rz);
		void SetAttributes(std::uint32_t key, DirectX::XMMATRIX& world, float tx, float ty, float tz, float rx, float ry, float rz);
		void VertexIndexStore(SceneObjects& storage, std::uint32_t key);
		void HideOrVisibleComponents(SceneObjects& storage, std::uint32_t key, bool isHide);
		void HideOrVisibleComponents(SceneObjects& storage, std::uint32_t key, std::uint32_t instances, bool isHide);
		void Hide(SceneObjects& storage, float angleA, float angleB, std::uint32_t keyA, std::uint32_t keyB, bool& lock);
		void Hide(SceneObjects& storage, float angle, std::uint32_t key, bool& lock);
		void Hide(
			SceneObjects& storage,
			float angleA,
			float angleB,
			std::uint32_t keyA,
			std::uint32_t keyB,
			std::uint32_t instancesA,
			std::uint32_t instancesB,
			bool& lock
		);
		void Hide(SceneObjects& storage, float angle, std::uint32_t key, std::uint32_t instances, bool& lock);

	private:
		DirectX::XMVECTOR m_planeXY{};
		DirectX::XMVECTOR m_planeYZ{};
		DirectX::XMVECTOR m_planeXZ{};

		std::unordered_map<std::uint32_t, DirectX::XMMATRIX> m_matrixStore{};
		std::unordered_map<std::uint32_t, std::tuple<std::uint32_t, std::uint32_t, std::uint32_t>> m_vertexIndexStore{};
	};
}

#endif // !PIVOT_TOOLS_CLASS_H