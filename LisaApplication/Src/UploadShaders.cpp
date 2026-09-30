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

#include "UploadShaders.h"
#include "HelperUtilities.h"

Microsoft::WRL::ComPtr<ID3DBlob> UploadShaders::CompileShader(
	const std::wstring& m_filename,
	const D3D_SHADER_MACRO* m_defines,
	const std::string& m_entrypoint,
	const std::string& m_target
)
{
	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr{ S_OK };

	Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errors;
	hr = D3DCompileFromFile(m_filename.c_str(), m_defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		m_entrypoint.c_str(), m_target.c_str(), compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr)
		OutputDebugStringA((char*)errors->GetBufferPointer());

	DX::ThrowIfFailed(hr);

	return byteCode;
}

Shaders::OutShaders UploadShaders::Default(const D3D_SHADER_MACRO* definesVS, const D3D_SHADER_MACRO* definesPS)
{
	std::filesystem::path path{ m_path.string() };
	path.append("Default.hlsl");

	Shaders::OutShaders os{
		.VS = CompileShader(path, definesVS, "VS", "vs_5_1"),
		.PS = CompileShader(path, definesPS, "PS", "ps_5_1")
	};

	return os;
}

Shaders::OutShaders UploadShaders::Shadows(const D3D_SHADER_MACRO* definesVS, const D3D_SHADER_MACRO* definesPS)
{
	std::filesystem::path path{ m_path.string() };
	path.append("Shadows.hlsl");

	Shaders::OutShaders os{
		.VS = CompileShader(path, definesVS, "VS", "vs_5_1"),
		.PS = CompileShader(path, definesPS, "PS", "ps_5_1")
	};

	return os;
};

Shaders::OutShaders UploadShaders::ShadowDebug(const D3D_SHADER_MACRO* definesVS, const D3D_SHADER_MACRO* definesPS)
{
	std::filesystem::path path{ m_path.string() };
	path.append("ShadowDebug.hlsl");

	Shaders::OutShaders os{
		.VS = CompileShader(path, definesVS, "VS", "vs_5_1"),
		.PS = CompileShader(path, definesPS, "PS", "ps_5_1")
	};

	return os;
};

Shaders::OutShaders UploadShaders::DrawNormals(const D3D_SHADER_MACRO* definesVS, const D3D_SHADER_MACRO* definesPS)
{
	std::filesystem::path path{ m_path.string() };
	path.append("DrawNormals.hlsl");

	Shaders::OutShaders os{
		.VS = CompileShader(path, definesVS, "VS", "vs_5_1"),
		.PS = CompileShader(path, definesPS, "PS", "ps_5_1")
	};

	return os;
};

Shaders::OutShaders UploadShaders::SSAO(const D3D_SHADER_MACRO* definesVS, const D3D_SHADER_MACRO* definesPS)
{
	std::filesystem::path path{ m_path.string() };
	path.append("Ssao.hlsl");

	Shaders::OutShaders os{
		.VS = CompileShader(path, definesVS, "VS", "vs_5_1"),
		.PS = CompileShader(path, definesPS, "PS", "ps_5_1")
	};

	return os;
};

Shaders::OutShaders UploadShaders::SSAOBlur(const D3D_SHADER_MACRO* definesVS, const D3D_SHADER_MACRO* definesPS)
{
	std::filesystem::path path{ m_path.string() };
	path.append("SsaoBlur.hlsl");

	Shaders::OutShaders os{
		.VS = CompileShader(path, definesVS, "VS", "vs_5_1"),
		.PS = CompileShader(path, definesPS, "PS", "ps_5_1")
	};

	return os;
};

Shaders::OutShaders UploadShaders::Sky(const D3D_SHADER_MACRO* definesVS, const D3D_SHADER_MACRO* definesPS)
{
	std::filesystem::path path{ m_path.string() };
	path.append("Sky.hlsl");

	Shaders::OutShaders os{
		.VS = CompileShader(path, definesVS, "VS", "vs_5_1"),
		.PS = CompileShader(path, definesPS, "PS", "ps_5_1")
	};

	return os;
};

