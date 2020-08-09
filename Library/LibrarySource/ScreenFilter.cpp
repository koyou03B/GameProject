#include "ScreenFilter.h"


namespace Source
{
	namespace ScreenFilter
	{
		ScreenFilter::ScreenFilter(ID3D11Device* device) : FullScreen(device)
		{
			m_screenFilterPixel = std::make_unique<Source::Shader::PixelShader>(device, "../Library/LibraryShader/ScreenFilter_ps.cso");
			_screenBuffer = std::make_unique<Source::ConstantBuffer::ConstantBuffer<ScreenBuffer>>(device);
	

		}

		void ScreenFilter::Blit(ID3D11DeviceContext* immediateContext, ID3D11ShaderResourceView* colourMap)
		{
			_screenBuffer->Activate(immediateContext, SLOT0, false, true);

			immediateContext->PSSetShaderResources(SLOT0, 1, &colourMap);

			m_screenFilterPixel->Activate(immediateContext);
			ActivateSampler(immediateContext, SLOT0);
			FullScreen::Blit(immediateContext);
			DeactivateSampler(immediateContext);
			m_screenFilterPixel->Deactivate(immediateContext);


			ID3D11ShaderResourceView* null_srv = nullptr;
			immediateContext->PSSetShaderResources(SLOT0, 1, &null_srv);
			_screenBuffer->Deactivate(immediateContext);

		}

	}
}
