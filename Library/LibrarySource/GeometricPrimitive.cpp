#include "..\LibrarySource\GeometricPrimitve.h"
#include "Framework.h"

namespace Source
{
	namespace GeometricPrimitive
	{
		GeometricPrimitive::GeometricPrimitive(ID3D11Device* device, std::string fileName)
		{
			HRESULT hr = S_OK;

			m_geometricPrimitiveVS = std::make_unique<Source::Shader::VertexShader<Vertex>>(device, "../Library/LibraryShader/GeometricPrimitive_vs.cso");
			m_geometricPrimitivePS = std::make_unique<Source::Shader::PixelShader>(device, "../Library/LibraryShader/GeometricPrimitive_ps.cso");

			m_constantBuffer = std::make_unique<Source::ConstantBuffer::ConstantBuffer<ShaderConstants>>(device);

			if (fileName.empty())
				Texture::MakeDummyTexture(device, m_shaderResourceView.GetAddressOf());
			else
				Texture::LoadTextureFromFile(device, fileName.c_str(), m_shaderResourceView.GetAddressOf(), true);

			Texture::Texture2dDescription(m_shaderResourceView.Get(), m_texture2dDesc);

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
		}



		void GeometricPrimitive::CreateBuffers(ID3D11Device* device, Vertex* vertices, int num_vertices, u_int* indices, int num_indices)
		{
			HRESULT hr;
			{
				D3D11_BUFFER_DESC bufferDesc = {};
				D3D11_SUBRESOURCE_DATA subresource_data = {};

				bufferDesc.ByteWidth = sizeof(Vertex) * num_vertices;
				//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
				bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
				bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				bufferDesc.MiscFlags = 0;
				bufferDesc.StructureByteStride = 0;
				subresource_data.pSysMem = vertices;
				subresource_data.SysMemPitch = 0; //Not use for vertex buffers.mm 
				subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.

				hr = device->CreateBuffer(&bufferDesc, &subresource_data, m_vertexBuffer.ReleaseAndGetAddressOf());
				if (FAILED(hr))
					assert(!"Could not Create a VertexBuffer");
			}
			{
				D3D11_BUFFER_DESC bufferDesc = {};
				D3D11_SUBRESOURCE_DATA subresourceData = {};

				bufferDesc.ByteWidth = sizeof(u_int) * num_indices;
				//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
				bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
				bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				bufferDesc.CPUAccessFlags = 0;
				bufferDesc.MiscFlags = 0;
				bufferDesc.StructureByteStride = 0;
				subresourceData.pSysMem = indices;
				subresourceData.SysMemPitch = 0; //Not use for index buffers.
				subresourceData.SysMemSlicePitch = 0; //Not use for index buffers.
				hr = device->CreateBuffer(&bufferDesc, &subresourceData, m_indexBuffer.ReleaseAndGetAddressOf());
				if (FAILED(hr))
					assert(!"Could not Create a IndexBuffer");


			}
		}

		void GeometricPrimitive::Begin(ID3D11DeviceContext* immediateContext, bool isScroll, bool wireframe)
		{
			UINT strides[2] = { sizeof(Vertex), sizeof(Instance) };
			UINT offsets[2] = { 0, 0 };
			ID3D11Buffer* vbs[2] = { m_vertexBuffer.Get(), m_instanceBuffer.Get() };
			immediateContext->IASetVertexBuffers(0, 2, vbs, strides, offsets);
			immediateContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			m_geometricPrimitiveVS->Activate(immediateContext);
			m_geometricPrimitivePS->Activate(immediateContext);

			immediateContext->OMSetBlendState(Framework::GetBlendState(Framework::BS_ADD), nullptr, 0xFFFFFFFF);
			immediateContext->OMSetDepthStencilState(Framework::GetDephtStencilState(Framework::DS_TRUE), 1);

			if (wireframe)
				immediateContext->RSSetState(Framework::GetRasterizerState(Framework::RS_CULL_BACK_WIRE));
			else
				immediateContext->RSSetState(Framework::GetRasterizerState(Framework::RS_CULL_BACK));


			immediateContext->PSSetSamplers(0, 1, Framework::GetSamplerState(Framework::SS_WRAP));
		}

