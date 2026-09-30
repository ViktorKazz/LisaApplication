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

#ifndef BACKGROUND_CLASS_H
#define BACKGROUND_CLASS_H

#include "pch.h"
#include "PolygonPrimitives.h"
#include "PipelineState.h"
#include "UploadShaders.h"

namespace LisaApp
{
	class Background
	{
	public:
		Background() = default;
		~Background() = default;

		void Create(
			_In_ ID3D12Device3* device,
			_In_ ID3D12GraphicsCommandList* commandList,
			_In_ ID3D12RootSignature* rootSignature,
			std::uint32_t sampleCount,
			std::uint32_t sampleQuality,
			INT material,
			float radius = 1000.0f, 
			UINT subdivisions = 3
		);

		void CreatePso(
			_In_ ID3D12Device3* device,
			_In_ ID3D12RootSignature* rootSignature,
			std::uint32_t sampleCount,
			std::uint32_t sampleQuality
		);

		void Draw(_In_ ID3D12GraphicsCommandList* commandList);
		void UpdateCB(
			const DirectX::XMMATRIX& getViev,
			const DirectX::BoundingFrustum& camFrustum,
			bool frustumCullingEnabled
		);

	private:
		std::unique_ptr<PolygonPrimitives> m_background;

		enum Mode : std::uint32_t { Sky = 0 };
		PipelineState m_pso;
		UploadShaders m_shader;
	};
}

#endif // !BACKGROUND_CLASS_H
