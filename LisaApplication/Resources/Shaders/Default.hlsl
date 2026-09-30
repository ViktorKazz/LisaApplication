
// Defaults for number of lights.
#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 2
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
    float4 ShadowPosH : POSITION0;
    float4 SsaoPosH : POSITION1;
    float3 PosW : POSITION2;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 TexC : TEXCOORD;
    float4x4 World : POSITION3;
    int TriangleNumber : INDEX;
    uint HoldDataSize : INDEX1;
    uint Mode : INDEX2;
    
    // nointerpolation is used so the index is not interpolated 
	// across the triangle.
    nointerpolation uint MatIndex : MATINDEX;
};

VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
{
    VertexOut vout = (VertexOut) 0.0f;
	
    // Fetch the instance data.
    InstanceData instData = gInstanceData[instanceID];
    float4x4 world = instData.World;
    float4x4 texTransform = instData.TexTransform;
    uint matIndex = instData.MaterialIndex;
    
    vout.TexC = vin.TexC;
    
    vout.MatIndex = matIndex;
	
	// Fetch the material data.
    MaterialData matData = gMaterialData[matIndex];
	
    // Transform to world space.
    float4 posW = mul(float4(vin.PosL, 1.0f), world);
    vout.PosW = posW.xyz;
    
    // Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
    vout.NormalW = mul(vin.NormalL, (float3x3) world);
	
    vout.TangentW = mul(vin.TangentU, (float3x3) world);

    // Transform to homogeneous clip space.
    vout.PosH = mul(posW, gViewProj);

    // Generate projective tex-coords to project SSAO map onto scene.
    vout.SsaoPosH = mul(posW, gViewProjTex);
	
	// Output vertex attributes for interpolation across triangle.
    float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), texTransform);
    vout.TexC = mul(texC, matData.MatTransform).xy;

    // Generate projective tex-coords to project shadow map onto scene.
    vout.ShadowPosH = mul(posW, gShadowTransform);
    
    vout.TriangleNumber = instData.TriangleNumber;
    vout.Mode = instData.Mode;
    vout.HoldDataSize = instData.HoldDataSize;
    vout.World = instData.World;
    
    return vout;
}

float TriangleArea(float3 p0, float3 p1, float3 p2)
{
    float3 u = p1 - p0;
    float3 v = p2 - p0;
    
    float vw0 = ((u.y * v.z) - (u.z * v.y));
    float vw1 = ((u.x * v.z) - (u.z * v.x));
    float vw2 = ((u.x * v.y) - (u.y * v.x));
    
    float m = vw0 * vw0 + vw1 * vw1 + vw2 * vw2;
    
    float s = sqrt(m) * 0.5;
    
    return s;
}

bool PointInTriangle(float3 pt, float3 v0, float3 v1, float3 v2)
{

    float ta = TriangleArea(v0, v1, v2);
    

    float d0 = TriangleArea(pt, v0, v1);
    float d1 = TriangleArea(pt, v1, v2);
    float d2 = TriangleArea(pt, v2, v0);
    
    float deviation = 1.0e-6f;
    float o = (ta + deviation) - (d0 + d1 + d2);

    if (o >= 0)
        return true;
    else
        return false;
}

