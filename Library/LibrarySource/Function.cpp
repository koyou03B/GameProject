#include <wrl.h>
#include <memory>
#include <map>
#include <string>
#include <locale>
#include <codecvt>
#include <Shlwapi.h>
#include "WICTextureLoader.h"
#include "Function.h"
#include "Shlwapi.h"
#include "misc.h"
#include "Framework.h"
#include "Camera.h"


namespace Source
{
	namespace Shader
	{
		struct SetOfVertexShaderAndInputLayout
		{
			SetOfVertexShaderAndInputLayout(ID3D11VertexShader* setVertexShader, ID3D11InputLayout* setInputLayout) : setVertexShader(setVertexShader), setInputLayout(setInputLayout) {}
			Microsoft::WRL::ComPtr<ID3D11VertexShader> setVertexShader;
			Microsoft::WRL::ComPtr<ID3D11InputLayout>  setInputLayout;
		};

		static std::map<std::string, SetOfVertexShaderAndInputLayout> g_cachedVertexShaders;
		HRESULT CreateVsFromCso(ID3D11Device* device, const char* csoName, ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements)
		{
			std::map<std::string, SetOfVertexShaderAndInputLayout>::iterator it = g_cachedVertexShaders.find(csoName);;

			//decltype(cache)::iterator it = cache.find(cso_name);
			if (it != g_cachedVertexShaders.end())
			{
				*vertexShader = it->second.setVertexShader.Get();
				(*vertexShader)->AddRef();
				if (inputLayout)
				{
					*inputLayout = it->second.setInputLayout.Get();
					(*inputLayout)->AddRef();
				}
				return S_OK;
			}

			FILE* fp = nullptr;
			fopen_s(&fp, csoName, "rb");


			fseek(fp, 0, SEEK_END);
			long csoSZ = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			std::unique_ptr<unsigned char[]> csoData = std::make_unique<unsigned char[]>(csoSZ);
			fread(csoData.get(), csoSZ, 1, fp);
			fclose(fp);

			HRESULT hr = device->CreateVertexShader(csoData.get(), csoSZ, nullptr, vertexShader);
			if (FAILED(hr))
				assert(!"Could not Create a CreateVertexShader");
			if (inputLayout)
			{
				hr = device->CreateInputLayout(inputElementDesc, numElements, csoData.get(), csoSZ, inputLayout);
				if (FAILED(hr))
					assert(!"Could not Create a InputLayout");
			}
			g_cachedVertexShaders.insert(std::make_pair(csoName, SetOfVertexShaderAndInputLayout(*vertexShader, inputLayout ? *inputLayout : 0)));

			return hr;
		}

		static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>> g_cachedPixelShaders;
		HRESULT CreatePsFromCso(ID3D11Device* device, const char* csoName, ID3D11PixelShader** pixelShader)
		{
			std::map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>>::iterator it = g_cachedPixelShaders.find(csoName);
			if (it != g_cachedPixelShaders.end())
			{
				//it->second.Attach(*pixel_shader);
				*pixelShader = it->second.Get();
				(*pixelShader)->AddRef();
				return S_OK;
			}

			FILE* fp = nullptr;
			fopen_s(&fp, csoName, "rb");


			fseek(fp, 0, SEEK_END);
			long csoSZ = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			std::unique_ptr<unsigned char[]> csoData = std::make_unique<unsigned char[]>(csoSZ);
			fread(csoData.get(), csoSZ, 1, fp);
			fclose(fp);

			HRESULT hr = device->CreatePixelShader(csoData.get(), csoSZ, nullptr, pixelShader);
			if (FAILED(hr))
				assert(!"Could not Create a CreatePixelShader");

			g_cachedPixelShaders.insert(std::make_pair(csoName, *pixelShader));

			return hr;
		}
	
		void ReleaseAllCachedVertexShaders()
		{
			g_cachedVertexShaders.clear();
		}
		void ReleaseAllCachedPixelShaders()
		{
			g_cachedPixelShaders.clear();
		}
	}

