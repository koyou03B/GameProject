#include "SceneTutorial.h"
//#include"Framework.h"
//#include"vectorCombo.h"

bool Tutorial::Initialize(ID3D11Device* device)
{
	//VECTOR4F _color = VECTOR4F(.0f, .0f, .0f,1.0f);
	//CameraManager::GetInstance()->Initialize(device);
	//CameraManager::GetInstance()->SetTarget(VECTOR4F(0.0f,0.0f,10.0f,0.0f));
	//float tmp = 0;
	//CameraManager::GetInstance()->Update(tmp);


	//_cube = std::make_unique<GeometryObject<GeometricSphere>>(device);
	//_cube->AddInstanceData(VECTOR3F(0.0f, 0.0f, 0.0f));
	//_cube->AddInstanceData(VECTOR3F(5.0f, 0.0f, 0.0f));
	//_cube->AddInstanceData(VECTOR3F(10.0f, 0.0f, 0.0f));
	//_cube->AddInstanceData(VECTOR3F(15.0f, 0.0f, 0.0f));
	return true;
}

void Tutorial::Update(float& elapsedTime)
{
	//CameraManager::GetInstance()->SetTarget(VECTOR4F(0.0f, 0.0f, 10.0f, 0.0f));
	//CameraManager::GetInstance()->Update(elapsedTime);
}

void Tutorial::Render(ID3D11DeviceContext* immediateContext, float elapsedTime)
{
	{

		//VECTOR4F lightDirection(0, -1, 1, 0);
		//VECTOR4F materialColor(.0f, 1.0f, 1.0f, 1.0f);

		//CameraManager::GetInstance()->Activate(immediateContext);

		//_cube->Render(immediateContext, CameraManager::GetInstance()->GetProjection(),
		//	CameraManager::GetInstance()->GetView(), lightDirection, materialColor, false);

		//CameraManager::GetInstance()->Deactivate(immediateContext);

	}
}

void Tutorial::ImGui()
{
#ifdef _DEBUG
//	ImGui::Begin("Title");
//	static int chois = 0;
//	std::vector<std::string> sceneList = { "Title","Tutorial","Game","Over","Clear" };
//	ImGui::Combo("SceneList",
//		&chois,
//		vectorGetter,
//		static_cast<void*>(&sceneList),
//		static_cast<int>(sceneList.size())
//	);
//	{
//		static bool isSceneChange = false;
//		if (ImGui::Button("SceneChange"))
//			isSceneChange = true;
//
//		if (isSceneChange)
//		{
//			ImGui::StyleColorsDark();
//			ImGuiStyle& style = ImGui::GetStyle();
//			ImGui::SetNextWindowPos(ImVec2(710, 350));
//			ImGui::SetNextWindowSize(ImVec2(525, 130));
//			ImGui::Begin("SceneChange");
//			if (ImGui::Button("YES", ImVec2(240, 80)))
//			{
//				SceneLabel label = static_cast<SceneLabel>(chois);
//				ActivateScene->ChangeScene(label);
//				isSceneChange = false;
//			}
//			ImGui::SameLine();
//			if (ImGui::Button("No", ImVec2(260, 80)))
//				isSceneChange = false;
//			ImGui::End();
//		}
//	}
//
//	ImGuiStyle& style = ImGui::GetStyle();
//	style.Colors[ImGuiCol_Separator] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
//	ImGui::Separator();
//	ImGui::Separator();
//
//	if (ImGui::CollapsingHeader("Camera"))
//	{
//		ImGui::SetNextWindowSize(ImVec2(400, Framework::GetInstance().SCREEN_HEIGHT), ImGuiSetCond_Once);//サイズ
//		ImGui::SetNextWindowPos(ImVec2(1520, 0), ImGuiSetCond_Once);//ポジション
//		ImGui::Begin("CameraEditer");
//		CameraManager::GetInstance()->Editor();
//		ImGui::End();
//	}
//	ImGui::End();
#endif
}

void Tutorial::Uninitialize()
{
}
