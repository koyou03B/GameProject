#include "SpriteBatch.hlsli"
#include "Function.hlsli"
Texture2D diffuse_map : register(t0);
Texture2D noiseTex : register(t1);
SamplerState diffuse_map_sampler_state : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float dissolve = noiseTex.Sample(diffuse_map_sampler_state, pin.texcoord).r;
    float4 baseColorMap = diffuse_map.SampleLevel(diffuse_map_sampler_state, pin.texcoord, 0);

    return Dissolve(dissolve, baseColorMap, pin.color);
}