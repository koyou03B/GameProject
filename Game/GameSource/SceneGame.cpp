#include "SceneGame.h"
#include "MessengTo.h"
#include "Stone.h"
#include "Arrow.h"
#include ".\LibrarySource\ModelData.h"
#include ".\LibrarySource\Framework.h"
#include ".\LibrarySource\VectorCombo.h"
#include ".\LibrarySource\GeometricPrimitve.h"

bool Game::Initialize(ID3D11Device* device)
{
	#pragma region  Market
	auto& wepon = RunningMarket().AddProductConer(0);
	wepon.AddProduct<Stone>();
	wepon.AddProduct<Arrow>();
	#pragma endregion

	#pragma region SceneConstantBuffer
	m_sceneConstantBuffer = std::make_unique<Source::ConstantBuffer::ConstantBuffer<Source::Constants::SceneConstants>>(device);
	m_sceneConstantBuffer->data.directionalLight.direction = { 0.0f, -1.0f, 1.0f, 0.0f };
	m_sceneConstantBuffer->data.ambientColor = { 0.147f,0.147f,0.147f,1.0f };
	#pragma endregion

	#pragma region Shader
	m_sceneEffect.ChoiceSceneEffect(device, SceneEffectType::SCREEN_FILTER);
	m_sceneEffect.ChoiceSceneEffect(device, SceneEffectType::FOG);
	#pragma endregion

	#pragma region Object
	m_metaAI = std::make_shared<MetaAI>();
	m_metaAI->Init(device);
	m_stage = std::make_unique<Stage>();
	m_stage->Init();
	MESSENGER.GetInstance().SetMetaAI(m_metaAI);
	#pragma endregion

	#pragma region UI
	m_uiAdominist = std::make_shared<UIAdominist>();
	m_uiAdominist->Init();
	MESSENGER.GetInstance().SetUIAdominist(m_uiAdominist);
	#pragma endregion

	#pragma region Camera
	CharacterAI* player = m_metaAI->GetPlayerAdominist().GetSelectPlayer(PlayerType::Fighter);
	VECTOR3F position = player->GetWorldTransform().position;
	position.y = offsetY[0];
	Source::CameraControlle::CameraManager().GetInstance()->Initialize(device);
	Source::CameraControlle::CameraManager().GetInstance()->SetFocus(position);
	float tmp = 0;
	Source::CameraControlle::CameraManager().GetInstance()->Update(tmp);

	#pragma endregion



	return true;
}