	namespace Texture
	{
		static std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> g_cachedTextures;
		HRESULT LoadTextureFromFile(ID3D11Device* device, const char* filename, ID3D11ShaderResourceView** shaderResourceView, bool sRGB)
		{
			HRESULT	hr = S_OK;

			wchar_t filenameW[256];
			mbstowcs_s(0, filenameW, filename, strlen(filename) + 1);

			std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>::iterator it = g_cachedTextures.find(filenameW);

			if (it != g_cachedTextures.end())
			{
				//it->second.Attach(*pixel_shader);
				*shaderResourceView = it->second.Get();
				(*shaderResourceView)->AddRef();
				return S_OK;
			}


			std::wstring extension = PathFindExtensionW(filenameW);
			std::transform(extension.begin(), extension.end(), extension.begin(), ::towlower);
			ID3D11Resource* resource = nullptr;
			if (extension == L".png" || extension == L".jpeg" || extension == L".jpg" || extension == L".JPG" || extension == L".bmp")
			{
				HRESULT hr = DirectX::CreateWICTextureFromFile(device, filenameW, &resource, shaderResourceView);
				if (FAILED(hr))
					assert(!"Unable to load texture from file");

			}
			else
			{
				assert(!"File format not found");
			}

			if (FAILED(hr))
			{
				assert(!"Could not Create a ShaderResourceView from a texture");
			}

			g_cachedTextures.insert(std::make_pair(filenameW, *shaderResourceView));
			resource->Release();
			return hr;

		}

		HRESULT Texture2dDescription(ID3D11ShaderResourceView* shaderResourceView, D3D11_TEXTURE2D_DESC& texture2dDesc)
		{
			HRESULT hr = S_OK;
			Microsoft::WRL::ComPtr<ID3D11Resource> resource;
			shaderResourceView->GetResource(resource.GetAddressOf());

			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
			hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
			if (FAILED(hr))
				assert(!"Could not Create a ShaderResourceView from a dummy texture");

			texture2d->GetDesc(&texture2dDesc);
			return hr;
		}

		HRESULT MakeDummyTexture(ID3D11Device* device, ID3D11ShaderResourceView** shaderResourceView)
		{
			HRESULT hr = S_OK;

			D3D11_TEXTURE2D_DESC texture2dDesc = {};
			texture2dDesc.Width = 1;
			texture2dDesc.Height = 1;
			texture2dDesc.MipLevels = 1;
			texture2dDesc.ArraySize = 1;
			texture2dDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			texture2dDesc.SampleDesc.Count = 1;
			texture2dDesc.SampleDesc.Quality = 0;
			texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
			texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			texture2dDesc.CPUAccessFlags = 0;
			texture2dDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA subresourceData = {};
			u_int color = 0xFFFFFFFF;
			subresourceData.pSysMem = &color;
			subresourceData.SysMemPitch = 4;
			subresourceData.SysMemSlicePitch = 4;

			ID3D11Texture2D* texture2d;
			hr = device->CreateTexture2D(&texture2dDesc, &subresourceData, &texture2d);
			if (FAILED(hr))
				assert(!"Could not Create a CreateTexture2D");

			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
			shaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;

			hr = device->CreateShaderResourceView(texture2d, &shaderResourceViewDesc, shaderResourceView);
			if (FAILED(hr))
				assert(!"Could not Create a CreateShaderResourceView");

			texture2d->Release();

			return hr;
		}

		void ReleaseAllCachedTextures()
		{
			g_cachedTextures.clear();
		}
	}

	namespace Path
	{
		void CombineResourcePath(wchar_t(&combinedResourcePath)[256], const wchar_t* referrerFilename, const wchar_t* referentFilename)
		{
			std::wstring referent_str(referentFilename);
			if (referent_str.front() == L'.') return;

			std::wstring str(referrerFilename);
			str = str.substr(0, str.find_last_of(L"/"));
			str += L"/";
			str += referentFilename;
			wcscpy_s(combinedResourcePath, str.c_str());
		}
		void ConvertResourcePath(wchar_t(&combinedResourcePath)[256], const wchar_t* referentFilename)
		{
			std::wstring str(referentFilename);
			for (auto& s : str)
			{
				std::wstring::size_type pos = str.find(L"\\");
				if (pos != std::wstring::npos)
				{
					str.replace(pos, 1, 1, L'/');
				}
			}

			wcscpy_s(combinedResourcePath, str.c_str());
		}

