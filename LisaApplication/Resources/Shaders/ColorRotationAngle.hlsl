// Include common HLSL code.
#include "Common.hlsl"

struct VertexIn
{
    float3 PosL : POSITION;
    float4 ColorW : COLOR;
};

struct VertexOut
{
    float3 PosW : POSITION;
    float4 ColorW : COLOR;
};

struct GeoOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float4 ColorW : COLOR;
};

VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
{
    VertexOut vout = (VertexOut) 0.0f;
	
	// Fetch the instance data.
    InstanceData instData = gInstanceData[instanceID];
    float4x4 world = instData.World;
    
    vout.ColorW = vin.ColorW;
    
    // Transform to world space.
    float4 posW = mul(float4(vin.PosL, 1.0f), world);

    vout.PosW = posW.xyz;
   
    return vout;
}

// The function of rotating a vector by an arbitrary angle using the Rodrigues rotation formula.
//
// Formal expression: if v is a vector in three-dimensional space,
// and k is a unit vector describing the axis of rotation about which v rotates by an angle θ, 
// then the rotation vector vrot is calculated by the formula:
// vrot = v cos θ + (k × v) sin θ + k (k ⋅ v) (1 − cos θ).
//
// Formula components:
// v cos θ is the projection of v along itself, scaled by cos θ.
// (k × v) sin θ is the contribution from the perpendicular component through rotation.
// k (k ⋅ v) (1 − cos θ) is the contribution from the parallel component.
float3 RotateVector(float3 vec, float3 axis, float angle)
{
    // Normalize the axis vector.
    axis = normalize(axis);

    // Create a rotation matrix.
    float cosinus = cos(angle);
    float sinus = sin(angle);
    float3 temp = (1.0f - cosinus) * axis;

    float3x3 rotationMatrix;
    rotationMatrix[0][0] = cosinus + temp.x * axis.x;
    rotationMatrix[0][1] = temp.x * axis.y + sinus * axis.z;
    rotationMatrix[0][2] = temp.x * axis.z - sinus * axis.y;

    rotationMatrix[1][0] = temp.y * axis.x - sinus * axis.z;
    rotationMatrix[1][1] = cosinus + temp.y * axis.y;
    rotationMatrix[1][2] = temp.y * axis.z + sinus * axis.x;

    rotationMatrix[2][0] = temp.z * axis.x + sinus * axis.y;
    rotationMatrix[2][1] = temp.z * axis.y - sinus * axis.x;
    rotationMatrix[2][2] = cosinus + temp.z * axis.z;

    // Multiply the vector by the rotation matrix.
    return mul(vec, rotationMatrix);
}

