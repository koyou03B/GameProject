#include "StaticMesh.hlsli"
#include "SceneConstants.hlsli"

VS_OUT main(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD,
 column_major float4x4 world : WORLD,uint instID : SV_instanceID)
{
    VS_OUT vout;
    vout.sv_position = mul(position, mul(world, camera.view_projection));
    vout.position = mul(position, world);
    normal.w = 0;
    vout.normal = normalize(mul(normal, world));
    
    vout.color = material_color;

    vout.texcoord = texcoord;

    return vout;
}