#pragma once
#include <d3d11.h>
#include <memory>

#include "Vector.h"
#include "ConstantBuffer.h"
#include "Rasterizer.h"
#include "FrameBuffer.h"
#include "Function.h"
#include "Framework.h"

namespace Source
{
    namespace ScreenFilter
    {
        class ScreenFilter : Source::FullScreen::FullScreen
        {
        public:
            struct ScreenBuffer
            {
                float bright = 0.f;   //ñæìx 
                float contrast = 1.f; //îZíW 
                float saturate = 1.f; //ç ìx
                float dummy = 0.f;
                VECTOR4F screenColor = VECTOR4F(1.f, 1.f, 1.f, 1.f);
            };

            std::unique_ptr<Source::ConstantBuffer::ConstantBuffer<ScreenBuffer>> _screenBuffer;
            ScreenFilter(ID3D11Device* device);
            virtual ~ScreenFilter() = default;


            void Blit(ID3D11DeviceContext* immediateContext, ID3D11ShaderResourceView* colourMap);

            void ActivateSampler(ID3D11DeviceContext* immediateContext, unsigned int slot)
            {
               m_usingSlot = slot;
                immediateContext->PSGetSamplers(m_usingSlot, 1, m_defaultStateObjects.ReleaseAndGetAddressOf());
                immediateContext->PSSetSamplers(m_usingSlot, 1, Framework::GetSamplerState(Framework::SS_BORDER));
            }

            void DeactivateSampler(ID3D11DeviceContext* immediateContext)
            {
                immediateContext->PSGetSamplers(m_usingSlot, 1, m_defaultStateObjects.ReleaseAndGetAddressOf());
            }

        private:
            std::unique_ptr<Source::Shader::PixelShader> m_screenFilterPixel;
            Microsoft::WRL::ComPtr<ID3D11SamplerState> m_defaultStateObjects;
            Microsoft::WRL::ComPtr<ID3D11SamplerState> m_stateObject;

            unsigned int m_usingSlot = 0;



        };
    }
}
