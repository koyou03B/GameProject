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
		
		m_metaAI->UpdateOfEnemys(elapsedTime);
		m_metaAI->UpdateOfPlayers(elapsedTime);
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

	m_stage.Render(immediateContext);

	m_metaAI->RenderOfEnemy(immediateContext, 0);
	
	m_metaAI->RenderOfPlayer(immediateContext, 0);
	m_metaAI->RenderOfPlayer(immediateContext, 1);
	//m_metaAI->RenderOfPlayer(immediateContext, 2);


//	


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

	if (!ActivateScene.GetIsStart())
	{
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
				//player->GetChangeComand().changeType = CharacterParameter::Change::PlayerType::SABER;
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
	}

	ImGui::End();
	if (!ActivateScene.GetIsStart())
	{
		m_metaAI->ImGuiOfEnemy(Framework::GetInstance().GetDevice(), 0);
		m_stage.ImGui();
	}
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
