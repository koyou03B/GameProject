#include "SceneClear.h"
#include ".\LibrarySource\Input.h"
//#include"vectorCombo.h"
bool Clear::Initialize(ID3D11Device* device)
{
	return true;
}

void Clear::Update(float& elapsedTime)
{
	if (KEYBOARD._keys[DIK_3] == 1)
	{
		SceneLabel label = SceneLabel::TITLE;
		ActivateScene.ChangeScene(label);
	}
}

void Clear::Render(ID3D11DeviceContext* immediateContext, float elapsedTime)
{

}

void Clear::ImGui()
{
#ifdef _DEBUG/*
	ImGui::Begin("Title");
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
				ActivateScene->ChangeScene(label);
				isSceneChange = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("No", ImVec2(260, 80)))
				isSceneChange = false;
			ImGui::End();
		}
	}

	ImGui::End();*/
#endif
}

void Clear::Uninitialize()
{
}
