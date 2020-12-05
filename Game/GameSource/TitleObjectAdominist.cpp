#include "TitleObjectAdominist.h"
#include ".\LibrarySource\ModelData.h"

void TitleObjectData::Init(std::string name)
{
	m_worldData.WorldClear();
	m_name = name;
	if (PathFileExistsA((std::string("../Asset/Binary/Title/")+ name + ".bin").c_str()))
	{
		std::ifstream ifs;
		ifs.open((std::string("../Asset/Binary/Title/") + name + ".bin").c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);
	}
}

void TitleObjectData::Update(float elapsedTime)
{
	if (m_actorModel)
	{
		m_animationBlend.ChangeSampler(0, 1, m_actorModel);
		m_animationBlend.Update(m_actorModel, elapsedTime);
	}

	if (m_isAction)
	{
		if (m_sprite)
		{
			m_spriteData.m_dissolve->data.dissolveGlowAmoument+=0.025f;
			if (m_spriteData.m_dissolve->data.dissolveGlowAmoument >= 1.0f)
			{
				m_spriteData.m_dissolve->data.dissolveGlowAmoument = 1.0f;
				m_isActionEnd = true;
			}
		}
	}
}

void TitleObjectData::Render(ID3D11DeviceContext* immediateContext)
{
	VECTOR4F color{ 1.0f,1.0f,1.0f,1.0f };
	if (m_actorModel)
	{
		auto& localTransforms = m_animationBlend._blendLocals;
		m_actorModel->Render(immediateContext, m_worldData.world,color, localTransforms,1);
	}

	if (m_staticModel)
	{
		m_staticModel->_shaderON.normalMap = true;
		m_staticModel->Render(immediateContext, m_worldData.world, color);
	}

	if (m_sprite)
	{
		m_spriteData.m_dissolve->Activate(immediateContext, SLOT5, true, true);

		m_sprite->Begin(immediateContext);
		m_sprite->RenderCenter(m_spriteData.position, m_spriteData.scale,
			m_spriteData.texPos, m_spriteData.texSize, 0, VECTOR4F(1.0, 1.0, 1.0, 1.0), false);
		m_sprite->End(immediateContext);

		m_spriteData.m_dissolve->Deactivate(immediateContext);

	}
}

void TitleObjectData::Release()
{
	if (m_actorModel)
	{
		if (m_actorModel.unique())
		{
			m_actorModel.reset();
		}
	}

	if (m_staticModel)
	{
		if (m_staticModel.unique())
		{
			m_staticModel.reset();
		}
	}

	if (m_sprite)
	{
		m_spriteData.SpriteClear();
		m_sprite.reset();
	}

	m_worldData.WorldClear();

}

