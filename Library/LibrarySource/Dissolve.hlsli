//struct VS_OUT
//{
//    float4 sv_position : SV_POSITION;
//    float2 texcoord : TEXCOORD;
//    float4 color : COLOR;
//};

cbuffer Dissolve : register(b5)
{
    float dissolveGlowAmoument;
    float dissolveGlowRange;
    float dissolveGlowFalloff;
    float dissolveDummy;
    float4 dissolveGlowColor;
    float4 dissolveEmission;
};