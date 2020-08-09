#include "Rasterizer.hlsli"

VS_OUT main(float4 position : POSITION, float2 texcoord : TEXCOORD, float4 color : COLOR)
{
	VS_OUT vout;
	vout.sv_position = position;
    vout.texcoord = texcoord;
	vout.color = color;
	return vout;
};