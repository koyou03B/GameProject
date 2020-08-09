#pragma once
#include <d3d11.h>
#include <DirectXTex.h>
#include <wrl.h>
#include "Misc.h"
#include"Vector.h"

namespace Source
{
	namespace Rasterizer
	{
		class Rasterizer
		{
		public:
			struct Vertex
			{
				VECTOR3F position;
				VECTOR2F texcoord;
				VECTOR4F color;
			};

			Rasterizer(ID3D11Device* device, D3D11_FILTER samplerFilter = D3D11_FILTER_ANISOTROPIC,
				D3D11_TEXTURE_ADDRESS_MODE samplerTextureAddressMode = D3D11_TEXTURE_ADDRESS_BORDER,
				DirectX::XMFLOAT4 samplerBoarderColor = DirectX::XMFLOAT4(0, 0, 0, 0));
			virtual ~Rasterizer() = default;

			void Blit
			(
				ID3D11DeviceContext* immediateContext,
				ID3D11ShaderResourceView* shaderResourceView,
				float dx, float dy, float dw, float dh,
				float sx, float sy, float sw, float sh,
				float angle/*degree*/,
				float r, float g, float b, float a,
				bool useEmbeddedVertexShader = true,
				bool useEmbeddedPixelShader = true,
				bool useEmbeddedRasterizerState = true,
				bool useEmbeddedDepthStencilState = true,
				bool useEmbeddedSamplerState = true
				) const;
			void Blit(
				ID3D11DeviceContext* immediateContext,
				ID3D11ShaderResourceView* shaderResourceView,
				float dx, float dy, float dw, float dh,
				float angle = 0/*degree*/,
				float r = 1, float g = 1, float b = 1, float a = 1,
				bool useEmbeddedVertexShader = true,
				bool useEmbeddedPixelShader = true,
				bool useEmbeddedRasterizerState = true,
				bool useEmbeddedDepthStencilState = true,
				bool useEmbeddedSamplerState = true
				) const
			{
				HRESULT hr = S_OK;

				Microsoft::WRL::ComPtr<ID3D11Resource> resource;
				shaderResourceView->GetResource(resource.GetAddressOf());

				Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
				hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				D3D11_TEXTURE2D_DESC texture2dDesc;
				texture2d->GetDesc(&texture2dDesc);

				Blit(immediateContext, shaderResourceView, dx, dy, dw, dh, 0.0f, 0.0f, static_cast<float>(texture2dDesc.Width), static_cast<float>(texture2dDesc.Height), angle, r, g, b, a,
					useEmbeddedVertexShader, useEmbeddedPixelShader, useEmbeddedRasterizerState, useEmbeddedDepthStencilState, useEmbeddedSamplerState);

			};
		private:
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;

			Microsoft::WRL::ComPtr<ID3D11VertexShader> m_embeddedVertexShader;
			Microsoft::WRL::ComPtr<ID3D11InputLayout> m_embeddedInputLayout;
			Microsoft::WRL::ComPtr<ID3D11PixelShader> m_embeddedPixelShaders;

			Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_embeddedRasterizerState;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_embeddedDepthStencilState;
			Microsoft::WRL::ComPtr<ID3D11SamplerState> m_embeddedSamplerState;

		};
	}

	namespace FullScreen
	{
		class FullScreen
		{
		public:
			FullScreen(ID3D11Device* device);
			virtual ~FullScreen() = default;

			void Blit(ID3D11DeviceContext* immediateContext, bool useEmbeddedRasterizerState = true,
				bool useEmbeddedDepthStencilState = true, bool useEmbeddedPixelShader = false);
		private:
			Microsoft::WRL::ComPtr<ID3D11VertexShader>      m_embeddedVertexShader;
			Microsoft::WRL::ComPtr<ID3D11PixelShader>       m_embeddedPixelShaders;
			Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_embeddedRasterizerState;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_embeddedDepthStencilState;
			Microsoft::WRL::ComPtr<ID3D11InputLayout>		m_inputLayout;
		};
	}
}


