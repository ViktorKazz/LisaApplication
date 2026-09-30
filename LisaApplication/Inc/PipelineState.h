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

#ifndef PIPELINE_STATE_CLASS_H
#define PIPELINE_STATE_CLASS_H

#include "RootSignature.h"
#include "HelperStructs.h"

namespace LisaApp
{
	namespace PSOConfig
	{
		static D3D12_RASTERIZER_DESC RasterizerDesc()
		{
			D3D12_RASTERIZER_DESC desc{};
			desc.FillMode = D3D12_FILL_MODE_SOLID;
			desc.CullMode = D3D12_CULL_MODE_NONE;
			desc.FrontCounterClockwise = false;
			desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
			desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
			desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
			desc.DepthClipEnable = true;
			desc.MultisampleEnable = true;
			desc.AntialiasedLineEnable = false;
			desc.ForcedSampleCount = 0;
			desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

			return desc;
		}

		static D3D12_DEPTH_STENCIL_DESC DepthStencil()
		{
			D3D12_DEPTH_STENCIL_DESC desc{};
			desc.DepthEnable = false;
			desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
			desc.StencilEnable = false;
			desc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
			desc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
			desc.FrontFace = { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
			desc.BackFace = { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };

			return desc;
		}

		static D3D12_RENDER_TARGET_BLEND_DESC BlendDesc()
		{
			D3D12_RENDER_TARGET_BLEND_DESC desc{};
			desc.BlendEnable = true;
			desc.LogicOpEnable = false;
			desc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			desc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			desc.BlendOp = D3D12_BLEND_OP_ADD;
			desc.SrcBlendAlpha = D3D12_BLEND_ONE;
			desc.DestBlendAlpha = D3D12_BLEND_ZERO;
			desc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
			desc.LogicOp = D3D12_LOGIC_OP_NOOP;
			desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

			return desc;
		}

		static D3D12_GRAPHICS_PIPELINE_STATE_DESC Initial()
		{
			D3D12_RASTERIZER_DESC r{ RasterizerDesc() };

			D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};
			pso.StreamOutput = {};
			pso.BlendState = { CD3DX12_BLEND_DESC(D3D12_DEFAULT) };
			pso.SampleMask = { UINT_MAX };
			pso.RasterizerState = { r };
			pso.DepthStencilState = { CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT) };
			pso.IBStripCutValue = {};
			pso.PrimitiveTopologyType = { D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE };
			pso.NumRenderTargets = { 1 };
			pso.RTVFormats[0] = { DXGI_FORMAT_R8G8B8A8_UNORM };
			pso.DSVFormat = { DXGI_FORMAT_D24_UNORM_S8_UINT };
			pso.SampleDesc = { 1, 0 };
			pso.NodeMask = {};
			pso.CachedPSO = {};
			pso.Flags = {};

			return pso;
		}

