// GBuffer.hlsl — geometry pass: outputs albedo, world-space normals, ORM, emissive, motion vectors.
// Instance and material data are fetched from global bindless scene buffers.

#include "Common/Bindless.hlsli"
#include "Common/Scene.hlsli"

#pragma vertex VSMain
#pragma pixel FSMain

struct PushConstants
{
    column_major float4x4 ViewProjection;  // 64 bytes
    int InstanceID;                         //  4 bytes
    int InstanceBufferIndex;                //  4 bytes
    int MaterialBufferIndex;                //  4 bytes
    int _pad;                               //  4 bytes
};                                          // Total: 80 bytes
PUSH_CONSTANTS(PushConstants);

struct VSOut
{
    float4 Position       : SV_Position;
    float3 WorldNormal    : NORMAL;
    float3 WorldTangent   : TANGENT;
    float3 WorldBitangent : BINORMAL;
    float2 UV             : TEXCOORD0;
    nointerpolation int MaterialIndex : TEXCOORD1;
};

struct GBufferOut
{
    float4 Albedo        : SV_Target0; // RGBA8_UNORM
    float4 Normals       : SV_Target1; // RGBA16_FLOAT  world-space normal in [0,1]
    float4 ORM           : SV_Target2; // RGBA8_UNORM   R=occlusion, G=roughness, B=metallic
    float4 Emissive      : SV_Target3; // RGBA16_FLOAT
    float4 MotionVectors : SV_Target4; // RGBA16_FLOAT  (zeroed for now)
};

VSOut VSMain(uint vertexID : SV_VertexID)
{
    StructuredBuffer<GPUInstance> instances = ResourceDescriptorHeap[PUSH.InstanceBufferIndex];
    GPUInstance inst = instances[PUSH.InstanceID];

    StructuredBuffer<ModelVertex> vertices = ResourceDescriptorHeap[inst.VertexBufferIndex];
    ModelVertex v = vertices[vertexID + inst.BaseVertex];

    float4x4 MVP = mul(PUSH.ViewProjection, inst.WorldMatrix);

    // Transform normals to world space using the upper-left 3x3 of the world matrix.
    // Correct for uniform or no scaling. For non-uniform scale, the normal matrix
    // (inverse transpose) should be stored in GPUInstance.
    float3x3 normalMat = (float3x3)inst.WorldMatrix;

    VSOut o;
    o.Position       = mul(MVP, float4(v.Position, 1.0f));
    o.WorldNormal    = normalize(mul(normalMat, v.Normal));
    o.WorldTangent   = normalize(mul(normalMat, v.Tangent.xyz));
    o.WorldBitangent = cross(o.WorldNormal, o.WorldTangent) * v.Tangent.w;
    o.UV             = v.UV;
    o.MaterialIndex  = inst.MaterialIndex;
    return o;
}

GBufferOut FSMain(VSOut i)
{
    StructuredBuffer<GPUMaterial> materials = ResourceDescriptorHeap[PUSH.MaterialBufferIndex];
    GPUMaterial mat = materials[i.MaterialIndex];

    SamplerState samp = SamplerDescriptorHeap[mat.SamplerIndex];

    // --- Albedo ---
    float4 albedo = float4(1.0f, 1.0f, 1.0f, 1.0f);
    if (mat.AlbedoIndex >= 0)
    {
        Texture2D<float4> tex = ResourceDescriptorHeap[mat.AlbedoIndex];
        albedo = tex.Sample(samp, i.UV);
        if (albedo.a < 0.25f)
            discard;
    }

    // --- Normal map ---
    float3 N = normalize(i.WorldNormal);
    if (mat.NormalIndex >= 0)
    {
        Texture2D<float4> normalTex = ResourceDescriptorHeap[mat.NormalIndex];
        float3 ts = normalTex.Sample(samp, i.UV).rgb * 2.0f - 1.0f;
        float3 T  = normalize(i.WorldTangent);
        float3 B  = normalize(i.WorldBitangent);
        N = normalize(T * ts.x + B * ts.y + N * ts.z);
    }

    // --- ORM (Occlusion, Roughness, Metallic) ---
    float3 orm = float3(1.0f, 0.5f, 0.0f); // defaults: full occlusion, mid roughness, non-metal
    if (mat.ORMIndex >= 0)
    {
        Texture2D<float4> ormTex = ResourceDescriptorHeap[mat.ORMIndex];
        orm = ormTex.Sample(samp, i.UV).rgb;
    }

    // --- Emissive ---
    float3 emissive = float3(0.0f, 0.0f, 0.0f);
    if (mat.EmissiveIndex >= 0)
    {
        Texture2D<float4> emTex = ResourceDescriptorHeap[mat.EmissiveIndex];
        emissive = emTex.Sample(samp, i.UV).rgb;
    }

    GBufferOut o;
    o.Albedo        = float4(albedo.rgb, 1.0f);
    o.Normals       = float4(N * 0.5f + 0.5f, 0.0f); // encode to [0,1]
    o.ORM           = float4(orm, 0.0f);
    o.Emissive      = float4(emissive, 0.0f);
    o.MotionVectors = float4(0.0f, 0.0f, 0.0f, 0.0f);
    return o;
}