		void GeometricPrimitive::Render(ID3D11DeviceContext* immediateContext, const FLOAT4X4& view, const FLOAT4X4& projection,
			const std::vector<Source::InstanceData::InstanceData>& instanceData, const VECTOR4F& materialColor,const VECTOR4F& scrollValue)
		{
			D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
			D3D11_MAPPED_SUBRESOURCE mappedBuffer;
			HRESULT hr = immediateContext->Map(m_instanceBuffer.Get(), 0, map, 0, &mappedBuffer);
			if (FAILED(hr))
				assert(!"Map miss");
			Instance* instances = nullptr;
			instances = static_cast<Instance*>(mappedBuffer.pData);
			for (UINT i = 0; i < instanceData.size(); i++)
			{
				DirectX::XMMATRIX W, V, P;

				W = DirectX::XMLoadFloat4x4(&instanceData.at(i).world);
				V = DirectX::XMLoadFloat4x4(&view);
				P = DirectX::XMLoadFloat4x4(&projection);

				DirectX::XMStoreFloat4x4(&instances[i].worldViewProjection, (W * V * P));
				instances[i].world = instanceData.at(i).world;
				instances[i].materialColor = instanceData.at(i).color;
			}
			immediateContext->Unmap(m_instanceBuffer.Get(), 0);

			m_constantBuffer->data.scrollValue = scrollValue;
			m_constantBuffer->Activate(immediateContext, SLOT0, true, true);

			immediateContext->PSSetShaderResources(SLOT0, 1, m_shaderResourceView.GetAddressOf());


			D3D11_BUFFER_DESC bufferDesc;
			m_indexBuffer->GetDesc(&bufferDesc);
			immediateContext->DrawIndexedInstanced(bufferDesc.ByteWidth / sizeof(u_int), static_cast<UINT>(instanceData.size()), 0, 0, 0);
		}

		void GeometricPrimitive::End(ID3D11DeviceContext* immediateContext)
		{
			m_constantBuffer->Deactivate(immediateContext);

			m_geometricPrimitivePS->Deactivate(immediateContext);
			m_geometricPrimitiveVS->Deactivate(immediateContext);
		}



