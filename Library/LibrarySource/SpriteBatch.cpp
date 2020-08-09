#include"SpriteBatch.h"
#include"Function.h"
#include"Framework.h"

namespace Source
{
	namespace Sprite
	{
		SpriteBatch::SpriteBatch(ID3D11Device* device, const char* filename, UINT maxInstance)
		{
			HRESULT hr = S_OK;

			Vertex vertices[] =
			{
				{ VECTOR3F(0, 0, 0), VECTOR2F(0, 0) },
				{ VECTOR3F(1, 0, 0), VECTOR2F(1, 0) },
				{ VECTOR3F(0, 1, 0), VECTOR2F(0, 1) },
				{ VECTOR3F(1, 1, 0), VECTOR2F(1, 1) },
			};

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = sizeof(vertices);
			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			D3D11_SUBRESOURCE_DATA subresource_data = {};
			subresource_data.pSysMem = vertices;
			subresource_data.SysMemPitch = 0; //Not use for vertex buffers.
			subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.
			hr = device->CreateBuffer(&bufferDesc, &subresource_data, m_vertexBuffer.GetAddressOf());
			if (FAILED(hr))
				assert(!"Could not Create a VertexBuffer");

			Instance* instances = new Instance[MAX_INSTANCES];
			{
				D3D11_BUFFER_DESC bufferDesc = {};
				D3D11_SUBRESOURCE_DATA subresourceData = {};

				bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Instance) * MAX_INSTANCES);
				bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
				bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				bufferDesc.MiscFlags = 0;
				bufferDesc.StructureByteStride = 0;
				subresourceData.pSysMem = instances;
				subresourceData.SysMemPitch = 0; //Not use for vertex buffers.mm 
				subresourceData.SysMemSlicePitch = 0; //Not use for vertex buffers.
				hr = device->CreateBuffer(&bufferDesc, &subresourceData, m_instanceBuffer.GetAddressOf());
				if (FAILED(hr))
					assert(!"Could not Create a InstanceBuffer");
			}
			delete[] instances;

			bool sRGB = false;
			m_spriteVS = std::make_unique<Source::Shader::VertexShader<Vertex>>(device, "../Library/LibraryShader/SpriteBatch_vs.cso");
			m_spritePS = std::make_unique<Source::Shader::PixelShader>(device, "../Library/LibraryShader/SpriteBatch_ps.cso");

