struct VS_OUT
{
    float4 sv_position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

cbuffer SCREEN_FILTER : register(b0)
{
    float bright;
    float contrast;
    float saturate;
    float dummy;
    float4 screenColor;
};