#include "SkinnedMesh.hlsli"
#include "SceneConstants.hlsli"
#include "Function.hlsli"

Texture2D diffuse_map : register(t0);
Texture2D ambient_map : register(t1);
Texture2D specular_map : register(t2);
Texture2D normal_map : register(t3);


SamplerState diffuse_map_sampler_state : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 diffuseColor = diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord);
    float4 specularColor = specular_map.Sample(diffuse_map_sampler_state, pin.texcoord);
    
    float4 normal_map_color = normal_map.Sample(diffuse_map_sampler_state, pin.texcoord);
    normal_map_color = (normal_map_color * 2.0) - 1.0;
    normal_map_color.w = 0;
    
	// transform to world space from tangent space
	//                 |Tx Ty Tz|
	// normal = |x y z||Bx By Bz|
	//                 |Nx Ny Nz|
   
    float3 N = normalize(pin.normal.xyz);
    float3 B = normalize(float3(0, 1, -0.001f));
    float3 T = normalize(cross(B, N));
    
    //N = normalize((normal_map_color.x * T) + (normal_map_color.y * B) + (normal_map_color.z * N));
    float3 L = normalize(directional_light.direction.xyz);
    float3 E = normalize(camera.position.xyz - pin.position.xyz);
    float3 C = directional_light.color.xyz;
    float3 A = ambient_color.xyz;
    float3 Kd = diffuseColor.rgb;
    float3 D = Diffuse(N, L, C, Kd);
    float3 Ks = specularColor.xyz;
    float3 S = BlinnPhongSpecular(N, L, C, E, Ks, 20);

    // point light
    float3 PD = float3(0, 0, 0);
    float3 PS = float3(0, 0, 0);
	{
        float3 point_light_direction = pin.position.xyz - point_light.position.xyz;

        float distance = length(point_light_direction);
        float range = point_light.position.w;
        float influence = saturate(1.f - distance / range);

        point_light_direction = normalize(point_light_direction);
        float3 point_light_color = point_light.color.xyz;

        PD += Diffuse(pin.normal.xyz, point_light_direction, point_light_color, Kd) * influence * influence;
        PS += BlinnPhongSpecular(N, point_light_direction, point_light_color, E, Ks, 20) * influence * influence;
    }
    
    return pin.color * float4(A + D + S + PS +PD, 1.0f);

}
