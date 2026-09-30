
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

void Point(VertexOut gin[3], int component, uint isComponentHold, inout TriangleStream<GeoOut> triStream)
{
    uint iPad = 0;
    
    if (component == 0)
        iPad = 0;
    if (component == 1)
        iPad = 1;
    if (component == 2)
        iPad = 2;
    
    // Compute the local coordinate system of the sprite relative to the world
	// space such that the billboard is aligned with the y-axis and faces the eye.

    // Camera position.
    float3 cameraPos = gEyePosW;
    
    // Direction of the vector from the camera to the center of the object.
    float3 look = gin[iPad].PosW - cameraPos;
    look = normalize(look);
    
    // Define the "up" vector.
    float3 up = float3(0.0f, 1.0f, 0.0f);

    // Calculates a normalized rightward vector that is perpendicular to both the "up" vector and the "look" vector.
    float3 right = normalize(cross(up, look));
    
    // Recalculate "up" according to "right" and "look".
    up = cross(look, right);

	// Compute triangle strip vertices (quad) in world space.
    
    float offset = 0;
    
    if (isComponentHold == 0)
        offset = 0.0025f;
    else
        offset = 0.002f;
    
    // The further the camera is, the larger the dot is drawn.
            
    float d = length(gin[iPad].PosW - gEyePosW);
    float scale = 1.0f;
    
    
    if (d > 1.0f)
    {
        d = (d - 1.0f) / 2;
        scale += d;
    }
    
    float halfWidth = offset * scale;
    float halfHeight = offset * scale;
	
    float4 v[4];
    v[0] = float4(gin[iPad].PosW + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(gin[iPad].PosW - halfWidth * right - halfHeight * up, 1.0f);
    v[2] = float4(gin[iPad].PosW + halfWidth * right + halfHeight * up, 1.0f);
    v[3] = float4(gin[iPad].PosW - halfWidth * right + halfHeight * up, 1.0f);
            
    GeoOut gout;
	[unroll]
    for (int i = 0; i < 4; ++i)
    {
        gout.PosH = mul(v[i], gViewProj);
        gout.PosW = v[i].xyz;
        gout.IsComponentHold = isComponentHold;
        triStream.Append(gout);
    }
    triStream.RestartStrip();
}

[maxvertexcount(16)]
void GS(triangle VertexOut gin[3], uint primitiveID : SV_PrimitiveID, inout TriangleStream<GeoOut> triStream)
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
                Point(gin, gHoldData[i].Component, 1, triStream);
            }
        }
    }
        
    // Pre select.
    
    if (triangleNumber == primitiveID && drawTriangle)
    {
        Point(gin, componentTriangle, 0, triStream);
    }
}

float4 PS(GeoOut pin) : SV_Target
{
    uint holdComponent = pin.IsComponentHold;
    
    float4 color = holdComponent ? float4(1.0f, 1.0f, 1.0f, 1.0f) : float4(1.0f, 1.0f, 0.0f, 1.0f);
       
    return color;
}
