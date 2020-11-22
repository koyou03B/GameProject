#include "SceneClear.h"
#include ".\LibrarySource\Input.h"
#include ".\LibrarySource\vectorCombo.h"

bool Clear::Initialize(ID3D11Device* device)
{
	m_sprite = TEXTURELOADER.GetTexture(Source::SpriteLoad::TextureLabel::CLEAR);
	position = VECTOR2F(640.0f, 450.0f);
	return true;
}

void Clear::Update(float& elapsedTime)
{
#ifdef _DEBUG
	if (KEYBOARD._keys[DIK_3] == 1)
	{
		SceneLabel label = SceneLabel::TITLE;
		ActivateScene.ChangeScene(label);
	}
#endif

	m_input = PAD.GetPad(0);
	if (m_input->GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_X) == 1)
	{
		SceneLabel label = SceneLabel::TITLE;
		ActivateScene.ChangeScene(label);
		return;
	}

	if (m_input->GetButtons(XINPUT_GAMEPAD_BUTTONS::PAD_A) == 1)
	{
		SceneLabel label = SceneLabel::GAME;
		ActivateScene.ChangeScene(label);
		return;

	}

}

void Clear::Render(ID3D11DeviceContext* immediateContext, float elapsedTime)
{
	m_sprite->Begin(immediateContext);
	m_sprite->RenderCenter(position, scale,texPos, texSize, 0, VECTOR4F(1.0, 1.0, 1.0, 1.0), false);
	m_sprite->End(immediateContext);
}

void Clear::ImGui()
{
#ifdef _DEBUG


	ImGui::Begin("Title");
	//------------
	// Sceneà⁄çs
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

		ImGui::InputFloat("pos X", &position.x, 0.f, 1980.0f, "%.4f");
		ImGui::InputFloat("pos Y", &position.y, 0.f, 1080.0f, "%.4f");

		ImGui::InputFloat("scale X", &scale.x, 0.f, 2.0f, "%.4f");
		ImGui::InputFloat("scale Y", &scale.y, 0.f, 2.0f, "%.4f");

		ImGui::InputFloat("texPos X", &texPos.x, 0.f, 640, "%.4f");
		ImGui::InputFloat("texPos Y", &texPos.y, 0.f, 640, "%.4f");

		ImGui::InputFloat("texSize X", &texSize.x, 0.f, 640, "%.4f");
		ImGui::InputFloat("texSize Y", &texSize.y, 0.f, 640, "%.4f");
	}

	ImGui::End();
#endif
}

void Clear::Uninitialize()
{
}
