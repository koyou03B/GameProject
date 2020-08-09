struct VS_OUT
{
	float4 sv_position : SV_POSITION;
    float4 position    : POSITION;
    float4 normal      : NORMAL;
	float4 color       : COLOR;
	float2 texcoord    : TEXCOORD;
};

#define MAX_BONES 128
cbuffer CONSTANT_BUFFER : register(b0)
{
    column_major float4x4 world;
    column_major float4x4 bone_transforms[MAX_BONES];
    float4 material_color;
};
