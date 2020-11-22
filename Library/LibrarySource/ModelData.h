#pragma once
#include <memory>
#include "d3d11.h"
#include "Vector.h"
#include "SkinnedMesh.h"
#include "StaticMesh.h"

namespace Source
{
	namespace ModelData
	{
		enum class ActorModel
		{
			Saber,
			Archer,
			Fighter,
			ENEMY,
			END,
		};

		enum class StaticModel
		{
			STAGE,
			ARROW,
			STONE,
			END,
		};


		struct LoadActorModel
		{
			ActorModel  fbxNum;
			const char* fileName;
			const int    samplingTime;
		};

		struct LoadStaticModel
		{
			StaticModel	fbxNum;
			const char* fileName;
		};


		class FbxLoader
		{
		public:
			FbxLoader() = default;
			~FbxLoader() = default;

			void SaveActForBinary(ActorModel modelNo);
			void SaveStaticForBinary(StaticModel modelNo);

			void Release()
			{
				for (int i = 0; i < static_cast<int>(ActorModel::END); ++i)
				{
					if (m_actorModel[i])
					{
						if(m_actorModel[i].unique())
							m_actorModel[i].reset();
					}
				}
				for (int i = 0; i < static_cast<int>(StaticModel::END); ++i)
				{
					if (m_staticModel[i])
					{
						if (m_staticModel[i].unique())
							m_staticModel[i].reset();
					}
				}
			};

			std::shared_ptr<Source::SkinnedMesh::SkinnedMesh>& GetActorModel(ActorModel modelNo);
			std::shared_ptr<Source::StaticMesh::StaticMesh>& GetStaticModel(StaticModel modelNo);

		private:
			std::shared_ptr<Source::SkinnedMesh::SkinnedMesh> m_actorModel[static_cast<int>(ActorModel::END)];
			std::shared_ptr<Source::StaticMesh::StaticMesh> m_staticModel[static_cast<int>(StaticModel::END)];
		};


		inline Source::ModelData::FbxLoader& fbxLoader()
		{
			static 	Source::ModelData::FbxLoader fbxLoder;
			return fbxLoder;
		}
	}
}
