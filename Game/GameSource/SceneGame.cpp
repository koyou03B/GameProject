#include "SceneGame.h"
#include "MessengTo.h"
#include "Arrow.h"
#include ".\LibrarySource\ModelData.h"
#include ".\LibrarySource\Framework.h"
#include ".\LibrarySource\VectorCombo.h"
#include ".\LibrarySource\GeometricPrimitve.h"


bool Game::Initialize(ID3D11Device* device)
{
	//**********************
	// SceneConstantBuffer
	//**********************
	{
		m_sceneConstantBuffer = std::make_unique<Source::ConstantBuffer::ConstantBuffer<Source::Constants::SceneConstants>>(device);
		m_sceneConstantBuffer->data.directionalLight.direction = { 0.0f, -1.0f, 1.0f, 0.0f };
		m_sceneConstantBuffer->data.ambientColor = { 0.147f,0.147f,0.147f,1.0f };
	}

	//*********************
	// MetaAI
	//*********************
	{
		m_metaAI = std::make_shared<MetaAI>();
		m_metaAI->Init();
		m_stage = std::make_unique<Stage>();
		m_stage->Init();
		MESSENGER.GetInstance().SetMetaAI(m_metaAI);

	}

	//*********************
	// Camera
	//*********************
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

	//*********************
	// Shader
	//*********************
	{
		m_frameBuffer = std::make_unique<Source::FrameBuffer::FrameBuffer>(device, static_cast<int>(Framework::GetInstance().SCREEN_WIDTH), static_cast<int>(Framework::GetInstance().SCREEN_HEIGHT),
			false/*enable_msaa*/, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
		m_fog = std::make_unique<Source::Fog::Fog>(device);

		m_fog->ReadBinary();

		m_screenFilter = std::make_unique<Source::ScreenFilter::ScreenFilter>(device);

	}


	return true;
}

void Game::Update(float& elapsedTime)
{	


	if (Source::CameraControlle::CameraManager().GetInstance()->GetCameraMode() != 
		Source::CameraControlle::CameraManager().GetInstance()->CHANGE_OBJECT)
	{
		CharacterAI* player = m_metaAI->GetPlayCharacter();

		VECTOR3F distance = DistancePlayerToEnemy();
		distance = NormalizeVec3(distance);
		VECTOR3F rightVaule = CameraRightValue();
		Source::CameraControlle::CameraManager().GetInstance()->SetDistance(distance);
		//Source::CameraControlle::CameraManager().GetInstance()->SetRigth(rightVaule);
		//Source::CameraControlle::CameraManager().GetInstance()->SetLength(player->GetCamera().lenght);
		//Source::CameraControlle::CameraManager().GetInstance()->SetValue(player->GetCamera().value);
		//Source::CameraControlle::CameraManager().GetInstance()->SetFocalLength(player->GetCamera().focalLength);
		//Source::CameraControlle::CameraManager().GetInstance()->SetHeightAboveGround(player->GetCamera().heightAboveGround);

		Source::CameraControlle::CameraManager().GetInstance()->Update(elapsedTime);

		VECTOR3F pos = player->GetWorldTransform().position;
		Source::CameraControlle::CameraManager().GetInstance()->SetObject(pos);
		CharacterAI* enemy = &(*m_metaAI->GetEnemys()[0]);
		pos = enemy->GetWorldTransform().position;
		Source::CameraControlle::CameraManager().GetInstance()->SetTarget(pos);
		
		m_metaAI->UpdateOfEnemys(elapsedTime);
		m_metaAI->UpdateOfPlayers(elapsedTime);
	}
	else
		Source::CameraControlle::CameraManager().GetInstance()->Update(elapsedTime);
	

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

	{
		m_frameBuffer->Clear(immediateContext);
		m_frameBuffer->Activate(immediateContext);

		m_sceneConstantBuffer->Activate(immediateContext, SLOT2, true, true);
		Source::CameraControlle::CameraManager().GetInstance()->Activate(immediateContext);

		m_stage->Render(immediateContext);

		m_metaAI->RenderOfEnemy(immediateContext, 0);

		m_metaAI->RenderOfPlayer(immediateContext, 0);
		//Archer
		//m_metaAI->RenderOfPlayer(immediateContext, 1);
		//m_metaAI->RenderOfPlayer(immediateContext, 2);
		
		m_frameBuffer->Deactivate(immediateContext);
		m_fog->Blit(immediateContext, m_frameBuffer->GetRenderTargetShaderResourceView().Get(), m_frameBuffer->GetDepthStencilShaderResourceView().Get());

		//Archer
		//m_metaAI->RenderOfScope(immediateContext);

		Source::CameraControlle::CameraManager().GetInstance()->Deactivate(immediateContext);
		m_sceneConstantBuffer->Deactivate(immediateContext);
	}
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

		if (ImGui::CollapsingHeader("Fog"))
		{
			ImGui::InputFloat("fogGlobalDensity", &m_fog->_fogBuffer->data.fogGlobalDensity, 0.0001f, 0.001f, "%.4f");
			ImGui::InputFloat("fogHeightFalloff", &m_fog->_fogBuffer->data.fogHeightFallOff, 0.0001f, 0.001f, "%.4f");
			ImGui::InputFloat("fogStartDepth", &m_fog->_fogBuffer->data.fogStartDepth, 0.1f, 1.0f);

			ImGuiColorEditFlags flag = ImGuiColorEditFlags_Float; // 0 ~ 255表記ではなく、0.0 ~ 1.0表記にします。

			ImGui::ColorEdit3("fogColor", reinterpret_cast<float*>(&m_fog->_fogBuffer->data.fogColor), flag);
			ImGui::ColorEdit3("fogHighlightColor", reinterpret_cast<float*>(&m_fog->_fogBuffer->data.fogHighlightColor), flag);

			ImGui::InputFloat("R", &m_fog->_fogBuffer->data.fogColor.x, 0.0f, 1.0f, "%f");
			ImGui::InputFloat("G", &m_fog->_fogBuffer->data.fogColor.y, 0.0f, 1.0f, "%f");
			ImGui::InputFloat("B", &m_fog->_fogBuffer->data.fogColor.z, 0.0f, 1.0f, "%f");

			if (ImGui::Button("Save"))
				m_fog->SaveBinary();
		}

		if (ImGui::CollapsingHeader("ScreenFilter"))
		{
			//--------------
			// Bright(明度)
			//--------------
			static float bright = 0.0f;
			ImGui::SliderFloat("Bright", &bright, -1.0f, 1.0f);
			m_screenFilter->_screenBuffer->data.bright = bright;

			//--------------
			// Contrast(濃淡)
			//--------------
			static float contrast = 1.0f;
			ImGui::SliderFloat("Contrast", &contrast, -1.0f, 1.0f);
			m_screenFilter->_screenBuffer->data.contrast = contrast;

			//-----------------
			// Saturate(彩度)
			//-----------------
			static float saturate = 1.0f;
			ImGui::SliderFloat("Saturate", &saturate, -1.0f, 1.0f);
			m_screenFilter->_screenBuffer->data.saturate = saturate;

			//--------
			// Color
			//--------
			static float color[4] = { 1.f, 1.f, 1.f, 1.f };
			ImGuiColorEditFlags flag = ImGuiColorEditFlags_Float; // 0 ~ 255表記ではなく、0.0 ~ 1.0表記にします。
			ImGui::ColorEdit4("Color", color, flag);
			m_screenFilter->_screenBuffer->data.screenColor = VECTOR4F(color[0], color[1], color[2], color[3]);

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
			//Source::CameraControlle::CameraManager().GetInstance()->SetLength(player->GetCamera().lenght);
			////Source::CameraControlle::CameraManager().GetInstance()->SetValue(player->GetCamera().value);
			//Source::CameraControlle::CameraManager().GetInstance()->SetFocalLength(player->GetCamera().focalLength);
			//Source::CameraControlle::CameraManager().GetInstance()->SetHeightAboveGround(player->GetCamera().heightAboveGround);

			VECTOR3F currentDistance = DistancePlayerToEnemy();
			currentDistance = NormalizeVec3(currentDistance);

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
		}
	}

	ImGui::End();
	if (!ActivateScene.GetIsStart())
	{
		m_metaAI->ImGuiOfEnemy(Framework::GetInstance().GetDevice(), 0);
		m_stage->ImGui();
		ArrowInstamce.ImGui(Framework::GetInstance().GetDevice());
		m_metaAI->ImGuiOfScope(Framework::GetInstance().GetDevice());
	}
#endif
}

void Game::Uninitialize()
{
	//GetEntityManager().Relese();
	m_stage->Release();
	m_metaAI->Release();
	Source::ModelData::fbxLoader().Release();
}

VECTOR3F Game::DistancePlayerToEnemy()
{
	CharacterAI* player = m_metaAI->GetPlayCharacter();
	CharacterAI* enemy = &(*m_metaAI->GetEnemys()[0]);

	VECTOR3F distance = player->GetWorldTransform().position - enemy->GetWorldTransform().position;

	return distance;
}

VECTOR3F Game::CameraRightValue()
{
	CharacterAI* player = m_metaAI->GetPlayCharacter();
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