[maxvertexcount(18)]
void GS(triangle VertexOut gin[3], inout TriangleStream<GeoOut> triStream)
{
    float3 v1 = gin[0].PosW - gin[1].PosW;
    float3 v2 = gin[2].PosW - gin[1].PosW;
    
    float3 nv1 = normalize(v1);
    float3 nv2 = normalize(v2);
    float3 axis = cross(nv1, nv2);
    
    // Find the angle between the vectors.
    
    float cosAlpha = dot(nv1, nv2);
    // Limitation in [-1,1]
    cosAlpha = max(-1.0f, min(1.0f, cosAlpha));
    // In radians.
    float alpha = acos(cosAlpha);
    
    const float PI = 3.14159265f;
    
    float beta = (PI / 2) - alpha;
    float radians = 1.57 - beta; // 1.57 radians = 90 degree

    float miniAngle = radians / 16;
    
    // The logic for constructing triangles is as follows.
    // Two previous vertices and one new vertex generate a new triangle.
    
    float3 newVector = float3(0.0f, 0.0f, 0.0f);
    float l = length(v1);
    float3 pt = float3(0.0f, 0.0f, 0.0f);
    
    float4 v[18];
    
    v[0] = float4(gin[0].PosW, 1.0f);
    v[1] = float4(gin[1].PosW, 1.0f);
        
    newVector = RotateVector(v1, axis, miniAngle * 1);
    pt = gin[1].PosW + l * normalize(newVector);
    v[2] = float4(pt, 1.0f);
       
    v[3] = float4(gin[2].PosW, 1.0f);
    
    
    newVector = RotateVector(v1, axis, miniAngle * 2);
    pt = gin[1].PosW + l * normalize(newVector);
    v[4] = float4(pt, 1.0f);

        
    newVector = RotateVector(v1, axis, miniAngle * 15);
    pt = gin[1].PosW + l * normalize(newVector);
    v[5] = float4(pt, 1.0f);
       
    newVector = RotateVector(v1, axis, miniAngle * 3);
    pt = gin[1].PosW + l * normalize(newVector);
    v[6] = float4(pt, 1.0f);
    
    newVector = RotateVector(v1, axis, miniAngle * 14);
    pt = gin[1].PosW + l * normalize(newVector);
    v[7] = float4(pt, 1.0f);
    
    newVector = RotateVector(v1, axis, miniAngle * 4);
    pt = gin[1].PosW + l * normalize(newVector);
    v[8] = float4(pt, 1.0f);
    
    newVector = RotateVector(v1, axis, miniAngle * 13);
    pt = gin[1].PosW + l * normalize(newVector);
    v[9] = float4(pt, 1.0f);
    
    newVector = RotateVector(v1, axis, miniAngle * 5);
    pt = gin[1].PosW + l * normalize(newVector);
    v[10] = float4(pt, 1.0f);
    
    newVector = RotateVector(v1, axis, miniAngle * 12);
    pt = gin[1].PosW + l * normalize(newVector);
    v[11] = float4(pt, 1.0f);
    
    newVector = RotateVector(v1, axis, miniAngle * 6);
    pt = gin[1].PosW + l * normalize(newVector);
    v[12] = float4(pt, 1.0f);
    
    newVector = RotateVector(v1, axis, miniAngle * 11);
    pt = gin[1].PosW + l * normalize(newVector);
    v[13] = float4(pt, 1.0f);
    
    newVector = RotateVector(v1, axis, miniAngle * 7);
    pt = gin[1].PosW + l * normalize(newVector);
    v[14] = float4(pt, 1.0f);
    
    newVector = RotateVector(v1, axis, miniAngle * 10);
    pt = gin[1].PosW + l * normalize(newVector);
    v[15] = float4(pt, 1.0f);
    
    newVector = RotateVector(v1, axis, miniAngle * 8);
    pt = gin[1].PosW + l * normalize(newVector);
    v[16] = float4(pt, 1.0f);
    
    newVector = RotateVector(v1, axis, miniAngle * 9);
    pt = gin[1].PosW + l * normalize(newVector);
    v[17] = float4(pt, 1.0f);   
    
    
    GeoOut gout;
	[unroll]
    for (int i = 0; i < 18; ++i)
    {
        gout.PosH = mul(v[i], gViewProj);
        gout.PosW = v[i].xyz;
        gout.ColorW = gin[0].ColorW;
        triStream.Append(gout);
    }
}

float4 PS(GeoOut pin) : SV_Target
{
    //float3 pxlPos = pin.PosW;
    
    //float3 screenRes = float3(200.0f, 200.0f, 0.0f);
    //float2 pixelCoord = pxlPos.xy;
    
    //int gridSize = 32;

    //// Determine which grid cell the pixel is in.
    //float3 gridCell = floor(pxlPos * screenRes / gridSize);

    //// Chess pattern.
    //bool isEven = (gridCell.x + gridCell.y + gridCell.z) % 2 == 0;

    //float4 color = isEven ? float4(0.5f, 0.0f, 0.0f, 0.5f) : float4(0.0f, 0.0f, 0.5f, 0.5f);  
    float4 color = pin.ColorW;
    
    clip(color.a - 0.1f);
    
    return pin.ColorW;
}