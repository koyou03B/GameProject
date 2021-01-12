#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <string>
#include <vector>
#include "Vector.h"

namespace Source
{
	namespace Shader
	{
		HRESULT CreateVsFromCso(ID3D11Device* device, const char* csoName, ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements);
		HRESULT CreatePsFromCso(ID3D11Device* device, const char* csoName, ID3D11PixelShader** pixelShader);

		void ReleaseAllCachedVertexShaders();
		void ReleaseAllCachedPixelShaders();


		template<class T>
		class VertexShader
		{
		public:
			//if you substitute 0 for 'cso' this creates null vertex shader object
			VertexShader(ID3D11Device* device, const char* cso)
			{
				size_t numElements;
				const D3D11_INPUT_ELEMENT_DESC* inputElementDesc = T::AcquireInputElementDescs(numElements);
				if (cso)
				{
					CreateVsFromCso(device, cso, m_vertexShader.GetAddressOf(), m_inputLayout.GetAddressOf(), const_cast<D3D11_INPUT_ELEMENT_DESC*>(inputElementDesc), static_cast<UINT>(numElements));
				}
			}
			virtual ~VertexShader() = default;


			void Activate(ID3D11DeviceContext* immediateContext)
			{
				immediateContext->IAGetInputLayout(m_defaultInputLayout.ReleaseAndGetAddressOf());
				immediateContext->VSGetShader(m_defaultVertexShader.ReleaseAndGetAddressOf(), 0, 0);

				immediateContext->IASetInputLayout(m_inputLayout.Get());
				immediateContext->VSSetShader(m_vertexShader.Get(), 0, 0);
			}
			void Deactivate(ID3D11DeviceContext* immediateContext)
			{
				immediateContext->IASetInputLayout(m_defaultInputLayout.Get());
				immediateContext->VSSetShader(m_defaultVertexShader.Get(), 0, 0);
			}

		private:
			Microsoft::WRL::ComPtr<ID3D11VertexShader> m_defaultVertexShader;
			Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_defaultInputLayout;
			Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
			Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_inputLayout;
		};

		class PixelShader
		{
		public:
			PixelShader(ID3D11Device* device, const char* cso)
			{
				if (cso)
				{
					Shader::CreatePsFromCso(device, cso, m_pixelShader.GetAddressOf());
				}
			}
			virtual ~PixelShader() = default;

			void Activate(ID3D11DeviceContext* immediateContext)
			{
				immediateContext->PSGetShader(m_defaultShaderObject.ReleaseAndGetAddressOf(), 0, 0);
				immediateContext->PSSetShader(m_pixelShader.Get(), 0, 0);
			}
			void Deactivate(ID3D11DeviceContext* immediateContext)
			{
				immediateContext->PSSetShader(m_defaultShaderObject.Get(), 0, 0);
			}
		private:
			Microsoft::WRL::ComPtr<ID3D11PixelShader> m_defaultShaderObject;
			Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
		};
	}

	namespace Texture
	{
		HRESULT LoadTextureFromFile(ID3D11Device* device, const char* filename, ID3D11ShaderResourceView** shaderResourceView, bool sRGB);
		HRESULT Texture2dDescription(ID3D11ShaderResourceView* shaderResourceView, D3D11_TEXTURE2D_DESC& texture2dDesc);
		HRESULT MakeDummyTexture(ID3D11Device* device, ID3D11ShaderResourceView** shaderResourceView);
		void ReleaseAllCachedTextures();
	}

	namespace Path
	{
		void CombineResourcePath(wchar_t(&combinedResourcePath)[256], const wchar_t* referrerFilename, const wchar_t* referentFilename);
		void ConvertResourcePath(wchar_t(&combinedResourcePath)[256], const wchar_t* referentFilename);
		bool GetFileNames(std::string folderPath, std::vector<std::string>& fileNames);
	}

	namespace Math
	{
		float Clamp(float a, float x, float b);
		float Max(float a, float b);

		void WorldToScreen(VECTOR3F* screenPosition, const VECTOR3F& worldPosition);

		void ScreenToWorld(VECTOR3F* worldPosition, const VECTOR3F& screenPosition);

	}

	namespace RayTriangle
	{
		inline bool PointInAABB(const float* p, const float* min, const float* max)
		{
			if (p[0] >= min[0] && p[0] <= max[0])
			{
				if(p[1] >= min[1] && p[1] <= max[1])
				{
					if (p[2] >= min[2] && p[2] <= max[2])
						return true;
				}
			}
			return (p[0] >= min[0] && p[0] <= max[0]) &&
				(p[1] >= min[1] && p[1] <= max[1]) &&
				(p[2] >= min[2] && p[2] <= max[2]);
		}

		int RayTrianglesIntersection
		(
			const float* positions,// model space
			const u_int stride,	   // byte size of one vertex
			const u_int* indices,
			const size_t indicesCount,
			const VECTOR3F& rayPosition, // model space
			const VECTOR3F& rayDirection,  // model space 
			float rayLengthLimit,   // model space 
			VECTOR3F* intersection // model space
		);

		int RayTrianglesIntersectionDownward
		(
			const float* positions,	// model space
			const u_int stride,  // byte size of one vertex
			const u_int* indices,
			const size_t indicesCount,
			const VECTOR3F& rayPosition, // model space
			float rayLengthLimit, 		 // model space 
			VECTOR3F* intersection		// model space
		);
	}
}