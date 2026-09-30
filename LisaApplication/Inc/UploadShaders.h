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

#ifndef UPLOAD_SHADERS_CLASS_H
#define UPLOAD_SHADERS_CLASS_H

#include "HelperStructs.h"
#include "pch.h"

class UploadShaders
{
public:
	UploadShaders() = default;
	~UploadShaders() = default;

	UploadShaders(const UploadShaders&) = default;
	UploadShaders& operator=(const UploadShaders&) = default;

	UploadShaders(UploadShaders&&) = default;
	UploadShaders& operator=(UploadShaders&&) = default;

	void SetPathToShaders(std::filesystem::path path) { m_path = path; };
	

	// Shader name Default.hlsl. Version 5.1.
	Shaders::OutShaders Default(const D3D_SHADER_MACRO* definesVS = nullptr, const D3D_SHADER_MACRO* definesPS = nullptr);

	// Shader name Shadows.hlsl. Version 5.1.
	Shaders::OutShaders Shadows(const D3D_SHADER_MACRO* definesVS = nullptr, const D3D_SHADER_MACRO* definesPS = nullptr);

	// Shader name ShadowDebug.hlsl. Version 5.1.
	Shaders::OutShaders ShadowDebug(const D3D_SHADER_MACRO* definesVS = nullptr, const D3D_SHADER_MACRO* definesPS = nullptr);

	// Shader name DrawNormals.hlsl. Version 5.1.
	Shaders::OutShaders DrawNormals(const D3D_SHADER_MACRO* definesVS = nullptr, const D3D_SHADER_MACRO* definesPS = nullptr);

	// Shader name Ssao.hlsl. Version 5.1.
	Shaders::OutShaders SSAO(const D3D_SHADER_MACRO* definesVS = nullptr, const D3D_SHADER_MACRO* definesPS = nullptr);

	// Shader name SsaoBlur.hlsl. Version 5.1.
	Shaders::OutShaders SSAOBlur(const D3D_SHADER_MACRO* definesVS = nullptr, const D3D_SHADER_MACRO* definesPS = nullptr);

	// Shader name Sky.hlsl. Version 5.1.
	Shaders::OutShaders Sky(const D3D_SHADER_MACRO* definesVS = nullptr, const D3D_SHADER_MACRO* definesPS = nullptr);

	// Shader name Wireframe.hlsl. Version 5.1.
	Shaders::OutShaders Wireframe(const D3D_SHADER_MACRO* definesVS = nullptr, const D3D_SHADER_MACRO* definesPS = nullptr);

	// Shader name PickingTriangle.hlsl. Version 5.1.
	Shaders::OutShaders PickingTriangle(const D3D_SHADER_MACRO* definesVS = nullptr, const D3D_SHADER_MACRO* definesPS = nullptr, const D3D_SHADER_MACRO* definesGS = nullptr);

	// Shader name PickingEdge.hlsl. Version 5.1.
	Shaders::OutShaders PickingEdge(const D3D_SHADER_MACRO* definesVS = nullptr, const D3D_SHADER_MACRO* definesPS = nullptr, const D3D_SHADER_MACRO* definesGS = nullptr);

	// Shader name PickingVertex.hlsl. Version 5.1.
	Shaders::OutShaders PickingVertex(const D3D_SHADER_MACRO* definesVS = nullptr, const D3D_SHADER_MACRO* definesPS = nullptr, const D3D_SHADER_MACRO* definesGS = nullptr);

	// Shader name PivotCenterFrame.hlsl. Version 5.1.
	Shaders::OutShaders PivotCenterFrame(const D3D_SHADER_MACRO* definesVS = nullptr, const D3D_SHADER_MACRO* definesPS = nullptr, const D3D_SHADER_MACRO* definesGS = nullptr);

	// Shader name SimpleColoring.hlsl. Version 5.1.
	Shaders::OutShaders SimpleColoring(const D3D_SHADER_MACRO* definesVS = nullptr, const D3D_SHADER_MACRO* definesPS = nullptr);

	// Shader name ColorRotationAngle.hlsl. Version 5.1.
	Shaders::OutShaders ColorRotationAngle(const D3D_SHADER_MACRO* definesVS = nullptr, const D3D_SHADER_MACRO* definesPS = nullptr, const D3D_SHADER_MACRO* definesGS = nullptr);

private:
	Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
		const std::wstring& m_filename,
		const D3D_SHADER_MACRO* m_defines,
		const std::string& m_entrypoint,
		const std::string& m_target
	);

	std::filesystem::path m_path;
};

#endif // !UPLOAD_SHADERS_CLASS_H