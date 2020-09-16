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
    namespace Fog
    {
        class Fog :FullScreen::FullScreen
        {
        public:
            struct FogBuffer
            {
                VECTOR3F fogColor;
                float    fogStartDepth;
                VECTOR3F fogHighlightColor;
                float    fogGlobalDensity;
                VECTOR3F dummy;
                float    fogHeightFallOff;

                template<class T>
                void serialize(T& archive)
                {
                    archive
                    (
                        fogColor,
                        fogStartDepth,
                        fogHighlightColor,
                        fogGlobalDensity,
                        dummy,
                        fogHeightFallOff
                    );
                }

            };
            std::unique_ptr<ConstantBuffer::ConstantBuffer<FogBuffer>> _fogBuffer;

            Fog(ID3D11Device* device);
            ~Fog() = default;

            void ReadBinary()
            {
                if (PathFileExistsA((std::string("../Asset/Binary/Shader/Fog/Parameter") + ".bin").c_str()))
                {
                    std::ifstream ifs;
                    ifs.open((std::string("../Asset/Binary/Shader/Fog/Parameter") + ".bin").c_str(), std::ios::binary);
                    cereal::BinaryInputArchive i_archive(ifs);
                    i_archive(_fogBuffer->data);
                }
            }

            void SaveBinary()
            {
                std::ofstream ofs;
                ofs.open((std::string("../Asset/Binary/Shader/Fog/Parameter") + ".bin").c_str(), std::ios::binary);
                cereal::BinaryOutputArchive o_archive(ofs);
                o_archive(_fogBuffer->data);
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
            std::unique_ptr<Shader::PixelShader> m_fogPixel;
            Microsoft::WRL::ComPtr<ID3D11SamplerState> m_defaultStateObjects;
            Microsoft::WRL::ComPtr<ID3D11SamplerState> m_stateObject;
            unsigned int m_usingSlot = 0;


        };
    }
}
