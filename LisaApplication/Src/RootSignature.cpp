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

#include "RootSignature.h"

RootSignature::RootSignature()
{
}

RootSignature::~RootSignature()
{
}

void RootSignature::RootSignatureInitial(ID3D12Device3* device)
{
	// Shader programs typically require resources as input (constant buffers,
	// textures, samplers).  The root signature defines the resources the shader
	// programs expect.  If we think of the shader programs as a function, and
	// the input resources as function parameters, then the root signature can be
	// thought of as defining the function signature.  

	// Now just understand that the code ->
	// Root parameter can be a table, root descriptor or root constants.
	

	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	// This is the highest version the sample supports. 
	// If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

	if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
	{
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	// !!!--->> Create a single descriptor table of CBVs.
	CD3DX12_DESCRIPTOR_RANGE ranges[1]{};
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 7, 0, 0);

	CD3DX12_ROOT_PARAMETER slotRootParameter[4]{};
	slotRootParameter[0].InitAsShaderResourceView(0, 1);
	slotRootParameter[1].InitAsShaderResourceView(1, 1);
	slotRootParameter[2].InitAsConstantBufferView(0);
	slotRootParameter[3].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);

	auto staticSamplers = GetStaticSamplers();

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Init(_countof(slotRootParameter), slotRootParameter, (UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// -> creates a root parameter that expects a descriptor table of 1 CBV that gets bound 
	// to constant buffer register 0 (i.e., register(b0)in the HLSL code).

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

	HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	DX::ThrowIfFailed(hr);

	
	DX::ThrowIfFailed(device->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_initialRootSignature))
	);
}

void RootSignature::RootSignaturePrepareSSAO(ID3D12Device3* device)
{
	CD3DX12_DESCRIPTOR_RANGE texTable0{};
	texTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0, 0);//t0, t1, t2

	CD3DX12_DESCRIPTOR_RANGE texTable1{};
	texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 10, 3, 0);// t3...t10?

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[6]{};

	// Perfomance TIP: Order from most frequent to least frequent.
	slotRootParameter[0].InitAsShaderResourceView(0, 1);//t0
	slotRootParameter[1].InitAsShaderResourceView(1, 1);//t1
	slotRootParameter[2].InitAsShaderResourceView(2, 1);//t2
	slotRootParameter[3].InitAsConstantBufferView(0);
	slotRootParameter[4].InitAsDescriptorTable(1, &texTable0, D3D12_SHADER_VISIBILITY_PIXEL);
	slotRootParameter[5].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_PIXEL);


	auto staticSamplers = GetStaticSamplers();

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(6, slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	DX::ThrowIfFailed(hr);

	DX::ThrowIfFailed(device->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(m_prepareSsaoRootSignature.GetAddressOf())));
}

void RootSignature::RootSignatureSSAO(ID3D12Device3* device)
{
	CD3DX12_DESCRIPTOR_RANGE texTable0{};
	texTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 0);

	CD3DX12_DESCRIPTOR_RANGE texTable1{};
	texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0);

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[4]{};

	// Perfomance TIP: Order from most frequent to least frequent.
	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstants(1, 1);
	slotRootParameter[2].InitAsDescriptorTable(1, &texTable0, D3D12_SHADER_VISIBILITY_PIXEL);
	slotRootParameter[3].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_PIXEL);

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC depthMapSam(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
		0.0f,
		0,
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE);

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	std::array<CD3DX12_STATIC_SAMPLER_DESC, 4> staticSamplers =
	{
		pointClamp, linearClamp, depthMapSam, linearWrap
	};

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(4, slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	DX::ThrowIfFailed(hr);

	DX::ThrowIfFailed(device->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(m_ssaoRootSignature.GetAddressOf())));
}

