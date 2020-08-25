#include "Dissolve.hlsli"

float4 Dissolve(float dissolve, float4 baseColorMap,float4 color)
{
    dissolve = dissolve * 0.999;

    float isVisible = dissolve - dissolveGlowAmoument;
    clip(isVisible);

    float isGlowing = smoothstep(dissolveGlowRange + dissolveGlowFalloff, dissolveGlowRange, isVisible);
    float4 glow = isGlowing * dissolveGlowColor.rgba;
	
    return float4((color * baseColorMap) * (dissolveEmission + glow));
}

//--------------------------------------------
//	�g�U���ˊ֐�
//--------------------------------------------
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// K:���˗�(0�`1.0)
float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
    float D = dot(N, -L) + 0.75;
    D = max(0,D);
    return K * C * D;
}

//--------------------------------------------
//	���ʔ��ˊ֐�(�u�����E�t�H��)
//--------------------------------------------
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// E:���_�����x�N�g��(���K���ς�)
// K:���˗�(0�`1.0)
// Power:�n�C���C�g�̋���(�P���x)

float3 BlinnPhongSpecular(float3 N, float3 L, float3 C, float3 E,
	float3 K, float Power)
{
	//�n�[�t�x�N�g��
    float3 H = normalize(-L + E);

    float3 S = dot(H, N);
    S = max(0, S);
    S = pow(S, Power);
    S = S * K * C;
    return S;
}

//--------------------------------------------
// �������C�g
//--------------------------------------------
// N           : �@�� 
// SkyColor    : ��(��)�F 
// GroundColor : �n��(��)�F 
float3 HemiSphereLight(float3 N, float3 SkyColor, float3 GroundColor)
{
    float skyblend = (N.y + 1.0) * 0.5;
    float groundblend = 1.0 - skyblend;
    return SkyColor * skyblend + GroundColor * groundblend;
}
