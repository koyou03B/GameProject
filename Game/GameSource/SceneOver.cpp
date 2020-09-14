#include "SceneOver.h"
#include".\LibrarySource\Framework.h"
#include".\LibrarySource\vectorCombo.h"

bool Over::Initialize(ID3D11Device* device)
{
	{
		m_board = std::make_unique<Source::BillBoard::BillBoard>(device, Source::SpriteLoad::loadTexture[static_cast<int>(Source::SpriteLoad::TextureLabel::SCOPE)].fileName, true);

	}

	{
		m_sceneConstantBuffer = std::make_unique<Source::ConstantBuffer::ConstantBuffer<Source::Constants::SceneConstants>>(device);
		m_sceneConstantBuffer->data.directionalLight.direction = { 0.0f, -1.0f, 1.0f, 0.0f };
	}

	{
		Source::CameraControlle::CameraManager().GetInstance()->Initialize(device);
		Source::CameraControlle::CameraManager().GetInstance()->SetTarget(VECTOR3F(0.0f, 0.0f, 10.0f));
		float tmp = 0;
		Source::CameraControlle::CameraManager().GetInstance()->Update(tmp);
	}


	return true;
}

void Over::Update(float& elapsedTime)
{
	Source::CameraControlle::CameraManager().GetInstance()->SetTarget(VECTOR3F(0.0f, 0.0f, 10.0f));
	Source::CameraControlle::CameraManager().GetInstance()->Update(elapsedTime);
}

void Over::Render(ID3D11DeviceContext* immediateContext, float elapsedTime)
{
	{
		VECTOR4F lightDirection(0, -1, 1, 0);
		VECTOR4F materialColor(1.0f, 1.0f, 1.0f, 1.0f);

		DirectX::XMMATRIX S, R, T;
		FLOAT4X4 world;
		S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
		T = DirectX::XMMatrixTranslation(translate.x, translate.y, translate.z);
		DirectX::XMStoreFloat4x4(&world, S * R * T);

		Source::CameraControlle::CameraManager().GetInstance()->Activate(immediateContext);

		m_board->Begin(immediateContext);
		m_board->Render(immediateContext, Source::CameraControlle::CameraManager().GetInstance()->GetProjection(),
			Source::CameraControlle::CameraManager().GetInstance()->GetView(),
			VECTOR3F(0, 0, 0), 1, VECTOR3F(0, 0, 0), materialColor);
		m_board->End(immediateContext);



		GetEntityManager().Render();
		m_sceneConstantBuffer->Deactivate(immediateContext);

		Source::CameraControlle::CameraManager().GetInstance()->Deactivate(immediateContext);
	}
}

void Over::ImGui()
{
#ifdef _DEBUG
	ImGui::Begin("GAMEOVER");
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
			ImGui::StyleColorsDark();
			ImGuiStyle& style = ImGui::GetStyle();
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

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Separator] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	ImGui::Separator();
	ImGui::Separator();

	//------------
	// Shader関係
	//------------
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

	if (ImGui::CollapsingHeader("ambientColor"))
	{
		//--------
		// Color
		//--------
		static float color[4] = { 0.7f, 0.7f, 0.7f, 1.f };
		ImGuiColorEditFlags flag = ImGuiColorEditFlags_Float; // 0 ~ 255表記ではなく、0.0 ~ 1.0表記にします。
		ImGui::ColorEdit4("ambientColorColor", color, flag);
		m_sceneConstantBuffer->data.ambientColor = VECTOR4F(color[0], color[1], color[2], color[3]);

	}

	if (ImGui::CollapsingHeader("Geometry"))
	{
		//-----------
		// Position
		//-----------
		static float position[3] = { 0.f, 0.f, 0.f};
		ImGui::DragFloat3("Position", position, 0.1f);
		VECTOR3F setPosition(position[0], position[1], position[2]);
		//-----------
		// Angle
		//-----------
		static float angle[3] = { 0.f, 0.f, 0.f};
		ImGui::DragFloat3("Angle", angle, 1.0f * 0.01745f);
		VECTOR3F setAngle(angle[0], angle[1], angle[2]);
		//-----------
		// Scale
		//-----------
		static float scale[3] = { 0.f, 0.f, 0.f };
		ImGui::DragFloat3("Scale", scale, 0.1f);
		VECTOR3F setScale(scale[0], scale[1], scale[2]);
		//--------
		// Color
		//--------
		static float color[4] = { 1.f, 1.f, 1.f, 1.f };
		ImGuiColorEditFlags flag = ImGuiColorEditFlags_Float; // 0 ~ 255表記ではなく、0.0 ~ 1.0表記にします。
		ImGui::ColorEdit4("Color", color, flag);
		VECTOR4F setColor(color[0], color[1], color[2], color[3]);


	}

	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::SetNextWindowSize(ImVec2(400, Framework::GetInstance().SCREEN_HEIGHT), ImGuiSetCond_Once);//サイズ
		ImGui::SetNextWindowPos(ImVec2(1520, 0), ImGuiSetCond_Once);//ポジション
		ImGui::Begin("CameraEditer");
		Source::CameraControlle::CameraManager().GetInstance()->Editor();
		ImGui::End();
	}
	ImGui::End();
#endif
}

void Over::Uninitialize()
{
}
