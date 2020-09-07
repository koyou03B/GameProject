#include "Fog.hlsli"

Texture2D diffuse_map : register(t0);
Texture2D depth_map : register(t1);

SamplerState diffuse_map_sampler_state : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 colour_map_color = diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord);
    float depth_map_color = depth_map.Sample(diffuse_map_sampler_state, pin.texcoord).x;

    float3 fragment_color = colour_map_color.rgb;
    float alpha = colour_map_color.a;
    
    //shadow mapping
    float4 position_in_ndc;
	//texture space to ndc
    position_in_ndc.x = pin.texcoord.x * 2 - 1;
    position_in_ndc.y = pin.texcoord.y * -2 + 1;
    position_in_ndc.z = depth_map_color;
    position_in_ndc.w = 1;

    //ndc to world space
    float4 position_in_world_space = mul(position_in_ndc, camera.inverse_view_projection);
    position_in_world_space /= position_in_world_space.w;
    
    fragment_color = ApplyFog(fragment_color,position_in_world_space.xyz,camera.position.xyz);
    
    return float4(fragment_color, alpha);
}