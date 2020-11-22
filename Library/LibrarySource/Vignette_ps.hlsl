#include "Vignette.hlsli"
//#include "FullScreen.hlsli"

Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sampler_state : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord);
    float2 d = abs(pin.texcoord - float2(0.5f, 0.5f)) * float2(2.0f /** ASPECT_RATIO*/, 2.0f);
    float r2 = dot(d, d);
    float vignettingFactor = 0.0f;

    vignettingFactor += pow(abs(min(1.0f, r2 / radius)), smoothness) * darkness;
    color.rgb = lerp(color.rgb, aroundColor.rgb, saturate(vignettingFactor));
    return color;

}