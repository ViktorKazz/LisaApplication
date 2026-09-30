
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
    uint Mode : INDEX;
};

VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
{
    VertexOut vout = (VertexOut) 0.0f;
	
    // Fetch the instance data.
    InstanceData instData = gInstanceData[instanceID];
    
    float4x4 world = instData.World;
	
    // Transform to world space.
    float4 posW = mul(float4(vin.PosL, 1.0f), world);
    
    if (instData.Hide == 1)
        posW = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    vout.PosW = posW.xyz;
    
    // Transform to homogeneous clip space.
    vout.PosH = mul(posW, gViewProj);
    
    vout.Mode = instData.Mode;

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    uint mode = pin.Mode;
    
    float4 frameColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
    
    if (mode == 1)
        frameColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
    
    else if (mode == 2)
        frameColor = float4(0.258f, 0.667f, 1.0f, 1.0f);
    
    else if (mode == 3)
        frameColor = float4(0.258f, 0.667f, 1.0f, 1.0f);
    
    else if (mode == 4)
        frameColor = float4(0.258f, 0.667f, 1.0f, 1.0f);
    
    else if (mode == 5)
        frameColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
       
    return frameColor;
}
