//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

// Defaults for number of lights.
#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif

// Include common HLSL code.
#include "Common.hlsl"

struct VertexIn
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 TexC : TEXCOORD;
    float3 TangentU : TANGENT;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION2;
    float3 NormalW : NORMAL;
    float2 TexC : TEXCOORD;
    
    // nointerpolation is used so the index is not interpolated 
	// across the triangle.
    nointerpolation uint MatIndex : MATINDEX;
};

static const float weights[] =
{
    0.1061154f,
    0.102850571f,
    0.102850571f,
    0.09364651f,
    0.09364651f,
    0.0801001f,
    0.0801001f,
    0.06436224f,
    0.06436224f,
    0.0485831723f,
    0.0485831723f,
    0.0344506279f,
    0.0344506279f,
    0.0229490642f,
    0.0229490642f,
};

static const float2 offsets[] =
{
    float2(0.0f, 0.0f),
    float2(0.00375f, 0.006250001f),
    float2(-0.00375f, -0.006250001f),
    float2(0.00875f, 0.01458333f),
    float2(-0.00875f, -0.01458333f),
    float2(0.01375f, 0.02291667f),
    float2(-0.01375f, -0.02291667f),
    float2(0.01875f, 0.03125f),
    float2(-0.01875f, -0.03125f),
    float2(0.02375f, 0.03958334f),
    float2(-0.02375f, -0.03958334f),
    float2(0.02875f, 0.04791667f),
    float2(-0.02875f, -0.04791667f),
    float2(0.03375f, 0.05625f),
    float2(-0.03375f, -0.05625f),
};

float4 mainBlur(VertexOut pin) : SV_TARGET
{
    // Fetch the material data.
    MaterialData matData = gMaterialData[pin.MatIndex];
    float4 diffuseAlbedo = matData.DiffuseAlbedo;
  
    uint diffuseMapIndex = matData.DiffuseMapIndex;
    
    
    // Dynamically look up the texture in the array.
    
    
    diffuseAlbedo = float4(0.0f, 0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 15; i++)
    {
        diffuseAlbedo += gTextureMaps[diffuseMapIndex].Sample(gsamAnisotropicWrap, pin.TexC + offsets[i]) * weights[i];
    }

    return diffuseAlbedo;
}
