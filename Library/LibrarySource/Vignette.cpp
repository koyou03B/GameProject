#include "Vignette.h"
#include "Framework.h"

namespace Source
{
	namespace Vignette
	{
		Vignette::Vignette(ID3D11Device* device) : FullScreen(device)
		{
			m_vignettePixel = std::make_unique<Shader::PixelShader>(device, "../Library/LibraryShader/Vignette_ps.cso");
			_vignetteBuffer = std::make_unique<ConstantBuffer::ConstantBuffer<VignetteBuffer>>(device);

			D3D11_SAMPLER_DESC samplerDesc;
			samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC; //D3D11_FILTER_MIN_MAG_MIP_POINT D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP; //D3D11_TEXTURE_ADDRESS_BORDER D3D11_TEXTURE_ADDRESS_CLAMP D3D11_TEXTURE_ADDRESS_MIRROR
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.MipLODBias = 0;
			samplerDesc.MaxAnisotropy = 16;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL; //D3D11_COMPARISON_LESS_EQUAL D3D11_COMPARISON_ALWAYS
			float colour[4] = { 1, 1, 1, 1 };
			memcpy(samplerDesc.BorderColor, colour, sizeof(colour));
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			HRESULT hr = device->CreateSamplerState(&samplerDesc, m_stateObject.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

			_vignetteBuffer->data.aroundColor = VECTOR4F(0.0f, 0.0f, 0.0f,0.0f);
			_vignetteBuffer->data.darkness = 0.0f;
			_vignetteBuffer->data.radius = 0.8f;
			_vignetteBuffer->data.smoothness = 1.0f;
		}

		void Vignette::Blit(ID3D11DeviceContext* immediateContext, ID3D11ShaderResourceView* colourMap, ID3D11ShaderResourceView* depthMap)
		{
			_vignetteBuffer->Activate(immediateContext, SLOT0, false, true);

			immediateContext->PSSetShaderResources(SLOT0, 1, &colourMap);
		//	immediateContext->PSSetShaderResources(SLOT1, 1, &depthMap);

			m_vignettePixel->Activate(immediateContext);
			ActivateSampler(immediateContext, SLOT0);
			FullScreen::Blit(immediateContext);
			DeactivateSampler(immediateContext);
			m_vignettePixel->Deactivate(immediateContext);


			ID3D11ShaderResourceView* null_srv = nullptr;
			immediateContext->PSSetShaderResources(SLOT0, 1, &null_srv);

			_vignetteBuffer->Deactivate(immediateContext);
		}

	}
}
