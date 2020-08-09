#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include "Vector.h"
#include "Constants.h"
#include "ConstantBuffer.h"
#include "Function.h"
#include "InstanceData.h"

namespace Source
{
	namespace GeometricPrimitive
	{
		class GeometricPrimitive
		{
		public:
			GeometricPrimitive(ID3D11Device* device);
			virtual ~GeometricPrimitive() = default;

			void Begin(ID3D11DeviceContext * immediateContext, bool wireframe = false);
			void Render(ID3D11DeviceContext * immediateContext,
				const FLOAT4X4 & view,
				const FLOAT4X4 & projection,
				const std::vector<Source::InstanceData::InstanceData> & instanceData,
				const VECTOR4F & materialColor);
			void End(ID3D11DeviceContext * immediateContext);

		protected:
			struct Vertex
			{
				VECTOR3F position;
				VECTOR2F texcoord;
				VECTOR3F normal;

				static const D3D11_INPUT_ELEMENT_DESC* AcquireInputElementDescs(size_t& numElements)
				{
					static const D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
					{
						{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
						{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
						{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

						{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },

						{ "WVP",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
						{ "WVP",   1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
						{ "WVP",   2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
						{ "WVP",   3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },

						{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
						{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
						{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
						{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
					};
					//---------------
					// óvëfêîÇÃéÊìæ
					//---------------
					numElements = _countof(inputElementDesc);
					return inputElementDesc;
				}
			};
			void CreateBuffers(ID3D11Device* device, Vertex* vertices, int numVertices, u_int* indices, int numIndices);

		private:

			//struct ShaderConstants
			//{
			//	VECTOR4F materialColor;
			//};

			struct Instance
			{
				VECTOR4F materialColor;
				FLOAT4X4 worldViewProjection;
				FLOAT4X4 world;
			};

			Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_instanceBuffer;

			std::unique_ptr<Source::Shader::VertexShader<Vertex>> m_geometricPrimitiveVS;
			std::unique_ptr<Source::Shader::PixelShader> m_geometricPrimitivePS;
		//	std::unique_ptr<Source::ConstantBuffer::ConstantBuffer<ShaderConstants>> m_constantBuffer;

			const size_t MAX_INSTANCES = 256;

		};

		class GeometricCube : public GeometricPrimitive
		{
		public:
			GeometricCube(ID3D11Device* device);
		};

		class GeometricCylinder : public GeometricPrimitive
		{
		public:
			GeometricCylinder(ID3D11Device* device, u_int slices = 16);
		};

		class GeometricSphere : public GeometricPrimitive
		{
		public:
			GeometricSphere(ID3D11Device* device, u_int slices = 16, u_int stacks = 16);
		};


		class GeometricCapsule : public GeometricPrimitive
		{
		public:
			GeometricCapsule(ID3D11Device* device, u_int slices = 35, u_int stacks = 35);
		};

	}
}