void TitleObjectData::ImGui(ID3D11Device* device)
{
#ifdef _DEBUG
	ImGui::Begin("Object", nullptr, ImGuiWindowFlags_MenuBar);//メニューバーをつかうならこのBEGIN

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))//ファイルの中には
		{
			if (ImGui::MenuItem("Save"))//データの保存とか
			{
				std::ofstream ofs;
				ofs.open((std::string("../Asset/Binary/Title/") + m_name + ".bin").c_str(), std::ios::binary);
				cereal::BinaryOutputArchive o_archive(ofs);
				o_archive(*this);
			}

			if (ImGui::MenuItem("Load"))
			{
				if (PathFileExistsA((std::string("../Asset/Binary/Title/") + m_name + ".bin").c_str()))
				{
					std::ifstream ifs;
					ifs.open((std::string("../Asset/Binary/Enemy/Parameter") + ".bin").c_str(), std::ios::binary);
					cereal::BinaryInputArchive i_archive(ifs);
					i_archive(*this);
				}
			}
			ImGui::EndMenu();

		}
		ImGui::EndMenuBar();
	}

	if (m_actorModel || m_staticModel)
	{
		float angle[3] = { m_worldData.angle.x,m_worldData.angle.y,m_worldData.angle.z };
		float scale[3] = { m_worldData.scale.x,m_worldData.scale.y,m_worldData.scale.z };
		float position[3] = { m_worldData.position.x,m_worldData.position.y,m_worldData.position.z };
		ImGui::SliderFloat3("Angle", angle, -180.0f * 0.01745f, 180.0f * 0.01745f);
		ImGui::SliderFloat3("Scale", scale, -1.0f, 13.0f);
		ImGui::SliderFloat3("Position", position, -50.0f, 50.0f);

		m_worldData.angle = { angle[0],angle[1],angle[2] };
		m_worldData.scale = { scale[0],scale[1],scale[2] };
		m_worldData.position = { position[0],position[1],position[2] };

		m_worldData.WorldUpdate();
	}

	if (m_actorModel)
	{
		auto animCount = m_actorModel->_resource->_animationTakes.size();

		static int currentAnim = 0;
		ImGui::BulletText("%d", currentAnim);
		ImGui::SameLine();
		if (ImGui::ArrowButton("Front", ImGuiDir_Left))
		{
			if (0 >= currentAnim)
				currentAnim = 0;
			else
				--currentAnim;
		}
		ImGui::SameLine();
		if (ImGui::ArrowButton("Next", ImGuiDir_Right))
		{
			if (currentAnim >= static_cast<int>(animCount) - 1)
				currentAnim = static_cast<int>(animCount) - 1;
			else
				++currentAnim;
		}

		if (ImGui::Button(u8"ChangeAnim"))
		{
			m_animationBlend.ChangeSampler(0, currentAnim, m_actorModel);
		}
		
		float speed = m_animationBlend._animationSpeed;
		ImGui::SliderFloat("AnimationSpeed", &speed, 0.0f, 2.0f);

		m_animationBlend.SetAnimationSpeed(speed);

	}

	if (m_sprite)
	{
		ImGui::InputFloat("pos X", &m_spriteData.position.x, 0.f, 1980.0f, "%.4f");
		ImGui::InputFloat("pos Y", &m_spriteData.position.y, 0.f, 1080.0f, "%.4f");

		ImGui::InputFloat("scale X", &m_spriteData.scale.x, 0.f, 2.0f, "%.4f");
		ImGui::InputFloat("scale Y", &m_spriteData.scale.y, 0.f, 2.0f, "%.4f");

		ImGui::InputFloat("texPos X", &m_spriteData.texPos.x, 0.f, 640, "%.4f");
		ImGui::InputFloat("texPos Y", &m_spriteData.texPos.y, 0.f, 640, "%.4f");

		ImGui::InputFloat("texSize X", &m_spriteData.texSize.x, 0.f, 640, "%.4f");
		ImGui::InputFloat("texSize Y", &m_spriteData.texSize.y, 0.f, 640, "%.4f");
		if (ImGui::CollapsingHeader("Dissolve"))
		{
			//--------------
			// GlowAmoument
			//--------------
			static float glowAmoument = 0.5f;
			ImGui::SliderFloat("GlowAmoument", &glowAmoument, -1.0f, 1.0f);
			m_spriteData.m_dissolve->data.dissolveGlowAmoument = glowAmoument;

			//--------------
			// GlowRange
			//--------------
			static float glowRange = 0.1f;
			ImGui::SliderFloat("GlowRange", &glowRange, -1.0f, 1.0f);
			m_spriteData.m_dissolve->data.dissolveGlowRange = glowRange;

			//-----------------
			// GlowFalloff
			//-----------------
			static float glowFalloff = 0.1f;
			ImGui::SliderFloat("GlowFalloff", &glowFalloff, -1.0f, 1.0f);
			m_spriteData.m_dissolve->data.dissolveGlowFalloff = glowFalloff;

			//------------
			// GlowColor
			//------------
			static float glowColor[4] = { 1.f, 1.f, 1.f, 1.f };
			ImGuiColorEditFlags flag = ImGuiColorEditFlags_Float; // 0 ~ 255表記ではなく、0.0 ~ 1.0表記にします。
			ImGui::ColorEdit4("GlowColor", glowColor, flag);
			m_spriteData.m_dissolve->data.dissolveGlowColor = VECTOR4F(glowColor[0], glowColor[1], glowColor[2], glowColor[3]);

			//------------
			// Emission
			//------------
			static float emission[4] = { 1.f, 1.f, 1.f, 1.f };
			ImGuiColorEditFlags flagE = ImGuiColorEditFlags_Float; // 0 ~ 255表記ではなく、0.0 ~ 1.0表記にします。
			ImGui::ColorEdit4("Emission", emission, flagE);
			m_spriteData.m_dissolve->data.dissolveEmission = VECTOR4F(emission[0], emission[1], emission[2], emission[3]);
		}
	}

	ImGui::End();
