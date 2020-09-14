#include "SkinnedMesh.h"

namespace Source
{
	namespace SkinnedMesh
	{
		SkinnedMesh::SkinnedMesh(ID3D11Device* device, const char* filename, int animationSamplingRate)
		{
			_resource = std::make_unique<ModelResource::Substance<Vertex>>(filename, animationSamplingRate);

			_resource->CreateRenderObjects(device);
			char media_directory[256];
			strcpy_s(media_directory, filename);
			PathFindFileNameA(media_directory)[0] = '\0';
			_resource->CreateShaderResourceViews(device, media_directory, true/*force_srgb*/, true/*enable_caching*/);

			m_skinnedMeshVS = std::make_unique<Source::Shader::VertexShader<SkinnedMesh::Vertex>>(device, "../Library/LibraryShader/SkinnedMesh_vs.cso");
			m_skinnedMeshPS = std::make_unique<Source::Shader::PixelShader>(device, "../Library/LibraryShader/SkinnedMesh_ps.cso");

			m_constantBuffer = std::make_unique<Source::ConstantBuffer::ConstantBuffer<ShaderConstants>>(device);



			for (auto& b : _resource->_animationTakes)
			{
				_modelConfig.animationName.emplace_back(b.name);
			}
		}

		void SkinnedMesh::Render(ID3D11DeviceContext* immediateContext, const FLOAT4X4& world,
			VECTOR4F& materialColor, size_t animationClip, size_t animationFrame)
		{
			m_skinnedMeshVS->Activate(immediateContext);
			m_skinnedMeshPS->Activate(immediateContext);
			size_t numberOfAnimations = _resource->_animationTakes.size();
			for (ModelResource::Substance<Vertex>::Mesh& mesh : _resource->_meshes)
			{
				unsigned int stride = sizeof(Vertex);
				unsigned int offset = 0;

				immediateContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
				immediateContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
				immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				immediateContext->OMSetBlendState(Framework::GetBlendState(Framework::BS_ALPHA), nullptr, 0xFFFFFFFF);

				immediateContext->OMSetDepthStencilState(Framework::GetDephtStencilState(Framework::DS_TRUE), 1);

				immediateContext->RSSetState(Framework::GetRasterizerState(Framework::RS_CLOCK_TRUE));


				DirectX::XMStoreFloat4x4
				(
					&m_constantBuffer->data.world,
					DirectX::XMMatrixTranspose
					(
						DirectX::XMLoadFloat4x4(&mesh.globalTransform) *
						DirectX::XMLoadFloat4x4(&_resource->axisSystemTransform) *
						DirectX::XMLoadFloat4x4(&world)
					)
				);

				//_ASSERT_EXPR(numberOfAnimations > animationClip, L"'animation' is invalid number");
				//_ASSERT_EXPR(numberOfAnimations == mesh.animations.size(), L"wrong size");
				if (numberOfAnimations > 0)
				{

					const ModelResource::Animation& animation = mesh.animations.at(animationClip);
					const ModelResource::Skeletal& skeletalTransform = animation.skeletalTransform.at(animationFrame);
					FLOAT4X4 actorTransform;
					actorTransform = animation.actorTransforms.size() > 0 ? animation.actorTransforms.at(animationFrame).transform : mesh.globalTransform;

					size_t numberOfBones = mesh.offsetTransforms.size();
					_ASSERT_EXPR(numberOfBones < MAX_BONES, L"'number_of_bones' exceeds MAX_BONES.");
					for (size_t i = 0; i < numberOfBones; i++)
					{
						DirectX::XMStoreFloat4x4(&m_constantBuffer->data.boneTransforms[i],
							DirectX::XMMatrixTranspose(
								DirectX::XMLoadFloat4x4(&mesh.offsetTransforms.at(i).transform) *
								DirectX::XMLoadFloat4x4(&skeletalTransform.bones.at(i).transform) *
								DirectX::XMMatrixInverse(0, DirectX::XMLoadFloat4x4(&actorTransform))
							)
						);
					}
				}
				else
				{
					for (int i = 0; i < MAX_BONES; i++)
					{
						m_constantBuffer->data.boneTransforms[i] = FLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
					}
				}

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
					immediateContext->DrawIndexed(material.indexCount, material.indexStart, 0);
				}
				m_constantBuffer->Deactivate(immediateContext);
			}

			m_skinnedMeshVS->Deactivate(immediateContext);
			m_skinnedMeshPS->Deactivate(immediateContext);
		}

		void SkinnedMesh::Render(ID3D11DeviceContext* immediateContext, const FLOAT4X4& world, VECTOR4F& materialColor, 
			std::vector<std::vector<FLOAT4X4>>& transform, int cullingMesh)
		{
			m_skinnedMeshVS->Activate(immediateContext);
			m_skinnedMeshPS->Activate(immediateContext);
			size_t numberOfAnimations = _resource->_animationTakes.size();
			for (int i = 0; i< static_cast<int>(_resource->_meshes.size());++i)
			{
				if (cullingMesh == i) continue;
				auto& mesh = _resource->_meshes[i];
				unsigned int stride = sizeof(Vertex);
				unsigned int offset = 0;

				immediateContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
				immediateContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
				immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				immediateContext->OMSetBlendState(Framework::GetBlendState(Framework::BS_ALPHA), nullptr, 0xFFFFFFFF);

				immediateContext->OMSetDepthStencilState(Framework::GetDephtStencilState(Framework::DS_TRUE), 1);

				immediateContext->RSSetState(Framework::GetRasterizerState(Framework::RS_CLOCK_TRUE));


				DirectX::XMStoreFloat4x4
				(
					&m_constantBuffer->data.world,
					DirectX::XMMatrixTranspose
					(
						DirectX::XMLoadFloat4x4(&mesh.globalTransform) *
						DirectX::XMLoadFloat4x4(&_resource->axisSystemTransform) *
						DirectX::XMLoadFloat4x4(&world)
					)
				);


				if (numberOfAnimations > 0)
				{
					size_t numberOfBones = mesh.offsetTransforms.size();
					_ASSERT_EXPR(numberOfBones < MAX_BONES, L"'number_of_bones' exceeds MAX_BONES.");
					for (size_t j = 0; j < numberOfBones; ++j)
					{
						DirectX::XMStoreFloat4x4(&m_constantBuffer->data.boneTransforms[j],
							DirectX::XMMatrixTranspose(
								DirectX::XMLoadFloat4x4(&mesh.offsetTransforms.at(j).transform) *
								DirectX::XMLoadFloat4x4(&transform.at(i).at(j)) 


							)
						);
					}
				}
				else
				{
					for (int i = 0; i < MAX_BONES; i++)
					{
						m_constantBuffer->data.boneTransforms[i] = FLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
					}
				}

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
					immediateContext->DrawIndexed(material.indexCount, material.indexStart, 0);

				}
				m_constantBuffer->Deactivate(immediateContext);

			}

			m_skinnedMeshPS->Deactivate(immediateContext);
			m_skinnedMeshVS->Deactivate(immediateContext);

		}
	}
}
