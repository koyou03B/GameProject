#include "SceneTitle.h"
#include ".\LibrarySource\vectorCombo.h"
#include ".\LibrarySource\Framework.h"
#include ".\LibrarySource\Input.h"
#include ".\LibrarySource\Camera.h"

bool Title::Initialize(ID3D11Device* device)
{

	m_frameBuffer[0] = std::make_unique<Source::FrameBuffer::FrameBuffer>(device, static_cast<int>(Framework::GetInstance().SCREEN_WIDTH), 
		static_cast<int>(Framework::GetInstance().SCREEN_HEIGHT),
		false/*enable_msaa*/, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
	m_screenFilter = std::make_unique<Source::ScreenFilter::ScreenFilter>(device);

	m_tObjectAdominist = std::make_unique<TitleObjctAdominist>();
	m_tObjectAdominist->SetObject(ObjectElement::JUGGERNAUT, ObjectType::ACTOR);
	m_tObjectAdominist->SetObject(ObjectElement::STAGE, ObjectType::STATIC);
	m_tObjectAdominist->SetObject(ObjectElement::TITLE_LOG, ObjectType::SPRITE);
	//*********************
	// Camera
	//*********************
	{
		Source::CameraControlle::CameraManager().GetInstance()->Initialize(device);
		VECTOR3F eye = { -25.1479f,7.2534f,7.4037f };
		VECTOR3F target = { -1.110f,3.6087f,9.350f };
		Source::CameraControlle::CameraManager().GetInstance()->GetCamera()->SetEye(eye);
		Source::CameraControlle::CameraManager().GetInstance()->GetCamera()->SetFocus(target);
		Source::CameraControlle::CameraManager().GetInstance()->SetCameraMode(Source::CameraControlle::CameraManager::CameraMode::ORBIT);
		float tmp = 0;
		Source::CameraControlle::CameraManager().GetInstance()->Update(tmp);
	}
	//**********************
	// SceneConstantBuffer
	//**********************
	{
		m_sceneConstantBuffer = std::make_unique<Source::ConstantBuffer::ConstantBuffer<Source::Constants::SceneConstants>>(device);
		m_sceneConstantBuffer->data.directionalLight.direction = { 0.0f, -1.0f, 1.0f, 0.0f };
		m_sceneConstantBuffer->data.ambientColor = { 0.147f,0.147f,0.147f,1.0f };
	}

	//*********************
	// Shader
	//*********************
	{
		m_frameBuffer[1] = std::make_unique<Source::FrameBuffer::FrameBuffer>(device, static_cast<int>(Framework::GetInstance().SCREEN_WIDTH), static_cast<int>(Framework::GetInstance().SCREEN_HEIGHT),
			false/*enable_msaa*/, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
		m_fog = std::make_unique<Source::Fog::Fog>(device);

		m_fog->ReadBinary();

		m_screenFilter = std::make_unique<Source::ScreenFilter::ScreenFilter>(device);

	}
	return true;
}

void Title::Update(float& elapsedTime)
{
#if _DEBUG
	if (KEYBOARD._keys[DIK_0] == 1)
	{
		SceneLabel label = SceneLabel::GAME;
		ActivateScene.ChangeScene(label);
	}
#endif
	Source::CameraControlle::CameraManager().GetInstance()->Update(elapsedTime);

	m_input = PAD.GetPad(0);
	if (!m_input) return;
	if (m_input->GetButtons())
	{
		isEvent = true;
	}

	if (isEvent)
	{
		switch (m_eventState)
		{
		case 0:
			m_tObjectAdominist->GetTitleObject(ObjectElement::TITLE_LOG)->SetAction(true);
			if (m_tObjectAdominist->GetTitleObject(ObjectElement::TITLE_LOG)->GetActionEnd())
				++m_eventState;
			break;

		case 1:
			m_screenFilter->_screenBuffer->data.screenColor.x -= 0.05f;
			m_screenFilter->_screenBuffer->data.screenColor.y -= 0.05f;
			m_screenFilter->_screenBuffer->data.screenColor.z -= 0.05f;

			if (m_screenFilter->_screenBuffer->data.screenColor.x <= 0.0f)
			{
				m_screenFilter->_screenBuffer->data.screenColor.x = 1.0f;
				SceneLabel label = SceneLabel::GAME;
				ActivateScene.ChangeScene(label);
				return;
			}
		}

	}
	m_tObjectAdominist->Update(elapsedTime);
}

void Title::Render(ID3D11DeviceContext* immediateContext, float elapsedTime)
{
	auto blend = Framework::GetInstance().GetBlendState(Framework::BS_ALPHA);
	immediateContext->OMSetBlendState(blend, nullptr, 0xffffffff);//Getはポインタ用
	{

		m_frameBuffer[0]->Clear(immediateContext);
		m_frameBuffer[0]->Activate(immediateContext);

		m_frameBuffer[1]->Clear(immediateContext);
		m_frameBuffer[1]->Activate(immediateContext);

		m_sceneConstantBuffer->Activate(immediateContext, SLOT2, true, true);
		Source::CameraControlle::CameraManager().GetInstance()->Activate(immediateContext);


		m_tObjectAdominist->Render(immediateContext,ObjectElement::JUGGERNAUT);
		m_tObjectAdominist->Render(immediateContext, ObjectElement::STAGE);
		m_frameBuffer[1]->Deactivate(immediateContext);

		m_fog->Blit(immediateContext, m_frameBuffer[1]->GetRenderTargetShaderResourceView().Get(), m_frameBuffer[1]->GetDepthStencilShaderResourceView().Get());

		m_tObjectAdominist->Render(immediateContext, ObjectElement::TITLE_LOG);
		m_frameBuffer[0]->Deactivate(immediateContext);

		m_screenFilter->Blit(immediateContext, m_frameBuffer[0]->GetRenderTargetShaderResourceView().Get());

		Source::CameraControlle::CameraManager().GetInstance()->Deactivate(immediateContext);
		m_sceneConstantBuffer->Deactivate(immediateContext);

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

		if (ImGui::CollapsingHeader("Fog"))
		{
			ImGui::InputFloat("fogGlobalDensity", &m_fog->_fogBuffer->data.fogGlobalDensity, 0.0001f, 0.001f, "%.4f");
			ImGui::InputFloat("fogHeightFalloff", &m_fog->_fogBuffer->data.fogHeightFallOff, 0.0001f, 0.001f, "%.4f");
			ImGui::InputFloat("fogStartDepth", &m_fog->_fogBuffer->data.fogStartDepth, 0.1f, 1.0f);

			ImGuiColorEditFlags flag = ImGuiColorEditFlags_Float; // 0 ~ 255表記ではなく、0.0 ~ 1.0表記にします。

			ImGui::ColorEdit3("fogColor", reinterpret_cast<float*>(&m_fog->_fogBuffer->data.fogColor), flag);
			ImGui::ColorEdit3("fogHighlightColor", reinterpret_cast<float*>(&m_fog->_fogBuffer->data.fogHighlightColor), flag);

			ImGui::InputFloat("R", &m_fog->_fogBuffer->data.fogColor.x, 0.0f, 1.0f, "%f");
			ImGui::InputFloat("G", &m_fog->_fogBuffer->data.fogColor.y, 0.0f, 1.0f, "%f");
			ImGui::InputFloat("B", &m_fog->_fogBuffer->data.fogColor.z, 0.0f, 1.0f, "%f");

			if (ImGui::Button("Save"))
				m_fog->SaveBinary();
		}

		if (ImGui::CollapsingHeader("directionalLight"))
		{
			//--------
			// Color
			//--------
			static float color[4] = { 1.f, 1.f, 1.f, 1.f };
			ImGuiColorEditFlags flag = ImGuiColorEditFlags_Float; // 0 ~ 255表記ではなく、0.0 ~ 1.0表記にします。
			ImGui::ColorEdit4("directionalLightColor", color, flag);
			m_sceneConstantBuffer->data.directionalLight.color = VECTOR4F(color[0], color[1], color[2], color[3]);

			//--------------
			// Direction
			//--------------
			static float direction[4] = { -0.5f, -1.f, 1.f, 0.f };
			ImGui::SliderFloat4("Direction", direction, -1.0f, 1.0f);
			m_sceneConstantBuffer->data.directionalLight.direction = VECTOR4F(direction[0], direction[1], direction[2], direction[3]);
		}

		if (ImGui::CollapsingHeader("TitleObject"))
		{
			ObjectElement element;
			static int item = 1;
			ImGui::Combo("ObjectElement", &item, "FIGHTER\0ARCHER\0JUGGERNAUT\0STONE\0STAGE\0TITLE_LOG\0\0");
			element = static_cast<ObjectElement>(item);
			static bool isActor = true;
			if (ImGui::Button("Actor"))
				isActor = true;
			ImGui::SameLine();
			if (ImGui::Button("Static"))
				isActor = false;

			ObjectType type;
			static int item2 = 1;
			ImGui::Combo("ObjectType", &item2, "ACTOR\0STATIC\0SPRITE\0\0");
			type = static_cast<ObjectType>(item2);


			if (ImGui::Button("Set"))
				m_tObjectAdominist->SetObject(element, type);

			static bool isDebug = false;
			if (ImGui::Button("SetUP"))
				isDebug = !isDebug;

			ImGui::Text("isDebug -> %d", isDebug);
			if (isDebug)
				m_tObjectAdominist->ImGui(element, Framework::GetInstance().GetDevice());
		}

		if (ImGui::CollapsingHeader("Camera"))
		{
			auto camera = Source::CameraControlle::CameraManager().GetInstance();
			VECTOR4F eye = camera->GetCamera()->GetEye();
			VECTOR4F target = camera->GetCamera()->GetFocus();

			float eyeArray[] = { eye.x,eye.y,eye.z };
			float targetArray[] = { target.x,target.y,target.z };
			ImGui::SliderFloat3("Eye", eyeArray, -1980.0f, 1980.0f, "%.4f");
			ImGui::SliderFloat3("Target", targetArray, -1980.0f, 1080.0f, "%.4f");
		}

	}

	ImGui::End();
#endif
}

void Title::Uninitialize()
{
	m_tObjectAdominist->Release();
}