#endif
}

void TitleObjectData::SetActModel(ObjectElement element)
{
	switch (element)
	{
	case FIGHTER:
	{
		m_actorModel = Source::ModelData::fbxLoader().GetActorModel(Source::ModelData::ActorModel::Fighter);
		VECTOR3F scale = { 0.005f,0.005f,0.005f };
		m_worldData.scale = scale;
	}
		break;
	case ARCHER:
	{
		m_actorModel = Source::ModelData::fbxLoader().GetActorModel(Source::ModelData::ActorModel::Archer);
		VECTOR3F scale = { 0.005f,0.005f,0.005f };
		m_worldData.scale = scale;
	}
	break;
	case JUGGERNAUT:
	{
		m_actorModel = Source::ModelData::fbxLoader().GetActorModel(Source::ModelData::ActorModel::ENEMY);

		VECTOR3F scale = { 0.08f,0.08f,0.08f };
		m_worldData.scale = scale;
	}
	break;
	default:
		break;
	}
	m_animationBlend.Init(m_actorModel);
	m_worldData.WorldUpdate();
}

void TitleObjectData::SetStaticModel(ObjectElement element)
{
	switch (element)
	{
	case STONE:
		m_staticModel = Source::ModelData::fbxLoader().GetStaticModel(Source::ModelData::StaticModel::STONE);
		break;
	case STAGE:
		m_staticModel = Source::ModelData::fbxLoader().GetStaticModel(Source::ModelData::StaticModel::STAGE);
		break;
	default:
		break;
	}
}

void TitleObjectData::SetSprite()
{
	m_sprite = TEXTURELOADER.GetTexture(Source::SpriteLoad::TextureLabel::TITLE);
	m_spriteData.m_dissolve = std::make_unique<Source::ConstantBuffer::ConstantBuffer<Source::Constants::Dissolve>>(Framework::GetInstance().GetDevice());
	m_spriteData.m_dissolve->data.dissolveGlowAmoument = 0.0f;
	m_spriteData.m_dissolve->data.dissolveGlowRange = 1.0f;
	m_spriteData.m_dissolve->data.dissolveGlowFalloff = 1.0f;
	m_spriteData.m_dissolve->data.dissolveGlowColor = VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f);
	m_spriteData.m_dissolve->data.dissolveEmission = VECTOR4F(0.0f, 0.0f, 0.0f, 0.0f);
}

void TitleObjctAdominist::init()
{

}

void TitleObjctAdominist::Update(float elapsedTime)
{
	if (m_objects.empty()) return;
	for (auto& object : m_objects)
	{
		if(object.second)
			object.second->Update(elapsedTime);
	}
}

void TitleObjctAdominist::Render(ID3D11DeviceContext* immediateContext, ObjectElement element)
{
	if (m_objects.empty()) return;
	std::map<ObjectElement, std::unique_ptr<TitleObjectData>>::iterator object = m_objects.find(element);
	if (object != m_objects.end())
	{
		if (object->second)
			object->second->Render(immediateContext);
	}
}

void TitleObjctAdominist::Release()
{
	if (m_objects.empty()) return;
	for (auto& object : m_objects)
	{
		if (object.second)
			object.second->Release();
	}
}

void TitleObjctAdominist::ImGui(ObjectElement objElement,ID3D11Device* device)
{
#ifdef _DEBUG
	
	for (auto& object : m_objects)
	{
		if (!object.second) continue;
		if (object.first == objElement)
			object.second->ImGui(device);
	}

#endif
}

void TitleObjctAdominist::SetObject(ObjectElement element, ObjectType type)
{

	std::string name = EnumToString(element);
	std::unique_ptr<TitleObjectData> object = std::make_unique<TitleObjectData>(name);

	switch (type)
	{
	case ACTOR:
		object->SetActModel(element);
		break;
	case STATIC:
		object->SetStaticModel(element);
		break;
	case SPRITE:
		object->SetSprite();
		break;
	default:
		break;
	}

	m_objects.insert(std::make_pair(element, std::move(object)));
}
