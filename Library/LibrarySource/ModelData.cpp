#include "ModelData.h"

namespace Source
{
	namespace ModelData
	{
		//	{ TEXNO::,"Data/Texture/.png","../Asset/Texture/Noise.png",1U },
		LoadActorModel actorModel[] =
		{
			{ ActorModel::Saber,	"../Asset/Model/Actor/Test/player_motion_tex.fbx", 60},
			{ ActorModel::Archer,	"../Asset/Model/Actor/Players/Archer/Archer.fbx", 60},
			{ ActorModel::Fighter,	"../Asset/Model/Actor/Players/Fighter/Fighter.fbx",   60},
			{ ActorModel::ENEMY,	"../Asset/Model/Actor/Enemy/Juggernaut.fbx", 60},
		};

		LoadStaticModel staticModel[] = {

			{ StaticModel::STAGE,	"../Asset/Model/Static/Stage/Stage.fbx"},
			{ StaticModel::ARROW,	"../Asset/Model/Static/Arrow/Arrow.fbx"},
			{ StaticModel::AXE,		"../Asset/Model/Static/Axe/BattleAxe_GEO.fbx"},

		};

		void FbxLoader::SaveActForBinary(ActorModel modelNo)
		{
			int num = static_cast<int>(modelNo);
			if (m_actorModel[num] != nullptr)
			{
				m_actorModel[num]->_resource->SaveForBinary(actorModel[num].fileName);
			}

		}

		void FbxLoader::SaveStaticForBinary(StaticModel modelNo)
		{
			int num = static_cast<int>(modelNo);
			if (m_staticModel[num] != nullptr)
			{
				m_staticModel[num]->_resource->SaveForBinary(staticModel[num].fileName);
			}
		}

		std::shared_ptr<Source::SkinnedMesh::SkinnedMesh>& FbxLoader::GetActorModel(ActorModel modelNo)
		{
			int num = static_cast<int>(modelNo);
			if (m_actorModel[num] == nullptr)
			{
				m_actorModel[num] = std::make_shared<Source::SkinnedMesh::SkinnedMesh>(Framework::GetInstance().GetDevice(), 
					actorModel[num].fileName, actorModel[num].samplingTime);
			}
			return m_actorModel[num];

		}
		std::shared_ptr<Source::StaticMesh::StaticMesh>& FbxLoader::GetStaticModel(StaticModel modelNo)
		{
			int num = static_cast<int>(modelNo);
			if (m_staticModel[num] == nullptr)
			{
				m_staticModel[num] = std::make_shared<Source::StaticMesh::StaticMesh>(Framework::GetInstance().GetDevice(), staticModel[num].fileName);
			}
			return m_staticModel[num];
		}
	}
}
