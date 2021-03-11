#include "BillBoard.h"
#include"Framework.h"
#include"Function.h"

namespace Source
{
	namespace BillBoard
	{
		BillBoard::BillBoard(ID3D11Device* device, const char* filename, bool forceSRGB)
		{
			HRESULT hr = S_OK;

			Vertex vertices[] =
			{
				{ VECTOR3F(-0.5f, +0.5f, 0.0f), VECTOR2F(1, 0) },
				{ VECTOR3F(+0.5f, +0.5f, 0.0f), VECTOR2F(0, 0) },
				{ VECTOR3F(-0.5f, -0.5f, 0.0f), VECTOR2F(1, 1) },
				{ VECTOR3F(+0.5f, -0.5f, 0.0f), VECTOR2F(0, 1) },
			};

			D3D11_BUFFER_DESC vertexBufferDesc = {};
			vertexBufferDesc.ByteWidth = sizeof(vertices);
			vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			vertexBufferDesc.MiscFlags = 0;
			vertexBufferDesc.StructureByteStride = 0;
			D3D11_SUBRESOURCE_DATA vertexSubresourceData = {};
			vertexSubresourceData.pSysMem = vertices;
			vertexSubresourceData.SysMemPitch = 0;
			vertexSubresourceData.SysMemSlicePitch = 0;
			hr = device->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, m_vertexBuffer.GetAddressOf());
			if (FAILED(hr))
				assert(!"Could not Create Buffer (BillBoard)");

			m_billBoardVS = std::make_unique<Source::Shader::VertexShader<Vertex>>(device, "../Library/LibraryShader/BillBorad_vs.cso");
			m_billBoardPS = std::make_unique<Source::Shader::PixelShader>(device, "../Library/LibraryShader/BillBorad_ps.cso");

			Source::Texture::LoadTextureFromFile(device, filename, m_shaderResourceView.GetAddressOf(), forceSRGB);
			Source::Texture::Texture2dDescription(m_shaderResourceView.Get(), m_texture2dDesc);

			m_constantBuffer = std::make_unique<Source::ConstantBuffer::ConstantBuffer<ShaderConstants>>(device);
		}

		void BillBoard::Begin(ID3D11DeviceContext* immediateContext)
		{
			UINT stride = sizeof(Vertex);
			UINT offset = 0;

			immediateContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

			immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

			m_billBoardVS->Activate(immediateContext);
			m_billBoardPS->Activate(immediateContext);

			immediateContext->PSSetShaderResources(0, 1, m_shaderResourceView.GetAddressOf());

			immediateContext->OMSetBlendState(Framework::GetBlendState(Framework::BS_ADD), nullptr, 0xFFFFFFFF);

			immediateContext->OMSetDepthStencilState(Framework::GetDephtStencilState(Framework::DS_TRUE), 1);
			immediateContext->RSSetState(Framework::GetRasterizerState(Framework::RS_CULL_BACK));
			immediateContext->PSSetSamplers(0, 1, Framework::GetSamplerState(Framework::SS_WRAP));
		}

		void BillBoard::TileCutRender(ID3D11DeviceContext* immediateContext,VECTOR2F texPos, VECTOR2F texSize)
		{
			HRESULT hr = S_OK;

			D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
			D3D11_MAPPED_SUBRESOURCE mapped_buffer;
			hr = immediateContext->Map(m_vertexBuffer.Get(), 0, map, 0, &mapped_buffer);
			if (FAILED(hr))
				assert(!"Map Miss (BillBoard)");

			Vertex* vertices = static_cast<Vertex*>(mapped_buffer.pData);
			vertices[0].position = VECTOR3F(-0.5f, +0.5f, 0.0f);
			vertices[1].position = VECTOR3F(+0.5f, +0.5f, 0.0f);
			vertices[2].position = VECTOR3F(-0.5f, -0.5f, 0.0f);
			vertices[3].position = VECTOR3F(+0.5f, -0.5f, 0.0f);

			vertices[0].texcoord.x = (texPos.x + texSize.x) / m_texture2dDesc.Width;
			vertices[0].texcoord.y = (texPos.y) / m_texture2dDesc.Height;
			vertices[1].texcoord.x = (texPos.x) / m_texture2dDesc.Width;
			vertices[1].texcoord.y = (texPos.y) / m_texture2dDesc.Height;
			vertices[2].texcoord.x = (texPos.x + texSize.x) / m_texture2dDesc.Width;
			vertices[2].texcoord.y = (texPos.y + texSize.y) / m_texture2dDesc.Height;
			vertices[3].texcoord.x = (texPos.x) / m_texture2dDesc.Width;
			vertices[3].texcoord.y = (texPos.y + texSize.y) / m_texture2dDesc.Height;

			immediateContext->Unmap(m_vertexBuffer.Get(), 0);
		}

		void BillBoard::Render(ID3D11DeviceContext* immediateContext, const FLOAT4X4& projection, const FLOAT4X4& view, 
			const VECTOR3F& position, float scale, const VECTOR3F& angle, const VECTOR4F& color)
		{
			//World Matrix
			DirectX::XMMATRIX W;
			{
				DirectX::XMMATRIX S, R, T;
				float aspectRatio = static_cast<float>(m_texture2dDesc.Width / m_texture2dDesc.Height);
				S = DirectX::XMMatrixScaling(scale, scale, scale);
				R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
				T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
				W = S * R * T;
			}
			//View Matrix
			DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
			//Projection Matrix
			DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);

			//Extract view's transelate component
			FLOAT4X4 inverse_view = view;
			//inverse_view._21 = 0.0f;
			//inverse_view._22 = 1.0f;
			//inverse_view._23 = 0.0f;
			inverse_view._41 = 0.0f; inverse_view._42 = 0.0f; inverse_view._43 = 0.0f; inverse_view._44 = 1.0f;
			DirectX::XMMATRIX inverse_view_matrix;
			inverse_view_matrix = DirectX::XMLoadFloat4x4(&inverse_view);
			inverse_view_matrix = DirectX::XMMatrixInverse(nullptr, inverse_view_matrix);

			DirectX::XMStoreFloat4x4(&m_constantBuffer->data.worldViewProjection, inverse_view_matrix * W * V * P);
			m_constantBuffer->data.color = color;

			m_constantBuffer->Activate(immediateContext, SLOT0, true, true);

			immediateContext->Draw(4, 0);
		}

		void BillBoard::End(ID3D11DeviceContext* immediateContext)
		{
			m_constantBuffer->Deactivate(immediateContext);

			m_billBoardPS->Deactivate(immediateContext);
			m_billBoardVS->Deactivate(immediateContext);

		}

	}
}
