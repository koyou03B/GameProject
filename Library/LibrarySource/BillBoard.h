#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <memory>
#include "vector.h"
#include "ConstantBuffer.h"
#include "Function.h"

namespace Source
{ 
	namespace BillBoard
	{
		class BillBoard
		{
		public:
			BillBoard(ID3D11Device* device, const char* filename, bool forceSRGB);

			~BillBoard() = default;

			void Begin(ID3D11DeviceContext* immediateContext);
			void Render(ID3D11DeviceContext* immediateContext, const FLOAT4X4& projection,
				const FLOAT4X4& view, const VECTOR3F& position, float scale, const VECTOR3F& angle, const VECTOR4F& color);
			void End(ID3D11DeviceContext* immediateContext);
		private:
			struct Vertex
			{
				VECTOR3F position;
				VECTOR2F texcoord;

				static const D3D11_INPUT_ELEMENT_DESC* AcquireInputElementDescs(size_t& numElements)
				{
					static const D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
					{
						{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
						{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

					};
					//---------------
					// óvëfêîÇÃéÊìæ
					//---------------
					numElements = _countof(inputElementDesc);
					return inputElementDesc;
				}
			};

			struct ShaderConstants
			{
				FLOAT4X4 worldViewProjection;
				VECTOR4F color;
			};
			std::unique_ptr<Source::Shader::VertexShader<Vertex>> m_billBoardVS;
			std::unique_ptr<Source::Shader::PixelShader> m_billBoardPS;
			Microsoft::WRL::ComPtr<ID3D11Buffer>				m_vertexBuffer;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_shaderResourceView;
			D3D11_TEXTURE2D_DESC    m_texture2dDesc;

			std::unique_ptr<Source::ConstantBuffer::ConstantBuffer<ShaderConstants>> m_constantBuffer;

		};
	}
}