Shaders::OutShaders UploadShaders::Wireframe(const D3D_SHADER_MACRO* definesVS, const D3D_SHADER_MACRO* definesPS)
{
	std::filesystem::path path{ m_path.string() };
	path.append("Wireframe.hlsl");

	Shaders::OutShaders os{
		.VS = CompileShader(path, definesVS, "VS", "vs_5_1"),
		.PS = CompileShader(path, definesPS, "PS", "ps_5_1")
	};

	return os;
};

Shaders::OutShaders UploadShaders::PickingTriangle(const D3D_SHADER_MACRO* definesVS, const D3D_SHADER_MACRO* definesPS, const D3D_SHADER_MACRO* definesGS)
{
	std::filesystem::path path{ m_path.string() };
	path.append("PickingTriangle.hlsl");

	Shaders::OutShaders os{
		.VS = CompileShader(path, definesVS, "VS", "vs_5_1"),
		.PS = CompileShader(path, definesPS, "PS", "ps_5_1"),
		.GS = CompileShader(path, definesGS, "GS", "gs_5_1")
	};

	return os;
};

Shaders::OutShaders UploadShaders::PickingEdge(const D3D_SHADER_MACRO* definesVS, const D3D_SHADER_MACRO* definesPS, const D3D_SHADER_MACRO* definesGS)
{
	std::filesystem::path path{ m_path.string() };
	path.append("PickingEdge.hlsl");

	Shaders::OutShaders os{
		.VS = CompileShader(path, definesVS, "VS", "vs_5_1"),
		.PS = CompileShader(path, definesPS, "PS", "ps_5_1"),
		.GS = CompileShader(path, definesGS, "GS", "gs_5_1")
	};

	return os;
};

Shaders::OutShaders UploadShaders::PickingVertex(const D3D_SHADER_MACRO* definesVS, const D3D_SHADER_MACRO* definesPS, const D3D_SHADER_MACRO* definesGS)
{
	std::filesystem::path path{ m_path.string() };
	path.append("PickingVertex.hlsl");

	Shaders::OutShaders os{
		.VS = CompileShader(path, definesVS, "VS", "vs_5_1"),
		.PS = CompileShader(path, definesPS, "PS", "ps_5_1"),
		.GS = CompileShader(path, definesGS, "GS", "gs_5_1")
	};

	return os;
};

Shaders::OutShaders UploadShaders::PivotCenterFrame(const D3D_SHADER_MACRO* definesVS, const D3D_SHADER_MACRO* definesPS, const D3D_SHADER_MACRO* definesGS)
{
	std::filesystem::path path{ m_path.string() };
	path.append("PivotCenterFrame.hlsl");

	Shaders::OutShaders os{
		.VS = CompileShader(path, definesVS, "VS", "vs_5_1"),
		.PS = CompileShader(path, definesPS, "PS", "ps_5_1"),
		.GS = CompileShader(path, definesGS, "GS", "gs_5_1")
	};

	return os;
}

Shaders::OutShaders UploadShaders::SimpleColoring(const D3D_SHADER_MACRO* definesVS, const D3D_SHADER_MACRO* definesPS)
{
	std::filesystem::path path{ m_path.string() };
	path.append("SimpleColoring.hlsl");

	Shaders::OutShaders os{
		.VS = CompileShader(path, definesVS, "VS", "vs_5_1"),
		.PS = CompileShader(path, definesPS, "PS", "ps_5_1")
	};

	return os;
}
Shaders::OutShaders UploadShaders::ColorRotationAngle(const D3D_SHADER_MACRO* definesVS, const D3D_SHADER_MACRO* definesPS, const D3D_SHADER_MACRO* definesGS)
{
	std::filesystem::path path{ m_path.string() };
	path.append("ColorRotationAngle.hlsl");

	Shaders::OutShaders os{
		.VS = CompileShader(path, definesVS, "VS", "vs_5_1"),
		.PS = CompileShader(path, definesPS, "PS", "ps_5_1"),
		.GS = CompileShader(path, definesGS, "GS", "gs_5_1")
	};

	return os;
}
