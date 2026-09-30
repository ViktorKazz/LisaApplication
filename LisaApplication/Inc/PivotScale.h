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

#ifndef PIVOT_SCALE_CLASS_H
#define PIVOT_SCALE_CLASS_H

#include "PivotTools.h"
#include "Globals.h"

namespace LisaApp
{
	class PivotScaleImpl
	{
	public:
		PivotScaleImpl() = default;
		~PivotScaleImpl() = default;

		// Accessors.

		SceneObjects& GetStorageScale()                      noexcept { return m_storageScale; };
		std::uint32_t GetObjectID()                    const noexcept { return m_objectID; };

		void Create(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList);
		void SelectingAdjacentPivotObjects(RefOnRender& refWrapOnRender);
		void UpdateMatrix(const DirectX::XMMATRIX& matrix);
		void Scale(const IPivot::Attributes& attributes, const DirectX::XMVECTOR& cameraPosition);
		void HidingPivotComponents(const DirectX::XMVECTOR& camerasLook);
		void PlaceThePivotInTheDesiredPosition(const IPivot::Attributes& attributes, const DirectX::XMVECTOR& camerasLook);
		void ComputeDotAndScale(
			const IPivot::Attributes& attributes,
			const DirectX::XMFLOAT4X4& matrixf,
			const DirectX::XMVECTOR& normal,
			float& dot, 
			float& scale
		);
		void MovementCubeMatrix(const IPivot::Attributes& attributes, std::int32_t x, std::int32_t y, std::int32_t z);
		void LButtonUp(const IPivot::Attributes& attributes);

		DirectX::XMVECTOR PointOnThePlane(
			const IPivot::Attributes& attributes,
			const DirectX::XMMATRIX& getViev, 
			const DirectX::XMFLOAT4X4& proj4x4f,
			std::int32_t sx, 
			std::int32_t sy, 
			std::int32_t screenWidth, 
			std::int32_t screenHeight
		);

		void LButtonDown(
			const IPivot::Attributes& attributes,
			const RefOnRender& refWrapOnRender, 
			const DirectX::XMMATRIX& getViev, 
			const DirectX::XMFLOAT4X4& proj4x4f,
			std::int32_t sx, 
			std::int32_t sy,
			std::int32_t screenWidth, 
			std::int32_t screenHeight
		);

		bool LButtonMove(
			const DirectX::XMMATRIX& getViev, 
			const DirectX::XMFLOAT4X4& proj4x4f,
			std::int32_t sx, 
			std::int32_t sy, 
			std::int32_t screenWidth, 
			std::int32_t screenHeight, 
			IPivot::Attributes& attributes
		);

		void Draw(
			_In_ ID3D12GraphicsCommandList* commandList,
			_In_ ID3D12PipelineState* mesh,
			_In_ ID3D12PipelineState* line
		);

		void UpdateCB(const DirectX::XMMATRIX& getViev, const DirectX::BoundingFrustum& camFrustum, bool frustumCullingEnabled);

	private:
		PivotTools m_tools;

		SceneObjects m_storageScale{};
		SceneObjects m_storageScaleAux{};

		DirectX::XMMATRIX m_plane = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		DirectX::XMVECTOR m_pivotStart{};
		DirectX::XMVECTOR m_pivotEnd{};

		DirectX::XMVECTOR m_cameraPosition{};

		DirectX::XMVECTOR m_normalAuxLineX{};
		DirectX::XMVECTOR m_normalAuxLineY{};
		DirectX::XMVECTOR m_normalAuxLineZ{};

		std::uint32_t m_objectID{};

		float m_offsetX{};
		float m_offsetY{};
		float m_offsetZ{};

		bool m_hideX{};
		bool m_hideY{};
		bool m_hideZ{};
		bool m_hidePlaneX{};
		bool m_hidePlaneY{};
		bool m_hidePlaneZ{};
	};
}

#endif // !PIVOT_SCALE_CLASS_H