void Game::Update(float& elapsedTime)
{
	


	#pragma region Update
	
	m_metaAI->Update(elapsedTime);
	m_uiAdominist->Update(elapsedTime);
	m_stage->Update(elapsedTime);
	#pragma endregion

	#pragma region Camera
	{
		if (m_eventState != GameEvent::WIN)
		{
			CharacterAI* player = m_metaAI->GetPlayerAdominist().GetSelectPlayer(PlayerType::Fighter);
			VECTOR3F position = player->GetWorldTransform().position;
			position.y = offsetY[0];
			Source::CameraControlle::CameraManager().GetInstance()->SetFocus(position);
		}

		Source::CameraControlle::CameraManager().GetInstance()->Update(elapsedTime);

		if (Source::CameraControlle::CameraManager().GetInstance()->GetCameraMode() ==
			Source::CameraControlle::CameraManager::CameraMode::LOCK_ON)
		{
			CharacterAI* enemy = m_metaAI->GetEnemyAdominist().GetSelectEnemy(EnemyType::Boss);
			VECTOR3F position = enemy->GetWorldTransform().position;
			position.y = offsetY[1];
			Source::CameraControlle::CameraManager().GetInstance()->SetLockOnTarget(position);
		}
	}

#pragma endregion

	#pragma region RayPick

	//VECTOR4F eye = Source::CameraControlle::CameraManager().GetInstance()->GetCamera()->GetEye();
	//VECTOR3F start = { eye.x,eye.y,eye.z };
	//VECTOR3F end = player->GetWorldTransform().position;
	//end.y = eye.y;
	//VECTOR3F intersection, hitNormal;
	//int ret = m_stage->RayPick(
	//	start,
	//	end,
	//	&intersection, &hitNormal);
	//if (ret != -1)
	//{
	//	start.x = intersection.x;
	//	start.y = intersection.y;
	//	start.z = intersection.z;
	//	Source::CameraControlle::CameraManager().GetInstance()->GetCamera()->SetEye(start);
	//}

	#pragma endregion

	#pragma region  EventFlow
	switch (m_eventState)
	{
	case GameEvent::START:
		if (m_sceneEffect.UpdateScreenFilter(0.05f, 1.0f))
		{
		#ifdef _DEBUG
			m_eventState = GameEvent::TUTORIAL;
		#else
			m_eventState = GameEvent::TUTORIAL;

		#endif // _DUBUG

		}
		break;
	case GameEvent::TUTORIAL:
	{
		CharacterAI* player = m_metaAI->GetPlayerAdominist().GetSelectPlayer(PlayerType::Fighter);
		bool tutorialComand[4] = { player->GetTutorialCommand()[0],player->GetTutorialCommand()[1],player->GetTutorialCommand()[2],false };
		tutorialComand[3] = Source::CameraControlle::CameraManager().GetInstance()->GetTutorialCommand();
		int checkCommand = 0;
		for (int i = 0; i < 4; ++i)
		{
			if (tutorialComand[i])
			{
				++checkCommand;
				m_uiAdominist->MaskUpdate(UIStaticLabel::COMMAND_MASK, i);
			}
		}
		if (checkCommand == 4)
		{
			static bool isCamera = false;
			if (!isCamera)
			{
				if (Source::CameraControlle::CameraManager().GetInstance()->GetCameraMode() !=
					Source::CameraControlle::CameraManager::CameraMode::LOCK_ON)
				{
					CharacterAI* enemy = m_metaAI->GetEnemyAdominist().GetSelectEnemy(EnemyType::Boss);
					VECTOR3F position = enemy->GetWorldTransform().position;
					position.y = offsetY[1];
					Source::CameraControlle::CameraManager().GetInstance()->SetLockOnTarget(position);
					Source::CameraControlle::CameraManager().GetInstance()->SetCameraMode(Source::CameraControlle::CameraManager::CameraMode::LOCK_ON);
				}
				isCamera = true;
			}
			if (m_stage->FanceBreak(elapsedTime))
			{
				isCamera = false;
				m_eventState = GameEvent::FIGHT;
				m_metaAI->ActivateEnemy();
				return;
			}
		}

		if (player->GetWorldTransform().position.z > -3.0f)
		{
			player->GetWorldTransform().position.z = -3.0f;
			player->GetWorldTransform().WorldUpdate();
		}
	}
	break;
	case GameEvent::FIGHT:
		if (m_metaAI->GetIsWinner())
		{
			m_eventState = GameEvent::WIN;
			const float frontOffset = 5.0f;
			const float yOffset = 70.0f;
			CharacterAI* enemy = m_metaAI->GetEnemyAdominist().GetSelectEnemy(EnemyType::Boss);
			enemy->GetWorldTransform().WorldUpdate();
			FLOAT4X4 world = enemy->GetWorldTransform().world;
			VECTOR3F front = { world._31,world._32,world._33 };
			front *= frontOffset;
			front.y = yOffset;
			VECTOR3F eye = (front + enemy->GetWorldTransform().position);
			auto& camera = Source::CameraControlle::CameraManager().GetInstance()->GetCamera();
			camera->SetEye(eye);
			camera->SetFocus(enemy->GetWorldTransform().position);
			Source::CameraControlle::CameraManager().GetInstance()->SetCameraMode(Source::CameraControlle::CameraManager::CameraMode::END);

		}
		if (m_metaAI->GetIsLoaser())
			m_eventState = GameEvent::LOSE;

		break;
	case GameEvent::WIN:
	{
		static float time = 0.0f;
		time += elapsedTime;
		if (time >= 2.0f)
		{
			if (m_sceneEffect.UpdateScreenFilter(0.01f, 0.0f))
			{
				time = 0.0f;
				SceneLabel label = SceneLabel::CLEAR;
				ActivateScene.ChangeScene(label);
			}
		}
	}
	break;
	case GameEvent::LOSE:
	{
		static float time = 0.0f;
		time += elapsedTime;
		if (time >= 2.0f)
		{
			if (m_sceneEffect.UpdateScreenFilter(0.01f, 0.0f))
			{
				time = 0.0f;
				SceneLabel label = SceneLabel::OVER;
				ActivateScene.ChangeScene(label);
			}
		}
	}
	break;
	}
	#pragma endregion

#if RELEASE
#pragma region Aspect
	if (KEYBOARD._keys[DIK_0] == 1)
	{
		m_isDebug = !m_isDebug;
		float height = Framework::GetInstance().SCREEN_HEIGHT;
		float width = Framework::GetInstance().SCREEN_WIDTH;
		m_debugAspectW = m_isDebug ? DEBUG_WIDTH : width;

		Framework::GetInstance().SetViewPort(m_debugAspectW, height);
	}
#pragma endregion
#endif

#if _DEBUG
	if (KEYBOARD._keys[DIK_1] == 1)
	{
		SceneLabel label = SceneLabel::CLEAR;
		ActivateScene.ChangeScene(label);
	}

	if (KEYBOARD._keys[DIK_2] == 1)
	{
		SceneLabel label = SceneLabel::OVER;
		ActivateScene.ChangeScene(label);
	}
#endif

}

