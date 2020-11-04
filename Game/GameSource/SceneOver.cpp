#include "SceneOver.h"
#include".\LibrarySource\Framework.h"
#include".\LibrarySource\vectorCombo.h"

bool Over::Initialize(ID3D11Device* device)
{
	//{
	//	m_board = std::make_unique<Source::BillBoard::BillBoard>(device, Source::SpriteLoad::loadTexture[static_cast<int>(Source::SpriteLoad::TextureLabel::SCOPE)].fileName, true);

	//}

	//{
	//	m_sceneConstantBuffer = std::make_unique<Source::ConstantBuffer::ConstantBuffer<Source::Constants::SceneConstants>>(device);
	//	m_sceneConstantBuffer->data.directionalLight.direction = { 0.0f, -1.0f, 1.0f, 0.0f };
	//}

	//{
	//	Source::CameraControlle::CameraManager().GetInstance()->Initialize(device);
	//	Source::CameraControlle::CameraManager().GetInstance()->SetTarget(VECTOR3F(0.0f, 0.0f, 10.0f));
	//	float tmp = 0;
	//	Source::CameraControlle::CameraManager().GetInstance()->Update(tmp);
	//}


	return true;
}

void Over::Update(float& elapsedTime)
{
	if (KEYBOARD._keys[DIK_3] == 1)
	{
		SceneLabel label = SceneLabel::TITLE;
		ActivateScene.ChangeScene(label);
	}
}

void Over::Render(ID3D11DeviceContext* immediateContext, float elapsedTime)
{
	//{
	//	VECTOR4F lightDirection(0, -1, 1, 0);
	//	VECTOR4F materialColor(1.0f, 1.0f, 1.0f, 1.0f);

	//	DirectX::XMMATRIX S, R, T;
	//	FLOAT4X4 world;
	//	S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	//	R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	//	T = DirectX::XMMatrixTranslation(translate.x, translate.y, translate.z);
	//	DirectX::XMStoreFloat4x4(&world, S * R * T);

	//	Source::CameraControlle::CameraManager().GetInstance()->Activate(immediateContext);

	//	m_board->Begin(immediateContext);
	//	m_board->Render(immediateContext, Source::CameraControlle::CameraManager().GetInstance()->GetProjection(),
	//		Source::CameraControlle::CameraManager().GetInstance()->GetView(),
	//		VECTOR3F(0, 0, 0), 1, VECTOR3F(0, 0, 0), materialColor);
	//	m_board->End(immediateContext);



	//	GetEntityManager().Render();
	//	m_sceneConstantBuffer->Deactivate(immediateContext);

	//	Source::CameraControlle::CameraManager().GetInstance()->Deactivate(immediateContext);
	//}
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


	ImGui::End();
#endif
}

void Over::Uninitialize()
{
}