		static D3D12_GRAPHICS_PIPELINE_STATE_DESC Shadow()
		{
			D3D12_RASTERIZER_DESC r{ RasterizerDesc() };
			r.DepthBias = { 100000 };
			r.DepthBiasClamp = { 0.0f };
			r.SlopeScaledDepthBias = { 1.0f };

			D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};
			pso.StreamOutput = {};
			pso.BlendState = { CD3DX12_BLEND_DESC(D3D12_DEFAULT) };
			pso.SampleMask = { UINT_MAX };
			pso.RasterizerState = { r };
			pso.DepthStencilState = { CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT) };
			pso.IBStripCutValue = {};
			pso.PrimitiveTopologyType = { D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE };
			pso.NumRenderTargets = { 0 };
			pso.RTVFormats[0] = { DXGI_FORMAT_UNKNOWN };
			pso.DSVFormat = { DXGI_FORMAT_D24_UNORM_S8_UINT };
			pso.SampleDesc = { 1, 0 };
			pso.NodeMask = {};
			pso.CachedPSO = {};
			pso.Flags = {};

			return pso;
		}

		static D3D12_GRAPHICS_PIPELINE_STATE_DESC SSAO()
		{
			D3D12_RASTERIZER_DESC r{ RasterizerDesc() };

			// SSAO effect does not need the depth buffer.
			D3D12_DEPTH_STENCIL_DESC d{ LisaApp::PSOConfig::DepthStencil() };

			D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};
			pso.StreamOutput = {};
			pso.BlendState = { CD3DX12_BLEND_DESC(D3D12_DEFAULT) };
			pso.SampleMask = { UINT_MAX };
			pso.RasterizerState = { r };
			pso.DepthStencilState = { d };
			pso.IBStripCutValue = {};
			pso.PrimitiveTopologyType = { D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE };
			pso.NumRenderTargets = { 1 };
			pso.RTVFormats[0] = { DXGI_FORMAT_R16_UNORM };
			pso.DSVFormat = { DXGI_FORMAT_UNKNOWN };
			pso.SampleDesc = { 1, 0 };
			pso.NodeMask = {};
			pso.CachedPSO = {};
			pso.Flags = {};

			return pso;
		}

		static D3D12_GRAPHICS_PIPELINE_STATE_DESC Sky()
		{
			// The camera is inside the sky sphere, so just turn off culling. -> CullMode = D3D12_CULL_MODE_NONE 
			D3D12_RASTERIZER_DESC r{ RasterizerDesc() };

			// SSAO effect does not need the depth buffer.
			D3D12_DEPTH_STENCIL_DESC d{ LisaApp::PSOConfig::DepthStencil() };
			d.DepthEnable = { true };
			d.DepthWriteMask = { D3D12_DEPTH_WRITE_MASK_ALL };
			// Make sure the depth function is LESS_EQUAL and not just LESS.  
			// Otherwise, the normalized depth values at z = 1 (NDC) will 
			// fail the depth test if the depth buffer was cleared to 1.
			d.DepthFunc = { D3D12_COMPARISON_FUNC_LESS_EQUAL };

			D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};
			pso.StreamOutput = {};
			pso.BlendState = { CD3DX12_BLEND_DESC(D3D12_DEFAULT) };
			pso.SampleMask = { UINT_MAX };
			pso.RasterizerState = { r };
			pso.DepthStencilState = { d };
			pso.IBStripCutValue = {};
			pso.PrimitiveTopologyType = { D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE };
			pso.NumRenderTargets = { 1 };
			pso.RTVFormats[0] = { DXGI_FORMAT_R8G8B8A8_UNORM };
			pso.DSVFormat = { DXGI_FORMAT_D24_UNORM_S8_UINT };
			pso.SampleDesc = { 1, 0 };
			pso.NodeMask = {};
			pso.CachedPSO = {};
			pso.Flags = {};

			return pso;
		}

		static D3D12_GRAPHICS_PIPELINE_STATE_DESC Highlight()
		{
			D3D12_RENDER_TARGET_BLEND_DESC b{ BlendDesc() };

			D3D12_RASTERIZER_DESC r{ LisaApp::PSOConfig::RasterizerDesc() };
			r.FillMode = D3D12_FILL_MODE_WIREFRAME;

			// SSAO effect does not need the depth buffer.
			D3D12_DEPTH_STENCIL_DESC d{ LisaApp::PSOConfig::DepthStencil() };
			d.DepthEnable = { true };
			d.DepthWriteMask = { D3D12_DEPTH_WRITE_MASK_ALL };
			// Change the depth test from < to <= so that if we draw the same triangle twice, it will
			// still pass the depth test.  This is needed because we redraw the picked triangle with a
			// different material to highlight it.  If we do not use <=, the triangle will fail the 
			// depth test the 2nd time we try and draw it.
			d.DepthFunc = { D3D12_COMPARISON_FUNC_LESS_EQUAL };

			D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};
			pso.StreamOutput = {};
			pso.BlendState = { false, false, b };
			pso.SampleMask = { UINT_MAX };
			pso.RasterizerState = { r };
			pso.DepthStencilState = { d };
			pso.IBStripCutValue = {};
			pso.PrimitiveTopologyType = { D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE };
			pso.NumRenderTargets = { 1 };
			pso.RTVFormats[0] = { DXGI_FORMAT_R8G8B8A8_UNORM };
			pso.DSVFormat = { DXGI_FORMAT_D24_UNORM_S8_UINT };
			pso.SampleDesc = { 1, 0 };
			pso.NodeMask = {};
			pso.CachedPSO = {};
			pso.Flags = {};

			return pso;
		}

		struct EffectInitial { D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{ Initial() }; };

		// PSO for shadow map pass.
		struct EffectShadow { D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{ Shadow() }; };

		// PSO for SSAO.
		struct EffectSSAO { D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{ SSAO() }; };

		// PSO for sky.
		struct EffectSky { D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{ Sky() }; };

		// PSO for highlight
		struct EffectHighlight { D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{ Highlight() }; };
	}
}

