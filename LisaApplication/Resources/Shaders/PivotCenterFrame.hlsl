
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
    float3 PosW : POSITION;
    float4 ColorW : COLOR;
};

struct VertexOut
{
    float3 CenterW : POSITION;
    float4 ColorW : COLOR;
    float4x4 World : WORLD;
    uint Mode : INDEX;
};

struct GeoOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float4 ColorW : COLOR;
    uint PrimID : SV_PrimitiveID;
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
	// Just pass data over to geometry shader.
    vout.CenterW = posW.xyz;
    vout.ColorW = vin.ColorW;
    vout.World = world;
    vout.Mode = instData.Mode;
    
    return vout;
}

 // We expand each point into a quad (5 vertices), so the maximum number of vertices
 // we output per geometry shader invocation is 5.
[maxvertexcount(6)]
void GS(triangle VertexOut gin[3], uint primID : SV_PrimitiveID,  inout LineStream<GeoOut> triStream)
{
 //   uint iPad0 = 0;

    //float4 v[5];
    //v[0] = float4(gin[iPad0].CenterW + halfWidth * right - halfHeight * up, 1.0f);
    //v[1] = float4(gin[iPad0].CenterW - halfWidth * right - halfHeight * up, 1.0f);
    //v[2] = float4(gin[iPad0].CenterW - halfWidth * right + halfHeight * up, 1.0f);
    //v[3] = float4(gin[iPad0].CenterW + halfWidth * right + halfHeight * up, 1.0f);
    //v[4] = float4(gin[iPad0].CenterW + halfWidth * right - halfHeight * up, 1.0f);
   
    float4 v[6];
    int3 iter = int3(0, 1, 2);
    float sign = 1.0f;
    
    if (primID == 0)
    {
        iter = int3(2, 0, 1);
    }
    else if (primID == 1)
    {
        iter = int3(1, 2, 0);
        sign = -1.0f;
    }
    
    v[0] = float4(gin[iter.x].CenterW, 1.0f);
    v[1] = float4(gin[iter.y].CenterW, 1.0f);
    v[2] = float4(gin[iter.z].CenterW, 1.0f);
    
    // Transform quad vertices to world space and output 
	// them as a triangle strip.	
	
    GeoOut gout;
	[unroll]
    for (int i = 0; i < 3; ++i)
    {
        gout.PosH = mul(v[i], gViewProj);
        gout.PosW = v[i].xyz;      
        gout.PrimID = primID;        
        gout.ColorW = gin[0].ColorW;       
        gout.Mode = gin[0].Mode;
        triStream.Append(gout);    
    }
    triStream.RestartStrip();

    float3 center = (gin[iter.x].CenterW + gin[iter.z].CenterW) * 0.5f;
    
    // Compute the local coordinate system of the sprite relative to the world
	// space such that the billboard is aligned with the y-axis and faces the eye.

    // Camera position.
    float3 cameraPos = gEyePosW;
    
    // Direction of the vector from the camera to the center of the object.
    float3 look = center - cameraPos;
    float3 scale = length(look);
    look = normalize(look);
       
    // Define the "up" vector.
    float3 up = float3(0.0f, 1.0f, 0.0f);

    // Calculates a normalized rightward vector that is perpendicular to both the "up" vector and the "look" vector.
    float3 right = normalize(cross(up, look));
    
    // Recalculate "up" according to "right" and "look".
    up = cross(look, right);
    
	// Compute triangle strip vertices (quad) in world space.
    float halfWidth = (0.001f * sign) * scale.x;
    float halfHeight = (0.001f * sign) * scale.x;
    
    v[3] = float4(center - halfWidth * right - halfHeight * up, 1.0f);
    v[4] = float4(center + halfWidth * right - halfHeight * up, 1.0f);
    v[5] = float4(center + halfWidth * right + halfHeight * up, 1.0f);
    
    [unroll]
    for (int j = 3; j < 6; ++j)
    {
        gout.PosH = mul(v[j], gViewProj);
        gout.PosW = v[j].xyz;
        gout.PrimID = primID;
        gout.ColorW = gin[0].ColorW;
        gout.Mode = gin[0].Mode;
        triStream.Append(gout);
    }
    triStream.RestartStrip();
}

float4 PS(GeoOut pin) : SV_Target
{
    uint mode = pin.Mode;
    
    float4 color;
    
    if (mode)
    {
        color = lerp(pin.ColorW, float4(1.0f, 1.0f, 1.0f, 1.0f), 0.85f);
    }
    else
    {
        color = lerp(pin.ColorW, float4(1.0f, 1.0f, 1.0f, 1.0f), 0.5f);
    }

    return color;
}
