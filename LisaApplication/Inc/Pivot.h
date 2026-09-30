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

#ifndef PIVOT_CLASS_H
#define PIVOT_CLASS_H

#include "pch.h"
#include <PipelineState.h>
#include <UploadShaders.h>
#include "Picking.h"
#include "Globals.h"
#include "PivotTranslate.h"
#include <PivotRotate.h>
#include <PivotScale.h>

namespace LisaApp
{
	class Pivot
	{
	public:
		Pivot() = default;
		~Pivot() = default;

		// Pivot Accessors.

		bool GetClickOnPivot()                                    const noexcept { return m_clickOnPivot; }
		bool GetPivotDirty()                                      const noexcept { return m_pivotDirty; }
		bool GetOnOff()                                           const noexcept { return m_onOff; }

		template<typename Self>
		IPivot::Axis& GetActiveAxis(this Self&& self)                   noexcept { return self.m_activeAxis; }
		template<typename Self>
		Global::PivotMode& GetMode(this Self&& self)                    noexcept { return self.m_mode; }
		template<typename Self>
		RefOnRender& GetRefWrapOnRender(this Self&& self)               noexcept { return self.m_refWrapOnRender; }

		DirectX::XMFLOAT3 GetPosition() noexcept
		{
			return { m_attributes.TranslateX, m_attributes.TranslateY, m_attributes.TranslateZ };
		}

		void SetClickOnPivot(bool click)                                           noexcept { m_clickOnPivot = click; }
		void SetPivotDirty(bool dirty)                                             noexcept { m_pivotDirty = dirty; }
		void SetMode(LisaApp::Global::PivotMode mode)                              noexcept { m_mode = mode; }
		void SetOnOff(bool onOff)                                                  noexcept { m_onOff = onOff; }
		void SetColorModeSelectedComponent(LisaApp::Global::PivotColorMode mode)   noexcept 
		{
			for (const auto& i : m_refWrapOnRender)
			{
				auto& [objectID, instancesID] = i.first;
				auto& onRender = i.second.front().get();

				onRender.Instances[instancesID].Mode = static_cast<UINT>(mode);
			}
		}

		void Create(
			_In_ ID3D12Device3* device, 
			_In_ ID3D12GraphicsCommandList* commandList, 
			_In_ ID3D12RootSignature* rootSignature,
			std::uint32_t sampleCount,
			std::uint32_t sampleQuality
		);
		void CreatePso(
			_In_ ID3D12Device3* device, 
			_In_ ID3D12RootSignature* rootSignature, 
			std::uint32_t sampleCount, 
			std::uint32_t sampleQuality
		);
		void Draw(_In_ ID3D12GraphicsCommandList* commandList);

		void UpdateCB(const DirectX::XMMATRIX& getViev, const DirectX::BoundingFrustum& camFrustum, bool frustumCullingEnabled);
		void ClearRefStorage();

		bool Pick(
			const DirectX::XMMATRIX& getViev, 
			const DirectX::XMFLOAT4X4& proj4x4f, 
			std::int32_t sx,
			std::int32_t sy,
			std::int32_t screenWidth,
			std::int32_t screenHeight
		);

		void Scale(
			const DirectX::XMMATRIX& getViev,
			const DirectX::XMFLOAT4X4& proj4x4f,
			const DirectX::XMVECTOR& cameraPosition,
			std::int32_t screenWidth,
			std::int32_t screenHeight
		);

		void HidingPivotComponents(const DirectX::XMVECTOR& camerasLook);
		void PlaceThePivotInTheDesiredPositionComponent(
			const RefOnRender& refOnRender,
			const DirectX::XMVECTOR& camerasLook,
			std::uint32_t selectMode
		);
		void PlaceThePivotInTheDesiredPosition(
			const RefOnRender& refOnRender, 
			const std::pair<std::uint32_t, std::uint32_t>& lastID,
			const DirectX::XMVECTOR& camerasLook
		);

		void OnLButtonUp();

		// Functions for capturing the initial coordinates when a mouse click occurs.
		bool OnLButtonDown(
			const DirectX::XMMATRIX& getViev, 
			const DirectX::XMFLOAT4X4& proj4x4f, 
			std::int32_t sx,
			std::int32_t sy,
			std::int32_t screenWidth,
			std::int32_t screenHeight
		);

		bool OnLButtonMove(
			const DirectX::XMMATRIX& getViev,
			const DirectX::XMFLOAT4X4& proj4x4f,
			std::int32_t sx,
			std::int32_t sy,
			std::int32_t screenWidth,
			std::int32_t screenHeight,
			IPivot::Attributes& attributes
		);

		bool OnKeyDown(
			HWND hwnd, 
			WPARAM wParam, 
			std::uint32_t selectMode,
			bool isSelectObject, 
			const RefOnRender& refOnRender, 
			const std::pair<std::uint32_t, std::uint32_t>& lastID,
			const DirectX::XMVECTOR& camerasLook
		);

		IPivot::Axis ProcessingObjectID(std::uint32_t objectID);

		void OnOffStep(bool onOff);

	private:
		std::unique_ptr<Picking> m_picking = std::make_unique<Picking>();

		PivotTools m_tools;
		PivotTranslateImpl m_pivotTr;
		PivotRotateImpl m_pivotRt;
		PivotScaleImpl m_pivotSc;

		RefOnRender m_refWrapOnRender;
		UploadShaders m_shader;
		
		enum PSOMode : std::uint32_t
		{
			Line = 0,
			LineCircle,
			CenterFrame,
			Mesh,
			Sphere,
			ColorRotationAngle
		};

		PipelineState m_pso;

		DirectX::XMMATRIX m_scaleMatrix = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		IPivot::Attributes m_attributes{};
		IPivot::Axis m_activeAxis{};

		bool m_clickOnPivot{};
		bool m_pivotDirty{};
		bool m_onOff{};

		float m_cameraDefaultLength{};
		float m_getDefaultPivotRadiusInPixel{};

		LisaApp::Global::PivotMode m_mode{};
	};
}

#endif // !PIVOT_CLASS_H
