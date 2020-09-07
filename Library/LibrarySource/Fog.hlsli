#include "SceneConstants.hlsli"

struct VS_OUT
{
    float4 sv_position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

cbuffer FOG : register(b0)
{
  #if 0
    float3 fogColor : packoffset(c0.x);
    float fogStartDepth : packoffset(c0.w);
    float3 fogHighlightColor : packoffset(c1.x);
    float fogGlobalDensity : packoffset(c1.w);
    float3 dummy : packoffset(c2.x);
    float fogHeightFallOff : packoffset(c2.w);
#else
    float3 fogColor;
    float fogStartDepth;
    float3 fogHighlightColor;
    float fogGlobalDensity;
    float3 dummy;
    float fogHeightFallOff;
  #endif
};


float3 ApplyFog(float3 defaultColor, float3 pixelPosition /*world space*/, float3 eyePosition /*world space*/)
{
    float3 eyeToPixel = pixelPosition - eyePosition;
    float  pixelDist = length(eyeToPixel);
    float3 eyeToPixelNorm = eyeToPixel / pixelDist;
    
    float fogDist = max(pixelDist - fogStartDepth, 0.0f);
    
    float fogHeightDensityAtViewer = exp(-fogHeightFallOff * eyePosition.y);
    float fogDistInt = fogDist * fogHeightDensityAtViewer;
    
    float eyeToPixelY = eyeToPixel.y * (fogDist / pixelDist);
    float t = fogHeightFallOff * eyeToPixelY;
    const float thresholdT = 0.01;
    float fogHeightInt = abs(t) > thresholdT ?
		(1.0 - exp(-t)) / t : 1.0;
    
    float fogFinalFactor = exp(-fogGlobalDensity * fogDistInt * fogHeightInt);
    
    float3 lightNomal = normalize(-directional_light.direction.xyz);
    float lightFactor = saturate(dot(lightNomal, eyeToPixelNorm));
    lightFactor = pow(lightFactor, 8.0);
    float3 fogFinalColor = lerp(fogColor, fogHighlightColor, lightFactor);
    
    return lerp(fogFinalColor, defaultColor, fogFinalFactor);

}
