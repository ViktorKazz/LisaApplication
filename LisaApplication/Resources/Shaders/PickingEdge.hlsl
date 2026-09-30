
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
    float3 PosW : POSITION1;
    uint DrawTriangle : INDEX;
    uint TriangleNumber : INDEX1;
    uint ComponentTriangle : INDEX2;
    uint HoldDataSize : INDEX3;
};

struct GeoOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION1;
    uint IsComponentHold : INDEX1;
};

VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
{
    VertexOut vout = (VertexOut) 0.0f;
	
    // Fetch the instance data.
    InstanceData instData = gInstanceData[instanceID];
    
    float4x4 world = instData.World;
	
    // Transform to world space.
    float4 posW = mul(float4(vin.PosL, 1.0f), world);
    
    vout.PosW = posW.xyz;
    
    // Transform to homogeneous clip space.
    vout.PosH = mul(posW, gViewProj);
    
    vout.DrawTriangle = instData.DrawTriangle;
    vout.TriangleNumber = instData.TriangleNumber;
    vout.ComponentTriangle = instData.ComponentTriangle;
    vout.HoldDataSize = instData.HoldDataSize;
    
    return vout;
}

void Line(VertexOut gin[3], int component, uint isComponentHold, inout LineStream<GeoOut> triStream)
{
    int2 i = int2(0, 0);
    
    if (component == 0)
        i = int2(0, 1);
    if (component == 1)
        i = int2(1, 2);
    if (component == 2)
        i = int2(2, 0);
    
    GeoOut gout;
    
    gout.PosH = gin[i.x].PosH;
    gout.PosW = gin[i.x].PosW;
    gout.IsComponentHold = isComponentHold;
                    
    triStream.Append(gout);
    
    gout.PosH = gin[i.y].PosH;
    gout.PosW = gin[i.y].PosW;
    gout.IsComponentHold = isComponentHold;
                    
    triStream.Append(gout);
    triStream.RestartStrip();
}

[maxvertexcount(8)]
void GS(triangle VertexOut gin[3], uint primitiveID : SV_PrimitiveID, inout LineStream<GeoOut> triStream)
{
    uint drawTriangle = gin[0].DrawTriangle;
    uint triangleNumber = gin[0].TriangleNumber;
    uint componentTriangle = gin[0].ComponentTriangle;
    uint holdDataSize = gin[0].HoldDataSize;
    
    
    if (holdDataSize)
    {
        uint t = 0;
        
        for (uint i = 0; i < holdDataSize; ++i)
        {
            t = gHoldData[i].Triangle;
              
            if (t == primitiveID)
            {
                Line(gin, gHoldData[i].Component, 1, triStream);             
            }
        }       
    }
        
    // Pre select.

    if (triangleNumber == primitiveID && drawTriangle)
    {
        Line(gin, componentTriangle, 0, triStream);        
    }
}

float4 PS(GeoOut pin) : SV_Target
{
    uint holdComponent = pin.IsComponentHold;
    
    float4 color = holdComponent ? float4(1.0f, 1.0f, 1.0f, 1.0f) : float4(1.0f, 1.0f, 0.0f, 1.0f);
       
    return color;
}
