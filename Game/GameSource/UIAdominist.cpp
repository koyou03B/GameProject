#include "UIAdominist.h"

void UIData::Init(std::string name)
{
	if (PathFileExistsA((std::string("../Asset/Binary/UI/") + name + ".bin").c_str()))
	{
		std::ifstream ifs;
		ifs.open((std::string("../Asset/Binary/UI/") + name + ".bin").c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);
	}
	m_binaryName = name;
	for (auto& param : m_spriteParam)
	{
		param.maxTexSizeX = param.currentMaxSize =  param.texSize.x;
	}
}

void UIData::Render(ID3D11DeviceContext* immediateContext)
{
	m_sprite->Begin(immediateContext);
	for (auto& param : m_spriteParam)
	{
		m_sprite->RenderLeft(param.position, param.scale,
			param.texPos, param.texSize, 0, VECTOR4F(1.0, 1.0, 1.0, 1.0), false);
	}
	m_sprite->End(immediateContext);
}

void UIData::Release()
{
	for (auto& param : m_spriteParam)
	{
		param.ParameterClear();
	}

	m_sprite.reset();
}

void UIData::ImGui()
{
#ifdef _DEBUG

	ImGui::Begin("UIData", nullptr, ImGuiWindowFlags_MenuBar);//メニューバーをつかうならこのBEGIN

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))//ファイルの中には
		{
			if (ImGui::MenuItem("Save"))//データの保存とか
			{
				std::ofstream ofs;
				ofs.open((std::string("../Asset/Binary/UI/")+ m_binaryName + ".bin").c_str(), std::ios::binary);
				cereal::BinaryOutputArchive o_archive(ofs);
				o_archive(*this);
			}

			if (ImGui::MenuItem("Load"))
			{
				if (PathFileExistsA((std::string("../Asset/Binary/UI/") + m_binaryName + ".bin").c_str()))
				{
					std::ifstream ifs;
					ifs.open((std::string("../Asset/Binary/UI/")+ m_binaryName + ".bin").c_str(), std::ios::binary);
					cereal::BinaryInputArchive i_archive(ifs);
					i_archive(*this);
				}
			}
			ImGui::EndMenu();

		}
		ImGui::EndMenuBar();
	}

	int size = static_cast<int>(m_spriteParam.size());
	static int select = 0;
	ImGui::SliderInt("Select", &select, 0, size-1);

	auto& param = m_spriteParam.at(select);

	if (ImGui::CollapsingHeader("Position"))
	{
		float pos[] = { param.position.x,param.position.y };
		ImGui::SliderFloat2("Position ->", pos, 0.0f, 1980.0f);
		param.position = { pos[0],pos[1] };
	}

	if (ImGui::CollapsingHeader("Scale"))
	{
		float scale[] = { param.scale.x,param.scale.y };
		ImGui::SliderFloat2("Scale ->", scale, 0.0f, 1980.0f);
		param.scale = { scale[0],scale[1] };
	}

	if (ImGui::CollapsingHeader("TexSize"))
	{
		float texSize[] = { param.texSize.x,param.texSize.y };
		ImGui::SliderFloat2("TexSize ->", texSize, 0.0f, 1980.0f);
		param.texSize = { texSize[0],texSize[1] };
	}
	if (ImGui::Button("Add"))
	{
		m_spriteParam.push_back(param);
	}
	ImGui::End();

#endif

}

void UIData::LifeUpdate(float elapsedTime)
{	
	auto& data = m_spriteParam.at(m_instanceNo);
	m_updateTime += elapsedTime;
	if (m_updateTime >= 2.0f)
	{
		data.texSize.x = m_updateValue;
		data.scale.x = data.texSize.x;
		data.currentMaxSize = data.texSize.x;
		m_hasUpdate = false;
		m_updateTime = 0.0f;
		return;
	}

	float offset = m_updateTime / 2.0f;
	data.texSize.x = data.texSize.x - (data.texSize.x - m_updateValue) * offset;
	data.scale.x = data.texSize.x;

}

