#include "Rasterizer.h"
#include "Function.h"
#include "Framework.h"

namespace Source
{
	namespace Rasterizer
	{
		Rasterizer::Rasterizer(ID3D11Device* device, D3D11_FILTER samplerFilter, D3D11_TEXTURE_ADDRESS_MODE samplerTextureAddressMode, DirectX::XMFLOAT4 samplerBoarderColor)
		{
			HRESULT hr = S_OK;

			Vertex vertices[] =
			{
				{ VECTOR3F(0, 0, 0), VECTOR2F(0, 0), VECTOR4F(1, 1, 1, 1) },
				{ VECTOR3F(0, 0, 0), VECTOR2F(0, 0), VECTOR4F(1, 1, 1, 1) },
				{ VECTOR3F(0, 0, 0), VECTOR2F(0, 0), VECTOR4F(1, 1, 1, 1) },
				{ VECTOR3F(0, 0, 0), VECTOR2F(0, 0), VECTOR4F(1, 1, 1, 1) },
			};

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = sizeof(vertices);
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			D3D11_SUBRESOURCE_DATA subresource_data = {};
			subresource_data.pSysMem = vertices;
			subresource_data.SysMemPitch = 0;
			subresource_data.SysMemSlicePitch = 0;
			hr = device->CreateBuffer(&bufferDesc, &subresource_data, m_vertexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

			D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT   , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT      , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			{
				Source::Shader::CreateVsFromCso(device, "../Library/LibraryShader/Rasterizer_vs.cso", m_embeddedVertexShader.GetAddressOf(), m_embeddedInputLayout.ReleaseAndGetAddressOf(),
					inputElementDesc, ARRAYSIZE(inputElementDesc));
				Source::Shader::CreatePsFromCso(device, "../Library/LibraryShader/Rasterizer_ps.cso", m_embeddedPixelShaders.GetAddressOf());
			}

			{
				D3D11_SAMPLER_DESC samplerDesc;
				samplerDesc.Filter = samplerFilter; //D3D11_FILTER_MIN_MAG_MIP_POINT D3D11_FILTER_ANISOTROPIC
				samplerDesc.AddressU = samplerTextureAddressMode;
				samplerDesc.AddressV = samplerTextureAddressMode;
				samplerDesc.AddressW = samplerTextureAddressMode;
				samplerDesc.MipLODBias = 0;
				samplerDesc.MaxAnisotropy = 16;
				samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
				memcpy(samplerDesc.BorderColor, &samplerBoarderColor, sizeof(DirectX::XMFLOAT4));
				samplerDesc.MinLOD = 0;
				samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
				hr = device->CreateSamplerState(&samplerDesc, m_embeddedSamplerState.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

#if 0
				D3D11_RASTERIZER_DESC rasterizerDesc = {};
				rasterizerDesc.FillMode = D3D11_FILL_SOLID;
				rasterizerDesc.CullMode = D3D11_CULL_NONE;
				rasterizerDesc.FrontCounterClockwise = FALSE;
				rasterizerDesc.DepthBias = 0;
				rasterizerDesc.DepthBiasClamp = 0;
				rasterizerDesc.SlopeScaledDepthBias = 0;
				rasterizerDesc.DepthClipEnable = FALSE;
				rasterizerDesc.ScissorEnable = FALSE;
				rasterizerDesc.MultisampleEnable = FALSE;
				rasterizerDesc.AntialiasedLineEnable = FALSE;
				hr = device->CreateRasterizerState(&rasterizerDesc, _embeddedRasterizerState.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
				depthStencilDesc.DepthEnable = FALSE;
				depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
				depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
				depthStencilDesc.StencilEnable = FALSE;
				depthStencilDesc.StencilReadMask = 0xFF;
				depthStencilDesc.StencilWriteMask = 0xFF;
				depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
				depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
				depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				hr = device->CreateDepthStencilState(&depthStencilDesc, _embeddedDepthStencilState.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
#endif
			}
		}

		void Rasterizer::Blit(ID3D11DeviceContext* immediateContext,
			ID3D11ShaderResourceView* shaderResourceView,
			float dx, float dy, float dw, float dh,
			float sx, float sy, float sw, float sh,
			float angle,
			float r, float g, float b, float a,
			bool useEmbeddedVertexShader,
			bool useEmbeddedPixelShader,
			bool useEmbeddedRasterizerState,
			bool useEmbeddedDepthStencilState,
			bool useEmbeddedSamplerState
		) const
		{
			HRESULT hr = S_OK;
			D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
			shaderResourceView->GetDesc(&shader_resource_view_desc);
			bool multisampled = shader_resource_view_desc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURE2DMS;

			Microsoft::WRL::ComPtr<ID3D11Resource> resource;
			shaderResourceView->GetResource(resource.GetAddressOf());

			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
			hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

			D3D11_TEXTURE2D_DESC texture2dDesc;
			texture2d->GetDesc(&texture2dDesc);

			D3D11_VIEWPORT viewport;
			UINT numViewports = 1;
			immediateContext->RSGetViewports(&numViewports, &viewport);
			float screen_width = viewport.Width;
			float screen_height = viewport.Height;

			// left-top
			float x0 = dx;
			float y0 = dy;
			// right-top
			float x1 = dx + dw;
			float y1 = dy;
			// left-bottom
			float x2 = dx;
			float y2 = dy + dh;
			// right-bottom
			float x3 = dx + dw;
			float y3 = dy + dh;

			// Translate sprite's centre to origin (rotate centre)
			float mx = dx + dw * 0.5f;
			float my = dy + dh * 0.5f;
			x0 -= mx;
			y0 -= my;
			x1 -= mx;
			y1 -= my;
			x2 -= mx;
			y2 -= my;
			x3 -= mx;
			y3 -= my;

			// Rotate each sprite's vertices by angle
			float rx, ry;
			float cos = cosf(angle * 0.01745f);
			float sin = sinf(angle * 0.01745f);
			rx = x0;
			ry = y0;
			x0 = cos * rx + -sin * ry;
			y0 = sin * rx + cos * ry;
			rx = x1;
			ry = y1;
			x1 = cos * rx + -sin * ry;
			y1 = sin * rx + cos * ry;
			rx = x2;
			ry = y2;
			x2 = cos * rx + -sin * ry;
			y2 = sin * rx + cos * ry;
			rx = x3;
			ry = y3;
			x3 = cos * rx + -sin * ry;
			y3 = sin * rx + cos * ry;

			// Translate sprite's centre to original position
			x0 += mx;
			y0 += my;
			x1 += mx;
			y1 += my;
			x2 += mx;
			y2 += my;
			x3 += mx;
			y3 += my;

			// Convert to NDC space
			x0 = 2.0f * x0 / screen_width - 1.0f;
			y0 = 1.0f - 2.0f * y0 / screen_height;
			x1 = 2.0f * x1 / screen_width - 1.0f;
			y1 = 1.0f - 2.0f * y1 / screen_height;
			x2 = 2.0f * x2 / screen_width - 1.0f;
			y2 = 1.0f - 2.0f * y2 / screen_height;
			x3 = 2.0f * x3 / screen_width - 1.0f;
			y3 = 1.0f - 2.0f * y3 / screen_height;

			D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
			D3D11_MAPPED_SUBRESOURCE mapped_buffer;
			hr = immediateContext->Map(m_vertexBuffer.Get(), 0, map, 0, &mapped_buffer);
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

			Vertex* vertices = static_cast<Vertex*>(mapped_buffer.pData);
			vertices[0].position.x = x0;
			vertices[0].position.y = y0;
			vertices[1].position.x = x1;
			vertices[1].position.y = y1;
			vertices[2].position.x = x2;
			vertices[2].position.y = y2;
			vertices[3].position.x = x3;
			vertices[3].position.y = y3;
			vertices[0].position.z = vertices[1].position.z = vertices[2].position.z = vertices[3].position.z = 0.0f;

			VECTOR4F colour(r, g, b, a);
			vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = colour;

			vertices[0].texcoord.x = (sx) / texture2dDesc.Width;
			vertices[0].texcoord.y = (sy) / texture2dDesc.Height;
			vertices[1].texcoord.x = (sx + sw) / texture2dDesc.Width;
			vertices[1].texcoord.y = (sy) / texture2dDesc.Height;
			vertices[2].texcoord.x = (sx) / texture2dDesc.Width;
			vertices[2].texcoord.y = (sy + sh) / texture2dDesc.Height;
			vertices[3].texcoord.x = (sx + sw) / texture2dDesc.Width;
			vertices[3].texcoord.y = (sy + sh) / texture2dDesc.Height;

			immediateContext->Unmap(m_vertexBuffer.Get(), 0);

			UINT stride = sizeof(Vertex);
			UINT offset = 0;
			immediateContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
			immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			immediateContext->IASetInputLayout(m_embeddedInputLayout.Get());

			if (useEmbeddedVertexShader)
			{
				immediateContext->VSSetShader(m_embeddedVertexShader.Get(), 0, 0);
			}
			if (useEmbeddedPixelShader)
			{
				immediateContext->PSSetShader(m_embeddedPixelShaders.Get(), 0, 0);
			}
			immediateContext->PSSetShaderResources(0, 1, &shaderResourceView);

			Microsoft::WRL::ComPtr<ID3D11RasterizerState> defaultRasterizerState;
			if (useEmbeddedRasterizerState)
			{
				immediateContext->RSGetState(defaultRasterizerState.ReleaseAndGetAddressOf());
				immediateContext->RSSetState(Framework::GetInstance().GetRasterizerState(Framework::RS_DEPTH_FALSE));
			}
			Microsoft::WRL::ComPtr<ID3D11DepthStencilState> defaultDepthStencilState;
			if (useEmbeddedDepthStencilState)
			{
				immediateContext->OMGetDepthStencilState(defaultDepthStencilState.ReleaseAndGetAddressOf(), 0);
				immediateContext->OMSetDepthStencilState(Framework::GetInstance().GetDephtStencilState(Framework::DS_FALSE), 1);
			}
			Microsoft::WRL::ComPtr<ID3D11SamplerState> defaultSamplerState;
			if (useEmbeddedSamplerState)
			{
				immediateContext->PSGetSamplers(0, 1, defaultSamplerState.ReleaseAndGetAddressOf());
				immediateContext->PSSetSamplers(0, 1, m_embeddedSamplerState.GetAddressOf());
			}
			immediateContext->Draw(4, 0);

			immediateContext->IASetInputLayout(0);
			immediateContext->VSSetShader(0, 0, 0);
			immediateContext->PSSetShader(0, 0, 0);

			ID3D11ShaderResourceView* nullShaderResourceView = 0;
			immediateContext->PSSetShaderResources(0, 1, &nullShaderResourceView);

			if (defaultRasterizerState)
			{
				immediateContext->RSSetState(defaultRasterizerState.Get());
			}
			if (defaultDepthStencilState)
			{
				immediateContext->OMSetDepthStencilState(defaultDepthStencilState.Get(), 1);
			}
			if (defaultSamplerState)
			{
				immediateContext->PSSetSamplers(0, 1, defaultSamplerState.GetAddressOf());
			}

		}
	}

	namespace FullScreen
	{
		FullScreen::FullScreen(ID3D11Device* device)
		{

			{
				Source::Shader::CreateVsFromCso(device, "../Library/LibraryShader/FullScreen_vs.cso", m_embeddedVertexShader.GetAddressOf(), nullptr, nullptr, 0);
				Source::Shader::CreatePsFromCso(device, "../Library/LibraryShader/FullScreen_ps.cso", m_embeddedPixelShaders.GetAddressOf());
			}

#if 0
			{
				HRESULT hr = S_OK;
				D3D11_RASTERIZER_DESC rasterizer_desc = {};
				rasterizer_desc.FillMode = D3D11_FILL_SOLID;
				rasterizer_desc.CullMode = D3D11_CULL_BACK;
				rasterizer_desc.FrontCounterClockwise = FALSE;
				rasterizer_desc.DepthBias = 0;
				rasterizer_desc.DepthBiasClamp = 0;
				rasterizer_desc.SlopeScaledDepthBias = 0;
				rasterizer_desc.DepthClipEnable = FALSE;
				rasterizer_desc.ScissorEnable = FALSE;
				rasterizer_desc.MultisampleEnable = FALSE;
				rasterizer_desc.AntialiasedLineEnable = FALSE;
				hr = device->CreateRasterizerState(&rasterizer_desc, _embeddedRasterizerState.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
				depthStencilDesc.DepthEnable = FALSE;
				depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
				depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
				depthStencilDesc.StencilEnable = FALSE;
				depthStencilDesc.StencilReadMask = 0xFF;
				depthStencilDesc.StencilWriteMask = 0xFF;
				depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
				depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
				depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				hr = device->CreateDepthStencilState(&depthStencilDesc, _embeddedDepthStencilState.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}
#endif
		}

		void FullScreen::Blit(ID3D11DeviceContext* immediateContext, bool useEmbeddedRasterizerState, bool useEmbeddedDepthStencilState, bool useEmbeddedPixelShader)
		{
			Microsoft::WRL::ComPtr<ID3D11RasterizerState> defaultRasterizerState;
			if (useEmbeddedRasterizerState)
			{
				immediateContext->RSGetState(defaultRasterizerState.ReleaseAndGetAddressOf());
				immediateContext->RSSetState(Framework::GetInstance().GetRasterizerState(Framework::RS_DEPTH_FALSE));
			}

			Microsoft::WRL::ComPtr<ID3D11DepthStencilState> defaultDepthStencilState;
			if (useEmbeddedDepthStencilState)
			{
				immediateContext->OMGetDepthStencilState(defaultDepthStencilState.ReleaseAndGetAddressOf(), 0);
				immediateContext->OMSetDepthStencilState(Framework::GetInstance().GetDephtStencilState(Framework::DS_FALSE), 1);
			}
			if (useEmbeddedPixelShader)
			{
				immediateContext->PSSetShader(m_embeddedPixelShaders.Get(), 0, 0);
			}

			immediateContext->IASetVertexBuffers(0, 0, 0, 0, 0);
			immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			immediateContext->IASetInputLayout(0);

			immediateContext->VSSetShader(m_embeddedVertexShader.Get(), 0, 0);

			immediateContext->Draw(4, 0);

			//immediateContext->IASetInputLayout(0);
			immediateContext->VSSetShader(0, 0, 0);
			//immediateContext->PSSetShader(0, 0, 0);

			if (defaultRasterizerState)
			{
				immediateContext->RSSetState(defaultRasterizerState.Get());
			}
			if (defaultDepthStencilState)
			{
				immediateContext->OMSetDepthStencilState(defaultDepthStencilState.Get(), 1);
			}
		}


	}
}


