#include "SpriteBatch.hlsli"
#include "Function.hlsli"

Texture2D diffuse_map : register(t0);
Texture2D noiseTex : register(t1);
SamplerState diffuse_map_sampler_state : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float dissolve = noiseTex.Sample(diffuse_map_sampler_state, pin.texcoord).r;
    float4 baseColorMap = diffuse_map.SampleLevel(diffuse_map_sampler_state, pin.texcoord, 0);

    float4 color = baseColorMap * pin.color;

#ifdef DISSOLVE
    color = Dissolve(dissolve, baseColorMap, pin.color);
#endif

    return color;

}

