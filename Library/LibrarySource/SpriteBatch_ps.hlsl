#include "SpriteBatch.hlsli"
#include "Function.hlsli"
Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sampler_state : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    return  diffuse_map.SampleLevel(diffuse_map_sampler_state, pin.texcoord, 0) * pin.color;
}