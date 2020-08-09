#pragma once

#include <d3d11.h>
#include <wrl.h>
#include "Misc.h"

namespace Source
{
	namespace FrameBuffer
	{
		class FrameBuffer
		{
		public:
			FrameBuffer(ID3D11Device* device, int width, int height,
				bool enableMsaa = false, int subsamples = 1, DXGI_FORMAT renderTargetTexture2dFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
				DXGI_FORMAT depthStencilTexture2dFormat = DXGI_FORMAT_R24G8_TYPELESS,
				bool needRenderTargetShaderResourceView = true,
				bool needDepthStencilShaderResourceView = true,
				bool generateMips = false);

			virtual ~FrameBuffer() = default;

			void Clear(ID3D11DeviceContext* immediateContext, float r = 0.f, float g = 0.f, float b = 0.f, float a = 1.f, u_int clearFlags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, float depth = 1.f, unsigned char stencil = 0);
			void Activate(ID3D11DeviceContext* immediateContext);
			void Deactivate(ID3D11DeviceContext* immediateContext);

			const Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRenderTargetView() const { return m_renderTargetView; }
			const Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDepthStencilView() const { return m_depthStencilView; }
			const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetRenderTargetShaderResourceView() const { return m_renderTargetShaderResourceView; }
			const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetDepthStencilShaderResourceView() const { return m_depthStencilShaderResourceView; }

		private:
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_renderTargetShaderResourceView;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_depthStencilShaderResourceView;

			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_defaultRenderTargetView;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_defaultDepthStencilView;

			u_int m_numberOfStoredViewports;
			D3D11_VIEWPORT m_defaultViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
			D3D11_VIEWPORT m_viewport;

		};
	}
}