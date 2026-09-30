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

#include "Background.h"

void LisaApp::Background::Create(
	_In_ ID3D12Device3* device,
	_In_ ID3D12GraphicsCommandList* commandList,
	_In_ ID3D12RootSignature* rootSignature,
	std::uint32_t sampleCount,
	std::uint32_t sampleQuality,
	INT material,
	float radius,
	UINT subdivisions
)
{
	CreatePso(device, rootSignature, sampleCount, sampleQuality);

	m_background = PolygonPrimitives::CreateGeoSphereCubeTex(device, commandList, radius, subdivisions);

    // Connect materials for each of the objects, shapes and meshes.
	m_background->ConnectMaterial(0, 0, material);
}

void LisaApp::Background::CreatePso(
	_In_ ID3D12Device3* device,
	_In_ ID3D12RootSignature* rootSignature,
	std::uint32_t sampleCount,
	std::uint32_t sampleQuality
)
{
	// Unzipping shaders.
   //m_newShadersPath = HelperUtilities::Unzipping("Resources\\Shaders\\rs.ls");
	m_shader.SetPathToShaders(LisaApp::HelperPath("..\\LisaApplication\\Resources\\Shaders")  /*m_newShadersPath*/);

	m_pso = { rootSignature, VertexStructs::VertexPositionNormalTextureTangentU::InputLayout };

	LisaApp::PSOConfig::EffectSky sky;

	sky.pso.SampleDesc = { sampleCount, sampleQuality };

	m_pso.CreatePipelineState(sky.pso, device, m_shader.Sky(), Sky);
}

void LisaApp::Background::Draw(_In_ ID3D12GraphicsCommandList* commandList)
{
	m_background->Draw(commandList, { m_pso.GetPipeline(Sky) });
}

void LisaApp::Background::UpdateCB(
	const DirectX::XMMATRIX& getViev, 
	const DirectX::BoundingFrustum& camFrustum,
	bool frustumCullingEnabled
)
{
	m_background->UpdateBuffer(getViev, camFrustum, frustumCullingEnabled);
}
