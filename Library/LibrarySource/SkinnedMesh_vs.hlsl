#include "SkinnedMesh.hlsli"
#include "SceneConstants.hlsli"

VS_OUT main(
	float4 position : POSITION,
	float4 normal : NORMAL,
	float2 texcoord : TEXCOORD,
	float4 bone_weights : WEIGHTS,
	uint4 bone_indices : BONES
)
{
    float3 blended_position = { 0, 0, 0 };
    float3 blended_normal = { 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        blended_position += (bone_weights[i] * mul(position, bone_transforms[bone_indices[i]])).xyz;
        blended_normal += (bone_weights[i] * mul(float4(normal.xyz, 0), bone_transforms[bone_indices[i]])).xyz;
    }

    position = float4(blended_position, 1.0f);
    normal   = float4(blended_normal, 0.0f);

	VS_OUT vout;
    vout.sv_position = mul(position, mul(world, camera.view_projection));
    vout.position = mul(position, world);

	normal.w = 0;
    vout.normal = normalize(mul(normal, world));
    vout.color = material_color;

	vout.texcoord = texcoord;

	return vout;
}