		GeometricCube::GeometricCube(ID3D11Device* device, std::string fileName) : GeometricPrimitive(device,fileName)
		{
			Vertex vertices[24] = {};
			u_int indices[36] = {};

			int face;

			// top-side
			// 0---------1
			// |         |
			// |   -Y    |
			// |         |
			// 2---------3
			face = 0;
			vertices[face * 4 + 0].position = VECTOR3F(-0.5f, +0.5f, +0.5f);
			vertices[face * 4 + 1].position = VECTOR3F(+0.5f, +0.5f, +0.5f);
			vertices[face * 4 + 2].position = VECTOR3F(-0.5f, +0.5f, -0.5f);
			vertices[face * 4 + 3].position = VECTOR3F(+0.5f, +0.5f, -0.5f);
			vertices[face * 4 + 0].normal = VECTOR3F(0.0f, +1.0f, 0.0f);
			vertices[face * 4 + 1].normal = VECTOR3F(0.0f, +1.0f, 0.0f);
			vertices[face * 4 + 2].normal = VECTOR3F(0.0f, +1.0f, 0.0f);
			vertices[face * 4 + 3].normal = VECTOR3F(0.0f, +1.0f, 0.0f);
			vertices[face * 4 + 0].texcoord = VECTOR2F(1, 0);
			vertices[face * 4 + 1].texcoord = VECTOR2F(0, 0);
			vertices[face * 4 + 2].texcoord = VECTOR2F(1, 1);
			vertices[face * 4 + 3].texcoord = VECTOR2F(0, 1);

			indices[face * 6 + 0] = face * 4 + 0;
			indices[face * 6 + 1] = face * 4 + 1;
			indices[face * 6 + 2] = face * 4 + 2;
			indices[face * 6 + 3] = face * 4 + 1;
			indices[face * 6 + 4] = face * 4 + 3;
			indices[face * 6 + 5] = face * 4 + 2;

			// bottom-side
			// 0---------1
			// |         |
			// |   -Y    |
			// |         |
			// 2---------3
			face += 1;
			vertices[face * 4 + 0].position = VECTOR3F(-0.5f, -0.5f, +0.5f);
			vertices[face * 4 + 1].position = VECTOR3F(+0.5f, -0.5f, +0.5f);
			vertices[face * 4 + 2].position = VECTOR3F(-0.5f, -0.5f, -0.5f);
			vertices[face * 4 + 3].position = VECTOR3F(+0.5f, -0.5f, -0.5f);
			vertices[face * 4 + 0].normal = VECTOR3F(0.0f, -1.0f, 0.0f);
			vertices[face * 4 + 1].normal = VECTOR3F(0.0f, -1.0f, 0.0f);
			vertices[face * 4 + 2].normal = VECTOR3F(0.0f, -1.0f, 0.0f);
			vertices[face * 4 + 3].normal = VECTOR3F(0.0f, -1.0f, 0.0f);
			vertices[face * 4 + 0].texcoord = VECTOR2F(1, 0);
			vertices[face * 4 + 1].texcoord = VECTOR2F(0, 0);
			vertices[face * 4 + 2].texcoord = VECTOR2F(1, 1);
			vertices[face * 4 + 3].texcoord = VECTOR2F(0, 1);

			indices[face * 6 + 0] = face * 4 + 0;
			indices[face * 6 + 1] = face * 4 + 2;
			indices[face * 6 + 2] = face * 4 + 1;
			indices[face * 6 + 3] = face * 4 + 1;
			indices[face * 6 + 4] = face * 4 + 2;
			indices[face * 6 + 5] = face * 4 + 3;

			// front-side
			// 0---------1
			// |         |
			// |   +Z    |
			// |         |
			// 2---------3
			face += 1;
			vertices[face * 4 + 0].position = VECTOR3F(-0.5f, +0.5f, -0.5f);
			vertices[face * 4 + 1].position = VECTOR3F(+0.5f, +0.5f, -0.5f);
			vertices[face * 4 + 2].position = VECTOR3F(-0.5f, -0.5f, -0.5f);
			vertices[face * 4 + 3].position = VECTOR3F(+0.5f, -0.5f, -0.5f);
			vertices[face * 4 + 0].normal = VECTOR3F(0.0f, 0.0f, -1.0f);
			vertices[face * 4 + 1].normal = VECTOR3F(0.0f, 0.0f, -1.0f);
			vertices[face * 4 + 2].normal = VECTOR3F(0.0f, 0.0f, -1.0f);
			vertices[face * 4 + 3].normal = VECTOR3F(0.0f, 0.0f, -1.0f);
			vertices[face * 4 + 0].texcoord = VECTOR2F(1, 0);
			vertices[face * 4 + 1].texcoord = VECTOR2F(0, 0);
			vertices[face * 4 + 2].texcoord = VECTOR2F(1, 1);
			vertices[face * 4 + 3].texcoord = VECTOR2F(0, 1);

			indices[face * 6 + 0] = face * 4 + 0;
			indices[face * 6 + 1] = face * 4 + 1;
			indices[face * 6 + 2] = face * 4 + 2;
			indices[face * 6 + 3] = face * 4 + 1;
			indices[face * 6 + 4] = face * 4 + 3;
			indices[face * 6 + 5] = face * 4 + 2;

			// back-side
			// 0---------1
			// |         |
			// |   +Z    |
			// |         |
			// 2---------3
			face += 1;
			vertices[face * 4 + 0].position = VECTOR3F(-0.5f, +0.5f, +0.5f);
			vertices[face * 4 + 1].position = VECTOR3F(+0.5f, +0.5f, +0.5f);
			vertices[face * 4 + 2].position = VECTOR3F(-0.5f, -0.5f, +0.5f);
			vertices[face * 4 + 3].position = VECTOR3F(+0.5f, -0.5f, +0.5f);
			vertices[face * 4 + 0].normal = VECTOR3F(+0.0f, +0.0f, +1.0f);
			vertices[face * 4 + 1].normal = VECTOR3F(+0.0f, +0.0f, +1.0f);
			vertices[face * 4 + 2].normal = VECTOR3F(+0.0f, +0.0f, +1.0f);
			vertices[face * 4 + 3].normal = VECTOR3F(+0.0f, +0.0f, +1.0f);
			vertices[face * 4 + 0].texcoord = VECTOR2F(1, 0);
			vertices[face * 4 + 1].texcoord = VECTOR2F(0, 0);
			vertices[face * 4 + 2].texcoord = VECTOR2F(1, 1);
			vertices[face * 4 + 3].texcoord = VECTOR2F(0, 1);

			indices[face * 6 + 0] = face * 4 + 0;
			indices[face * 6 + 1] = face * 4 + 2;
			indices[face * 6 + 2] = face * 4 + 1;
			indices[face * 6 + 3] = face * 4 + 1;
			indices[face * 6 + 4] = face * 4 + 2;
			indices[face * 6 + 5] = face * 4 + 3;

			// right-side
			// 0---------1
			// |         |      
			// |   -X    |
			// |         |
			// 2---------3
			face += 1;
			vertices[face * 4 + 0].position = VECTOR3F(+0.5f, +0.5f, -0.5f);
			vertices[face * 4 + 1].position = VECTOR3F(+0.5f, +0.5f, +0.5f);
			vertices[face * 4 + 2].position = VECTOR3F(+0.5f, -0.5f, -0.5f);
			vertices[face * 4 + 3].position = VECTOR3F(+0.5f, -0.5f, +0.5f);
			vertices[face * 4 + 0].normal = VECTOR3F(+1.0f, +0.0f, +0.0f);
			vertices[face * 4 + 1].normal = VECTOR3F(+1.0f, +0.0f, +0.0f);
			vertices[face * 4 + 2].normal = VECTOR3F(+1.0f, +0.0f, +0.0f);
			vertices[face * 4 + 3].normal = VECTOR3F(+1.0f, +0.0f, +0.0f);
			vertices[face * 4 + 0].texcoord = VECTOR2F(1, 0);
			vertices[face * 4 + 1].texcoord = VECTOR2F(0, 0);
			vertices[face * 4 + 2].texcoord = VECTOR2F(1, 1);
			vertices[face * 4 + 3].texcoord = VECTOR2F(0, 1);

			indices[face * 6 + 0] = face * 4 + 0;
			indices[face * 6 + 1] = face * 4 + 1;
			indices[face * 6 + 2] = face * 4 + 2;
			indices[face * 6 + 3] = face * 4 + 1;
			indices[face * 6 + 4] = face * 4 + 3;
			indices[face * 6 + 5] = face * 4 + 2;

			// left-side
			// 0---------1
			// |         |      
			// |   -X    |
			// |         |
			// 2---------3
			face += 1;
			vertices[face * 4 + 0].position = VECTOR3F(-0.5f, +0.5f, -0.5f);
			vertices[face * 4 + 1].position = VECTOR3F(-0.5f, +0.5f, +0.5f);
			vertices[face * 4 + 2].position = VECTOR3F(-0.5f, -0.5f, -0.5f);
			vertices[face * 4 + 3].position = VECTOR3F(-0.5f, -0.5f, +0.5f);
			vertices[face * 4 + 0].normal = VECTOR3F(-1.0f, +0.0f, +0.0f);
			vertices[face * 4 + 1].normal = VECTOR3F(-1.0f, +0.0f, +0.0f);
			vertices[face * 4 + 2].normal = VECTOR3F(-1.0f, +0.0f, +0.0f);
			vertices[face * 4 + 3].normal = VECTOR3F(-1.0f, +0.0f, +0.0f);
			vertices[face * 4 + 0].texcoord = VECTOR2F(1, 0);
			vertices[face * 4 + 1].texcoord = VECTOR2F(0, 0);
			vertices[face * 4 + 2].texcoord = VECTOR2F(1, 1);
			vertices[face * 4 + 3].texcoord = VECTOR2F(0, 1);

			indices[face * 6 + 0] = face * 4 + 0;
			indices[face * 6 + 1] = face * 4 + 2;
			indices[face * 6 + 2] = face * 4 + 1;
			indices[face * 6 + 3] = face * 4 + 1;
			indices[face * 6 + 4] = face * 4 + 2;
			indices[face * 6 + 5] = face * 4 + 3;

			CreateBuffers(device, vertices, 24, indices, 36);
		}

#include <vector>
		GeometricCylinder::GeometricCylinder(ID3D11Device* device,std::string fileName,u_int slices) : GeometricPrimitive(device, fileName)
		{
			std::vector<Vertex> vertices;
			std::vector<u_int> indices;

			float d = 2.0f * DirectX::XM_PI / slices;
			float r = 1.0f;

			Vertex vertex;
			u_int baseIndex = 0;

			// top cap centre
			vertex.position = VECTOR3F(0.0f, +0.5f, 0.0f);
			vertex.normal = VECTOR3F(0.0f, +1.0f, 0.0f);
			vertices.push_back(vertex);
			// top cap ring
			for (u_int i = 0; i < slices; ++i)
			{
				float x = r * cosf(i * d);
				float z = r * sinf(i * d);
				vertex.position = VECTOR3F(x, +0.5f, z);
				vertex.normal = VECTOR3F(0.0f, +1.0f, 0.0f);
				vertices.push_back(vertex);
			}
			baseIndex = 0;
			for (u_int i = 0; i < slices - 1; ++i)
			{
				indices.push_back(baseIndex + 0);
				indices.push_back(baseIndex + i + 2);
				indices.push_back(baseIndex + i + 1);
			}
			indices.push_back(baseIndex + 0);
			indices.push_back(baseIndex + 1);
			indices.push_back(baseIndex + slices);

			// bottom cap centre
			vertex.position = VECTOR3F(0.0f, -0.5f, 0.0f);
			vertex.normal = VECTOR3F(0.0f, -1.0f, 0.0f);
			vertices.push_back(vertex);
			// bottom cap ring
			for (u_int i = 0; i < slices; ++i)
			{
				float x = r * cosf(i * d);
				float z = r * sinf(i * d);
				vertex.position = VECTOR3F(x, -0.5f, z);
				vertex.normal = VECTOR3F(0.0f, -1.0f, 0.0f);
				vertices.push_back(vertex);
			}
			baseIndex = slices + 1;
			for (u_int i = 0; i < slices - 1; ++i)
			{
				indices.push_back(baseIndex + 0);
				indices.push_back(baseIndex + i + 1);
				indices.push_back(baseIndex + i + 2);
			}
			indices.push_back(baseIndex + 0);
			indices.push_back(baseIndex + (slices - 1) + 1);
			indices.push_back(baseIndex + (0) + 1);

			// side rectangle
			for (u_int i = 0; i < slices; ++i)
			{
				float x = r * cosf(i * d);
				float z = r * sinf(i * d);

				vertex.position = VECTOR3F(x, +0.5f, z);
				vertex.normal = VECTOR3F(x, 0.0f, z);
				vertices.push_back(vertex);

				vertex.position = VECTOR3F(x, -0.5f, z);
				vertex.normal = VECTOR3F(x, 0.0f, z);
				vertices.push_back(vertex);
			}
			baseIndex = slices * 2 + 2;
			for (u_int i = 0; i < slices - 1; ++i)
			{
				indices.push_back(baseIndex + i * 2 + 0);
				indices.push_back(baseIndex + i * 2 + 2);
				indices.push_back(baseIndex + i * 2 + 1);

				indices.push_back(baseIndex + i * 2 + 1);
				indices.push_back(baseIndex + i * 2 + 2);
				indices.push_back(baseIndex + i * 2 + 3);
			}
			indices.push_back(baseIndex + (slices - 1) * 2 + 0);
			indices.push_back(baseIndex + (0) * 2 + 0);
			indices.push_back(baseIndex + (slices - 1) * 2 + 1);

			indices.push_back(baseIndex + (slices - 1) * 2 + 1);
			indices.push_back(baseIndex + (0) * 2 + 0);
			indices.push_back(baseIndex + (0) * 2 + 1);

			CreateBuffers(device, vertices.data(), static_cast<int>(vertices.size()), indices.data(), static_cast<int>(indices.size()));
		}

