#include "StaticMesh.h"

namespace Source
{
	namespace StaticMesh
	{
		StaticMesh::StaticMesh(ID3D11Device* device, const char* filename)
		{
			_resource = std::make_unique<ModelResource::Substance<Vertex>>(filename, -1);

			_resource->CreateRenderObjects(device);
			char media_directory[256];
			strcpy_s(media_directory, filename);
			PathFindFileNameA(media_directory)[0] = '\0';
			_resource->CreateShaderResourceViews(device, media_directory, true/*force_srgb*/, true/*enable_caching*/);



			m_constantBuffer = std::make_unique<ConstantBuffer::ConstantBuffer<ShaderConstants>>(device);
			m_staticMeshVS = std::make_unique<Source::Shader::VertexShader<StaticMesh::Vertex>>(device, "../Library/LibraryShader/StaticMesh_vs.cso");
			m_staticMeshPS = std::make_unique<Source::Shader::PixelShader>(device, "../Library/LibraryShader/StaticMesh_ps.cso");

			
			for (auto& b : _resource->_meshes.begin()->offsetTransforms)
			{
				_modelConfig.boneName.emplace_back(b.name);
			}

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
				HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, m_instanceBuffer.GetAddressOf());
				if (FAILED(hr))
					assert(!"Could not Create a InstanceBuffer");
			}
			delete[] instances;
		}

		void StaticMesh::Render(ID3D11DeviceContext* immediateContext, std::vector<Source::InstanceData::InstanceData> instanceData, const VECTOR4F& materialColor)
		{
			m_staticMeshVS->Activate(immediateContext);
			m_staticMeshPS->Activate(immediateContext);
			for (ModelResource::Substance<Vertex>::Mesh& mesh : _resource->_meshes)
			{
				UINT strides[2] = { sizeof(Vertex), sizeof(Instance) };
				UINT offsets[2] = { 0, 0 };
				ID3D11Buffer* vertexBuffers[2] = { mesh.vertexBuffer.Get(), m_instanceBuffer.Get() };
				immediateContext->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);
				immediateContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
				immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				immediateContext->OMSetBlendState(Framework::GetBlendState(Framework::BS_ALPHA), nullptr, 0xFFFFFFFF);
				immediateContext->OMSetDepthStencilState(Framework::GetDephtStencilState(Framework::DS_TRUE), 1);
				immediateContext->RSSetState(Framework::GetRasterizerState(Framework::RS_CLOCK_TRUE));

				D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
				D3D11_MAPPED_SUBRESOURCE mappedBuffer;
				HRESULT hr = immediateContext->Map(m_instanceBuffer.Get(), 0, map, 0, &mappedBuffer);
				if (FAILED(hr))
					assert(!"Map miss");
				Instance* instances = nullptr;
				instances = static_cast<Instance*>(mappedBuffer.pData);
				for (UINT i = 0; i < instanceData.size(); i++)
				{
					DirectX::XMStoreFloat4x4
					(
						&instances[i].world,
						DirectX::XMMatrixTranspose
						(
							DirectX::XMLoadFloat4x4(&mesh.globalTransform) *
							DirectX::XMLoadFloat4x4(&_resource->axisSystemTransform) *
							DirectX::XMLoadFloat4x4(&instanceData.at(i).world)
						)
					);
				}
				immediateContext->Unmap(m_instanceBuffer.Get(), 0);

				for (ModelResource::Material& material : mesh.materials)
				{
					m_constantBuffer->data.materialColor = material.diffuse.color * materialColor;
					m_constantBuffer->Activate(immediateContext, SLOT0, true, false);

					if (_shaderON.diffuse)
						immediateContext->PSSetShaderResources(0, 1, material.diffuse.shaderResourceView.GetAddressOf());
					if (_shaderON.ambient)
						immediateContext->PSSetShaderResources(1, 1, material.ambient.shaderResourceView.GetAddressOf());
					if (_shaderON.specular)
						immediateContext->PSSetShaderResources(2, 1, material.specular.shaderResourceView.GetAddressOf());
					if (_shaderON.normalMap)
						immediateContext->PSSetShaderResources(3, 1, material.normalMap.shaderResourceView.GetAddressOf());

					immediateContext->PSSetSamplers(0, 1, Framework::GetSamplerState(Framework::SS_WRAP));
					immediateContext->DrawIndexedInstanced(material.indexCount, static_cast<UINT>(instanceData.size()), material.indexStart, 0, 0);
				
				}
				m_constantBuffer->Deactivate(immediateContext);

			}
			m_staticMeshPS->Deactivate(immediateContext);
			m_staticMeshVS->Deactivate(immediateContext);

		}

		void StaticMesh::Render(ID3D11DeviceContext* immediateContext, FLOAT4X4 world, const VECTOR4F& materialColor)
		{
			m_staticMeshVS->Activate(immediateContext);
			m_staticMeshPS->Activate(immediateContext);
			for (ModelResource::Substance<Vertex>::Mesh& mesh : _resource->_meshes)
			{
				UINT strides[2] = { sizeof(Vertex), sizeof(Instance) };
				UINT offsets[2] = { 0, 0 };
				ID3D11Buffer* vertexBuffers[2] = { mesh.vertexBuffer.Get(), m_instanceBuffer.Get() };
				immediateContext->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);
				immediateContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
				immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				immediateContext->OMSetBlendState(Framework::GetBlendState(Framework::BS_ALPHA), nullptr, 0xFFFFFFFF);
				immediateContext->OMSetDepthStencilState(Framework::GetDephtStencilState(Framework::DS_TRUE), 1);
				immediateContext->RSSetState(Framework::GetRasterizerState(Framework::RS_CLOCK_TRUE));

				D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
				D3D11_MAPPED_SUBRESOURCE mappedBuffer;
				HRESULT hr = immediateContext->Map(m_instanceBuffer.Get(), 0, map, 0, &mappedBuffer);
				if (FAILED(hr))
					assert(!"Map miss");
				Instance* instances = nullptr;
				instances = static_cast<Instance*>(mappedBuffer.pData);
					DirectX::XMStoreFloat4x4
					(
						&instances[0].world,
						DirectX::XMMatrixTranspose
						(
							DirectX::XMLoadFloat4x4(&mesh.globalTransform) *
							DirectX::XMLoadFloat4x4(&_resource->axisSystemTransform) *
							DirectX::XMLoadFloat4x4(&world)
						)
					);
				immediateContext->Unmap(m_instanceBuffer.Get(), 0);

				for (ModelResource::Material& material : mesh.materials)
				{
					m_constantBuffer->data.materialColor = material.diffuse.color * materialColor;
					m_constantBuffer->Activate(immediateContext, SLOT0, true, false);

					if (_shaderON.diffuse)
						immediateContext->PSSetShaderResources(0, 1, material.diffuse.shaderResourceView.GetAddressOf());
					if (_shaderON.ambient)
						immediateContext->PSSetShaderResources(1, 1, material.ambient.shaderResourceView.GetAddressOf());
					if (_shaderON.specular)
						immediateContext->PSSetShaderResources(2, 1, material.specular.shaderResourceView.GetAddressOf());
					if (_shaderON.normalMap)
						immediateContext->PSSetShaderResources(3, 1, material.normalMap.shaderResourceView.GetAddressOf());

					immediateContext->PSSetSamplers(0, 1, Framework::GetSamplerState(Framework::SS_WRAP));
					immediateContext->DrawIndexedInstanced(material.indexCount, 1, material.indexStart, 0, 0);

				}
				m_constantBuffer->Deactivate(immediateContext);

			}
			m_staticMeshPS->Deactivate(immediateContext);
			m_staticMeshVS->Deactivate(immediateContext);

		}

		int StaticMesh::RayPick(const DirectX::XMFLOAT3& startPosition,
			const DirectX::XMFLOAT3& endPosition,
			DirectX::XMFLOAT3* outPosition,
			DirectX::XMFLOAT3* outNormal,
			float* outLength)
		{
			int ret = -1;
			DirectX::XMVECTOR start = DirectX::XMLoadFloat3(&startPosition);
			DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&endPosition);
			DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(end, start);
			DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);
			DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(vec);

			float neart;
			DirectX::XMStoreFloat(&neart, length);

			DirectX::XMVECTOR position, normal;
			for (auto& mesh : _resource->_meshes)
			{
				for (const auto& it : mesh.faces)
				{
					//　面頂点取得
					DirectX::XMVECTOR a = DirectX::XMLoadFloat3(&it.position[0]);
					DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&it.position[1]);
					DirectX::XMVECTOR c = DirectX::XMLoadFloat3(&it.position[2]);
					//  ３辺算出
					DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(b, a);
					DirectX::XMVECTOR bc = DirectX::XMVectorSubtract(c, b);
					DirectX::XMVECTOR ca = DirectX::XMVectorSubtract(a, c);

					//	外積による法線算出		
					DirectX::XMVECTOR n = DirectX::XMVector3Cross(ab, bc);

					//	内積の結果がプラスならば裏向き
					DirectX::XMVECTOR dot = DirectX::XMVector3Dot(dir, n);
					float fdot;
					DirectX::XMStoreFloat(&fdot, dot);
					if (fdot >= 0) continue;

					//	交点算出
					DirectX::XMVECTOR v0 = DirectX::XMVectorSubtract(a, start);
					DirectX::XMVECTOR t = DirectX::XMVectorDivide(DirectX::XMVector3Dot(n, v0), dot);
					float ft;
					DirectX::XMStoreFloat(&ft, t);
					if (ft < .0f || ft > neart) continue;
					DirectX::XMVECTOR cp = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(dir, t), start);

					//  内点判定
					DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(a, cp);
					DirectX::XMVECTOR temp = DirectX::XMVector3Cross(v1, ab);
					DirectX::XMVECTOR work = DirectX::XMVector3Dot(temp, n);
					float fwork;
					DirectX::XMStoreFloat(&fwork, work);
					if (fwork < 0.0f) continue;

					DirectX::XMVECTOR v2 = DirectX::XMVectorSubtract(b, cp);
					temp = DirectX::XMVector3Cross(v2, bc);
					work = DirectX::XMVector3Dot(temp, n);
					DirectX::XMStoreFloat(&fwork, work);
					if (fwork < 0.0f) continue;

					DirectX::XMVECTOR v3 = DirectX::XMVectorSubtract(c, cp);
					temp = DirectX::XMVector3Cross(v3, ca);
					work = DirectX::XMVector3Dot(temp, n);
					DirectX::XMStoreFloat(&fwork, work);
					if (fwork < 0.0f) continue;

					// 情報保存
					position = cp;
					normal = n;
					neart = ft;
					ret = it.materialIndex;
				}
			}

			if (ret != -1)
			{
				DirectX::XMStoreFloat3(outPosition, position);
				DirectX::XMStoreFloat3(outNormal, normal);
			}

			//最も近いヒット位置
			*outLength = neart;

			return ret;
		}

		const char* StaticMesh::RayTrianglesIntersection(const VECTOR3F& rayPosition, const VECTOR3F& rayDirection,
			const FLOAT4X4& worldTransform, VECTOR3F* intersection)
		{
			for (const Source::ModelResource::Substance<Vertex>::Mesh& mesh : _resource->_meshes)
			{
				DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&mesh.globalTransform) *
					DirectX::XMLoadFloat4x4(&_resource->axisSystemTransform) * DirectX::XMLoadFloat4x4(&worldTransform);
				DirectX::XMMATRIX T = DirectX::XMMatrixInverse(0, M);

				VECTOR3F rayPositionInModelSpace;
				VECTOR3F rayDirectionInModelSpace;
				DirectX::XMStoreFloat3(&rayPositionInModelSpace, DirectX::XMVector4Transform(DirectX::XMVectorSet(rayPosition.x, rayPosition.y, rayPosition.z, 1), T));
				DirectX::XMStoreFloat3(&rayDirectionInModelSpace, DirectX::XMVector3Normalize(
					DirectX::XMVector4Transform(DirectX::XMVectorSet(rayDirection.x, rayDirection.y, rayDirection.z, 0), T)));

				if (!Source::RayTriangle::PointInAABB(reinterpret_cast<const float*>(&rayPositionInModelSpace),
					reinterpret_cast<const float*>(&mesh.boundingBox.min), reinterpret_cast<const float*>(&mesh.boundingBox.max)))
				{
					continue;
				}
				int indexOfIntersectedTriangle = Source::RayTriangle::RayTrianglesIntersection(&(mesh.vertices.at(0).position.x), sizeof(VECTOR3F),
					&(mesh.indices.at(0)), mesh.indices.size(),
					rayPositionInModelSpace, rayDirectionInModelSpace, FLT_MAX, intersection);

				if (indexOfIntersectedTriangle != -1)
				{
					DirectX::XMStoreFloat3(intersection, DirectX::XMVector3Transform(DirectX::XMVectorSet(intersection->x, intersection->y, intersection->z, 1), M));

					const char* materialName = mesh.FindMaterial(indexOfIntersectedTriangle);
					return materialName ? materialName : "";
				}
			}
			return nullptr;
		}

		const char* StaticMesh::RayTrianglesIntersectionDownward(const VECTOR3F& rayPosition, const FLOAT4X4& worldTransform, VECTOR3F* intersection)
		{
			const char* materialName = nullptr;
			for (const Source::ModelResource::Substance<Vertex>::Mesh& mesh : _resource->_meshes)
			{
				DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&mesh.globalTransform) *
					DirectX::XMLoadFloat4x4(&_resource->axisSystemTransform) *
					DirectX::XMLoadFloat4x4(&worldTransform);
				DirectX::XMMATRIX T = DirectX::XMMatrixInverse(NULL, M);

				VECTOR3F rayPositionInModelSpace;
				DirectX::XMStoreFloat3(&rayPositionInModelSpace, DirectX::XMVector4Transform(
					DirectX::XMVectorSet(rayPosition.x, rayPosition.y, rayPosition.z, 1), T));

				if (rayPositionInModelSpace.x >= mesh.boundingBox.max.x && rayPositionInModelSpace.x <= mesh.boundingBox.min.x &&
					rayPositionInModelSpace.z >= mesh.boundingBox.max.z && rayPositionInModelSpace.z <= mesh.boundingBox.min.z)
				{
					int indexOfIntersectedTriangle = Source::RayTriangle::RayTrianglesIntersectionDownward(&(mesh.vertices.at(0).position.x), sizeof(Vertex),
						&(mesh.indices.at(0)), mesh.indices.size(),
						rayPositionInModelSpace, FLT_MAX, intersection);
					if (indexOfIntersectedTriangle != -1)
					{
						DirectX::XMStoreFloat3(intersection, DirectX::XMVector3Transform(DirectX::XMVectorSet(intersection->x, intersection->y, intersection->z, 1), M));

						materialName = mesh.FindMaterial(indexOfIntersectedTriangle);
					//	return materialName ? materialName : "";

					}

				}

			}
			return materialName ? materialName : "";

			return nullptr;
		}
	}
}