std::array<const D3D12_STATIC_SAMPLER_DESC, 7> RootSignature::GetStaticSamplers()
{
	// Applications usually only need a handful of samplers.  So just define them all up front 
	// and keep them available as part of the root signature.                                

	D3D12_STATIC_SAMPLER_DESC pointWrap{};
	pointWrap.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	pointWrap.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pointWrap.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pointWrap.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pointWrap.MipLODBias = 0.0f;
	pointWrap.MaxAnisotropy = 0;
	pointWrap.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	pointWrap.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	pointWrap.MinLOD = 0.0f;
	pointWrap.MaxLOD = D3D12_FLOAT32_MAX;
	pointWrap.ShaderRegister = 0;
	pointWrap.RegisterSpace = 0;
	pointWrap.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC pointClamp{};
	pointClamp.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	pointClamp.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pointClamp.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pointClamp.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pointClamp.MipLODBias = 0.0f;
	pointClamp.MaxAnisotropy = 0;
	pointClamp.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	pointClamp.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	pointClamp.MinLOD = 0.0f;
	pointClamp.MaxLOD = D3D12_FLOAT32_MAX;
	pointClamp.ShaderRegister = 1;
	pointClamp.RegisterSpace = 0;
	pointClamp.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC linearWrap{};
	linearWrap.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	linearWrap.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	linearWrap.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	linearWrap.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	linearWrap.MipLODBias = 0.0f;
	linearWrap.MaxAnisotropy = 0;
	linearWrap.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	linearWrap.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	linearWrap.MinLOD = 0.0f;
	linearWrap.MaxLOD = D3D12_FLOAT32_MAX;
	linearWrap.ShaderRegister = 2;
	linearWrap.RegisterSpace = 0;
	linearWrap.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC linearClamp{};
	linearClamp.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	linearClamp.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	linearClamp.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	linearClamp.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	linearClamp.MipLODBias = 0.0f;
	linearClamp.MaxAnisotropy = 0;
	linearClamp.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	linearClamp.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	linearClamp.MinLOD = 0.0f;
	linearClamp.MaxLOD = D3D12_FLOAT32_MAX;
	linearClamp.ShaderRegister = 3;
	linearClamp.RegisterSpace = 0;
	linearClamp.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC anisotropicWrap{};
	anisotropicWrap.Filter = D3D12_FILTER_ANISOTROPIC;
	anisotropicWrap.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//D3D12_TEXTURE_ADDRESS_MODE_BORDER for transparent with shader
	anisotropicWrap.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//D3D12_TEXTURE_ADDRESS_MODE_BORDER for transparent with shader
	anisotropicWrap.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//D3D12_TEXTURE_ADDRESS_MODE_BORDER for transparent with shader
	anisotropicWrap.MipLODBias = 0.0f;
	anisotropicWrap.MaxAnisotropy = 8;
	anisotropicWrap.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	anisotropicWrap.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	anisotropicWrap.MinLOD = 0.0f;
	anisotropicWrap.MaxLOD = D3D12_FLOAT32_MAX;
	anisotropicWrap.ShaderRegister = 4;
	anisotropicWrap.RegisterSpace = 0;
	anisotropicWrap.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC anisotropicClamp{};
	anisotropicClamp.Filter = D3D12_FILTER_ANISOTROPIC;
	anisotropicClamp.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	anisotropicClamp.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	anisotropicClamp.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	anisotropicClamp.MipLODBias = 0.0f;
	anisotropicClamp.MaxAnisotropy = 8;
	anisotropicClamp.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	anisotropicClamp.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	anisotropicClamp.MinLOD = 0.0f;
	anisotropicClamp.MaxLOD = D3D12_FLOAT32_MAX;
	anisotropicClamp.ShaderRegister = 5;
	anisotropicClamp.RegisterSpace = 0;
	anisotropicClamp.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC shadow{};
	shadow.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	shadow.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	shadow.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	shadow.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	shadow.MipLODBias = 0.0f;
	shadow.MaxAnisotropy = 16;
	shadow.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	shadow.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	shadow.MinLOD = 0.0f;
	shadow.MaxLOD = D3D12_FLOAT32_MAX;
	shadow.ShaderRegister = 6;
	shadow.RegisterSpace = 0;
	shadow.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	return {
		pointWrap, pointClamp, linearWrap,
		linearClamp, anisotropicWrap, anisotropicClamp, shadow };
}