		bool GetFileNames(std::string folderPath, std::vector<std::string>& fileNames)
		{
			//HANDLE hFind;
			//WIN32_FIND_DATA win32fd;
			//std::string searchName = folderPath + "\\*";

			//hFind = FindFirstFile(searchName.c_str(), &win32fd);

			//if (hFind == INVALID_HANDLE_VALUE) 
			//{
			//	return false;
			//}

			///* 指定のディレクトリ以下のファイル名をファイルがなくなるまで取得する */
			//do {
			//	if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			//		/* ディレクトリの場合は何もしない */
			//		//printf("directory\n");
			//	}
			//	else {
			//		/* ファイルが見つかったらVector配列に保存する */
			//		fileNames.push_back(win32fd.cFileName);
			//	}
			//} while (FindNextFile(hFind, &win32fd));

			//FindClose(hFind);

			return true;
		}
	}

	namespace Math
	{
		float Clamp(float a, float x, float b)
		{
			const float min = x < b ? x : b;
			return min < a ? a : min;
		}
		float Max(float a, float b)
		{
			return (b < a) ? a : b;
		}

		void WorldToScreen(VECTOR3F* screenPosition, const VECTOR3F& worldPosition)
		{			
			float viewportX = 0.0f;
			float viewportY = 0.0f;
			float viewportW = static_cast<float>(Framework::GetInstance().SCREEN_WIDTH);
			float viewportH = static_cast<float>(Framework::GetInstance().SCREEN_HEIGHT);
			float viewportMinZ = 0.0f;
			float viewportMaxZ = 1.0f;

			DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&Source::CameraControlle::CameraManager().GetInstance()->GetView());
			DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&Source::CameraControlle::CameraManager().GetInstance()->GetProjection());
			DirectX::XMMATRIX W = DirectX::XMMatrixIdentity();//単位行列
		
