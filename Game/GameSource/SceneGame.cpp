#include "SceneGame.h"
#include "MessengTo.h"
#include ".\LibrarySource\Framework.h"
#include ".\LibrarySource\VectorCombo.h"
#include ".\LibrarySource\GeometricPrimitve.h"


bool Game::Initialize(ID3D11Device* device)
{
	{
		m_sceneConstantBuffer = std::make_unique<Source::ConstantBuffer::ConstantBuffer<Source::Constants::SceneConstants>>(device);
		m_sceneConstantBuffer->data.directionalLight.direction = { 0.0f, -1.0f, 1.0f, 0.0f };
		m_sceneConstantBuffer->data.ambientColor = { 0.147f,0.147f,0.147f,1.0f };
	}


	{
		m_metaAI = std::make_shared<MetaAI>();
		m_metaAI->Init();
		m_stage.Init();
		MESSENGER.GetInstance().SetMetaAI(m_metaAI);
	}


	{
		Source::CameraControlle::CameraManager().GetInstance()->Initialize(device);

		VECTOR3F distance = DistancePlayerToEnemy();
		Source::CameraControlle::CameraManager().GetInstance()->SetOldDistance(distance);
		Source::CameraControlle::CameraManager().GetInstance()->SetDistance(distance);
		CharacterAI* player = m_metaAI->GetPlayCharacter();
		VECTOR3F pos = player->GetWorldTransform().position;
		Source::CameraControlle::CameraManager().GetInstance()->SetObject(pos);
		Source::CameraControlle::CameraManager().GetInstance()->SetLength(player->GetCamera().lenght);
		Source::CameraControlle::CameraManager().GetInstance()->SetValue(player->GetCamera().value);
		Source::CameraControlle::CameraManager().GetInstance()->SetFocalLength(player->GetCamera().focalLength);
		Source::CameraControlle::CameraManager().GetInstance()->SetHeightAboveGround(player->GetCamera().heightAboveGround);
		CharacterAI* enemy = &(*m_metaAI->GetEnemys()[0]);
		pos = enemy->GetWorldTransform().position;
		Source::CameraControlle::CameraManager().GetInstance()->SetTarget(pos);
		Source::CameraControlle::CameraManager().GetInstance()->SetOldTarget(pos);

		float tmp = 0;
		Source::CameraControlle::CameraManager().GetInstance()->Update(tmp);
	}


	return true;
}

void Game::Update(float& elapsedTime)
{	
	VECTOR3F distance = DistancePlayerToEnemy();
	distance = NormalizeVec3(distance);
	Source::CameraControlle::CameraManager().GetInstance()->SetDistance(distance);
	Source::CameraControlle::CameraManager().GetInstance()->Update(elapsedTime);

	if (Source::CameraControlle::CameraManager().GetInstance()->GetCameraMode() != 
		Source::CameraControlle::CameraManager().GetInstance()->CHANGE_OBJECT)
	{


		CharacterAI* player = m_metaAI->GetPlayCharacter();
		VECTOR3F pos = player->GetWorldTransform().position;
		Source::CameraControlle::CameraManager().GetInstance()->SetObject(pos);
		CharacterAI* enemy = &(*m_metaAI->GetEnemys()[0]);
		pos = enemy->GetWorldTransform().position;
		Source::CameraControlle::CameraManager().GetInstance()->SetTarget(pos);

		m_metaAI->UpdateOfPlayers(elapsedTime);
		m_metaAI->UpdateOfEnemys(elapsedTime);
	}


	//GetEntityManager().Update(elapsedTime);

//	if (GetEntityManager().FindEntity(2).HasComponent<StaticModel>())
//	{
//#if 0
//		auto& terrain = GetEntityManager().FindEntity("TERRAIN").FindComponent<StaticModel>();
//		auto& player = GetEntityManager().FindEntity("PLAYER").FindComponent<WorldTransform>();
//		VECTOR3F position = player.GetTranslate();
//		VECTOR3F intersection,hitNormal;
//		FLOAT4X4 worldTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
//		int ret = 0;
//		const char* material_name = 0;
//		const float raycast_lift_up = 10.0f;
//		ret = terrain.RayPick(VECTOR3F(position.x, position.y + raycast_lift_up, position.z), VECTOR3F(position.x, position.y - raycast_lift_up, position.z),
//			&intersection, &hitNormal);
//
//		if (ret != -1)
//		{
//
//			position.x = intersection.x;
//			position.y = intersection.y;
//			position.z = intersection.z;
//
//			player.SetTranslate(position);
//			player.WorldUpdate();
//
//		}
//#else
//		auto& staticModelWorldTrandform = GetEntityManager().FindEntity(2).FindComponent<StaticModel>().GetInstanceData();
//		auto& staticModel = GetEntityManager().FindEntity(2).FindComponent<StaticModel>().GetStaticModel();
//		auto& player = GetEntityManager().FindEntity(0).FindComponent<WorldTransform>();
//		VECTOR3F position = player.GetTranslate();
//		VECTOR3F intersection;
//		int ret = 0;
//		const char* material_name = 0;
//		const float raycast_lift_up = 10.0f;
//
//
//
//		material_name = staticModel->RayTrianglesIntersectionDownward(VECTOR3F(position.x, position.y + raycast_lift_up, position.z),
//			staticModelWorldTrandform.at(0).world,&intersection);
//		if (material_name != "")
//		{
//			position.x = intersection.x;
//			position.y = intersection.y;
//			position.z = intersection.z;
//
//			player.SetTranslate(position);
//			player.WorldUpdate();
//		}
//#endif
//
//	}


}



