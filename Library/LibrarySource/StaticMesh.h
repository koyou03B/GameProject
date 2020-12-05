#pragma once

#include <memory>
#include <vector>
#include "Misc.h"
#include "Vector.h"
#include "ConstantBuffer.h"
#include "ModelResource.h"
#include "Framework.h"
#include "Function.h"
#include "InstanceData.h"

namespace Source
{
	namespace StaticMesh
	{
		class StaticMesh
		{
		public:
			StaticMesh(ID3D11Device* device, const char* filename);
			~StaticMesh() = default;

			void  Render(ID3D11DeviceContext * immediateContext, std::vector<Source::InstanceData::InstanceData> instanceData, const VECTOR4F & materialColor);

			void  Render(ID3D11DeviceContext* immediateContext, FLOAT4X4 world, const VECTOR4F& materialColor);


			int RayPick(
				const DirectX::XMFLOAT3 & startPosition,			//Start coordinates for flying a ray
				const DirectX::XMFLOAT3 & endPosition,			//End coordinates for flying a ray
				DirectX::XMFLOAT3 * outPosition,					//Coordinates for Ray's hit
				DirectX::XMFLOAT3 * outNormal,					//The normal of the plane the ray hit
				float* outLength,								//Distance to the surface where the ray hit.
				const std::vector<std::string>& searchMeshName);

			const char* RayTrianglesIntersection
			(
				const VECTOR3F & rayPosition, 	 // world space
				const VECTOR3F & rayDirection,	  // world space
				const FLOAT4X4 & worldTransform, // model to world space
				VECTOR3F * intersection // world space
				);

			const char* RayTrianglesIntersectionDownward
			(
				const VECTOR3F & rayPosition, // world space
				const FLOAT4X4 & worldTransform, // model to world space
				VECTOR3F * intersection // world space
				);
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
				std::vector<std::string> boneName;
				//std::vector<std::string> animationName;
			} _modelConfig;

			struct Vertex
			{
				VECTOR3F position;
				VECTOR3F normal;
				VECTOR2F texcoord;

				Vertex(const ModelResource::Vertex& rhs)
				{
					position = rhs.position;
					normal = rhs.normal;
					texcoord = rhs.texcoord;
				}
				Vertex() = default;

				//---------------------------
				// Getting the ELEMENT_DESC
				//---------------------------
				static const D3D11_INPUT_ELEMENT_DESC* AcquireInputElementDescs(size_t& numElements)
				{
					static const D3D11_INPUT_ELEMENT_DESC input_element_descs[] =
					{
						{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT   , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
						{ "NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT   , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
						{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT      , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

						{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
						{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
						{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
						{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },

					};
					//-----------------------------------
					// Getting the number of elements
					//-----------------------------------
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
						texcoord
						);
				}
			};
			std::unique_ptr<ModelResource::Substance<Vertex>> _resource;
		private:


			struct Instance
			{
				FLOAT4X4 world;
			};

			struct ShaderConstants
			{
				VECTOR4F materialColor = VECTOR4F(1, 1, 1, 1);
			};

			const size_t MAX_INSTANCES = 1000;
			Microsoft::WRL::ComPtr<ID3D11Buffer>       m_instanceBuffer;
			std::unique_ptr<Source::Shader::VertexShader<StaticMesh::Vertex>> m_staticMeshVS;
			std::unique_ptr<Source::Shader::PixelShader> m_staticMeshPS;
			std::unique_ptr<Source::ConstantBuffer::ConstantBuffer<ShaderConstants>> m_constantBuffer;

		};
	}
}