void UIData::FindValue(float value,const int id)
{
	if (id >= static_cast<int>(m_spriteParam.size())) return;
	auto& data = m_spriteParam.at(id);
	m_updateValue = data.maxTexSizeX * (1.0f - value);
	if (m_updateValue > data.maxTexSizeX)
		m_updateValue = data.maxTexSizeX;
	if (m_updateValue < 0.0f)
		m_updateValue = 0.0f;

	m_instanceNo = id;
	m_updateTime = 0.0f;
}

void UIData::SetSpriteS(UIStaticLabel label)
{
	switch (label)
	{
	case LIFE_GAGE:
		m_sprite = TEXTURELOADER.GetTexture(Source::SpriteLoad::TextureLabel::P_LIFE_GAGE);
		break;
	case COMMAND_F:
		m_sprite = TEXTURELOADER.GetTexture(Source::SpriteLoad::TextureLabel::COMMAND);
		break;
	case NAME_F:
		m_sprite = TEXTURELOADER.GetTexture(Source::SpriteLoad::TextureLabel::FIGHTER);
		break;
	case NAME_E:
		m_sprite = TEXTURELOADER.GetTexture(Source::SpriteLoad::TextureLabel::JUGGERNAUT);
		break;
	case NAME_A:
		m_sprite = TEXTURELOADER.GetTexture(Source::SpriteLoad::TextureLabel::ARCHER);
		break;
	}
	SpriteParam param;
	param.position = { 1980.0f * 0.5f,1080.0f * 0.5f };
	param.texPos = { 0.0f,0.0f };
	float width = static_cast<float>(m_sprite->m_texture2dDesc.Width);
	float height = static_cast<float>(m_sprite->m_texture2dDesc.Height);
	param.scale = { width ,height };
	param.texSize = { width ,height };
	m_spriteParam.emplace_back(param);
}

void UIData::SetSpriteA(UIActLabel label)
{
	switch (label)
	{
	case LIFE_P:
		m_sprite = TEXTURELOADER.GetTexture(Source::SpriteLoad::TextureLabel::P_LIFE);
		break;
	case LIFE_E:
		m_sprite = TEXTURELOADER.GetTexture(Source::SpriteLoad::TextureLabel::E_LIFE);
		break;
	}

	SpriteParam param;
	param.position = { 1980.0f * 0.5f,1080.0f * 0.5f };
	param.texPos = { 0.0f,0.0f };
	float width = static_cast<float>(m_sprite->m_texture2dDesc.Width);
	float height = static_cast<float>(m_sprite->m_texture2dDesc.Height);
	param.scale = { width ,height };
	param.texSize = { width ,height };
	m_spriteParam.emplace_back(param);
}

void UIAdominist::Init()
{
	//m_uiStaticData.;
	//m_uiActData.clear();
	UIData data;
	data.SetSpriteA(UIActLabel::LIFE_P);
	data.Init(EnumToStringA(UIActLabel::LIFE_P));
	m_uiActData.insert(std::make_pair(UIActLabel::LIFE_P, data));

	data.SetSpriteA(UIActLabel::LIFE_E);
	data.Init(EnumToStringA(UIActLabel::LIFE_E));
	m_uiActData.insert(std::make_pair(UIActLabel::LIFE_E, data));

	data.SetSpriteS(UIStaticLabel::LIFE_GAGE);
	data.Init(EnumToStringS(UIStaticLabel::LIFE_GAGE));
	m_uiStaticData.insert(std::make_pair(UIStaticLabel::LIFE_GAGE, data));

	data.SetSpriteS(UIStaticLabel::COMMAND_F);
	data.Init(EnumToStringS(UIStaticLabel::COMMAND_F));
	m_uiStaticData.insert(std::make_pair(UIStaticLabel::COMMAND_F, data));
	data.SetSpriteS(UIStaticLabel::NAME_F);

	data.Init(EnumToStringS(UIStaticLabel::NAME_F));
	m_uiStaticData.insert(std::make_pair(UIStaticLabel::NAME_F, data));

	data.SetSpriteS(UIStaticLabel::NAME_E);
	data.Init(EnumToStringS(UIStaticLabel::NAME_E));
	m_uiStaticData.insert(std::make_pair(UIStaticLabel::NAME_E, data));

	data.SetSpriteS(UIStaticLabel::NAME_A);
	data.Init(EnumToStringS(UIStaticLabel::NAME_A));
	m_uiStaticData.insert(std::make_pair(UIStaticLabel::NAME_A, data));
}

