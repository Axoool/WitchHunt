#ifndef SCENE_HLSLI
#define SCENE_HLSLI

/// Per-draw instance (80 bytes) — matches GPUInstance in GPUInstance.hpp
struct GPUInstance
{
    column_major float4x4 WorldMatrix;   // Combined actor × mesh local transform
    int   MaterialIndex;                 // Index into material buffer
    int   VertexBufferIndex;             // Bindless handle to the vertex buffer
    int   BaseVertex;                    // Base vertex offset
    int   _pad;
};

/// Per-material (32 bytes) — matches GPUMaterial in Material.hpp
struct GPUMaterial
{
    int AlbedoIndex;
    int NormalIndex;
    int ORMIndex;
    int EmissiveIndex;
    int SamplerIndex;
    int _pad0;
    int _pad1;
    int _pad2;
};

/// Shared mesh vertex format — matches ModelVertex in C++
struct ModelVertex
{
    float3 Position;
    float3 Normal;
    float2 UV;
    float4 Tangent; // xyz = tangent, w = handedness
};

#endif // SCENE_HLSLI