		GeometricSphere::GeometricSphere(ID3D11Device* device,std::string fileName, u_int slices, u_int stacks) : GeometricPrimitive(device,fileName)
		{
			std::vector<Vertex> vertices;
			std::vector<u_int> indices;

			float r = 1.0f;

			//
			// Compute the vertices stating at the top pole and moving down the stacks.
			//

			// Poles: note that there will be texture coordinate distortion as there is
			// not a unique point on the texture map to assign to the pole when mapping
			// a rectangular texture onto a sphere.
			Vertex topVertex;
			topVertex.position = VECTOR3F(0.0f, +r, 0.0f);
			topVertex.normal = VECTOR3F(0.0f, +1.0f, 0.0f);

			Vertex bottomVertex;
			bottomVertex.position = VECTOR3F(0.0f, -r, 0.0f);
			bottomVertex.normal = VECTOR3F(0.0f, -1.0f, 0.0f);

			vertices.push_back(topVertex);

			float phiStep = DirectX::XM_PI / stacks;
			float thetaStep = 2.0f * DirectX::XM_PI / slices;

			// Compute vertices for each stack ring (do not count the poles as rings).
			for (u_int i = 1; i <= stacks - 1; ++i)
			{
				float phi = i * phiStep;

				// Vertices of ring.
				for (u_int j = 0; j <= slices; ++j)
				{
					float theta = j * thetaStep;

					Vertex v;

					// spherical to cartesian
					v.position.x = r * sinf(phi) * cosf(theta);
					v.position.y = r * cosf(phi);
					v.position.z = r * sinf(phi) * sinf(theta);

					DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&v.position);
					DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));