void UIAdominist::Update(float& elapsedTime)
{
	if (m_uiActData.empty()) return;
	for (auto& param : m_uiActData)
	{
		if (param.second.GetHasUpdate())
			param.second.LifeUpdate(elapsedTime);
	}
}

void UIAdominist::Render(ID3D11DeviceContext* immediateContext)
{
	if (!m_uiStaticData.empty())
	{
		for (auto& ui : m_uiStaticData)
		{
			ui.second.Render(immediateContext);
		}
	}
	
	if (!m_uiActData.empty())
	{
		for (auto& ui : m_uiActData)
		{
			ui.second.Render(immediateContext);
		}
	}
}

void UIAdominist::Release()
{
	if (!m_uiStaticData.empty())
	{
		for (auto& ui : m_uiStaticData)
		{
			ui.second.Release();
		}
	}

	if (!m_uiActData.empty())
	{
		for (auto& ui : m_uiActData)
		{
			ui.second.Release();
		}
	}

	m_uiStaticData.clear();
	m_uiActData.clear();
}

void UIAdominist::ImGui()
{
#ifdef _DEBUG

	ImGui::Begin("UIAdominist", nullptr, ImGuiWindowFlags_MenuBar);//メニューバーをつかうならこのBEGIN
	if (ImGui::CollapsingHeader("SetUIData"))
	{		
		static int progress = 0;
		static int labelType = 0;
		static int uiType = 0;
		switch (progress)
		{
		case 0:
		{
			static int selectLabelType = 2;
			ImGui::RadioButton("Static", &selectLabelType, 0); ImGui::SameLine();
			ImGui::RadioButton("Actor" , &selectLabelType, 1); ImGui::SameLine();
			ImGui::RadioButton("NONE"  , &selectLabelType, 2);
			if (selectLabelType != 2)
			{
				ImGui::BulletText("Have you selected label?"); ImGui::SameLine();
				if (ImGui::Button("Yes"))
				{
					labelType = selectLabelType;
					selectLabelType = 2;
					++progress;
				}ImGui::SameLine();
				if (ImGui::Button("No"))
					selectLabelType = 2;
			}
		}
		break;
		case 1:
		{
			static int selectUI = 2;
			if (labelType == 0)
				ImGui::Combo("UI_NAME", &selectUI, "LIFE_GAGE\0COMMAND_F\0NAME_F\0NAME_E\0NAME_A\0END\0\0");
			else
				ImGui::Combo("UI_NAME", &selectUI, "LIFE_P\0LIFE_E\0END\0\0");
			
			if (selectUI != 5 && labelType == 0 || selectUI != 2 && labelType == 1)
			{
				ImGui::BulletText("Have you selected UI?"); ImGui::SameLine();
				if (ImGui::Button("Yes"))
				{
					uiType = selectUI;
					selectUI = 2;
					++progress;
				}ImGui::SameLine();
				if (ImGui::Button("No"))
					selectUI = 2;
			}
		}
		break;
		case 2:
		{
			if (labelType == 0)
			{
				UIStaticLabel label = static_cast<UIStaticLabel>(uiType);
				UIData data;
				data.SetSpriteS(label);
				data.Init(EnumToStringS(label));
				m_uiStaticData.insert(std::make_pair(label, data));

			}
			else
			{
				UIActLabel label = static_cast<UIActLabel>(uiType);
				UIData data;
				data.SetSpriteA(label);
				data.Init(EnumToStringA(label));
				m_uiActData.insert(std::make_pair(label, data));
			}

			ImGui::Text("The set of Sprite is complete!");
			if (ImGui::Button("close"))
				progress = 0;
		}
		break;
		}
	}

	if (ImGui::CollapsingHeader("PlacementOfSprite"))
	{
		static int progress = 0;
		static int selectUI = 0;
		switch (progress)
		{
		case 0:
		{
			static int selectLabelType = 2;
			ImGui::RadioButton("Static", &selectLabelType, 0); ImGui::SameLine();
			ImGui::RadioButton("Actor", &selectLabelType, 1); ImGui::SameLine();
			ImGui::RadioButton("NONE", &selectLabelType, 2);
			if (selectLabelType != 2)
			{
				ImGui::BulletText("Have you selected label?"); ImGui::SameLine();
				if (ImGui::Button("Yes"))
				{
					int next = selectLabelType == 0 ? 1 : 2;
					selectLabelType = 2;
					progress = next;
				}ImGui::SameLine();
				if (ImGui::Button("No"))
					selectLabelType = 2;
			}
		}
		break;
		case 1:
		{
			if (m_uiStaticData.empty())
			{
				ImGui::Text("Sprite does not exist");
				if (ImGui::Button("Close"))
					progress = 0;
			}
			else
			{
				ImGui::Combo("UI_NAME", &selectUI, "LIFE_GAGE\0COMMAND_F\0NAME_F\0NAME_E\0NAME_A\0END\0\0");
				ImGui::Text("Have you selected UI?");
				static bool hasStarted = false;
				if (ImGui::Button("Yes"))
				{
					hasStarted = true;
				}
				if (hasStarted)
				{
					std::map<UIStaticLabel, UIData>::iterator object = m_uiStaticData.find(static_cast<UIStaticLabel>(selectUI));
					if (object != m_uiStaticData.end())
					{
						object->second.ImGui();
					}
					else
						ImGui::Text("UILabel does not exist");

					if (ImGui::Button("Close"))
						hasStarted = false;
				}

				if (ImGui::Button("Return"))
					progress = 0;
			}
		}
		break;
		case 2:
		{
			if (m_uiActData.empty())
			{
				ImGui::Text("Sprite does not exist");
				if (ImGui::Button("Close"))
					progress = 0;
			}
			else
			{
				ImGui::Combo("UI_NAME", &selectUI, "LIFE_P\0LIFE_E\0END\0\0");
				ImGui::Text("Have you selected UI?");
				static bool hasStarted = false;
				if (ImGui::Button("Yes"))
				{
					hasStarted = true;
				}

				if (hasStarted)
				{
					std::map<UIActLabel, UIData>::iterator object = m_uiActData.find(static_cast<UIActLabel>(selectUI));
					if (object != m_uiActData.end())
						object->second.ImGui();
					else
						ImGui::Text("UILabel does not exist");
				}

				if (ImGui::Button("Close"))
				{
					hasStarted = false;
				}
			}

			if (ImGui::Button("Return"))
				progress = 0;
		}
		break;
		}
	}

	if (ImGui::CollapsingHeader("Damge"))
	{	
		static int selectUI = 2;
		ImGui::Combo("UI_NAME", &selectUI, "LIFE_P\0LIFE_E\0END\0\0");

		float maxSize = selectUI == 0 ? 100.0f : 700.0f;
		static float currentSize = 0.0f;;
		static float damageOffset = 10.0f;
		ImGui::SliderFloat("DamageValue", &damageOffset, -maxSize, maxSize);
		if(ImGui::Button("Size"))
			currentSize = selectUI == 0 ? 100.0f : 700.0f;
		ImGui::Text("CurrentSize->", currentSize);

		static int select = 0;
		ImGui::SliderInt("PorA", &select, 0, 1);


		if (ImGui::Button("RUN"))
		{
			currentSize -= damageOffset;
			LifeUpdate(static_cast<UIActLabel>(selectUI), (maxSize - currentSize) / maxSize, select);
		}
	}

	ImGui::End();

#endif
}
