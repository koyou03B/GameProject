#include "SceneTitle.h"
#include ".\LibrarySource\vectorCombo.h"
#include ".\LibrarySource\Framework.h"

bool Title::Initialize(ID3D11Device* device)
{
	m_sprite = Framework::GetInstance().GetTexture(Source::SpriteData::TextureLabel::TITLE);
	m_frameBuffer = std::make_unique<Source::FrameBuffer::FrameBuffer>(device, static_cast<int>(Framework::GetInstance().SCREEN_WIDTH), 
		static_cast<int>(Framework::GetInstance().SCREEN_HEIGHT),
		false/*enable_msaa*/, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
	m_screenFilter = std::make_unique<Source::ScreenFilter::ScreenFilter>(device);

	m_dissolve = std::make_unique<Source::ConstantBuffer::ConstantBuffer<Source::Constants::Dissolve>>(device);
	m_dissolve->data.dissolveGlowAmoument = 0.0f;
	m_dissolve->data.dissolveGlowRange = 1.0f;
	m_dissolve->data.dissolveGlowFalloff = 1.0f;
	m_dissolve->data.dissolveGlowColor = VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f);
	m_dissolve->data.dissolveEmission = VECTOR4F(0.0f, 0.0f, 0.0f, 0.0f);
	return true;
}

void Title::Update(float& elapsedTime)
{
}

void Title::Render(ID3D11DeviceContext* immediateContext, float elapsedTime)
{
	auto blend = Framework::GetInstance().GetBlendState(Framework::BS_ALPHA);
	immediateContext->OMSetBlendState(blend, nullptr, 0xffffffff);//Getはポインタ用
	{
		m_frameBuffer->Clear(immediateContext);
		m_frameBuffer->Activate(immediateContext);

		m_dissolve->Activate(immediateContext, SLOT5, true, true);

		m_sprite->Begin(immediateContext);
		m_sprite->RenderCenter(VECTOR2F(Framework::GetInstance().SCREEN_WIDTH * 0.5f, Framework::GetInstance().SCREEN_HEIGHT * 0.5f),
			VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0, VECTOR4F(1.0, 1.0, 1.0, 1.0), false);
		m_sprite->End(immediateContext);
		m_dissolve->Deactivate(immediateContext);

		m_frameBuffer->Deactivate(immediateContext);

		m_screenFilter->Blit(immediateContext, m_frameBuffer->GetRenderTargetShaderResourceView().Get());
	}
	blend = Framework::GetInstance().GetBlendState(Framework::BS_NONE);
	immediateContext->OMSetBlendState(blend, nullptr, 0xffffffff);//Getはポインタ用
}

void Title::ImGui()
{
#ifdef _DEBUG


	ImGui::Begin("Title");
	//------------
	// Scene移行
	//------------
	{
		static int chois = 0;
		std::vector<std::string> sceneList = { "Title","Tutorial","Game","Over","Clear" };
		ImGui::Combo("SceneList",
			&chois,
			vectorGetter,
			static_cast<void*>(&sceneList),
			static_cast<int>(sceneList.size())
		);
		{
			static bool isSceneChange = false;
			if (ImGui::Button("SceneChange"))
				isSceneChange = true;

			if (isSceneChange)
			{
				ImGui::SetNextWindowPos(ImVec2(710, 350));
				ImGui::SetNextWindowSize(ImVec2(525, 130));
				ImGui::Begin("SceneChange");
				if (ImGui::Button("YES", ImVec2(240, 80)))
				{
					SceneLabel label = static_cast<SceneLabel>(chois);
					ActivateScene.ChangeScene(label);
					isSceneChange = false;
				}
				ImGui::SameLine();
				if (ImGui::Button("No", ImVec2(260, 80)))
					isSceneChange = false;
				ImGui::End();
			}
		}

	}

	if (!ActivateScene.GetIsStart())
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_Separator] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		ImGui::Separator();
		ImGui::Separator();
		//------------
		// Shader関係
		//------------
		if (ImGui::CollapsingHeader("ScreenFilter"))
		{
			//--------------
			// Bright(明度)
			//--------------
			static float bright = 0.0f;
			ImGui::SliderFloat("Bright", &bright, -1.0f, 1.0f);
			m_screenFilter->_screenBuffer->data.bright = bright;

			//--------------
			// Contrast(濃淡)
			//--------------
			static float contrast = 1.0f;
			ImGui::SliderFloat("Contrast", &contrast, -1.0f, 1.0f);
			m_screenFilter->_screenBuffer->data.contrast = contrast;

			//-----------------
			// Saturate(彩度)
			//-----------------
			static float saturate = 1.0f;
			ImGui::SliderFloat("Saturate", &saturate, -1.0f, 1.0f);
			m_screenFilter->_screenBuffer->data.saturate = saturate;

			//--------
			// Color
			//--------
			static float color[4] = { 1.f, 1.f, 1.f, 1.f };
			ImGuiColorEditFlags flag = ImGuiColorEditFlags_Float; // 0 ~ 255表記ではなく、0.0 ~ 1.0表記にします。
			ImGui::ColorEdit4("Color", color, flag);
			m_screenFilter->_screenBuffer->data.screenColor = VECTOR4F(color[0], color[1], color[2], color[3]);

		}

		if (ImGui::CollapsingHeader("Dissolve"))
		{
			//--------------
			// GlowAmoument
			//--------------
			static float glowAmoument = 0.5f;
			ImGui::SliderFloat("GlowAmoument", &glowAmoument, -1.0f, 1.0f);
			m_dissolve->data.dissolveGlowAmoument = glowAmoument;

			//--------------
			// GlowRange
			//--------------
			static float glowRange = 0.1f;
			ImGui::SliderFloat("GlowRange", &glowRange, -1.0f, 1.0f);
			m_dissolve->data.dissolveGlowRange = glowRange;

			//-----------------
			// GlowFalloff
			//-----------------
			static float glowFalloff = 0.1f;
			ImGui::SliderFloat("GlowFalloff", &glowFalloff, -1.0f, 1.0f);
			m_dissolve->data.dissolveGlowFalloff = glowFalloff;

			//------------
			// GlowColor
			//------------
			static float glowColor[4] = { 1.f, 1.f, 1.f, 1.f };
			ImGuiColorEditFlags flag = ImGuiColorEditFlags_Float; // 0 ~ 255表記ではなく、0.0 ~ 1.0表記にします。
			ImGui::ColorEdit4("GlowColor", glowColor, flag);
			m_dissolve->data.dissolveGlowColor = VECTOR4F(glowColor[0], glowColor[1], glowColor[2], glowColor[3]);

			//------------
			// Emission
			//------------
			static float emission[4] = { 1.f, 1.f, 1.f, 1.f };
			ImGuiColorEditFlags flagE = ImGuiColorEditFlags_Float; // 0 ~ 255表記ではなく、0.0 ~ 1.0表記にします。
			ImGui::ColorEdit4("Emission", emission, flagE);
			m_dissolve->data.dissolveEmission = VECTOR4F(emission[0], emission[1], emission[2], emission[3]);
		}
	}
	ImGui::End();
#endif
}

void Title::Uninitialize()
{

}
