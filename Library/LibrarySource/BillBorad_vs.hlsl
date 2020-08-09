#include "BillBorad.hlsli"

VS_OUT main(float4 pos : POSITION, float2 texcoord : TEXCOORD)
{
	VS_OUT vout;
	vout.position = mul(pos, world_view_projection);
	vout.texcoord = texcoord;
	return vout;
}