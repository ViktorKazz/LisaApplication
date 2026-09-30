
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

[maxvertexcount(3)]
void GS(triangle VertexOut gin[3], uint primitiveID : SV_PrimitiveID, inout TriangleStream<GeoOut> triStream)
{
    uint drawTriangle = gin[0].DrawTriangle;
    uint triangleNumber = gin[0].TriangleNumber;
    uint componentTriangle = gin[0].ComponentTriangle;
    uint holdDataSize = gin[0].HoldDataSize;
    
    GeoOut gout;

    // Pre select.

    if (triangleNumber == primitiveID && drawTriangle)
    {
        [unroll]
        for (int i = 0; i < 3; ++i)
        {
            gout.PosH = gin[i].PosH;
            gout.PosW = gin[i].PosW;
            gout.IsComponentHold = 0;
            triStream.Append(gout);
        }
    }
   
    if (holdDataSize)
    {
        uint t = 0;
        
        for (uint i = 0; i < holdDataSize; ++i)
        {
            t = gHoldData[i].Triangle;
              
            if (t == primitiveID)
            {
                [unroll]
                for (int i = 0; i < 3; ++i)
                {
                    gout.PosH = gin[i].PosH;
                    gout.PosW = gin[i].PosW;
                    gout.IsComponentHold = 1;
                    triStream.Append(gout);
                }
            }
        }       
    }
        
}

float4 PS(GeoOut pin) : SV_Target
{
    uint holdComponent = pin.IsComponentHold;
    
    float4 color = holdComponent ? float4(1.0f, 0.0f, 0.0f, 0.5f) : float4(0.5f, 0.5f, 0.0f, 0.5f);
    
    clip(color.a - 0.1f);
    
    return color;
}
