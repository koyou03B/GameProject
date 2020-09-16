#include "Fog.h"
#include"Framework.h"

namespace Source
{
	namespace Fog
	{
		Fog::Fog(ID3D11Device* device) : FullScreen(device)
		{
			m_fogPixel = std::make_unique<Shader::PixelShader>(device, "../Library/LibraryShader/Fog_ps.cso");
			_fogBuffer = std::make_unique<ConstantBuffer::ConstantBuffer<FogBuffer>>(device);



			D3D11_SAMPLER_DESC samplerDesc;
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; //D3D11_FILTER_MIN_MAG_MIP_POINT D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER; //D3D11_TEXTURE_ADDRESS_BORDER D3D11_TEXTURE_ADDRESS_CLAMP D3D11_TEXTURE_ADDRESS_MIRROR
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
			samplerDesc.MipLODBias = 0;
			samplerDesc.MaxAnisotropy = 16;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL; //D3D11_COMPARISON_LESS_EQUAL D3D11_COMPARISON_ALWAYS
			float colour[4] = { 1, 1, 1, 1 };
			memcpy(samplerDesc.BorderColor, colour, sizeof(colour));
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			HRESULT hr = device->CreateSamplerState(&samplerDesc, m_stateObject.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		}

		void Fog::Blit(ID3D11DeviceContext* immediateContext, ID3D11ShaderResourceView* colourMap, ID3D11ShaderResourceView* depthMap)
		{
			_fogBuffer->Activate(immediateContext, SLOT0, false, true);

			immediateContext->PSSetShaderResources(SLOT0, 1, &colourMap);
			immediateContext->PSSetShaderResources(SLOT1, 1, &depthMap);

			m_fogPixel->Activate(immediateContext);
			ActivateSampler(immediateContext, SLOT0);
			FullScreen::Blit(immediateContext);
			DeactivateSampler(immediateContext);
			m_fogPixel->Deactivate(immediateContext);


			ID3D11ShaderResourceView* null_shader_resource_views[2] = {};
			immediateContext->PSSetShaderResources(0, 2, null_shader_resource_views);

			_fogBuffer->Deactivate(immediateContext);
		}

	}
}
