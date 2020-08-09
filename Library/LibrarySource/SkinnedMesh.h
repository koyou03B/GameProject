#pragma once

#include <memory>
#include <vector>
#include <string>
#include "Misc.h"
#include "Vector.h"
#include "ConstantBuffer.h"
#include "ModelResource.h"
#include "Framework.h"
#include "Function.h"


namespace Source
{
	namespace SkinnedMesh
	{
		class SkinnedMesh
		{
		public:
			struct ShaderRabel
			{
				bool diffuse = true;
				bool ambient = false;
				bool specular = true;
				bool bump = false;
				bool normalMap = false;
			} _shaderON;

			struct ModelConfig
			{
				//std::vector<std::string> boneName;
				std::vector<std::string> animationName;
			} _modelConfig;

			struct Vertex
			{
				VECTOR3F position;
				VECTOR3F normal;
				VECTOR2F texcoord;
				float    boneWeights[MAX_BONE_INFLUENCES] = { 1.0f,.0f,.0f,.0f };
				int      boneIndices[MAX_BONE_INFLUENCES] = {};

				Vertex(const ModelResource::Vertex& rhs)
				{
					position = rhs.position;
					normal = rhs.normal;
					texcoord = rhs.texcoord;
					memcpy(boneWeights, rhs.boneWeights, sizeof(boneWeights));
					memcpy(boneIndices, rhs.boneIndices, sizeof(boneIndices));
				}
				Vertex() = default;

				//---------------------
				// ELEMENT_DESCÇÃéÊìæ
				//---------------------
				static const D3D11_INPUT_ELEMENT_DESC* AcquireInputElementDescs(size_t& numElements)
				{
					static const D3D11_INPUT_ELEMENT_DESC input_element_descs[] =
					{
						{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT   , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
						{ "NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT   , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
						{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT      , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
						{ "WEIGHTS" , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
						{ "BONES"   , 0, DXGI_FORMAT_R32G32B32A32_UINT , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					};
					//---------------
					// óvëfêîÇÃéÊìæ
					//---------------
					numElements = _countof(input_element_descs);
					return input_element_descs;
				}

				template<class T>
				void serialize(T& archive)
				{
					archive
					(
						position,
						normal,
						texcoord,
						boneWeights,
						boneIndices
						);
				}
			};
			std::unique_ptr<ModelResource::Substance<Vertex>> _resource;

		public:
			SkinnedMesh(ID3D11Device* device, const char* filename, int animationSamplingRate = 0);
			~SkinnedMesh() = default;

			void Render(ID3D11DeviceContext * immediateContext,
				const FLOAT4X4 & world,
				VECTOR4F & materialColor,
				size_t animationClip, size_t animationFrame);

			void Render(ID3D11DeviceContext * immediateContext,
				const FLOAT4X4 & world,
				VECTOR4F & materialColor,
				std::vector<std::vector<FLOAT4X4>> & transform,int cullingMesh = -1);


		private:
			struct ShaderConstants
			{
				FLOAT4X4 world;
				FLOAT4X4 boneTransforms[MAX_BONES] = { { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
				VECTOR4F materialColor;
			};

			std::unique_ptr<Source::Shader::VertexShader<SkinnedMesh::Vertex>> m_skinnedMeshVS;
			std::unique_ptr<Source::Shader::PixelShader> m_skinnedMeshPS;
			std::unique_ptr<Source::ConstantBuffer::ConstantBuffer<ShaderConstants>> m_constantBuffer;
		
		};
	}
}
