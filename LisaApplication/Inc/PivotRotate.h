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

#ifndef PIVOT_ROTATE_CLASS_H
#define PIVOT_ROTATE_CLASS_H

#include "PivotTools.h"
#include "Globals.h"

// Note: Pivot components and their auxiliary components are created in a specific way in space.
// When a pivot or its additional components change, 
// it is not the components themselves that change, but their matrices.

namespace LisaApp
{
	class PivotRotateImpl
	{
	public:
		PivotRotateImpl() = default;
		~PivotRotateImpl() = default;

		// Accessors.

		SceneObjects& GetStorageRotate()                     noexcept { return m_storageRotate; }
		std::uint32_t GetObjectID()                    const noexcept { return m_objectID; }

		void OnOffStep(bool onOff)                           noexcept { m_isStep = onOff; }

		void Create(_In_ ID3D12Device3* device, _In_ ID3D12GraphicsCommandList* commandList);
		void Scale(const IPivot::Attributes& attributes, const DirectX::XMVECTOR& cameraPosition);
		void PlaceThePivotInTheDesiredPosition(const IPivot::Attributes& attributes);
		void LButtonUp(const IPivot::Attributes& attributes);

		DirectX::XMVECTOR PointOnThePlaneAndSphere(
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
			_In_ ID3D12PipelineState* line,
			_In_ ID3D12PipelineState* mesh,
			_In_ ID3D12PipelineState* lineCircle,
			_In_ ID3D12PipelineState* colorRotationAngle
		);

		void UpdateCB(const DirectX::XMMATRIX& getViev, const DirectX::BoundingFrustum& camFrustum, bool frustumCullingEnabled);

	private:
		PivotTools m_tools;

		SceneObjects m_storageRotate{};
		SceneObjects m_storageRotateAux{};
		SceneObjects m_innerCircle{};

		DirectX::XMMATRIX m_billboardingMatrix = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		DirectX::XMMATRIX m_sphereX = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		DirectX::XMMATRIX m_sphereY = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		DirectX::XMMATRIX m_sphereZ = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		DirectX::XMMATRIX m_sphereXYZ = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		DirectX::XMVECTOR m_pivotStart{};
		DirectX::XMVECTOR m_pivotEnd{};

		DirectX::XMVECTOR m_intersectionWithAux{};

		DirectX::XMVECTOR m_cameraPosition{};

		DirectX::XMVECTOR m_quaternionAux = DirectX::XMQuaternionIdentity();
		DirectX::XMVECTOR m_quaternionAuxTriangle = DirectX::XMQuaternionIdentity();

		DirectX::XMVECTOR m_rotationOrigin{ 0.0f, 0.0f, 0.0f, 1.0f };

		std::uint32_t m_objectID{};

		float m_bufferDist{};

		bool m_isStep{};
		const float m_radiansStep{ 0.087266f }; // 5.0f degrees.
	};
}

#endif // !PIVOT_ROTATE_CLASS_H