void Game::Render(ID3D11DeviceContext* immediateContext, float elapsedTime)
{
	VECTOR4F lightDirection(0, -1, 1, 0);

	{
		m_sceneEffect.ActivateEffect(immediateContext,SceneEffectType::SCREEN_FILTER);
		m_metaAI->ActivateEffect(immediateContext, SceneEffectType::VIGNETTE);
		m_sceneEffect.ActivateEffect(immediateContext, SceneEffectType::FOG);
		m_sceneConstantBuffer->Activate(immediateContext, SLOT2, true, true);
		Source::CameraControlle::CameraManager().GetInstance()->Activate(immediateContext);

		m_stage->Render(immediateContext);
		m_metaAI->Render(immediateContext);
		if(m_eventState == GameEvent::FIGHT)
			m_metaAI->RenderOfEffect(immediateContext);

		m_sceneEffect.DeactivateEffect(immediateContext,SceneEffectType::FOG);
		m_metaAI->DeactivateEffect(immediateContext, SceneEffectType::VIGNETTE);
		m_uiAdominist->Render(immediateContext);
		
		m_sceneEffect.DeactivateEffect(immediateContext, SceneEffectType::SCREEN_FILTER);
		Source::CameraControlle::CameraManager().GetInstance()->Deactivate(immediateContext);
		m_sceneConstantBuffer->Deactivate(immediateContext);
	}
}

void Game::ImGui()
{
#if _DEBUG
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
			CharacterAI* player = m_metaAI->GetPlayerAdominist().GetPlayPlayer();
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

		if (ImGui::CollapsingHeader("PostEffect"))
		{
			m_sceneEffect.ImGui();
		}

		if (ImGui::CollapsingHeader("Camera"))
		{	
			CharacterAI* enemy = m_metaAI->GetEnemyAdominist().GetSelectEnemy(EnemyType::Boss);
			VECTOR3F position = enemy->GetWorldTransform().position;
			position.y = offsetY[1];
			Source::CameraControlle::CameraManager().GetInstance()->SetLockOnTarget(position);
			ImGui::SetNextWindowSize(ImVec2(400, Framework::GetInstance().SCREEN_HEIGHT), ImGuiSetCond_Once);//サイズ
			ImGui::SetNextWindowPos(ImVec2(1520, 0), ImGuiSetCond_Once);//ポジション
			ImGui::Begin("CameraEditer");
			Source::CameraControlle::CameraManager().GetInstance()->Editor();
			ImGui::End();
		}

		if (ImGui::CollapsingHeader("UI"))
		{
			m_uiAdominist->ImGui();
		}	
		
		if (ImGui::CollapsingHeader("MetaAI"))
		{
			if (ImGui::CollapsingHeader("Player"))
			{
				m_metaAI->ImGuiOfPlayer(Framework::GetInstance().GetDevice());
			}

			if (ImGui::CollapsingHeader("Enemy"))
			{
				m_metaAI->ImGuiOfEnemy(Framework::GetInstance().GetDevice());
			}

			if (ImGui::CollapsingHeader("Effect"))
			{
				m_metaAI->ImGuiOfEffect();
			}
			
			if (ImGui::CollapsingHeader("Shader"))
			{
				m_metaAI->ImGuiOfShader();
			}

		}


		if (ImGui::CollapsingHeader("Stage"))
		{
			m_stage->ImGui();
		}
	}

	ImGui::End();

#else 
	if (m_isDebug)
	{
		ImGui::SetNextWindowPos(ImVec2(DEBUG_WIDTH, 0.0f));
		ImGui::Begin("SCENE_GAME");
		if (ImGui::CollapsingHeader(u8"味方"))
		{
			m_metaAI->ImGuiOfPlayer(Framework::GetInstance().GetDevice());
		}

		//if (ImGui::CollapsingHeader("Enemy"))
		//{
		//	m_metaAI->ImGuiOfEnemy(Framework::GetInstance().GetDevice());
		//}
		ImGui::End();
	}
#endif
}

void Game::Uninitialize()
{
	//GetEntityManager().Relese();
	m_stage->Release();
	RunningMarket().Reset();
	m_metaAI->Release();
	m_uiAdominist->Release();
	Source::ModelData::fbxLoader().Release();
	Source::Shader::ReleaseAllCachedVertexShaders();
	Source::Shader::ReleaseAllCachedPixelShaders();
	Source::Texture::ReleaseAllCachedTextures();
	float height = Framework::GetInstance().SCREEN_HEIGHT;
	float width = Framework::GetInstance().SCREEN_WIDTH;
	Framework::GetInstance().SetViewPort(width, height);
}

VECTOR3F Game::DistancePlayerToEnemy()
{
	CharacterAI* player = m_metaAI->GetPlayerAdominist().GetPlayPlayer();
	CharacterAI* enemy = m_metaAI->GetEnemyAdominist().GetSelectEnemy(EnemyType::Boss);
	VECTOR3F pos[2] = { player->GetWorldTransform().position ,enemy->GetWorldTransform().position };
	pos[0].y = offsetY[0];
	pos[1].y = offsetY[1];
	VECTOR3F distance = pos[0] - pos[1];
	return distance;
}

VECTOR3F Game::CameraRightValue()
{
	CharacterAI* player = m_metaAI->GetPlayerAdominist().GetPlayPlayer();
	//*****************
	// Camera
	//*****************
	FLOAT4X4 modelAxisTransform = player->GetModel()->_resource->axisSystemTransform;
	FLOAT4X4 world = modelAxisTransform * player->GetWorldTransform().world;

	VECTOR3F rightAxis = { world._11,world._12,world._13 };
	rightAxis = NormalizeVec3(rightAxis);
	VECTOR3F rigthValue = rightAxis * player->GetCamera().rightValue;
	
	return rigthValue;
}
