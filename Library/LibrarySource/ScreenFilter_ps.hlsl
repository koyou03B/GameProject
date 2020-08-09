#include "ScreenFilter.hlsli"
//#include "FullScreen.hlsli"

Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sampler_state : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord) * pin.color;

    color.rgb += bright;

    color.rgb = ((color.rgb - 0.5f) * contrast) + 0.5f;

    float average = (color.r + color.g + color.b) / 3;
    color.rgb = (color.rgb - average) * saturate + average;

    color.rgb *= screenColor.rgb;

    return color;
}