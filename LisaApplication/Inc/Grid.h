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

#ifndef GRID_CLASS_H
#define GRID_CLASS_H

#include <memory>
#include "PolygonPrimitives.h"
#include "Materials.h"
#include "Camera.h"
#include "PipelineState.h"
#include "UploadShaders.h"
#include "AppColors.h"

namespace LisaApp
{
	class Grid
	{
	public:
		Grid() = default;
		~Grid() = default;

		enum Axis : bool { X = false, Z = true };

		void ArrangementOfLinesAndSymbols(
			float lengthAndWidth,
			float gridLinesEvery,
			size_t subdivisions
		);

		void Create(
			_In_ ID3D12Device3* device, 
			_In_ ID3D12GraphicsCommandList* commandList,
			_In_ ID3D12RootSignature* rootSignature,
			std::uint32_t sampleCount,
			std::uint32_t sampleQuality,
			float lengthAndWidth = 12.0f,
			float gridLinesEvery = 5.0f,
			size_t subdivisions = 5
		);

		void CreatePso(
			_In_ ID3D12Device3* device,
			_In_ ID3D12RootSignature* rootSignature,
			std::uint32_t sampleCount,
			std::uint32_t sampleQuality
		);

		void SettingUpGridSymbols(
			ID3D12Device3* device,
			ID3D12CommandQueue* commandQueue,
			const DXGI_FORMAT& backBufferFormat,
			const DXGI_FORMAT& depthBufferFormat
		);

		void SettingUpGraphicsMemory(ID3D12Device3* device);

		void CommitGraphicsMemory(ID3D12CommandQueue* commandQueue);

		void UpdateSpriteBatch(const D3D12_VIEWPORT& viewport);

		void UpdateGrid(
			const Camera& camera, 
			const D3D12_VIEWPORT& viewport
		);

		void UpdateCB(
			const DirectX::XMMATRIX& getViev,
			const DirectX::BoundingFrustum& camFrustum,
			bool frustumCullingEnabled);

		void Draw(ID3D12GraphicsCommandList* commandList);

		void DrawSymbolsOnAGrid(
			_In_ ID3D12GraphicsCommandList* commandList,
			const D3D12_VIEWPORT& viewport,
			const D3D12_RECT& scissorRect,
			const CD3DX12_CPU_DESCRIPTOR_HANDLE& rtvDescriptor,
			ID3D12DescriptorHeap* descriptorHeaps[]
		);

		std::pair<float, float> ConvertWorldSpaceToViewSpace(
			const DirectX::XMMATRIX& getViev,
			const DirectX::XMFLOAT4X4& proj4x4f,
			float ScreenViewportX, 
			float ScreenViewportY, 
			float x, 
			float y, 
			float z
		);

		void ColorChange(
			const DirectX::XMFLOAT4& centerColor,
			const DirectX::XMFLOAT4& gridLinesEveryColor,
			const DirectX::XMFLOAT4& subdivisionsColor
		);

		void Resize(float lengthAndWidth, float gridLinesEvery, size_t subdivisions);

	private:
		std::unique_ptr<DirectX::GraphicsMemory>        m_graphicsMemory{ nullptr };

		std::unique_ptr<DirectX::SpriteBatch>           m_spriteBatch{ nullptr };
		DirectX::SimpleMath::Vector2                    m_fontPos{};

		std::unique_ptr<DirectX::DescriptorHeap>        m_resourceDescriptors{ nullptr };
		std::unique_ptr<DirectX::SpriteFont>            m_font{ nullptr };

		enum Descriptors
		{
			UIFont,
			MyFont,
			Count
		};

		std::unique_ptr<DirectX::SpriteFont>            m_smallFont{ nullptr };
		std::unique_ptr<DirectX::CommonStates>          m_states{ nullptr };

		std::vector<std::pair<float, float>>            m_gridCharWorld{};
		std::vector<std::pair<float, float>>            m_gridCharScreen{};

		std::unique_ptr<PolygonPrimitives>              m_centerLineX{ nullptr };
		std::unique_ptr<PolygonPrimitives>              m_centerLineZ{ nullptr };

		UploadShaders m_shader;

		enum PSOMode : std::uint32_t
		{
			Line = 0
		};

		PipelineState m_pso;

		std::vector<size_t> m_positiveGridLinesEveryIndex{};
		std::vector<size_t> m_negativegridLinesEveryIndex{};

		std::vector<size_t> m_positiveSubdivisionsIndex{};
		std::vector<size_t> m_negativeSubdivisionsIndex{};

		DirectX::XMFLOAT4 m_centerColor{ AppColors::Color::CornflowerBlue };
		DirectX::XMFLOAT4 m_gridLinesEveryColor{ AppColors::Color::CornflowerBlue };
		DirectX::XMFLOAT4 m_subdivisionsColor{ AppColors::Color::CornflowerBlue };
		DirectX::XMFLOAT4 m_symbolsColor{ AppColors::Color::CornflowerBlue };
	};
}

#endif // !GRID_CLASS_H