bool DrawLine(float3 pt, float3 v0, float3 v1)
{
    // Calculating the line vector and the vector from the vertex to the pixel.
    float3 lineVector = v1 - v0;
    float3 pixelVector = pt - v0;
    
    // Calculate the distance from a pixel to a line
    float distance = length(cross(lineVector, pixelVector)) / length(lineVector);
           
    // Calculating the t parameter: The t parameter determines the position of the pixel projection on the line. 
    // If t is between 0 and 1, the projection lies within the segment.
    // isWithinSegment check: The isWithinSegment Boolean variable checks whether t is within the valid range.
            
    // Find the parameter t for projecting a pixel onto a line.
    float t = dot(pixelVector, lineVector) / dot(lineVector, lineVector);

    // Check if the projection lies inside the segment [0, 1].
    bool isWithinSegment = (t >= 0.0f) && (t <= 1.0f);
            
    // Line width.
    float lineWidth = 0.001f;
            
    // The further the camera is, the larger the line is drawn.
    float l = length(lineVector - gEyePosW);
            
    if (l > 2.0f)
    {
        lineWidth *= (l - 1.0f);
                
        if (lineWidth >= 0.0075f)
        {
            lineWidth = 0.0075f;
        }
    }
            
    float4 edgeColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
            
    // Draw?
    if (distance < lineWidth && isWithinSegment)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool DrawPoint(float3 pt, float3 v0)
{
    float4 vertexColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
    float distance = length(pt - v0);
    float radius = 0.002;
            
    // The further the camera is, the larger the dot is drawn.
            
    float d = length(pt - gEyePosW);
            
    if (d > 2.0f)
    {
        radius *= (d - 1.0f);
    }
    
    // Draw?
    if (distance <= radius)
    {
        return true;
    }
    else
    {
        return false;
    }
}

float4 PS(VertexOut pin, in float4 screenPos : SV_Position, uint primitiveID : SV_PrimitiveID) : SV_Target
{
    // Fetch the material data.
    MaterialData matData = gMaterialData[pin.MatIndex];
    float4 diffuseAlbedo = matData.DiffuseAlbedo;
    float3 fresnelR0 = matData.FresnelR0;
    float roughness = matData.Roughness;
    uint diffuseMapIndex = matData.DiffuseMapIndex;
    uint normalMapIndex = matData.NormalMapIndex;
    
    int triangleNumber = pin.TriangleNumber;
    uint mode = pin.Mode;
    uint holdDataSize = pin.HoldDataSize;
    
    HoldData hold;
    
    // Dynamically look up the texture in the array.
    diffuseAlbedo *= gTextureMaps[diffuseMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);
    
    
#ifdef ALPHA_TEST
    // Discard pixel if texture alpha < 0.1.  We do this test as soon 
    // as possible in the shader so that we can potentially exit the
    // shader early, thereby skipping the rest of the shader code.
    clip(diffuseAlbedo.a - 0.1f);
#endif

	// Interpolating normal can unnormalize it, so renormalize it.
    pin.NormalW = normalize(pin.NormalW);
	
    float4 normalMapSample = gTextureMaps[normalMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);
    // Multiplying by 2 will enhance the bump mapping effect.
    float3 bumpedNormalW = NormalSampleToWorldSpace(normalMapSample.rgb, pin.NormalW, pin.TangentW * -2.0f);

	// Uncomment to turn off normal mapping.
    bumpedNormalW = pin.NormalW;

    // Vector from point being lit to eye. 
    float3 toEyeW = normalize(gEyePosW - pin.PosW);

    // Finish texture projection and sample SSAO map.
    pin.SsaoPosH /= pin.SsaoPosH.w;
    float ambientAccess = gSsaoMap.Sample(gsamLinearClamp, pin.SsaoPosH.xy, 0.0f).r;

    // Light terms.
    float4 ambient = ambientAccess * gAmbientLight * diffuseAlbedo;

    // Only the first light casts a shadow.
    float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
    shadowFactor[0] = CalcShadowFactor(pin.ShadowPosH);

    const float shininess = (1.0f - roughness) * normalMapSample.a;
    Material mat = { diffuseAlbedo, fresnelR0, shininess };
    float4 directLight = ComputeLighting(gLights, mat, pin.PosW, bumpedNormalW, toEyeW, shadowFactor);

    float4 litColor = ambient + directLight;

	// Add in specular reflections.
    float3 r = reflect(-toEyeW, bumpedNormalW);
    float4 reflectionColor = gCubeMap.Sample(gsamLinearWrap, r);
    float3 fresnelFactor = SchlickFresnel(fresnelR0, bumpedNormalW, r);
    litColor.rgb += shininess * fresnelFactor * reflectionColor.rgb;
    
    litColor.a = diffuseAlbedo.a;
    
    return litColor;
}
