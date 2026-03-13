// Mesh.hlsl — bindless static mesh rendering

#include "Common/Bindless.hlsli"

#pragma vertex VSMain
#pragma pixel FSMain

struct ModelVertex
{
    float3 Position;
    float3 Normal;
    float2 UV;
    float4 Tangent;
};

struct PushConstants
{
    column_major float4x4 MVP;
    int VertexBufferIndex;
    int BaseVertex;   // added to SV_VertexID (Metal: DrawIndexed baseVertex doesn't reach SV_VertexID for bindless)
    int AlbedoIndex;  // -1 = no texture
    int SamplerIndex;
};
PUSH_CONSTANTS(PushConstants);

struct VSOut
{
    float4 Position : SV_Position;
    float3 Normal   : NORMAL;
    float2 UV       : TEXCOORD0;
};

VSOut VSMain(uint vertexID : SV_VertexID)
{
    StructuredBuffer<ModelVertex> vertices = ResourceDescriptorHeap[PUSH.VertexBufferIndex];
    ModelVertex v = vertices[vertexID + PUSH.BaseVertex];

    VSOut o;
    o.Position = mul(PUSH.MVP, float4(v.Position, 1.0f));
    o.Normal   = v.Normal;
    o.UV       = v.UV;
    return o;
}

float4 FSMain(VSOut i) : SV_Target
{
    float3 lightDir = normalize(float3(0.0f, 1.0f, -0.3f));
    float ndotl = saturate(dot(normalize(i.Normal), lightDir));
    float lighting = 0.15f + 0.85f * ndotl;

    float4 albedo = float4(1.0f, 1.0f, 1.0f, 1.0f);
    if (PUSH.AlbedoIndex >= 0)
    {
        SamplerState samp = SamplerDescriptorHeap[PUSH.SamplerIndex];
        Texture2D<float4> tex = ResourceDescriptorHeap[PUSH.AlbedoIndex];
        albedo = tex.Sample(samp, i.UV);
        if (albedo.a < 0.25f)
            discard;
    }

    return float4(albedo.rgb * lighting, 1.0f);
}
