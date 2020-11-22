#include "SceneConstants.hlsli"

struct VS_OUT
{
    float4 sv_position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

cbuffer VIGNETTE : register(b0)
{
    float4 aroundColor;
    float radius;
    float smoothness;
    float darkness;
    float dammy;
};


