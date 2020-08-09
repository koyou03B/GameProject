#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <memory>
#include "Vector.h"
#include "Constants.h"
#include "ConstantBuffer.h"
#include "Function.h"

namespace Source
{
	namespace Sprite
	{
		class SpriteBatch
		{
		public:
			SpriteBatch(ID3D11Device* device, const char* filename, UINT maxInstance);
			SpriteBatch(ID3D11Device* device, const char* filename, const char* noisTexName, UINT maxInstance);
			virtual ~SpriteBatch() = default;
			void Begin(ID3D11DeviceContext * immediateContext);

			void RenderLeft(const VECTOR2F & position, const VECTOR2F & scale,
				const VECTOR2F & texPos, const VECTOR2F & texSize,
				float angle, const VECTOR4F & color, bool reverse);

			void RenderCenter(const VECTOR2F & position, const VECTOR2F & scale,
				const VECTOR2F & texPos, const VECTOR2F & texSize,
				float angle, const VECTOR4F & color, bool reverse);

			void End(ID3D11DeviceContext * immediateContext);


		private:
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	 m_shaderResourceView;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	 m_dissolveShaderResourceView;
			Microsoft::WRL::ComPtr<ID3D11Buffer>				 m_vertexBuffer;
			Microsoft::WRL::ComPtr<ID3D11Buffer>				 m_instanceBuffer;

			D3D11_TEXTURE2D_DESC m_texture2dDesc;
			D3D11_TEXTURE2D_DESC m_dissolveTexture2dDesc;
			D3D11_VIEWPORT		 m_viewPort;

			struct Vertex
			{
				VECTOR3F position;
				VECTOR2F texcoord;

				static const D3D11_INPUT_ELEMENT_DESC* AcquireInputElementDescs(size_t& numElements)
				{
					static const D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
					{
						{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
						{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
						{ "NDC_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
						{ "NDC_TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
						{ "NDC_TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
						{ "NDC_TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
						{ "TEXCOORD_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
						{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
					};
					//---------------
					// óvëfêîÇÃéÊìæ
					//---------------
					numElements = _countof(inputElementDesc);
					return inputElementDesc;
				}
			};

			struct Instance
			{
				FLOAT4X4 ndcTransform;
				VECTOR4F texcoordTransform;
				VECTOR4F color;
			};

			std::unique_ptr<Source::Shader::VertexShader<Vertex>> m_spriteVS;
			std::unique_ptr<Source::Shader::PixelShader> m_spritePS;

			const size_t MAX_INSTANCES = 256;
			UINT m_countInstance = 0;
			Instance* m_instances = nullptr;
		};
	}
}
