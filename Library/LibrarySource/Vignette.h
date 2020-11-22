#pragma once
#include <d3d11.h>
#include <iostream>
#include <fstream>
#include <shlwapi.h>
#include <string>
#include <memory>
#include "Vector.h"
#include "ConstantBuffer.h"
#include "Rasterizer.h"
#include "FrameBuffer.h"
#include "Function.h"

#undef max
#undef min
#include<cereal/cereal.hpp>
#include<cereal/archives/binary.hpp>
#include<cereal/types/memory.hpp>
#include<cereal/types/vector.hpp>

namespace Source
{
    namespace Vignette
    {
        class Vignette : FullScreen::FullScreen
        {
        public:
            struct VignetteBuffer
            {
                VECTOR4F aroundColor;
                float radius;
                float smoothness;
                float darkness;
                float dummy;

                template<class T>
                void serialize(T& archive)
                {
                    archive
                    (
                        aroundColor,
                        radius,
                        smoothness,
                        darkness,
                        dummy
                    );
                }

            };
            std::unique_ptr<ConstantBuffer::ConstantBuffer<VignetteBuffer>> _vignetteBuffer;

            Vignette(ID3D11Device* device);
            ~Vignette() = default;

            void ReadBinary()
            {
                if (PathFileExistsA((std::string("../Asset/Binary/Shader/Vignette/Parameter") + ".bin").c_str()))
                {
                    std::ifstream ifs;
                    ifs.open((std::string("../Asset/Binary/Shader/Vignette/Parameter") + ".bin").c_str(), std::ios::binary);
                    cereal::BinaryInputArchive i_archive(ifs);
                    i_archive(_vignetteBuffer->data);
                }
            }

            void SaveBinary()
            {
                std::ofstream ofs;
                ofs.open((std::string("../Asset/Binary/Shader/Vignette/Parameter") + ".bin").c_str(), std::ios::binary);
                cereal::BinaryOutputArchive o_archive(ofs);
                o_archive(_vignetteBuffer->data);
            }

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

        private:
            std::unique_ptr<Shader::PixelShader> m_vignettePixel;
            Microsoft::WRL::ComPtr<ID3D11SamplerState> m_defaultStateObjects;
            Microsoft::WRL::ComPtr<ID3D11SamplerState> m_stateObject;
            unsigned int m_usingSlot = 0;


        };
    }
}
