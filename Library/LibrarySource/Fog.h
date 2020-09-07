#pragma once
#include <d3d11.h>
#include <memory>
#include "Vector.h"
#include "ConstantBuffer.h"
#include "Rasterizer.h"
#include "FrameBuffer.h"
#include "Function.h"

namespace Source
{
    namespace Fog
    {
        class Fog :FullScreen::FullScreen
        {
        private:
            std::unique_ptr<Shader::PixelShader> m_fogPixel;
            Microsoft::WRL::ComPtr<ID3D11SamplerState> m_defaultStateObjects;
            Microsoft::WRL::ComPtr<ID3D11SamplerState> m_stateObject;
            unsigned int m_usingSlot = 0;

            struct FogBuffer
            {
                VECTOR3F fogColor;
                float    fogStartDepth;
                VECTOR3F fogHighlightColor;
                float    fogGlobalDensity;
                VECTOR3F dummy;
                float    fogHeightFallOff;


            };
        public:
            std::unique_ptr<ConstantBuffer::ConstantBuffer<FogBuffer>> _fogBuffer;

            Fog(ID3D11Device* device);
            ~Fog() = default;


            void Blit(ID3D11DeviceContext* immediateContext, ID3D11ShaderResourceView* colourMap, ID3D11ShaderResourceView* depthMap);

            void ActivateSampler(ID3D11DeviceContext* immediateContext, unsigned int slot)
            {
                m_usingSlot = slot;
                immediateContext->PSGetSamplers(m_usingSlot, 1, m_defaultStateObjects.ReleaseAndGetAddressOf());
                immediateContext->PSSetSamplers(m_usingSlot, 1, m_stateObject.GetAddressOf());
            }

            void DeactivateSampler(ID3D11DeviceContext* immediateContext)
            {
                immediateContext->PSSetSamplers(m_usingSlot, 1, m_defaultStateObjects.GetAddressOf());
            }


        };
    }
}
