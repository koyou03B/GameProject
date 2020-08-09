#pragma once
#include<DirectXMath.h>
#include "Vector.h"

namespace Source
{
    namespace Constants
    {
        struct SceneConstants
        {
            struct DirectionalLightShaderConstants
            {
                VECTOR4F direction = VECTOR4F(-0.5f, -1.f, 1.f, 0.f);
                VECTOR4F color = VECTOR4F(1.f, 1.f, 1.f, 1.f);
            };
            DirectionalLightShaderConstants directionalLight;

            struct PointLightShaderConstants
            {
                VECTOR4F position = VECTOR4F(0.f, 0.f, 0.f, 15.f);
                VECTOR4F color = VECTOR4F(228.f / 255.f, 200.f / 255.f, 148.f / 255.f, 1.f / 4.f);
            };
            PointLightShaderConstants pointLight;

            struct SpotLightShaderConstants
            {
                VECTOR4F position = VECTOR4F(0.f, 0.f, 0.f, 15.f);
                VECTOR4F color = VECTOR4F(228.f / 255.f, 200.f / 255.f, 148.f / 255.f, 1.f / 4.f);
                VECTOR4F direction = VECTOR4F(0.f, -1.f, 0.f, 0.f);
            };
            SpotLightShaderConstants spotLight;

            VECTOR4F ambientColor = VECTOR4F(0.7f, 0.7f, 0.7f, 1.f);
        };

        struct Dissolve
        {
            float	 dissolveGlowAmoument;
            float	 dissolveGlowRange;
            float	 dissolveGlowFalloff;
            float    dissolveDummy;
            VECTOR4F dissolveGlowColor;
            VECTOR4F dissolveEmission;
        };
    }
}