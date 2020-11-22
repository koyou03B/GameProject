#include"Vignette.hlsli"

VS_OUT main(in uint vertex_id : SV_VERTEXID)
{
    VS_OUT vout;
    vout.texcoord = float2((vertex_id << 1) & 2, vertex_id & 2);
    vout.sv_position = float4(vout.texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    vout.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    return vout;
};