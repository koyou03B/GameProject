#include "framebuffer.h"

namespace Source
{
	namespace FrameBuffer
	{
		FrameBuffer::FrameBuffer(ID3D11Device* device, int width, int height, bool enableMsaa, int subsamples,
			DXGI_FORMAT renderTargetTexture2dFormat, DXGI_FORMAT depthStencilTexture2dFormat,
			bool needRenderTargetShaderResourceView, bool needDepthStencilShaderResourceView, bool generateMips)
		{
			HRESULT hr = S_OK;

			UINT msaaQualityLevel;
			UINT sampleCount = subsamples;
			hr = device->CheckMultisampleQualityLevels(renderTargetTexture2dFormat, sampleCount, &msaaQualityLevel);
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

			if (renderTargetTexture2dFormat != DXGI_FORMAT_UNKNOWN)
			{
				D3D11_TEXTURE2D_DESC texture2dDesc = {};
				texture2dDesc.Width = width;
				texture2dDesc.Height = height;
				texture2dDesc.MipLevels = generateMips ? 0 : 1;
				texture2dDesc.ArraySize = 1;
				texture2dDesc.Format = renderTargetTexture2dFormat;
				texture2dDesc.SampleDesc.Count = enableMsaa ? sampleCount : 1;
				texture2dDesc.SampleDesc.Quality = enableMsaa ? msaaQualityLevel - 1 : 0;
				texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
				texture2dDesc.BindFlags = needRenderTargetShaderResourceView ? D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_RENDER_TARGET;
				texture2dDesc.CPUAccessFlags = 0;
				texture2dDesc.MiscFlags = generateMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

				Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
				hr = device->CreateTexture2D(&texture2dDesc, nullptr, texture2d.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
				renderTargetViewDesc.Format = texture2dDesc.Format;
				renderTargetViewDesc.ViewDimension = enableMsaa ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
				hr = device->CreateRenderTargetView(texture2d.Get(), &renderTargetViewDesc, m_renderTargetView.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				if (needRenderTargetShaderResourceView)
				{
					D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
					shaderResourceViewDesc.Format = texture2dDesc.Format;
					shaderResourceViewDesc.ViewDimension = enableMsaa ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
					shaderResourceViewDesc.Texture2D.MipLevels = generateMips ? -1 : 1;
					hr = device->CreateShaderResourceView(texture2d.Get(), &shaderResourceViewDesc, m_renderTargetShaderResourceView.GetAddressOf());
					_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
				}
			}

			if (depthStencilTexture2dFormat != DXGI_FORMAT_UNKNOWN)
			{
				const DXGI_FORMAT combinationsOfDepthStencilFormats[3][3] =
				{
					{ DXGI_FORMAT_R24G8_TYPELESS, DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_R24_UNORM_X8_TYPELESS },
					{ DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_D32_FLOAT, DXGI_FORMAT_R32_FLOAT },
					{ DXGI_FORMAT_R16_TYPELESS, DXGI_FORMAT_D16_UNORM, DXGI_FORMAT_R16_UNORM },
				};

				int depthStencilTexture2dFormatIndex = 0;
				switch (depthStencilTexture2dFormat)
				{
				case DXGI_FORMAT_R24G8_TYPELESS:
					depthStencilTexture2dFormatIndex = 0;
					break;
				case DXGI_FORMAT_R32_TYPELESS:
					depthStencilTexture2dFormatIndex = 1;
					break;
				case DXGI_FORMAT_R16_TYPELESS:
					depthStencilTexture2dFormatIndex = 2;
					break;
				}

				D3D11_TEXTURE2D_DESC texture2dDesc = {};
				texture2dDesc.Width = width;
				texture2dDesc.Height = height;
				texture2dDesc.MipLevels = 1;
				texture2dDesc.ArraySize = 1;
				texture2dDesc.Format = combinationsOfDepthStencilFormats[depthStencilTexture2dFormatIndex][0];
				texture2dDesc.SampleDesc.Count = enableMsaa ? sampleCount : 1;
				texture2dDesc.SampleDesc.Quality = enableMsaa ? msaaQualityLevel - 1 : 0;
				texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
				texture2dDesc.BindFlags = needRenderTargetShaderResourceView ? D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_DEPTH_STENCIL;
				texture2dDesc.CPUAccessFlags = 0;
				texture2dDesc.MiscFlags = 0;

				Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
				hr = device->CreateTexture2D(&texture2dDesc, nullptr, texture2d.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
				depthStencilViewDesc.Format = combinationsOfDepthStencilFormats[depthStencilTexture2dFormatIndex][1];
				depthStencilViewDesc.ViewDimension = enableMsaa ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
				depthStencilViewDesc.Flags = 0;
				hr = device->CreateDepthStencilView(texture2d.Get(), &depthStencilViewDesc, m_depthStencilView.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				if (needDepthStencilShaderResourceView)
				{
					D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
					shaderResourceViewDesc.Format = combinationsOfDepthStencilFormats[depthStencilTexture2dFormatIndex][2];
					shaderResourceViewDesc.ViewDimension = enableMsaa ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
					shaderResourceViewDesc.Texture2D.MipLevels = 1;
					hr = device->CreateShaderResourceView(texture2d.Get(), &shaderResourceViewDesc, m_depthStencilShaderResourceView.GetAddressOf());
					_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
				}
			}

			m_viewport.Width = static_cast<float>(width);
			m_viewport.Height = static_cast<float>(height);
			m_viewport.MinDepth = 0.f;
			m_viewport.MaxDepth = 1.f;
			m_viewport.TopLeftX = 0.f;
			m_viewport.TopLeftY = 0.f;
		}

		void FrameBuffer::Clear(ID3D11DeviceContext* immediateContext, float r, float g, float b, float a, u_int clearFlags, float depth, unsigned char stencil)
		{
			float color[4] = { r, g, b, a };
			if (m_renderTargetView)
			{
				immediateContext->ClearRenderTargetView(m_renderTargetView.Get(), color);
			}
			if (m_depthStencilView)
			{
				immediateContext->ClearDepthStencilView(m_depthStencilView.Get(), clearFlags, depth, stencil);
			}
		}

		void FrameBuffer::Activate(ID3D11DeviceContext* immediateContext)
		{
			m_numberOfStoredViewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
			immediateContext->RSGetViewports(&m_numberOfStoredViewports,m_defaultViewports);
			immediateContext->RSSetViewports(1, &m_viewport);

			immediateContext->OMGetRenderTargets(1, m_defaultRenderTargetView.ReleaseAndGetAddressOf(),
				m_defaultDepthStencilView.ReleaseAndGetAddressOf());
			immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
		}

		void FrameBuffer::Deactivate(ID3D11DeviceContext* immediateContext)
		{
			immediateContext->RSSetViewports(m_numberOfStoredViewports, m_defaultViewports);
			immediateContext->OMSetRenderTargets(1, m_defaultRenderTargetView.GetAddressOf(), m_defaultDepthStencilView.Get());
		}
	}
}