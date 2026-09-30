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

#ifndef ROOT_SIGNATURE_CLASS_H
#define ROOT_SIGNATURE_CLASS_H

#include "pch.h"

class RootSignature
{
public:
	RootSignature();
	virtual ~RootSignature();

	ID3D12RootSignature* GetInitialRootSignature() const noexcept { return m_initialRootSignature.Get(); };
	ID3D12RootSignature* GetPrepareSsaoRootSignature() const noexcept { return m_prepareSsaoRootSignature.Get(); };
	ID3D12RootSignature* GetSsaoRootSignature() const noexcept { return m_ssaoRootSignature.Get(); };

	void RootSignatureInitial(ID3D12Device3* device);
	void RootSignaturePrepareSSAO(ID3D12Device3* device);
	void RootSignatureSSAO(ID3D12Device3* device);

	std::array<const D3D12_STATIC_SAMPLER_DESC, 7> GetStaticSamplers();

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_initialRootSignature{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_prepareSsaoRootSignature{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_ssaoRootSignature{ nullptr };
};

#endif // !ROOT_SIGNATURE_CLASS_H