			hr = Source::Texture::LoadTextureFromFile(device, filename, m_shaderResourceView.GetAddressOf(), sRGB);
			hr = Source::Texture::Texture2dDescription(m_shaderResourceView.Get(), m_texture2dDesc);
		}

		SpriteBatch::SpriteBatch(ID3D11Device* device, const char* filename, const char* noisTexName, UINT maxInstance)
		{
			HRESULT hr = S_OK;

			Vertex vertices[] =
			{
				{ VECTOR3F(0, 0, 0), VECTOR2F(0, 0) },
				{ VECTOR3F(1, 0, 0), VECTOR2F(1, 0) },
				{ VECTOR3F(0, 1, 0), VECTOR2F(0, 1) },
				{ VECTOR3F(1, 1, 0), VECTOR2F(1, 1) },
			};

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = sizeof(vertices);
			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			D3D11_SUBRESOURCE_DATA subresource_data = {};
			subresource_data.pSysMem = vertices;
			subresource_data.SysMemPitch = 0; //Not use for vertex buffers.
			subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.
			hr = device->CreateBuffer(&bufferDesc, &subresource_data, m_vertexBuffer.GetAddressOf());
			if (FAILED(hr))
				assert(!"Could not Create a VertexBuffer");

			Instance* instances = new Instance[MAX_INSTANCES];
			{
				D3D11_BUFFER_DESC bufferDesc = {};
				D3D11_SUBRESOURCE_DATA subresourceData = {};

				bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Instance) * MAX_INSTANCES);
				bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
				bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				bufferDesc.MiscFlags = 0;
				bufferDesc.StructureByteStride = 0;
				subresourceData.pSysMem = instances;
				subresourceData.SysMemPitch = 0; //Not use for vertex buffers.mm 
				subresourceData.SysMemSlicePitch = 0; //Not use for vertex buffers.
				hr = device->CreateBuffer(&bufferDesc, &subresourceData, m_instanceBuffer.GetAddressOf());
				if (FAILED(hr))
					assert(!"Could not Create a InstanceBuffer");
			}
			delete[] instances;

			bool sRGB = false;

			m_spriteVS = std::make_unique<Source::Shader::VertexShader<Vertex>>(device, "../Library/LibraryShader/SpriteBatch_vs.cso");
			m_spritePS = std::make_unique<Source::Shader::PixelShader>(device, "../Library/LibraryShader/SpriteDissolve_ps.cso");

			hr = Source::Texture::LoadTextureFromFile(device, filename, m_shaderResourceView.GetAddressOf(), sRGB);
			hr = Source::Texture::Texture2dDescription(m_shaderResourceView.Get(), m_texture2dDesc);

			hr = Source::Texture::LoadTextureFromFile(device, noisTexName, m_dissolveShaderResourceView.GetAddressOf(), sRGB);
			hr = Source::Texture::Texture2dDescription(m_dissolveShaderResourceView.Get(), m_dissolveTexture2dDesc);

		}

		void SpriteBatch::Begin(ID3D11DeviceContext* immediateContext)
		{
			m_spriteVS->Activate(immediateContext);
			m_spritePS->Activate(immediateContext);

			UINT strides[2] = { sizeof(Vertex), sizeof(Instance) };
			UINT offsets[2] = { 0, 0 };
			ID3D11Buffer* vbs[2] = { m_vertexBuffer.Get(), m_instanceBuffer.Get() };
			immediateContext->IASetVertexBuffers(0, 2, vbs, strides, offsets);
			immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			immediateContext->OMSetDepthStencilState(Framework::GetDephtStencilState(Framework::DS_FALSE), 1);
			immediateContext->RSSetState(Framework::GetRasterizerState(Framework::RS_CULL_BACK));

			immediateContext->PSSetShaderResources(SLOT0, 1, m_shaderResourceView.GetAddressOf());
			if (m_dissolveShaderResourceView.Get())
				immediateContext->PSSetShaderResources(SLOT1, 1, m_dissolveShaderResourceView.GetAddressOf());
			immediateContext->PSSetSamplers(0, 1, Framework::GetSamplerState(Framework::SS_MIRROR));

			UINT numViewports = 1;
			immediateContext->RSGetViewports(&numViewports, &m_viewPort);

			D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
			D3D11_MAPPED_SUBRESOURCE mappedBuffer;
			HRESULT hr = S_OK;

			hr = immediateContext->Map(m_instanceBuffer.Get(), 0, map, 0, &mappedBuffer);
			if (FAILED(hr))
				assert(!"Map miss");
			m_instances = static_cast<Instance*>(mappedBuffer.pData);

			m_countInstance = 0;
		}

		void SpriteBatch::RenderCenter(const VECTOR2F& position, const VECTOR2F& scale,
			const VECTOR2F& texPos, const VECTOR2F& texSize,
			float angle, const VECTOR4F& color, bool reverse)
		{
			_ASSERT_EXPR(m_countInstance < MAX_INSTANCES, L"Number of instances must be less than MAX_INSTANCES.");

			float cx = scale.x * 0.5f, cy = scale.y * 0.5f; /*Center of Rotation*/
			VECTOR2F leftTop;
			leftTop.x = position.x - cx;
			leftTop.y = position.y - cy;
#if 0
			DirectX::XMVECTOR scaling = DirectX::XMVectorSet(dw, dh, 1.0f, 0.0f);
			DirectX::XMVECTOR origin = DirectX::XMVectorSet(cx, cy, 0.0f, 0.0f);
			DirectX::XMVECTOR translation = DirectX::XMVectorSet(dx, dy, 0.0f, 0.0f);
			DirectX::XMMATRIX M = DirectX::XMMatrixAffineTransformation2D(scaling, origin, angle * 0.01745f, translation);
			DirectX::XMMATRIX N(
				2.0f / viewport.Width, 0.0f, 0.0f, 0.0f,
				0.0f, -2.0f / viewport.Height, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				-1.0f, 1.0f, 0.0f, 1.0f);
			XMStoreFloat4x4(&instances[countInstance].ndcTransform, DirectX::XMMatrixTranspose(M * N)); //column_major
#else
			FLOAT c = cosf(angle * 0.01745f);
			FLOAT s = sinf(angle * 0.01745f);
			FLOAT w = 2.0f / m_viewPort.Width;
			FLOAT h = -2.0f / m_viewPort.Height;
			m_instances[m_countInstance].ndcTransform._11 = w * scale.x * c;
			m_instances[m_countInstance].ndcTransform._21 = h * scale.x * s;
			m_instances[m_countInstance].ndcTransform._31 = 0.0f;
			m_instances[m_countInstance].ndcTransform._41 = 0.0f;
			m_instances[m_countInstance].ndcTransform._12 = w * scale.y * -s;
			m_instances[m_countInstance].ndcTransform._22 = h * scale.y * c;
			m_instances[m_countInstance].ndcTransform._32 = 0.0f;
			m_instances[m_countInstance].ndcTransform._42 = 0.0f;
			m_instances[m_countInstance].ndcTransform._13 = 0.0f;
			m_instances[m_countInstance].ndcTransform._23 = 0.0f;
			m_instances[m_countInstance].ndcTransform._33 = 1.0f;
			m_instances[m_countInstance].ndcTransform._43 = 0.0f;
			m_instances[m_countInstance].ndcTransform._14 = w * (-cx * c + -cy * -s + cx + leftTop.x) - 1.0f;
			m_instances[m_countInstance].ndcTransform._24 = h * (-cx * s + -cy * c + cy + leftTop.y) + 1.0f;
			m_instances[m_countInstance].ndcTransform._34 = 0.0f;
			m_instances[m_countInstance].ndcTransform._44 = 1.0f;
#endif
			float tw = static_cast<float>(m_texture2dDesc.Width);
			float th = static_cast<float>(m_texture2dDesc.Height);
			if (reverse)
				m_instances[m_countInstance].texcoordTransform = VECTOR4F(texPos.x / tw + 1.0f, texPos.y / th, texSize.x / tw, texSize.y / th);
			else
				m_instances[m_countInstance].texcoordTransform = VECTOR4F(texPos.x / tw, texPos.y / th, texSize.x / tw, texSize.y / th);
			m_instances[m_countInstance].color = color;

			m_countInstance++;
		}

		void SpriteBatch::RenderLeft(const VECTOR2F& position, const VECTOR2F& scale,
			const VECTOR2F& texPos, const VECTOR2F& texSize,
			float angle, const VECTOR4F& color, bool reverse)
		{
			_ASSERT_EXPR(m_countInstance < MAX_INSTANCES, L"Number of instances must be less than MAX_INSTANCES.");

			float cx = scale.x * 0.5f, cy = scale.y * 0.5f; /*Center of Rotation*/
#if 0
			DirectX::XMVECTOR scaling = DirectX::XMVectorSet(dw, dh, 1.0f, 0.0f);
			DirectX::XMVECTOR origin = DirectX::XMVectorSet(cx, cy, 0.0f, 0.0f);
			DirectX::XMVECTOR translation = DirectX::XMVectorSet(dx, dy, 0.0f, 0.0f);
			DirectX::XMMATRIX M = DirectX::XMMatrixAffineTransformation2D(scaling, origin, angle * 0.01745f, translation);
			DirectX::XMMATRIX N(
				2.0f / viewport.Width, 0.0f, 0.0f, 0.0f,
				0.0f, -2.0f / viewport.Height, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				-1.0f, 1.0f, 0.0f, 1.0f);
			XMStoreFloat4x4(&instances[countInstance].ndcTransform, DirectX::XMMatrixTranspose(M * N)); //column_major
#else
			FLOAT c = cosf(angle * 0.01745f);
			FLOAT s = sinf(angle * 0.01745f);
			FLOAT w = 2.0f / m_viewPort.Width;
			FLOAT h = -2.0f / m_viewPort.Height;
			m_instances[m_countInstance].ndcTransform._11 = w * scale.x * c;
			m_instances[m_countInstance].ndcTransform._21 = h * scale.x * s;
			m_instances[m_countInstance].ndcTransform._31 = 0.0f;
			m_instances[m_countInstance].ndcTransform._41 = 0.0f;
			m_instances[m_countInstance].ndcTransform._12 = w * scale.y * -s;
			m_instances[m_countInstance].ndcTransform._22 = h * scale.y * c;
			m_instances[m_countInstance].ndcTransform._32 = 0.0f;
			m_instances[m_countInstance].ndcTransform._42 = 0.0f;
			m_instances[m_countInstance].ndcTransform._13 = 0.0f;
			m_instances[m_countInstance].ndcTransform._23 = 0.0f;
			m_instances[m_countInstance].ndcTransform._33 = 1.0f;
			m_instances[m_countInstance].ndcTransform._43 = 0.0f;
			m_instances[m_countInstance].ndcTransform._14 = w * (-cx * c + -cy * -s + cx + position.x) - 1.0f;
			m_instances[m_countInstance].ndcTransform._24 = h * (-cx * s + -cy * c + cy + position.y) + 1.0f;
			m_instances[m_countInstance].ndcTransform._34 = 0.0f;
			m_instances[m_countInstance].ndcTransform._44 = 1.0f;
#endif
			float tw = static_cast<float>(m_texture2dDesc.Width);
			float th = static_cast<float>(m_texture2dDesc.Height);
			if (reverse)
				m_instances[m_countInstance].texcoordTransform = VECTOR4F(texPos.x / tw + 1.0f, texPos.y / th, texSize.x / tw, texSize.y / th);
			else
				m_instances[m_countInstance].texcoordTransform = VECTOR4F(texPos.x / tw, texPos.y / th, texSize.x / tw, texSize.y / th);
			m_instances[m_countInstance].color = color;

			m_countInstance++;
		}

		void SpriteBatch::End(ID3D11DeviceContext* immediateContext)
		{
			immediateContext->Unmap(m_instanceBuffer.Get(), 0);
			immediateContext->DrawInstanced(4, m_countInstance, 0, 0);

			ID3D11ShaderResourceView* nullSrv = nullptr;
			immediateContext->PSSetShaderResources(SLOT0, 1, &nullSrv);
			immediateContext->PSSetShaderResources(SLOT1, 1, &nullSrv);

			m_spritePS->Deactivate(immediateContext);
			m_spriteVS->Deactivate(immediateContext);
		}
	}
}

