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
    float4 ColorW : COLOR;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float4x4 World : POSITION1;
    float3 pxlPos : POSITION8;
    float4 ColorW : COLOR;
    uint Mode : INDEX;
};

VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
{
    VertexOut vout = (VertexOut) 0.0f;
	
	// Fetch the instance data.
    InstanceData instData = gInstanceData[instanceID];
    float4x4 world = instData.World;
    
    vout.World = instData.World;
    vout.Mode = instData.Mode;
   
    // In this way, each instance can be given its own color.
#ifdef COLORING_PER_INSTANCES
    vout.ColorW = instData.Color;
#else
    if (instanceID == 0)
        vout.ColorW = vin.ColorW;
    else
        vout.ColorW = instData.Color;
#endif
    
    // Transform to world space.
    
    float4 posW = mul(float4(vin.PosL, 1.0f), world);
    
    if (instData.Hide == 1)
        posW = float4(0.0f, 0.0f, 0.0f, 0.0f);

    vout.PosW = posW.xyz;
    vout.pxlPos = posW.xyz;
    
    // Transform to homogeneous clip space.
    vout.PosH = mul(posW, gViewProj);
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    uint mode = pin.Mode;
    float4x4 world = pin.World;

    float beta = 0.0f;
    
#ifdef CHANGE_COLOR
    // If the pixel normal faces away from the camera, the color changes.
    
    float3 center = float3(world[3].x, world[3].y, world[3].z);
    float3 pxlPos = pin.pxlPos;
    
    float3 length = normalize(pxlPos - center);
    float3 look = normalize(gEyePosW - center);
    
    // The angle between two vectors is determined through the scalar product.

    float cosAlpha = dot(look, length);
    
    // Limitation in [-1,1]
    cosAlpha = max(-1.0f, min(1.0f, cosAlpha));
    // In radians.
    float alpha = acos(cosAlpha);

    const float PI = 3.14159265f;
    beta = (PI/2) - alpha;

    float angle = degrees(beta);
#endif
    
    float4 colorMode1 = float4(0.4f, 0.4f, 0.5f, 1.0f);
    float4 colorMode2 = float4(0.9f, 0.9f, 1.0f, 1.0f);
    
#ifdef PIVOT_SPHERE
    colorMode1 = float4(0.0f, 0.0f, 0.1f, 0.5f);
    colorMode2 = colorMode1;
#endif
    
    float4 color = pin.ColorW;
    float4 changeColor = float4(1.0f, 1.0f, 1.0f, 0.0f);
    
    if (beta < 0.0f)
        color = changeColor;
    
    if (mode == 1)
    {
        if (beta < 0.0f)
            color = changeColor;
        else
            color = colorMode1;
    }
    else if (mode == 2)
    {
        if (beta < 0.0f)
            color = changeColor;
        else
            color = colorMode2;
    }
    
    clip(color.a - 0.1f);
    
    return color;
}