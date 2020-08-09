#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cassert>
#include "Misc.h"

#define USAGE_DYNAMIC
#define SLOT0 0
#define SLOT1 1
#define SLOT2 2
#define SLOT3 3
#define SLOT4 4
#define SLOT5 5

namespace Source
{
	namespace ConstantBuffer
	{
		template <class T>
		struct ConstantBuffer
		{
			T data;
			ConstantBuffer(ID3D11Device* device)
			{
				_ASSERT_EXPR(sizeof(T) % 16 == 0, L"constant buffer's need to be 16 byte aligned");
				HRESULT hr = S_OK;
				D3D11_BUFFER_DESC bufferDesc = {};
				bufferDesc.ByteWidth = sizeof(T);
				bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				bufferDesc.MiscFlags = 0;
				bufferDesc.StructureByteStride = 0;
#ifdef USAGE_DYNAMIC
				bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
				bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				hr = device->CreateBuffer(&bufferDesc, nullptr, m_constantBuffer.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
#else
				bufferDesc.Usage = D3D11_USAGE_DEFAULT;
				bufferDesc.CPUAccessFlags = 0;
				D3D11_SUBRESOURCE_DATA subresourceData;
				subresourceData.pSysMem = &data;
				subresourceData.SysMemPitch = 0;
				subresourceData.SysMemSlicePitch = 0;
				hr = device->CreateBuffer(&bufferDesc, &subresourceData, m_constantBuffer.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#endif
			}

			virtual ~ConstantBuffer() = default;

			void Activate(ID3D11DeviceContext* immediateContext, int slot, bool vsON = true, bool psON = true)
			{
				HRESULT hr = S_OK;

#ifdef USAGE_DYNAMIC
				D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
				D3D11_MAPPED_SUBRESOURCE mappedBuffer;
				hr = immediateContext->Map(m_constantBuffer.Get(), 0, map, 0, &mappedBuffer);
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
				UINT a = sizeof(T);
				memcpy_s(mappedBuffer.pData, sizeof(T), &data, sizeof(T));
				immediateContext->Unmap(m_constantBuffer.Get(), 0);
#else
				immediateContext->UpdateSubresource(m_constantBuffer.Get(), 0, 0, &data, 0, 0);
#endif
				m_usingSlot = slot;

				ID3D11Buffer* nullBuffer = nullptr;
				vsON ? immediateContext->VSSetConstantBuffers(m_usingSlot, 1, m_constantBuffer.GetAddressOf()) : immediateContext->VSSetConstantBuffers(slot, 1, &nullBuffer);
				psON ? immediateContext->PSSetConstantBuffers(m_usingSlot, 1, m_constantBuffer.GetAddressOf()) : immediateContext->PSSetConstantBuffers(slot, 1, &nullBuffer);
			}

			void Deactivate(ID3D11DeviceContext* immediateContext)
			{
				ID3D11Buffer* nullBuffer = nullptr;
				immediateContext->VSSetConstantBuffers(m_usingSlot, 1, &nullBuffer);
				immediateContext->PSSetConstantBuffers(m_usingSlot, 1, &nullBuffer);
			}

			Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;

		private:
			u_int m_usingSlot = 0;
		};
	}
}