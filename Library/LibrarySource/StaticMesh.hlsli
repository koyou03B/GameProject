struct VS_OUT
{
    float4 sv_position : SV_POSITION;
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

cbuffer OBJECT_CONSTANTS : register(b0)
{
    float4 material_color;
};