			DirectX::XMMATRIX WVP = W * V * P;
			DirectX::XMVECTOR NDCPosition = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&worldPosition), WVP);
			DirectX::XMFLOAT3 ndcPosition;
			DirectX::XMStoreFloat3(&ndcPosition, NDCPosition);

			screenPosition->x = (ndcPosition.x + 1.0f) * viewportW / 2.0f;
			screenPosition->y = (ndcPosition.y - 1.0f) * viewportH / -2.0f;
			screenPosition->z = viewportMinZ + ndcPosition.z * (viewportMaxZ - viewportMinZ);

		}

		void ScreenToWorld(VECTOR3F* worldPosition, const VECTOR3F& screenPosition)
		{

			float viewportX = 0.0f;
			float viewportY = 0.0f;
			float viewportW = static_cast<float>(Framework::GetInstance().SCREEN_WIDTH);
			float viewportH = static_cast<float>(Framework::GetInstance().SCREEN_HEIGHT);
			float viewportMinZ = 0.0f;
			float viewportMaxZ = 1.0f;

			DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&Source::CameraControlle::CameraManager().GetInstance()->GetView());
			DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&Source::CameraControlle::CameraManager().GetInstance()->GetProjection());
			DirectX::XMMATRIX W = DirectX::XMMatrixIdentity();//単位行列


			DirectX::XMVECTOR ndcPosition = DirectX::XMVectorSet
			(
				2.0f * screenPosition.x / viewportW - 1.0f,
				1.0f - 2.0f * screenPosition.y / viewportH,
				screenPosition.z / viewportMaxZ,screenPosition.z
			);

			// NDC座標からワールド座標へ変換
			DirectX::XMMATRIX WVP = W * V * P;
			DirectX::XMMATRIX IWVP = DirectX::XMMatrixInverse(NULL, WVP);
			DirectX::XMVECTOR WPos = DirectX::XMVector3TransformCoord(ndcPosition, IWVP);
			DirectX::XMStoreFloat3(worldPosition, WPos);
		}


	}

	namespace RayTriangle
	{

		int RayTrianglesIntersection(const float* positions, const u_int stride, const u_int* indices, const size_t indicesCount,
			const VECTOR3F& rayPosition, const VECTOR3F& rayDirection, float rayLengthLimit, VECTOR3F* intersection)
		{
			const int c0 = 0;
			const int c1 = 1;
			const int c2 = 2;

			int numberOfIntersections = 0;
			int indexOfIntersectedTriangle = -1;
			float shortestDistance = FLT_MAX;
			DirectX::XMVECTOR X;
			DirectX::XMVECTOR P = DirectX::XMVectorSet(rayPosition.x, rayPosition.y, rayPosition.z, 1);
			DirectX::XMVECTOR D = DirectX::XMVector3Normalize(DirectX::XMVectorSet(rayDirection.x, rayDirection.y, rayDirection.z, 0));

			const uint8_t* p = reinterpret_cast<const uint8_t*>(positions);
			const size_t numberOfTriangles = indicesCount / 3;
			for (size_t indexOfTriangle = 0; indexOfTriangle < numberOfTriangles; ++indexOfTriangle)
			{
				VECTOR3F p0 = VECTOR3F(
					(reinterpret_cast<const float*>(p + indices[indexOfTriangle * 3 + c0] * stride))[0],
					(reinterpret_cast<const float*>(p + indices[indexOfTriangle * 3 + c0] * stride))[1],
					(reinterpret_cast<const float*>(p + indices[indexOfTriangle * 3 + c0] * stride))[2]);
				VECTOR3F p1 = VECTOR3F(
					(reinterpret_cast<const float*>(p + indices[indexOfTriangle * 3 + c1] * stride))[0],
					(reinterpret_cast<const float*>(p + indices[indexOfTriangle * 3 + c1] * stride))[1],
					(reinterpret_cast<const float*>(p + indices[indexOfTriangle * 3 + c1] * stride))[2]);
				VECTOR3F p2 = VECTOR3F(
					(reinterpret_cast<const float*>(p + indices[indexOfTriangle * 3 + c2] * stride))[0],
					(reinterpret_cast<const float*>(p + indices[indexOfTriangle * 3 + c2] * stride))[1],
					(reinterpret_cast<const float*>(p + indices[indexOfTriangle * 3 + c2] * stride))[2]);

				DirectX::XMVECTOR A, B, C;
				A = DirectX::XMLoadFloat3(&p0);
				B = DirectX::XMLoadFloat3(&p1);
				C = DirectX::XMLoadFloat3(&p2);

				DirectX::XMVECTOR Q;
				DirectX::XMVECTOR N = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(B, A), DirectX::XMVectorSubtract(C, A)));
				float d = DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(N, A), 0);

				float denominator = DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(N, D), 0);
				if (denominator < 0)
				{
					float numerator = d - DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(N, P), 0);
					float t = numerator / denominator;

					if (t > 0 && t < rayLengthLimit)
					{
						Q = DirectX::XMVectorAdd(P, DirectX::XMVectorScale(D, t));

						static const DirectX::XMVECTOR Z = DirectX::XMVectorSet(0, 0, 0, 1);
						static const DirectX::XMVECTOR E = DirectX::XMVectorSet(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON, 1);

						DirectX::XMVECTOR QA = DirectX::XMVectorSubtract(A, Q);
						DirectX::XMVECTOR QB = DirectX::XMVectorSubtract(B, Q);
						DirectX::XMVECTOR QC = DirectX::XMVectorSubtract(C, Q);

						DirectX::XMVECTOR U = DirectX::XMVector3Cross(QB, QC);
						if (DirectX::XMVector3NearEqual(U, Z, E))
						{
							U = Z;
						}
						DirectX::XMVECTOR V = DirectX::XMVector3Cross(QC, QA);
						if (DirectX::XMVector3NearEqual(V, Z, E))
						{
							V = Z;
						}
						if (DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(U, V), 0) < 0)
						{
							continue;
						}

						DirectX::XMVECTOR W = DirectX::XMVector3Cross(QA, QB);
						if (DirectX::XMVector3NearEqual(W, Z, E))
						{
							W = Z;
						}
						if (DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(U, W), 0) < 0)
						{
							continue;
						}
						if (DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(V, W), 0) < 0)
						{
							continue;
						}

						// Otherwise Q must be in (or on) the triangle
						if (t < shortestDistance)
						{
							shortestDistance = t;
							indexOfIntersectedTriangle = static_cast<int>(indexOfTriangle);
							X = Q;

						}
						numberOfIntersections++;
					}
				}
			}

			if (numberOfIntersections > 0)
			{
				XMStoreFloat3(intersection, X);
			}
			return indexOfIntersectedTriangle;
		}

		int RayTrianglesIntersectionDownward(const float* positions, const u_int stride, const u_int* indices, const size_t indicesCount,
			const VECTOR3F& rayPosition, float rayLengthLimit, VECTOR3F* intersection)
		{
			const int c0 = 0;
			const int c1 = 1;
			const int c2 = 2;

			int numberOfIntersections = 0;
			int indexOfIntersectedTriangle = -1;
			float shortestDistance = FLT_MAX;
			DirectX::XMVECTOR X;
			DirectX::XMVECTOR P = DirectX::XMVectorSet(rayPosition.x, rayPosition.y, rayPosition.z, 1);
			DirectX::XMVECTOR D = DirectX::XMVector3Normalize(DirectX::XMVectorSet(0, -1, 0, 0));

			const uint8_t* p = reinterpret_cast<const uint8_t*>(positions);
			const size_t numberOfTriangles = indicesCount / 3;
			for (size_t indexOfTriangle = 0; indexOfTriangle < numberOfTriangles; ++indexOfTriangle)
			{
				VECTOR3F p0 = VECTOR3F(
					(reinterpret_cast<const float*>(p + indices[indexOfTriangle * 3 + c0] * stride))[0],
					(reinterpret_cast<const float*>(p + indices[indexOfTriangle * 3 + c0] * stride))[1],
					(reinterpret_cast<const float*>(p + indices[indexOfTriangle * 3 + c0] * stride))[2]);
				VECTOR3F p1 = VECTOR3F(
					(reinterpret_cast<const float*>(p + indices[indexOfTriangle * 3 + c1] * stride))[0],
					(reinterpret_cast<const float*>(p + indices[indexOfTriangle * 3 + c1] * stride))[1],
					(reinterpret_cast<const float*>(p + indices[indexOfTriangle * 3 + c1] * stride))[2]);
				VECTOR3F p2 = VECTOR3F(
					(reinterpret_cast<const float*>(p + indices[indexOfTriangle * 3 + c2] * stride))[0],
					(reinterpret_cast<const float*>(p + indices[indexOfTriangle * 3 + c2] * stride))[1],
					(reinterpret_cast<const float*>(p + indices[indexOfTriangle * 3 + c2] * stride))[2]);

				DirectX::XMVECTOR A, B, C;
				A = DirectX::XMLoadFloat3(&p0);
				B = DirectX::XMLoadFloat3(&p1);
				C = DirectX::XMLoadFloat3(&p2);

				DirectX::XMVECTOR Q;
				DirectX::XMVECTOR N = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(B, A), DirectX::XMVectorSubtract(C, A)));
				float d = DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(N, A), 0);

				float denominator = DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(N, D), 0);
				if (denominator < 0)
				{
					float numerator = d - DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(N, P), 0);
					float t = numerator / denominator;

					if (t > 0 && t < rayLengthLimit)
					{
						Q = DirectX::XMVectorAdd(P, DirectX::XMVectorScale(D, t));

						static const DirectX::XMVECTOR Z = DirectX::XMVectorSet(0, 0, 0, 1);
						static const DirectX::XMVECTOR E = DirectX::XMVectorSet(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON, 1);

						DirectX::XMVECTOR QA = DirectX::XMVectorSubtract(A, Q);
						DirectX::XMVECTOR QB = DirectX::XMVectorSubtract(B, Q);
						DirectX::XMVECTOR QC = DirectX::XMVectorSubtract(C, Q);

						DirectX::XMVECTOR U = DirectX::XMVector3Cross(QB, QC);
						if (DirectX::XMVector3NearEqual(U, Z, E))
						{
							U = Z;
						}
						DirectX::XMVECTOR V = DirectX::XMVector3Cross(QC, QA);
						if (DirectX::XMVector3NearEqual(V, Z, E))
						{
							V = Z;
						}
						if (DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(U, V), 0) < 0)
						{
							continue;
						}

						DirectX::XMVECTOR W = DirectX::XMVector3Cross(QA, QB);
						if (DirectX::XMVector3NearEqual(W, Z, E))
						{
							W = Z;
						}
						if (DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(U, W), 0) < 0)
						{
							continue;
						}
						if (DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(V, W), 0) < 0)
						{
							continue;
						}

						if (t < shortestDistance)
						{
							shortestDistance = t;
							indexOfIntersectedTriangle = static_cast<int>(indexOfTriangle);
							X = Q;

						}
						numberOfIntersections++;
					}
				}
			}

			if (numberOfIntersections > 0)
			{
				XMStoreFloat3(intersection, X);
			}
			return indexOfIntersectedTriangle;
		}
	}
}

