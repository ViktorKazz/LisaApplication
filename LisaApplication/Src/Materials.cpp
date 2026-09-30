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

#include "Materials.h"
#include "Globals.h"

void LisaApp::Materials::Create(this Materials& object, const std::wstring& name, UINT material)
{
    using namespace LisaApp::Global;

    // Create a material.
    std::unique_ptr<Item::Material> mat{ nullptr };

    //std::wstring nm{};

    switch (material)
    {
    case MATERIAL::BASE:
        mat = Materials::Base();
        //nm = L"mBase";
        break;
    default:
        assert(false && L"There is no material with this number.");
        break;
    }

    object.m_materials.insert({ name, std::move(mat) });


    object.UpdateBuffer(name, true);
    object.m_matIndex++;
}

void LisaApp::Materials::UpdateMaterial(this Materials& object, const std::wstring& name, const Item::Material& updateMat)
{
    object.m_materials[name]->DiffuseSrvHeapIndex = updateMat.DiffuseSrvHeapIndex;
    object.m_materials[name]->NormalSrvHeapIndex = updateMat.NormalSrvHeapIndex;
    object.m_materials[name]->DiffuseAlbedo = updateMat.DiffuseAlbedo;
    object.m_materials[name]->FresnelR0 = updateMat.FresnelR0;
    object.m_materials[name]->Roughness = updateMat.Roughness;

    object.UpdateBuffer(name, false);
}

void LisaApp::Materials::UpdateBuffer(this Materials& object, const std::wstring& name, bool newMaterial)
{
    const auto& mat = object.m_materials[name].get();

    DirectX::XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

    Constants::MaterialData matData;
    matData.DiffuseAlbedo = mat->DiffuseAlbedo;
    matData.FresnelR0 = mat->FresnelR0;
    matData.Roughness = mat->Roughness;
    XMStoreFloat4x4(&matData.MatTransform, XMMatrixTranspose(matTransform));
    matData.DiffuseMapIndex = mat->DiffuseSrvHeapIndex;
    matData.NormalMapIndex = mat->NormalSrvHeapIndex;

    if(newMaterial)
        mat->MatCBIndex = object.m_matIndex;


    object.m_materialBuffer->CopyData(mat->MatCBIndex, matData);
}

std::unique_ptr<Item::Material> LisaApp::Materials::Base()
{
	auto material = std::make_unique<Item::Material>();

	material->MatTransform = MathHelper::Identity4x4();
	material->MatCBIndex = -1;
	material->DiffuseSrvHeapIndex = -1;
	material->NormalSrvHeapIndex = -1;
	material->DiffuseAlbedo = DirectX::XMVECTORF32{ { { 1.000000000f, 1.000000000f, 1.000000000f, 1.f } } };
	material->FresnelR0 = DirectX::XMFLOAT3(0.02f, 0.02f, 0.02f);
	material->Roughness = 0.1f;

	DirectX::XMMATRIX rotationMatrixMat =
		DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(0.0f)) *
		DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(0.0f)) *
		DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(0.0f));

	XMStoreFloat4x4(&material->MatTransform,
		DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f) *
		rotationMatrixMat *
		DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f)
	);

	return material;
}