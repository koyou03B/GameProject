#include "Stage.h"
#include ".\LibrarySource\ModelData.h"

void Stage::Init()
{
	VECTOR3F translate = { 0.0f,0.0f,0.0f };
	VECTOR3F angle = { 0.0f * 0.01745f, 180.0f * 0.01745f,0.0f * 0.017454f };
	VECTOR3F scale = { 0.008f,0.008f,0.008f };
	
	AddInstanceData(translate, angle, scale, VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));
	m_model = Source::ModelData::fbxLoader().GetStaticModel(Source::ModelData::StaticModel::STAGE);
	Source::ModelData::fbxLoader().SaveStaticForBinary(Source::ModelData::StaticModel::STAGE);
	m_model->_shaderON.normalMap = true;
}

void Stage::Render(ID3D11DeviceContext* immediateContext)
{
	if (!m_instanceData.empty())
	{
		m_model->Render(Framework::GetContext(), m_instanceData, VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));
	}
}

void Stage::ImGui()
{
#ifdef _DEBUG

	ImGui::Begin("Stage", nullptr, ImGuiWindowFlags_MenuBar);//メニューバーをつかうならこのBEGIN

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))//ファイルの中には
		{

			//if (ImGui::MenuItem("Save"))//データの保存とか
			//{
			//	std::ofstream ofs;
			//	ofs.open((std::string("./Data/Bin/player") + ".bin").c_str(), std::ios::binary);
			//	cereal::BinaryOutputArchive o_archive(ofs);
			//	o_archive(*this);
			//}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (!m_instanceData.empty())
	{
		auto& data = m_instanceData[0];
		static float scale = 0.008f;
		ImGui::SliderFloat("Scale", &scale, 0.001f, 1.0f);

		data.scale = VECTOR3F(scale, scale, scale);
		data.CreateWorld();

	}


	ImGui::End();

#endif


}
