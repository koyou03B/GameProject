#include "GeometricPrimitive.hlsli"
#include "SceneConstants.hlsli"

VS_OUT main(
	float4 position : POSITION,
	float2 texcoord : TEXCOORD,
	float4 normal : NORMAL,
    float4 color : COLOR,
	row_major float4x4 wvp : WVP,
	row_major float4x4 world : WORLD,
	uint instID : SV_instanceID
)
{
    VS_OUT vout;
    vout.position = mul(position, wvp);

    normal.w = 0;
    float4 N = normalize(mul(normal, world));

    float4 L = normalize(-directional_light.direction);
    vout.color = color * max(0, dot(L, N));
    vout.color.a = color.a;
    vout.texcoord = texcoord;

    return vout;
}