void Game::Render(ID3D11DeviceContext* immediateContext, float elapsedTime)
{
	VECTOR4F lightDirection(0, -1, 1, 0);

	Source::CameraControlle::CameraManager().GetInstance()->Activate(immediateContext);
	m_sceneConstantBuffer->Activate(immediateContext, SLOT2, true, true);

	m_metaAI->RenderOfEnemy(immediateContext, 0);

	m_metaAI->RenderOfPlayer(immediateContext, 0);
	m_metaAI->RenderOfPlayer(immediateContext, 1);
	m_metaAI->RenderOfPlayer(immediateContext, 2);

	m_stage.Render(immediateContext);

	m_sceneConstantBuffer->Deactivate(immediateContext);
	Source::CameraControlle::CameraManager().GetInstance()->Deactivate(immediateContext);

}

void Game::ImGui()
{
#ifdef _DEBUG
	ImGui::Begin("SCENE_GAME");
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

	if (ImGui::CollapsingHeader("PointLight"))
	{
		//-----------
		// Position
		//-----------
		CharacterAI* player = m_metaAI->GetPlayCharacter();
		VECTOR3F playerPosition = player->GetWorldTransform().position;
		 float position[4] = { playerPosition.x, 0.0f, playerPosition.z, 20.0f };
		ImGui::DragFloat4("Position", position, 0.1f);
		m_sceneConstantBuffer->data.pointLight.position = VECTOR4F(position[0], position[1], position[2], position[3]);

		//--------
		// Color
		//--------
		static float color[4] = { 228.f / 255.f, 200.f / 255.f, 148.f / 255.f, 1.f / 4.f };
		ImGuiColorEditFlags flag = ImGuiColorEditFlags_Float; // 0 ~ 255表記ではなく、0.0 ~ 1.0表記にします。
		ImGui::ColorEdit4("PointLightColor", color, flag);
		m_sceneConstantBuffer->data.pointLight.color = VECTOR4F(color[0], color[1], color[2], color[3]);

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

	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::SetNextWindowSize(ImVec2(400, Framework::GetInstance().SCREEN_HEIGHT), ImGuiSetCond_Once);//サイズ
		ImGui::SetNextWindowPos(ImVec2(1520, 0), ImGuiSetCond_Once);//ポジション
		ImGui::Begin("CameraEditer");
		Source::CameraControlle::CameraManager().GetInstance()->Editor();
		ImGui::End();
	}

	//ImGui::SetNextTreeNodeOpen(true, ImGuiSetCond_Once);
	//if (ImGui::CollapsingHeader("PLAYER"))
	//{
	//	auto& player = GetEntityManager().FindEntity(0);
	//	if (ImGui::TreeNode("AnimationBlend"))
	//	{
	//		//auto& blend = player.FindComponent<AnimationBlend>();
	//		//if (ImGui::TreeNode("BlendRatio"))
	//		//{
	//		//	ImGui::SliderFloat("Ratio", &blend._blendRatio, 0.0f, 1.0f);
	//		//	ImGui::TreePop();
	//		//}
	//		//if (ImGui::TreeNode("AnimationBlend"))
	//		//{
	//		//	auto& actModel = player.FindComponent<ActorModel>();
	//		//	static int curringAnimation = 0;
	//		//	ImGui::Combo("Name_of_Model's_Animation",
	//		//		&curringAnimation,
	//		//		vectorGetter,
	//		//		static_cast<void*>(&actModel.GetActorModel()->_modelConfig.animationName),
	//		//		static_cast<int>(actModel.GetActorModel()->_modelConfig.animationName.size())
	//		//		);
	//		//	std::vector<std::string> boneParentName;
	//		//	for (auto m : actModel.GetActorModel()->_resource->_meshes.at(0).offsetTransforms)
	//		//	{
	//		//		boneParentName.push_back(m.parentName);
	//		//	}
	//		//	static int boneName = 0;
	//		//	ImGui::Combo("Name_of_BoneParentName",
	//		//		&boneName,
	//		//		vectorGetter,
	//		//		static_cast<void*>(&boneParentName),
	//		//		static_cast<int>(boneParentName.size())
	//		//	);
	//		//	auto BoneName = blend.GetBoneName().at(0);
	//		//	static int boneName2 = 0;
	//		//	ImGui::Combo("Name_of_BoneName",
	//		//		&boneName2,
	//		//		vectorGetter,
	//		//		static_cast<void*>(&BoneName),
	//		//		static_cast<int>(BoneName.size())
	//		//		);
	//		//	int num0, num1 = -1;
	//		//	num0 = blend.GetSampler().front().first;
	//		//	ImGui::BulletText(u8"現在のBlendAnimation %s", actModel.GetActorModel()->_modelConfig.animationName[num0].c_str());
	//		//	if (blend.GetSampler().size() == 2)
	//		//	{
	//		//		num1 = blend.GetSampler().back().first;
	//		//		if (num1 != -1)
	//		//			ImGui::BulletText(u8"現在のBlendAnimation %s", actModel.GetActorModel()->_modelConfig.animationName[num1].c_str());
	//		//	}
	//		//	{
	//		//		if (ImGui::Button("Add Animation ?"))
	//		//			blend.AddSampler(curringAnimation);
	//		//		ImGui::SameLine();
	//		//		static bool isChangeAnim = false;
	//		//		if (ImGui::Button("Change Animation ?"))
	//		//			isChangeAnim = true;
	//		//		if (isChangeAnim)
	//		//		{
	//		//			if (ImGui::Button("FRONT"))
	//		//			{
	//		//				blend.ChangeSampler(curringAnimation);
	//		//				isChangeAnim = false;
	//		//			}
	//		//			ImGui::SameLine();
	//		//			if (ImGui::Button("BACK"))
	//		//			{
	//		//				blend.ChangeSampler(curringAnimation,false);
	//		//				isChangeAnim = false;
	//		//			}
	//		//		}
	//		//		if (ImGui::Button("Release Animation ?"))
	//		//			blend.ReleaseSampler();
	//		//	}
	//		//	ImGui::TreePop();
	//		//}
	//		ImGui::TreePop();
	//	}
	//	if (ImGui::TreeNode("Bone"))
	//	{
	//		//if (ImGui::TreeNode("Wepon"))
	//		//{
	//		//	auto& blend = player.FindComponent<AnimationBlend>();
	//		//	
	//		//	std::vector<std::vector<std::string>> blendBoneName = blend.GetBoneName();
	//		//	static int curringBone = 0;
	//		//	ImGui::Combo("Name_of_Bone's_Animation",
	//		//		&curringBone,
	//		//		vectorGetter,
	//		//		static_cast<void*>(&blendBoneName[0]),
	//		//		static_cast<int>(blendBoneName[0].size())
	//		//		);
	//		//	FLOAT4X4 blendBone = blend._blendLocals[0].at(curringBone);
	//		//	FLOAT4X4 blendBoneToParent = blend._blendLocalsToParents[0].at(curringBone);
	//		//	FLOAT4X4 modelWorld = player.FindComponent<WorldTransform>().GetWorld();
	//		//	FLOAT4X4 modelAxisTransform = player.FindComponent<ActorModel>().GetActorModel()->_resource->axisSystemTransform;
	//		//	FLOAT4X4 meshOffsetTransform = player.FindComponent<ActorModel>().GetActorModel()->_resource->_meshes[0].offsetTransforms[curringBone].transform;
	//		//	FLOAT4X4 getBone =  blendBone * modelWorld * modelAxisTransform;
	//		//	static bool isAddComponent = false;
	//		//	if (ImGui::Button("Add Component?"))
	//		//		isAddComponent = true;
	//		//	if (isAddComponent)
	//		//	{
	//		//		if (player.HasComponent<Geometry>())
	//		//			isAddComponent = false;
	//		//		player.AddComponent<Geometry>();
	//		//		player.FindComponent<Geometry>().AddGeometricPrimitive(std::make_unique<Source::GeometricPrimitive::GeometricSphere>((Framework::GetInstance().GetDevice())));
	//		//		player.FindComponent<Geometry>().AddInstanceData(VECTOR3F(0.0f, 0.0f, 0.0f), VECTOR3F(0.0f, 0.0f, 0.0f),
	//		//			VECTOR3F(2.0f, 2.0f, 2.0f), VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));
	//		//		isAddComponent = false;
	//		//	}
	//		//	auto& sword = GetEntityManager().FindEntity(1);
	//		//	auto& swordData =  sword.FindComponent<StaticModel>().GetInstanceData();
	//		//	VECTOR3F bonePos = { getBone._41 ,getBone._42 ,getBone._43 };
	//		//	swordData[0].position = bonePos;
	//		//	swordData[0].CreateWorld();
	//		//	if (player.HasComponent<Geometry>())
	//		//	{
	//		//		auto& GeometryData = player.FindComponent<Geometry>().GetInstanceData(0);
	//		//		VECTOR3F boneScale= { getBone._41 ,getBone._42 ,getBone._43 };
	//		//		VECTOR3F boneAngle = { getBone._41 ,getBone._42 ,getBone._43 };
	//		//		VECTOR3F bonePos = { getBone._41 ,getBone._42 ,getBone._43 };
	//		//		GeometryData.position = bonePos;
	//		//		GeometryData.CreateWorld();
	//		//	}
	//		//	ImGui::TreePop();
	//		//}
	//		ImGui::TreePop();
	//	}
	//	if (ImGui::TreeNode("Move Parm"))
	//	{
	//		//float& rotationalSpeed = player.FindComponent<Actor>().GetRotationalSpeed();
	//		//ImGui::SliderFloat("Ratio", &rotationalSpeed, 0.0f, 10.0f);
	//		//VECTOR4F angle = player.FindComponent<WorldTransform>().GetAngle();
	//		// float position[4] = { angle.x, angle.y,angle.z,angle.w };
	//		//ImGui::DragFloat4("Position", position, 0.1f);
	//		ImGui::TreePop();
	//	}
	//}
	//if (ImGui::TreeNode(""))
	//{
	//	ImGui::TreePop();
	//}


	{
		CharacterAI* player = m_metaAI->GetPlayCharacter();

		player->ImGui(Framework::GetInstance().GetDevice());
		Source::CameraControlle::CameraManager().GetInstance()->SetLength(player->GetCamera().lenght);
		Source::CameraControlle::CameraManager().GetInstance()->SetValue(player->GetCamera().value);
		Source::CameraControlle::CameraManager().GetInstance()->SetFocalLength(player->GetCamera().focalLength);
		Source::CameraControlle::CameraManager().GetInstance()->SetHeightAboveGround(player->GetCamera().heightAboveGround);

		if (ImGui::Button("kkkk"))
		{

			MESSENGER.MessageFromPlayer(player->GetID(), MessengType::CALL_HELP);
		}

		if (ImGui::Button("Saber"))
		{
			player->GetChangeComand().changeType = CharacterParameter::Change::PlayerType::SABER;
			MESSENGER.MessageFromPlayer(player->GetID(), MessengType::CHANGE_PLAYER);
		}
		ImGui::SameLine();
		if (ImGui::Button("Archer"))
		{
			player->GetChangeComand().changeType = CharacterParameter::Change::PlayerType::ARCHER;
			MESSENGER.MessageFromPlayer(player->GetID(), MessengType::CHANGE_PLAYER);
		}
		ImGui::SameLine();
		if (ImGui::Button("Fighter"))
		{
			player->GetChangeComand().changeType = CharacterParameter::Change::PlayerType::FIGHTER;
			MESSENGER.MessageFromPlayer(player->GetID(), MessengType::CHANGE_PLAYER);
		}

		if (!player->GetChangeComand().isPlay)
		{

			VECTOR3F distance = DistancePlayerToEnemy();
			distance = NormalizeVec3(distance);
			Source::CameraControlle::CameraManager().GetInstance()->SetDistance(distance);

			player = m_metaAI->GetPlayCharacter();
			VECTOR3F position = player->GetWorldTransform().position;
			Source::CameraControlle::CameraManager().GetInstance()->SetObject(position);
	//		Source::CameraControlle::CameraManager().GetInstance()->SetCameraMode(Source::CameraControlle::CameraManager().CameraMode::CHANGE_OBJECT);
		}
	}

	ImGui::End();

	m_metaAI->ImGuiOfEnemy(Framework::GetInstance().GetDevice(), 0);

	m_stage.ImGui();
#endif
}

void Game::Uninitialize()
{
	GetEntityManager().Relese();
	m_metaAI->Release();
}

VECTOR3F Game::DistancePlayerToEnemy()
{
	CharacterAI* player = m_metaAI->GetPlayCharacter();
	CharacterAI* enemy = &(*m_metaAI->GetEnemys()[0]);

	VECTOR3F distance = player->GetWorldTransform().position - enemy->GetWorldTransform().position;

	return distance;
}