					v.texcoord.x = 1.0f - (float)j / slices;
					v.texcoord.y = (float)i / stacks - 1.0f;
					vertices.push_back(v);
				}
			}

			vertices.push_back(bottomVertex);

			//
			// Compute indices for top stack.  The top stack was written first to the vertex buffer
			// and connects the top pole to the first ring.
			//
			for (UINT i = 1; i <= slices; ++i)
			{
				indices.push_back(0);
				indices.push_back(i + 1);
				indices.push_back(i);
			}

			//
			// Compute indices for inner stacks (not connected to poles).
			//

			// Offset the indices to the index of the first vertex in the first ring.
			// This is just skipping the top pole vertex.
			u_int baseIndex = 1;
			u_int ringVertexCount = slices + 1;
			for (u_int i = 0; i < stacks - 2; ++i)
			{
				for (u_int j = 0; j < slices; ++j)
				{
					indices.push_back(baseIndex + i * ringVertexCount + j);
					indices.push_back(baseIndex + i * ringVertexCount + j + 1);
					indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

					indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
					indices.push_back(baseIndex + i * ringVertexCount + j + 1);
					indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
				}
			}

			//
			// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
			// and connects the bottom pole to the bottom ring.
			//

			// South pole vertex was added last.
			u_int south_pole_index = (u_int)vertices.size() - 1;

			// Offset the indices to the index of the first vertex in the last ring.
			baseIndex = south_pole_index - ringVertexCount;

			for (u_int i = 0; i < slices; ++i)
			{
				indices.push_back(south_pole_index);
				indices.push_back(baseIndex + i);
				indices.push_back(baseIndex + i + 1);
			}
			CreateBuffers(device, vertices.data(), static_cast<int>(vertices.size()), indices.data(), static_cast<int>(indices.size()));
		}

		GeometricCapsule::GeometricCapsule(ID3D11Device* device,std::string fileName, u_int slices, u_int stacks) : GeometricPrimitive(device,fileName)
		{
			//Vertex Data Input
			std::vector<Vertex> vertices = {};
			std::vector<u_int> indices = {};

			float d = 2.0f * DirectX::XM_PI / slices;
			float r = 1.0f;

			Vertex _vertex;
			u_int base_index = 0;

			// top cap centre
			_vertex.position = VECTOR3F(0.0f, +0.5f, 0.0f);
			_vertex.normal = VECTOR3F(0.0f, +1.0f, 0.0f);
			vertices.push_back(_vertex);

			// top cap ring
			for (u_int i = 0; i < slices; ++i)
			{
				float x = r * cosf(i * d);
				float z = r * sinf(i * d);
				_vertex.position = VECTOR3F(x, +0.5f, z);
				_vertex.normal = VECTOR3F(0.0f, +1.0f, 0.0f);
				vertices.push_back(_vertex);
			}
			base_index = 0;
			for (u_int i = 0; i < slices - 1; ++i)
			{
				indices.push_back(base_index + 0);
				indices.push_back(base_index + i + 2);
				indices.push_back(base_index + i + 1);
			}
			indices.push_back(base_index + 0);
			indices.push_back(base_index + 1);
			indices.push_back(base_index + slices);

			_vertex.position = VECTOR3F(0.0f, -0.5f, 0.0f);
			_vertex.normal = VECTOR3F(0.0f, -1.0f, 0.0f);
			vertices.push_back(_vertex);
			// bottom cap ring
			for (u_int i = 0; i < slices; ++i)
			{
				float x = r * cosf(i * d);
				float z = r * sinf(i * d);
				_vertex.position = VECTOR3F(x, -0.5f, z);
				_vertex.normal = VECTOR3F(0.0f, -1.0f, 0.0f);
				vertices.push_back(_vertex);
			}
			base_index = slices + 1;

			for (u_int i = 0; i < slices - 1; ++i)
			{
				indices.push_back(base_index + 0);
				indices.push_back(base_index + i + 1);
				indices.push_back(base_index + i + 2);
			}
			indices.push_back(base_index + 0);
			indices.push_back(base_index + (slices - 1) + 1);
			indices.push_back(base_index + (0) + 1);

			// side rectangle
			for (u_int i = 0; i < slices; ++i)
			{
				float x = r * cosf(i * d);
				float z = r * sinf(i * d);

				_vertex.position = VECTOR3F(x, +0.5f, z);
				_vertex.normal = VECTOR3F(x, 0.0f, z);
				vertices.push_back(_vertex);

				_vertex.position = VECTOR3F(x, -0.5f, z);
				_vertex.normal = VECTOR3F(x, 0.0f, z);
				vertices.push_back(_vertex);
			}

			for (u_int i = 0; i < slices; ++i)
			{
				float x = r * cosf(i * d);
				float z = r * sinf(i * d);

				_vertex.position = VECTOR3F(x, +0.5f, z);
				_vertex.normal = VECTOR3F(x, 0.0f, z);
				vertices.push_back(_vertex);
			}
			base_index = slices * 2 + 2;
			for (u_int i = 0; i < slices - 1; ++i)
			{
				indices.push_back(base_index + i * 2 + 0);
				indices.push_back(base_index + i * 2 + 2);
				indices.push_back(base_index + i * 2 + 1);

				indices.push_back(base_index + i * 2 + 1);
				indices.push_back(base_index + i * 2 + 2);
				indices.push_back(base_index + i * 2 + 3);
			}
			indices.push_back(base_index + (slices - 1) * 2 + 0);
			indices.push_back(base_index + (0) * 2 + 0);
			indices.push_back(base_index + (slices - 1) * 2 + 1);

			indices.push_back(base_index + (slices - 1) * 2 + 1);
			indices.push_back(base_index + (0) * 2 + 0);
			indices.push_back(base_index + (0) * 2 + 1);


			Vertex top_vertex;
			r = 1.0f;
			top_vertex.position = VECTOR3F(0.0f, +r + 0.5f, 0.0f);
			top_vertex.normal = VECTOR3F(0.0f, +1.0f, 0.0f);

			Vertex bottom_vertex;
			bottom_vertex.position = VECTOR3F(0.0f, -r - 0.5f, 0.0f);
			bottom_vertex.normal = VECTOR3F(0.0f, -1.0f, 0.0f);

			vertices.push_back(top_vertex);

			float phi_step = DirectX::XM_PI / stacks;
			float theta_step = 2.0f * DirectX::XM_PI / slices;


			for (u_int i = 1; i <= stacks - 1; ++i)
			{
				float phi = i * phi_step;

				for (u_int j = 0; j <= slices; ++j)
				{
					float theta = j * theta_step;

					Vertex v;

					v.position.x = r * sinf(phi) * cosf(theta);
					if (i <= stacks / 2)
					{
						v.position.y = r * cosf(phi) + 0.5f;
					}
					else
					{
						v.position.y = r * cosf(phi) - 0.5f;
					}
					v.position.z = r * sinf(phi) * sinf(theta);

					DirectX::XMVECTOR p = XMLoadFloat3(&v.position);
					DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));

					vertices.push_back(v);
				}
			}

			vertices.push_back(bottom_vertex);
			base_index = slices * 5 + 3;

			for (UINT i = 1; i <= slices; ++i)
			{
				indices.push_back(base_index);
				indices.push_back(base_index + i + 1);
				indices.push_back(base_index + i);
			}


			u_int ring_vertex_count = slices + 1;
			for (u_int i = 0; i < stacks - 2; ++i)
			{
				for (u_int j = 0; j < slices; ++j)
				{
					indices.push_back(base_index + i * ring_vertex_count + j);
					indices.push_back(base_index + i * ring_vertex_count + j + 1);
					indices.push_back(base_index + (i + 1) * ring_vertex_count + j);

					indices.push_back(base_index + (i + 1) * ring_vertex_count + j);
					indices.push_back(base_index + i * ring_vertex_count + j + 1);
					indices.push_back(base_index + (i + 1) * ring_vertex_count + j + 1);
				}
			}


			u_int south_pole_index = (u_int)vertices.size() - 1;


			base_index = south_pole_index - ring_vertex_count;

			for (u_int i = 0; i < slices; ++i)
			{
				indices.push_back(south_pole_index);
				indices.push_back(base_index + i);
				indices.push_back(base_index + i + 1);
			}

			CreateBuffers(device, vertices.data(), static_cast<int>(vertices.size()), indices.data(), static_cast<int>(indices.size()));
		}

	}
}