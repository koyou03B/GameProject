#include "SceneEffect.h"

void SceneEffect::ChoiceSceneEffect(ID3D11Device* device,const SceneEffectType type)
{
	switch (type)
	{
	case SceneEffectType::SCREEN_FILTER:
		m_screenFilter.InitOfScreenFilter(device);
		break;
	case SceneEffectType::VIGNETTE:
		m_vignette.InitOfVignette(device);
		break;
	case SceneEffectType::FOG:
		m_fog.InitOfFog(device);
		break;
	}
	m_frameBuffer[type] = std::make_unique<Source::FrameBuffer::FrameBuffer>(device, static_cast<int>(Framework::GetInstance().SCREEN_WIDTH), static_cast<int>(Framework::GetInstance().SCREEN_HEIGHT),
		false/*enable_msaa*/, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
	m_activateEffect.push_back(type);
}

bool SceneEffect::UpdateScreenFilter(const float value, const float targetValue)
{
	return m_screenFilter.Update(value,targetValue);
}

void SceneEffect::UpdateVignette(const float targetTime, const float targetDarkness, const float darknessValue, float& elapsedTime)
{
	 m_vignette.Update(targetTime, targetDarkness, darknessValue,elapsedTime);
}

void SceneEffect::ActivateEffect(ID3D11DeviceContext* immediateContext, const SceneEffectType type)
{
	m_frameBuffer[type]->Clear(immediateContext);
	m_frameBuffer[type]->Activate(immediateContext);

}

void SceneEffect::DeactivateEffect(ID3D11DeviceContext* immediateContext, const SceneEffectType type)
{
	m_frameBuffer[type]->Deactivate(immediateContext);

	switch (type)
	{
	case SceneEffectType::SCREEN_FILTER:
		m_screenFilter.screenFilter->Blit(immediateContext, m_frameBuffer[type]->GetRenderTargetShaderResourceView().Get());
		break;
	case SceneEffectType::VIGNETTE:
		m_vignette.vignette->Blit(immediateContext, m_frameBuffer[type]->GetRenderTargetShaderResourceView().Get(), nullptr);
		break;
	case SceneEffectType::FOG:
		m_fog.fog->Blit(immediateContext, m_frameBuffer[type]->GetRenderTargetShaderResourceView().Get(),
			m_frameBuffer[type]->GetDepthStencilShaderResourceView().Get());
		break;
	}

}

void SceneEffect::FogEffect::InitOfFog(ID3D11Device* device)
{
	fog = std::make_unique<Source::Fog::Fog>(device);
	fog->ReadBinary();
}

void SceneEffect::ScreenFilterEffect::InitOfScreenFilter(ID3D11Device* device)
{
	screenFilter = std::make_unique<Source::ScreenFilter::ScreenFilter>(device);
	screenFilter->_screenBuffer->data.screenColor = { 0.0f,0.0f,0.0f,1.0f };
}

bool SceneEffect::ScreenFilterEffect::Update(const float value, const float targetValue)
{
	bool judgeRule = targetValue == 1.0f;
	float decisionValue = judgeRule ? value : value * -1.0f;

	screenFilter->_screenBuffer->data.screenColor.x += decisionValue;
	screenFilter->_screenBuffer->data.screenColor.y += decisionValue;
	screenFilter->_screenBuffer->data.screenColor.z += decisionValue;
	if (judgeRule)
		return screenFilter->_screenBuffer->data.screenColor.x >= targetValue;
	else
		return screenFilter->_screenBuffer->data.screenColor.x <= targetValue;

	return false;
}

void SceneEffect::VignetteEffect::InitOfVignette(ID3D11Device* device)
{
	vignette = std::make_unique<Source::Vignette::Vignette>(device);
	vignette->ReadBinary();
	vignetteTimer = 0.0f;
	isStartEffect = false;
}

void SceneEffect::VignetteEffect::Update(const float targetTime, const float targetDarkness,
	const float darknessValue, float& elapsedTime)
{
	if (isStartEffect)
	{
		vignetteTimer += elapsedTime;
		if (vignetteTimer >= targetTime)
		{
			vignetteTimer = 0.0f;
			isStartEffect = false;
		}
		else
			vignette->_vignetteBuffer->data.darkness = targetDarkness;
	}
	else
	{
		if (vignette->_vignetteBuffer->data.darkness > 0.0f)
			vignette->_vignetteBuffer->data.darkness -= darknessValue;

		if (vignette->_vignetteBuffer->data.darkness < 0.0f)
			vignette->_vignetteBuffer->data.darkness = 0.0f;
	}
}

void SceneEffect::ImGui()
{
#ifdef _DEBUG
	ImGui::Begin("Scene Effect");

	if (m_fog.fog)
	{
		if (ImGui::CollapsingHeader("Fog"))
		{
			ImGui::InputFloat("fogGlobalDensity", &m_fog.fog->_fogBuffer->data.fogGlobalDensity, 0.0001f, 0.001f, "%.4f");
			ImGui::InputFloat("fogHeightFalloff", &m_fog.fog->_fogBuffer->data.fogHeightFallOff, 0.0001f, 0.001f, "%.4f");
			ImGui::InputFloat("fogStartDepth", &m_fog.fog->_fogBuffer->data.fogStartDepth, 0.1f, 1.0f);

			ImGuiColorEditFlags flag = ImGuiColorEditFlags_Float; // 0 ~ 255表記ではなく、0.0 ~ 1.0表記にします。

			ImGui::ColorEdit3("fogColor", reinterpret_cast<float*>(&m_fog.fog->_fogBuffer->data.fogColor), flag);
			ImGui::ColorEdit3("fogHighlightColor", reinterpret_cast<float*>(&m_fog.fog->_fogBuffer->data.fogHighlightColor), flag);

			ImGui::InputFloat("R", &m_fog.fog->_fogBuffer->data.fogColor.x, 0.0f, 1.0f, "%f");
			ImGui::InputFloat("G", &m_fog.fog->_fogBuffer->data.fogColor.y, 0.0f, 1.0f, "%f");
			ImGui::InputFloat("B", &m_fog.fog->_fogBuffer->data.fogColor.z, 0.0f, 1.0f, "%f");

			if (ImGui::Button("Save"))
				m_fog.fog->SaveBinary();
		}
	}

	if (m_screenFilter.screenFilter)
	{
		if (ImGui::CollapsingHeader("ScreenFilter"))
		{
			//--------------
			// Bright(明度)
			//--------------
			static float bright = 0.0f;
			ImGui::SliderFloat("Bright", &bright, -1.0f, 1.0f);
			m_screenFilter.screenFilter->_screenBuffer->data.bright = bright;

			//--------------
			// Contrast(濃淡)
			//--------------
			static float contrast = 1.0f;
			ImGui::SliderFloat("Contrast", &contrast, -1.0f, 1.0f);
			m_screenFilter.screenFilter->_screenBuffer->data.contrast = contrast;

			//-----------------
			// Saturate(彩度)
			//-----------------
			static float saturate = 1.0f;
			ImGui::SliderFloat("Saturate", &saturate, -1.0f, 1.0f);
			m_screenFilter.screenFilter->_screenBuffer->data.saturate = saturate;

			//--------
			// Color
			//--------
			static float color[4] = { 1.f, 1.f, 1.f, 1.f };
			ImGuiColorEditFlags flag = ImGuiColorEditFlags_Float; // 0 ~ 255表記ではなく、0.0 ~ 1.0表記にします。
			ImGui::ColorEdit4("Color", color, flag);
			m_screenFilter.screenFilter->_screenBuffer->data.screenColor = VECTOR4F(color[0], color[1], color[2], color[3]);

		}
	}

	if (m_vignette.vignette)
	{
		if (ImGui::CollapsingHeader("Vignette"))
		{
			//--------------
			// Radius(半径)
			//--------------
			ImGui::SliderFloat("Radius", &m_vignette.vignette->_vignetteBuffer->data.radius, 0.0f, 10.0f);


			//--------------
			// Smoothness
			//--------------
			ImGui::SliderFloat("Smoothness", &m_vignette.vignette->_vignetteBuffer->data.smoothness, 0.0f, 10.0f);


			//-----------------
			// Darkness
			//-----------------
			ImGui::SliderFloat("Darkness", &m_vignette.vignette->_vignetteBuffer->data.darkness, 0.0f, 10.0f);


			//--------------
			// AroundColor
			//-------------
			static float color[4] = { 1.f, 1.f, 1.f, 1.f };
			ImGuiColorEditFlags flag = ImGuiColorEditFlags_Float; // 0 ~ 255表記ではなく、0.0 ~ 1.0表記にします。
			ImGui::ColorEdit4("AroundColor", color, flag);
			m_vignette.vignette->_vignetteBuffer->data.aroundColor = VECTOR4F(color[0], color[1], color[2], color[3]);


			if (ImGui::Button("Save"))
				m_vignette.vignette->SaveBinary();
		}

	}

	ImGui::End();
#endif
}