class PipelineState
{
public:
	PipelineState() = default;

	PipelineState(
		ID3D12RootSignature* rootSignature, D3D12_INPUT_LAYOUT_DESC inputLayout
	) : m_rootSignature{ rootSignature }, m_inputLayout{ inputLayout } {};

	~PipelineState() = default;

	PipelineState(PipelineState&&) = default;
	PipelineState& operator= (PipelineState&&) = default;

	PipelineState(PipelineState const&) = delete;
	PipelineState& operator= (PipelineState const&) = delete;

	auto GetPipeline(const std::uint32_t PSO) noexcept { return m_PSO[PSO].Get(); };
	

	void CreatePipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& setDesc, ID3D12Device3* device,
		const Shaders::OutShaders& inShader, const std::uint32_t namePipelineState
	)
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
		ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		psoDesc.pRootSignature = m_rootSignature;
		psoDesc.VS =
		{
			reinterpret_cast<BYTE*>(inShader.VS->GetBufferPointer()),
			inShader.VS->GetBufferSize()
		};
		if (inShader.GS)
		{
			psoDesc.GS =
			{
				reinterpret_cast<BYTE*>(inShader.GS->GetBufferPointer()),
				inShader.GS->GetBufferSize()
			};
		}
		psoDesc.PS =
		{
			reinterpret_cast<BYTE*>(inShader.PS->GetBufferPointer()),
			inShader.PS->GetBufferSize()
		};
		psoDesc.StreamOutput = setDesc.StreamOutput;
		psoDesc.BlendState = setDesc.BlendState;
		psoDesc.SampleMask = setDesc.SampleMask;
		psoDesc.RasterizerState = setDesc.RasterizerState;
		psoDesc.DepthStencilState = setDesc.DepthStencilState;
		psoDesc.InputLayout =
		{
			m_inputLayout.pInputElementDescs,
			(UINT)m_inputLayout.NumElements
		};
		psoDesc.IBStripCutValue = setDesc.IBStripCutValue;
		psoDesc.PrimitiveTopologyType = setDesc.PrimitiveTopologyType;
		psoDesc.NumRenderTargets = setDesc.NumRenderTargets;
		psoDesc.RTVFormats[0] = setDesc.RTVFormats[0];
		psoDesc.DSVFormat = setDesc.DSVFormat;
		psoDesc.SampleDesc = setDesc.SampleDesc;
		psoDesc.NodeMask = setDesc.NodeMask;
		psoDesc.CachedPSO = setDesc.CachedPSO;
		psoDesc.Flags = setDesc.Flags;

		DX::ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSO[namePipelineState])));
	}

private:
	ID3D12RootSignature* m_rootSignature{ nullptr };
	D3D12_INPUT_LAYOUT_DESC m_inputLayout{};

	std::unordered_map<std::uint32_t, Microsoft::WRL::ComPtr<ID3D12PipelineState>> m_PSO;
};

#endif // !PIPELINE_STATE_CLASS_H