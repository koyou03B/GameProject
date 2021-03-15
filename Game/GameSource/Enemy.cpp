#include "Enemy.h"
#include "MessengTo.h"
#include "EnemyTaskAll.h"
#include ".\LibrarySource\ModelData.h"
#include ".\LibrarySource\VectorCombo.h"

//CEREAL_CLASS_VERSION(CharacterParameter::Collision, 1);
CEREAL_CLASS_VERSION(Enemy, 10);

void Enemy::Init()
{
	m_transformParm.position = { 0.0f,0.0f,30.0f };
	m_transformParm.angle = { 0.0f * 0.01745f, 180.0f * 0.01745f,0.0f * 0.017454f };
	m_transformParm.scale = { 0.087f,0.087f,0.087f };
	m_transformParm.WorldUpdate();

	m_model = Source::ModelData::fbxLoader().GetActorModel(Source::ModelData::ActorModel::ENEMY);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Enemy/Animation/StandardAnim/Damage.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Enemy/Animation/StandardAnim/Knockdown.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Enemy/Animation/StandardAnim/Die.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Enemy/Animation/StandardAnim/Wrath.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Enemy/Animation/StandardAnim/LeftTurn.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Enemy/Animation/StandardAnim/RightTurn.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Enemy/Animation/StandardAnim/Run.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Enemy/Animation/StandardAnim/Step.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Enemy/Animation/StandardAnim/BackFlip.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Enemy/Animation/StandardAnim/StandUp.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Enemy/Animation/SignalAnim/MuscleSignal.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Enemy/Animation/SignalAnim/RelaxSignal.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Enemy/Animation/SignalAnim/RoaringSignal.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Enemy/Animation/SignalAnim/RunSignal.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Enemy/Animation/AttackAnim/CrossPunch.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Enemy/Animation/AttackAnim/TurnAttackLower.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Enemy/Animation/AttackAnim/Hook.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Enemy/Animation/AttackAnim/RightPunchLower.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Enemy/Animation/AttackAnim/TurnAttackHeight.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Enemy/Animation/AttackAnim/FallFlat_edit.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Enemy/Animation/AttackAnim/RightPunchUpper.fbx", 60);
	//m_model->_resource->AddAnimation("../Asset/Model/Actor/Enemy/Animation/AttackAnim/RunAttack.fbx", 60);
	//Source::ModelData::fbxLoader().SaveActForBinary(Source::ModelData::ActorModel::ENEMY);

	m_statusParm.isExit = true;
	m_statusParm.life = 700.0f;
	m_statusParm.maxLife = 700.0f;
	m_blendAnimation.animationBlend.Init(m_model);
	m_blendAnimation.animationBlend.ChangeSampler(0, Animation::IDLE, m_model);
	m_attackParm.resize(13);
	//SerialVersionUpdate(1);
;
	for (auto& atk : m_attackParm)
	{
		atk.serialVersion = 17;
	}

	m_behaviorTree.CreateRootNode();
	if (PathFileExistsA((std::string("../Asset/Binary/Enemy/Parameter") + ".bin").c_str()))
	{
		std::ifstream ifs;
		ifs.open((std::string("../Asset/Binary/Enemy/Parameter") + ".bin").c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);
	}
	m_collision.resize(6);
	m_behaviorTree.SetRootNodeChild();
	m_behaviorTree.SetTaskToNode();
	//TaskData& taskData = m_behaviorTree.GetTaskData();
	//m_selectTask = taskData.intimidateTask;

	m_moveState = 0;
	//m_judgeElementPram.targetID

	auto& wepon = RunningMarket().FindProductConer(0);
	std::unique_ptr<Stone> stone{ wepon.GiveProduct<Stone>() };
	m_stone = std::move(stone);

#if _DEBUG
//	m_blendAnimation.animationBlend.ChangeSampler(0, Animation::WRATH, m_model);
//	m_isAction = true;
#else
	m_blendAnimation.animationBlend.ChangeSampler(0, Animation::WRATH, m_model);
	m_isAction = true;
#endif
}

void Enemy::Update(float& elapsedTime)
{
	m_elapsedTime = elapsedTime;
	if (m_active)
	{
		switch (m_moveState)
		{
		case 0:
		{
			if (!m_blendAnimation.animationBlend.SearchSampler(Animation::WRATH))
			{
				m_blendAnimation.animationBlend.ResetAnimationFrame();
				m_blendAnimation.animationBlend.ResetAnimationSampler(0);
				m_blendAnimation.animationBlend.AddSampler(Animation::WRATH,m_model);
			}
			else
			{
				bool hasAnimation = JudgeBlendRatio();
				if (hasAnimation)
				{
					uint32_t currentAnimationTime = m_blendAnimation.animationBlend.GetAnimationTime(0);
					if (currentAnimationTime >= 169)
					{
						++m_moveState;
						m_selectTask = m_behaviorTree.SearchOfActiveTask(0);
					}
				}
			}
		}
		break;
		case 1:
			m_selectTask->Run(this);
			m_stone->Update(m_elapsedTime);
			m_behaviorTree.UpdateUseTasks(elapsedTime);
			if (m_selectTask->GetTaskState() == EnemyBehaviorTask::TASK_STATE::END)
			{					
				if(m_selectTask->GetParentNodeName() == "SpecialAttack" || 
					m_selectTask->GetParentNodeName() == "UnSpecialAttack")
				m_behaviorTree.AddUseTask(m_selectTask);
				m_selectTask.reset();
				m_selectTask = m_behaviorTree.SearchOfActiveTask(0);
			}

			if (m_statusParm.life <= 0)
			{
				m_selectTask->SetMoveState(0);
				m_selectTask.reset();
				size_t samplerSize = m_blendAnimation.animationBlend.GetSampler().size();
				for (size_t i = 0; i < samplerSize; ++i)
				{
					m_blendAnimation.animationBlend.ReleaseSampler(0);
				}
				m_blendAnimation.animationBlend.ChangeSampler(0, Animation::DIE, m_model);
				m_blendAnimation.animationBlend.FalseAnimationLoop(0);
				m_blendAnimation.animationBlend.SetAnimationSpeed(1.0f);
				++m_moveState;
			}
			break;
		case 2:
		{
			//float blendRatio = 0.08f;
			//m_blendAnimation.blendRatio -= blendRatio;
			//if (m_blendAnimation.blendRatio <= 0.0f)//magicNumber
			//{
			//	m_blendAnimation.animationBlend._blendRatio = 0.0f;
			//	size_t samplerSize = m_blendAnimation.animationBlend.GetSampler().size();
			//	for (size_t i = 0; i < samplerSize; ++i)
			//	{
			//		m_blendAnimation.animationBlend.ReleaseSampler(1);
			//	}
			//	m_blendAnimation.animationBlend.FalseAnimationLoop(0);
			//}
		}
		break;

		}

	}
	m_blendAnimation.animationBlend.Update(m_model, elapsedTime);

	int currentMesh = m_collision[0].GetCurrentMesh(0);
	int currentBone = m_collision[0].GetCurrentBone(0);
	FLOAT4X4 boneTransform = m_blendAnimation.animationBlend._blendLocals[currentMesh].at(currentBone);
	boneSpine = boneTransform;
	FLOAT4X4 modelAxisTransform = m_model->_resource->axisSystemTransform;
	FLOAT4X4 getBoneTransform = boneTransform * modelAxisTransform * m_transformParm.world;
	m_collision[0].position[0] = { m_transformParm.position.x,getBoneTransform._42,m_transformParm.position.z };

}

void Enemy::Render(ID3D11DeviceContext* immediateContext)
{
	auto& localTransforms = m_blendAnimation.animationBlend._blendLocals;
	//	auto& localTransforms = m_blendAnimation.partialBlend._blendLocals;
	VECTOR4F color{ 1.0f,1.0f,1.0f,1.0f };
	m_model->Render(immediateContext, m_transformParm.world, color, localTransforms);	
	m_stone->Render(immediateContext);
	m_debugObjects.debugObject.Render(immediateContext, VECTOR4F(0, 0, 0, 0),true);
}

bool Enemy::JudgeBlendRatio()
{
	m_blendAnimation.animationBlend._blendRatio += 0.1f;
	if (m_blendAnimation.animationBlend._blendRatio >= m_blendAnimation.blendRatioMax)//magicNumber
	{
		m_blendAnimation.animationBlend._blendRatio = 0.0f;
		size_t samplerSize = m_blendAnimation.animationBlend.GetSampler().size();
		for (size_t i = 0; i < samplerSize; ++i)
		{
			m_blendAnimation.animationBlend.ReleaseSampler(0);
		}
		m_blendAnimation.animationBlend.FalseAnimationLoop(0);

		return true;
	}

	return false;
}

void Enemy::Release()
{
	//m_behaviorTree.Release();
	//if (m_selectTask)
	//{
	//	m_selectTask.reset();
	//}
	//m_stone->Release();

	m_blendAnimation.animationBlend.ReleaseAllSampler();

	if (m_model)
	{
		if (m_model.unique())
		{
			m_model.reset();
		}
	}

}

void Enemy::ImGui(ID3D11Device* device)
{
#ifdef _DEBUG

	ImGui::Begin("Enemy", nullptr, ImGuiWindowFlags_MenuBar);//メニューバーをつかうならこのBEGIN

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))//ファイルの中には
		{
			if (ImGui::MenuItem("Save"))//データの保存とか
			{
				std::ofstream ofs;
				ofs.open((std::string("../Asset/Binary/Enemy/Parameter") + ".bin").c_str(), std::ios::binary);
				cereal::BinaryOutputArchive o_archive(ofs);
				o_archive(*this);
			}

			if (ImGui::MenuItem("Load"))
			{
				if (PathFileExistsA((std::string("../Asset/Binary/Enemy/Parameter") + ".bin").c_str()))
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

	ImGui::Text("askAttackFlg %d", m_messageParm.askAttack);


	static int currentMesh = 0;
	ImGui::BulletText(u8"Mesh%d番目", currentMesh);

	static int currentBone = 0;

	ImGui::Combo("Name_of_BoneName",
		&currentBone,
		vectorGetter,
		static_cast<void*>(&m_blendAnimation.animationBlend.GetBoneName()[currentMesh]),
		static_cast<int>(m_blendAnimation.animationBlend.GetBoneName()[currentMesh].size())
	);

	//**************************************
	// Collision
	//**************************************
	if (ImGui::CollapsingHeader("Collision"))
	{
		static int current = 0;
		ImGui::RadioButton("Body", &current, 0);
		ImGui::RadioButton("RightPunch", &current, 1); ImGui::SameLine();
		ImGui::RadioButton("LeftPunch", &current, 2); 
		ImGui::RadioButton("RangeAttack", &current, 3); ImGui::SameLine();
		ImGui::RadioButton("RunAttack", &current, 4); ImGui::SameLine();
		ImGui::RadioButton("Stone", &current, 5);
		FLOAT4X4 blendBone = m_blendAnimation.animationBlend._blendLocals[currentMesh].at(currentBone);
		FLOAT4X4 modelAxisTransform = m_model->_resource->axisSystemTransform;
		FLOAT4X4 getBone = blendBone * modelAxisTransform * m_transformParm.world;

		float bonePositions[] = { getBone._41,getBone._42,getBone._43 };
		VECTOR3F bonePosition = { bonePositions[0],bonePositions[1],bonePositions[2] };

		ImGui::SliderFloat3("BoneTranslate", bonePositions, -10.0f, 10.0f);

		static bool isVisualization = false;
		if (ImGui::Button("Visible?"))
			isVisualization = true;

		if (isVisualization)
		{
			auto primitive = m_debugObjects.GetSphere(device, "");
			m_debugObjects.debugObject.AddGeometricPrimitive(std::move(primitive));
			m_debugObjects.debugObject.AddInstanceData(bonePosition, VECTOR3F(0.0f * 0.01745f, 180.0f * 0.01745f, 0.0f * 0.017454f),
				VECTOR3F(1.0f, 1.0f, 1.0f), VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));

			isVisualization = false;
		}

		if (m_debugObjects.debugObject.IsGeomety())
		{
			if (m_debugObjects.debugObject.GetInstance().size() > current)
			{
				auto& geomtry = m_debugObjects.debugObject.GetInstanceData(current);

				//Position
				{
					geomtry.position = bonePosition;
				}
				//Scale
				{
					static float scale = m_collision[current].scale;
					ImGui::SliderFloat("Scale", &scale, 1.0f, 10.0f);
					geomtry.scale = { scale ,scale ,scale };
					m_collision[current].scale = scale;
				}

				//Radius
				{
					static float radius = m_collision[current].radius;
					ImGui::SliderFloat("Radius", &radius, 1, 50);
					geomtry.scale *= radius;
					m_collision[current].radius = radius;
				}

				//Mesh & Bone SetNumber
				{
					if (ImGui::Button("Mesh 0"))
						m_collision[current].SetCurrentMesh(currentMesh, 0);
					ImGui::SameLine();
					if (ImGui::Button("Mesh 1"))
						m_collision[current].SetCurrentMesh(currentMesh, 0);

					int mesh0 = m_collision[current].GetCurrentMesh(0);
					ImGui::BulletText("Mesh Number %d", mesh0);
					int mesh1 = m_collision[current].GetCurrentMesh(1);
					ImGui::BulletText("Mesh Number %d", mesh1);

					if (ImGui::Button("Bone 0"))
						m_collision[current].SetCurrentBone(currentBone, 0);
					ImGui::SameLine();
					if (ImGui::Button("Bone 1"))
						m_collision[current].SetCurrentBone(currentBone, 1);


					int bone0 = m_collision[current].GetCurrentBone(0);
					ImGui::BulletText("Bone Number %d", bone0);
					int bone1 = m_collision[current].GetCurrentBone(1);
					ImGui::BulletText("Bone Number %d", bone1);
				}

				//CollisionType
				{
					std::vector<std::string> collisionType = { "Cube","SPHER","CAPSULE","CIRCLE","RECT" };
					static int currentCollisionType = 0;

					ImGui::Combo("Name_of_CollisionType",
						&currentCollisionType,
						vectorGetter,
						static_cast<void*>(&collisionType),
						static_cast<int>(collisionType.size())
					);

					if (ImGui::Button("CollisionType"))
						m_collision[current].collisionType = static_cast<CharacterParameter::Collision::CollisionType>(currentCollisionType);
				}
				geomtry.CreateWorld();
			}
		}
	}

	//*******************************************
	// Status
	//*******************************************
	if (ImGui::CollapsingHeader("Status"))
	{
		ImGui::BulletText("LIFE : %f", m_statusParm.life);

		static float positionX = m_transformParm.position.x;
		//float positionY = m_transformParm.position.y;
		static float positionZ = m_transformParm.position.z;
		ImGui::SliderFloat("PositionX", &positionX, -1000.0f, 1000.0f);
		//ImGui::SliderFloat("PositionY", &positionY, 0.0f, 100.0f);
		ImGui::SliderFloat("PositionZ", &positionZ, -1000.0f, 1000.0f);

		m_transformParm.position.x = positionX;
	//	m_transformParm.position.y += positionY;
		m_transformParm.position.z = positionZ;

		m_transformParm.WorldUpdate();
	}

	//*******************************************
	// SignalAnim
	//*******************************************
	if (ImGui::CollapsingHeader("SignalAnim"))
	{
		static int current = 0;		
		ImGui::RadioButton("MUSCLE_SIGNAL", &current, 0); ImGui::SameLine();
		ImGui::RadioButton("RELAX_SIGNAL", &current, 1); 
		ImGui::RadioButton("ROARING_SIGNAL", &current, 2);ImGui::SameLine();
		ImGui::RadioButton("RUN_SIGNAL", &current, 3); 

		int frameCount = static_cast<int>(m_signalFrame[current]);
		ImGui::InputInt("FrameCount", &frameCount, 0, 100);

		m_signalFrame[current] = static_cast<uint32_t>(frameCount);

	}
	//*******************************************
	// BehaviorTree
	//*******************************************
	if (ImGui::CollapsingHeader("BehaviorTree"))
	{
		std::vector<std::string> nodeName =
		{
			"RootNode",
			"WaitNode",
			"ChaseNode",
			"FightNode",
			"FightNearNode",
			"FightFarNode",
			"SpecialAttack",
			"UnSpecialAttack"
		};

		static int selectNode = 0;
		ImGui::Combo("SelectNode",
			&selectNode,
			vectorGetter,
			static_cast<void*>(&nodeName),
			static_cast<int>(nodeName.size())
		);

		if (ImGui::CollapsingHeader("NodeData"))
		{
			NodeData& nodeData = m_behaviorTree.GetNodeData();
			static bool isCreate = false;
			if (!isCreate)
			{
				nodeData.CreateNodeData(ENTRY_NODE::WAIT_NODE);		
				nodeData.CreateNodeData(ENTRY_NODE::CHASE_NODE);
				nodeData.CreateNodeData(ENTRY_NODE::FIGHT_NODE);
				nodeData.CreateNodeData(ENTRY_NODE::FIGHT_NEAR_NODE);
				nodeData.CreateNodeData(ENTRY_NODE::FIGHT_FAR_NODE);
				nodeData.CreateNodeData(ENTRY_NODE::SPECIAL_ATTACK);
				nodeData.CreateNodeData(ENTRY_NODE::UNSPECIAL_ATTACK);
				isCreate = true;
			}

			if (ImGui::CollapsingHeader("RootNode"))
			{
				auto& rootNode = m_behaviorTree.GetRootNode();

				ImGui::TextColored(ImVec4(1, 1, 1, 1), "------NodeName------");
				{
					std::string currentNodeName = rootNode->GetNodeName();
					ImGui::BulletText("NodeName -> %s", currentNodeName.data());

					if (ImGui::Button("SetName"))
						rootNode->SetNodeName(nodeName[selectNode]);
				}

				ImGui::TextColored(ImVec4(1, 1, 1, 1), "------Rule------");
				{
					static int selectRule = 0;
					static int currentRule = 0;
					std::vector<std::string> ruleName = { "NON", "PRIORITY","RANDOM","SEQUENCE" };

					ImGui::Combo("SelectRulename",
						&selectRule,
						vectorGetter,
						static_cast<void*>(&ruleName),
						static_cast<int>(ruleName.size())
					);

					SELECT_RULE setSelectRule;
					SELECT_RULE getCurrentRule;

					getCurrentRule = rootNode->GetSelectRule();
					currentRule = static_cast<int>(getCurrentRule);
					std::string currentRuleName = ruleName[currentRule];
					ImGui::BulletText("NodeCurrentRule -> %s", currentRuleName.data());

					setSelectRule = static_cast<SELECT_RULE>(selectRule);
					if (ImGui::Button("SetRule"))
						rootNode->SetSelectRule(setSelectRule);
				}

				ImGui::TextColored(ImVec4(1, 1, 1, 1), "------Child------");
				{
					if (!rootNode->GetChild().empty())
					{
						int childCount = static_cast<int>(rootNode->GetChild().size());
						ImGui::BulletText("Child -> %d", childCount);
					}
					else
						ImGui::BulletText("Child -> 0");

				}

			}

			if (ImGui::CollapsingHeader("ChaseNode"))
			{
				std::shared_ptr<EnemyChaseNode> selectNearNode = nodeData.chaseNode;

				if (ImGui::Button("Save"))
					selectNearNode->SaveOfBinaryFile();
				ImGui::SameLine();
				if (ImGui::Button("Load"))
					selectNearNode->LoadOfBinaryFile(nodeName[selectNode]);

				ImGui::TextColored(ImVec4(1, 1, 1, 1), "------NodeName------");
				{
					std::string currentNodeName = selectNearNode->GetNodeName();
					ImGui::BulletText("NodeName -> %s", currentNodeName.data());

					if (ImGui::Button("SetName"))
						selectNearNode->SetNodeName(nodeName[selectNode]);
				}

				ImGui::TextColored(ImVec4(1, 1, 1, 1), "------Priority------");
				{
					static int priority = static_cast<int>(selectNearNode->GetPriority());
					ImGui::BulletText("NodePriotiry -> %d", static_cast<int>(selectNearNode->GetPriority()));
					ImGui::SliderInt("Priority", &priority, 1, 4);
					if (ImGui::Button("SetPriority"))
						selectNearNode->SetPriority(static_cast<uint32_t>(priority));
				}

				ImGui::TextColored(ImVec4(1, 1, 1, 1), "------Rule------");
				{
					static int selectRule = 0;
					static int currentRule = 0;
					std::vector<std::string> ruleName = { "NON", "PRIORITY","RANDOM","SEQUENCE" };

					ImGui::Combo("SelectRulename",
						&selectRule,
						vectorGetter,
						static_cast<void*>(&ruleName),
						static_cast<int>(ruleName.size())
					);

					SELECT_RULE setSelectRule;
					SELECT_RULE getCurrentRule;

					getCurrentRule = selectNearNode->GetSelectRule();
					currentRule = static_cast<int>(getCurrentRule);
					std::string currentRuleName = ruleName[currentRule];
					ImGui::BulletText("NodeCurrentRule -> %s", currentRuleName.data());

					setSelectRule = static_cast<SELECT_RULE>(selectRule);
					if (ImGui::Button("SetRule"))
						selectNearNode->SetSelectRule(setSelectRule);
				}

				ImGui::TextColored(ImVec4(1, 1, 1, 1), "------ParentName------");
				{
					std::string currentParentNodeName = selectNearNode->GetParentName();
					ImGui::BulletText("ParentNodeName -> %s", currentParentNodeName.data());

					if (ImGui::Button("SetParentName"))
						selectNearNode->SetParentName(nodeName[selectNode]);
				}

				ImGui::TextColored(ImVec4(1, 1, 1, 1), "------Child------");
				{
					if (!selectNearNode->GetChild().empty())
					{
						int childCount = static_cast<int>(selectNearNode->GetChild().size());
						ImGui::BulletText("Child -> %d", childCount);
					}
					else
						ImGui::BulletText("Child -> 0");

				}

				ImGui::TextColored(ImVec4(1, 1, 1, 1), "------MaxDirection------");
				{
					CharacterAI* player = MESSENGER.CallPlayerInstance(PlayerType::Fighter);
					VECTOR3F playerPosition = player->GetWorldTransform().position;
					VECTOR3F directionToPlayer = playerPosition - m_transformParm.position;

					float currentDirection = ToDistVec3(directionToPlayer);
					ImGui::BulletText("DistanceToPlayer -> %f", currentDirection);
					float maxDirection = static_cast<float>(selectNearNode->GetMaxDirection());
					ImGui::BulletText("MaxDirection -> %f", maxDirection);
					static float direction = 0.0f;
					ImGui::SliderFloat("MaxDirection", &direction, 0.0f, 100.0f);
					if (ImGui::Button("SetDirection"))
						selectNearNode->SetMaxDirection(direction);

				}
			}

			if (ImGui::CollapsingHeader("OtherNodes"))
			{
				static std::shared_ptr<EnemyBehaviorNode> selectBehaviorNode;
				if (ImGui::Button("Wait"))
					selectBehaviorNode = nodeData.waitNode;
				ImGui::SameLine();
				if (ImGui::Button("Fight"))
					selectBehaviorNode = nodeData.fightNode;
				ImGui::SameLine();
				if (ImGui::Button("Special"))
					selectBehaviorNode = nodeData.specialAttackNode;
				ImGui::SameLine();
				if (ImGui::Button("UnSpecial"))
					selectBehaviorNode = nodeData.unSpecialAttackNode;

				if (selectBehaviorNode)
				{
					if (ImGui::Button("Save"))
						selectBehaviorNode->SaveOfBinaryFile();
					ImGui::SameLine();
					if (ImGui::Button("Load"))
						selectBehaviorNode->LoadOfBinaryFile(nodeName[selectNode]);

					ImGui::TextColored(ImVec4(1, 1, 1, 1), "------NodeName------");
					{
						std::string currentNodeName = selectBehaviorNode->GetNodeName();
						ImGui::BulletText("NodeName -> %s", currentNodeName.data());

						if (ImGui::Button("SetName"))
							selectBehaviorNode->SetNodeName(nodeName[selectNode]);
					}

					ImGui::TextColored(ImVec4(1, 1, 1, 1), "------Priority------");
					{
						static int priority = static_cast<int>(selectBehaviorNode->GetPriority());
						ImGui::BulletText("NodePriotiry -> %d", static_cast<int>(selectBehaviorNode->GetPriority()));
						ImGui::SliderInt("Priority", &priority, 1, 4);
						if (ImGui::Button("SetPriority"))
							selectBehaviorNode->SetPriority(static_cast<uint32_t>(priority));
					}

					ImGui::TextColored(ImVec4(1, 1, 1, 1), "------Rule------");
					{
						static int selectRule = 0;
						static int currentRule = 0;
						std::vector<std::string> ruleName = { "NON", "PRIORITY","RANDOM","SEQUENCE" };

						ImGui::Combo("SelectRulename",
							&selectRule,
							vectorGetter,
							static_cast<void*>(&ruleName),
							static_cast<int>(ruleName.size())
						);

						SELECT_RULE setSelectRule;
						SELECT_RULE getCurrentRule;

						getCurrentRule = selectBehaviorNode->GetSelectRule();
						currentRule = static_cast<int>(getCurrentRule);
						std::string currentRuleName = ruleName[currentRule];
						ImGui::BulletText("NodeCurrentRule -> %s", currentRuleName.data());

						setSelectRule = static_cast<SELECT_RULE>(selectRule);
						if (ImGui::Button("SetRule"))
							selectBehaviorNode->SetSelectRule(setSelectRule);
					}

					ImGui::TextColored(ImVec4(1, 1, 1, 1), "------ParentName------");
					{
						std::string currentParentNodeName = selectBehaviorNode->GetParentName();
						ImGui::BulletText("ParentNodeName -> %s", currentParentNodeName.data());

						if (ImGui::Button("SetParentName"))
							selectBehaviorNode->SetParentName(nodeName[selectNode]);
					}

					ImGui::TextColored(ImVec4(1, 1, 1, 1), "------Child------");
					{
						if (!selectBehaviorNode->GetChild().empty())
						{
							int childCount = static_cast<int>(selectBehaviorNode->GetChild().size());
							ImGui::BulletText("Child -> %d", childCount);
						}
						else
							ImGui::BulletText("Child -> 0");

					}

					ImGui::TextColored(ImVec4(1, 1, 1, 1), "------Task------");
					{
						if (!selectBehaviorNode->GetTask().empty())
						{
							int taskCount = static_cast<int>(selectBehaviorNode->GetTask().size());
							ImGui::BulletText("Task -> %d", taskCount);
						}
						else
							ImGui::BulletText("Task -> 0");
					}

				}
			}
		}

		//*******************************************
		// Task
		//*******************************************
		std::vector<std::string> taskName =
		{
			"RestTask",
			"IntimidateTask",
			"WalkTask",
			"TargetTurnTask",
			"NearAttack0Task",
			"NearAttack1Task",
			"NearAttack2Task",
			"NearAttack3Task",
			"NearSpecialAttack0Task",
			"FarAttack0Task",
			"FarAttack1Task",
			"FarAttack2Task",
			"FarSpecialAttack0Task",
		};

		static int selectTask = 0;
		ImGui::Combo("SelectTask",
			&selectTask,
			vectorGetter,
			static_cast<void*>(&taskName),
			static_cast<int>(taskName.size())
		);

		if (ImGui::CollapsingHeader("Task"))
		{
			TaskData& taskData = m_behaviorTree.GetTaskData();

			static std::shared_ptr<EnemyBehaviorTask> selectBehaviorTask;

			if (ImGui::Button("RestTask"))
			{
				taskData.CreateTaskData(ENTRY_TASK::REST_TASK);
				selectBehaviorTask = taskData.restTask;
			}
			ImGui::SameLine();
			if (ImGui::Button("IntimidateTask"))
			{
				taskData.CreateTaskData(ENTRY_TASK::INTIMIDATE_TASK);
				selectBehaviorTask = taskData.intimidateTask;
			}

			if (ImGui::Button("WalkTask"))
			{
				taskData.CreateTaskData(ENTRY_TASK::WALK_TASK);
				selectBehaviorTask = taskData.walkTask;
			}
			ImGui::SameLine();
			if (ImGui::Button("TargetTurnTask"))
			{
				taskData.CreateTaskData(ENTRY_TASK::TARGET_TURN_TASK);
				selectBehaviorTask = taskData.targetTurnTask;
			}


			if (ImGui::Button("NearAttack0Task"))
			{
				taskData.CreateTaskData(ENTRY_TASK::NEAR_ATTACK0_TASK);
				selectBehaviorTask = taskData.fightNearTask0;
			}
			ImGui::SameLine();
			if (ImGui::Button("NearAttack1Task"))
			{
				taskData.CreateTaskData(ENTRY_TASK::NEAR_ATTACK1_TASK);
				selectBehaviorTask = taskData.fightNearTask1;
			}
			
			if (ImGui::Button("NearAttack2Task"))
			{
				taskData.CreateTaskData(ENTRY_TASK::NEAR_ATTACK2_TASK);
				selectBehaviorTask = taskData.fightNearTask2;
			}
			ImGui::SameLine();
			if (ImGui::Button("NearAttack3Task"))
			{
				taskData.CreateTaskData(ENTRY_TASK::NEAR_ATTACK3_TASK);
				selectBehaviorTask = taskData.fightNearTask3;
			}

			if (ImGui::Button("FarAttack0Task"))
			{
				taskData.CreateTaskData(ENTRY_TASK::FAR_ATTACK0_TASK);
				selectBehaviorTask = taskData.fightFarTask0;
			}
			ImGui::SameLine();
			if (ImGui::Button("FarAttack1Task"))
			{
				taskData.CreateTaskData(ENTRY_TASK::FAR_ATTACK1_TASK);
				selectBehaviorTask = taskData.fightFarTask1;
			}
			ImGui::SameLine();
			if (ImGui::Button("FarAttack2Task"))
			{
				taskData.CreateTaskData(ENTRY_TASK::FAR_ATTACK2_TASK);
				selectBehaviorTask = taskData.fightFarTask2;
			}

			if (ImGui::Button("NearSpecialAttack0"))
			{
				taskData.CreateTaskData(ENTRY_TASK::NEAR_SPECIAL_TASK);
				selectBehaviorTask = taskData.fightNearSpecialTask0;

			}
			ImGui::SameLine();
			if (ImGui::Button("FarSpecialAttack0"))
			{
				taskData.CreateTaskData(ENTRY_TASK::FAR_SPECIAL_TASK);
				selectBehaviorTask = taskData.fightFarSpecialTask0;
			}

			if (selectBehaviorTask)
			{
				if (ImGui::Button("SaveTask"))
					selectBehaviorTask->SaveOfBinaryFile();
				ImGui::SameLine();
				if (ImGui::Button("LoadTask"))
					selectBehaviorTask->LoadOfBinaryFile(taskName[selectTask]);

				ImGui::TextColored(ImVec4(1, 1, 1, 1), "------ParentNodeName------");
				{
					std::string currentParentNodeName = selectBehaviorTask->GetParentNodeName();
					ImGui::BulletText("ParentNodeName -> %s", currentParentNodeName.data());

					if (ImGui::Button("SetParentNodeName"))
						selectBehaviorTask->SetParentNodeName(nodeName[selectNode]);
				}

				ImGui::TextColored(ImVec4(1, 1, 1, 1), "------TaskName------");
				{
					std::string currentTaskName = selectBehaviorTask->GetTaskName();
					ImGui::BulletText("TaskName -> %s", currentTaskName.data());

					if (ImGui::Button("SetTaskName"))
						selectBehaviorTask->SetTaskName(taskName[selectTask]);
				}

				ImGui::TextColored(ImVec4(1, 1, 1, 1), "------Priority------");
				{
					static int priority = static_cast<int>(selectBehaviorTask->GetPriority());
					ImGui::BulletText("TaskPriotiry -> %d", static_cast<int>(selectBehaviorTask->GetPriority()));
					ImGui::SliderInt("SelectTaskPriority", &priority, 1, 7);
					if (ImGui::Button("SetTaskPriority"))
						selectBehaviorTask->SetPriority(static_cast<uint32_t>(priority));
				}

				ImGui::TextColored(ImVec4(1, 1, 1, 1), "------CoolTime------");
				{
					static int  coolTime = static_cast<int>(selectBehaviorTask->GetCoolTimer());
					ImGui::BulletText("CoolTime -> %d", static_cast<int>(selectBehaviorTask->GetCoolTimer()));
					ImGui::SliderInt("CurrentCoolTime", &coolTime, 0, 10);

					//if (ImGui::Button("SetCoolTime"))
					//	selectBehaviorTask->SetCoolTimer(static_cast<uint32_t>(coolTime));
				}
				ImGui::TextColored(ImVec4(1, 1, 1, 1), "------SelectTask------");
				{
					if (ImGui::Button("Set ActiveTask"))
						m_selectTask = selectBehaviorTask;

					if (m_selectTask)
					{
						m_selectTask->Run(this);
						m_stone->Update(m_elapsedTime);
						int state = m_selectTask->GetMoveState();
						ImGui::SliderInt("MoveState", &state, 0, 10);


						if (m_selectTask->GetTaskState() == EnemyBehaviorTask::TASK_STATE::END)
						{
							//m_behaviorTree.AddUseTask(m_selectTask);
							m_selectTask.reset();
						}
						if (ImGui::Button("Release"))
						{
							m_selectTask->SetMoveState(0);
							m_selectTask.reset();

						}
					}
				}
				
			}
		}
	}

	//*******************************************
	// StandardValue
	//*******************************************
	if (ImGui::CollapsingHeader("StandardValue"))
	{
		ImGui::TextColored(ImVec4(1, 1, 1, 1), "------AttackCountValue------");
		{
			int attackCountValue = static_cast<int>(m_standardValuePram.attackCountValue);
			ImGui::SliderInt("AttackCountValue", &attackCountValue, 1, 20);
			m_standardValuePram.attackCountValue = static_cast<uint32_t>(attackCountValue);
		}


		ImGui::TextColored(ImVec4(1, 1, 1, 1), "------AttackRatio------");
		{
			float attackRatio = m_standardValuePram.attackRatio;
			ImGui::SliderFloat("AttackRatio", &attackRatio, 0.0f, 1.0f);
			m_standardValuePram.attackRatio = attackRatio;
		}
		ImGui::TextColored(ImVec4(1, 1, 1, 1), "------ViewFrontValue------");
		{
			CharacterAI* player = MESSENGER.CallPlayerInstance(PlayerType::Fighter);
			int targetID = m_judgeElementPram.targetID;
			VECTOR3F playerPosition = player->GetWorldTransform().position;
			FLOAT4X4 blendBone = m_blendAnimation.animationBlend._blendLocals[currentMesh].at(currentBone);
			FLOAT4X4 modelAxisTransform = m_model->_resource->axisSystemTransform;
			FLOAT4X4 getBone = blendBone * modelAxisTransform * m_transformParm.world;
			VECTOR3F enemyPosition = VECTOR3F(getBone._41,getBone._42,getBone._43);

			float direction = ToDistVec3(playerPosition - enemyPosition);
			VECTOR3F normalizeDist = NormalizeVec3(playerPosition - enemyPosition);

			VECTOR3F angle = m_transformParm.angle;
			VECTOR3F front = VECTOR3F(sinf(angle.y), 0.0f, cosf(angle.y));
			front = NormalizeVec3(front);

			float dot = DotVec3(front, normalizeDist);
			//アサ―と
			float cosTheta = acosf(dot);
			
			float angleValue = cosTheta * (180 / 3.14f);
			ImGui::BulletText("RadOfPlayerToEnemy -> %f", cosTheta);
			ImGui::BulletText("AngleOfPlayerToEnemy -> %f", angleValue);

			float viewFrontValue = m_standardValuePram.viewFrontValue;
			ImGui::SliderFloat("ViewFrontValue", &viewFrontValue, 0.0f, 3.14f);
			 angleValue = viewFrontValue * (180 / 3.14f);
			 ImGui::BulletText("AngleOfViewFrontValue -> %f", angleValue);

			m_standardValuePram.viewFrontValue = viewFrontValue;

			if(cosTheta <= viewFrontValue)
				ImGui::BulletText(u8"見えてます");


		}
	}

	//*******************************************
	// Emotion
	//*******************************************
	if (ImGui::CollapsingHeader("Emotion"))
	{
		if (ImGui::CollapsingHeader("Exhaustion"))
		{
			ImGui::TextColored(ImVec4(1, 1, 1, 1), "------MaxExhaustionCost------");
			{
				int maxExhaustionCost = static_cast<uint32_t>(m_emotionParm.exhaustionParm.maxExhaustionCost);
				ImGui::SliderInt("MaxExhaustionCost", &maxExhaustionCost, 1, 100);
				m_emotionParm.exhaustionParm.maxExhaustionCost = static_cast<uint32_t>(maxExhaustionCost);
			}

			ImGui::TextColored(ImVec4(1, 1, 1, 1), "------MaxWalkExhaustionCost------");
			{
				int maxWakExhaustionCost = static_cast<uint32_t>(m_emotionParm.exhaustionParm.maxWakExhaustionCost);
				ImGui::SliderInt("MaxWalkExhaustionCost", &maxWakExhaustionCost, 1, 100);
				m_emotionParm.exhaustionParm.maxWakExhaustionCost = static_cast<uint32_t>(maxWakExhaustionCost);
			}

			ImGui::TextColored(ImVec4(1, 1, 1, 1), "------ForgetExhaustionCost------");
			{
				int forgetExhaustionCost = static_cast<uint32_t>(m_emotionParm.exhaustionParm.forgetExhaustionCost);
				ImGui::SliderInt("ForgetExhaustionCost", &forgetExhaustionCost, 1, 100);
				m_emotionParm.exhaustionParm.forgetExhaustionCost = static_cast<uint32_t>(forgetExhaustionCost);
			}

			ImGui::TextColored(ImVec4(1, 1, 1, 1), "------MoveExhaustionCost------");
			{
				int moveExhaustionCost = static_cast<uint32_t>(m_emotionParm.exhaustionParm.moveExhaustionCost);
				ImGui::SliderInt("MoveExhaustionCost", &moveExhaustionCost, 1, 100);
				m_emotionParm.exhaustionParm.moveExhaustionCost = static_cast<uint32_t>(moveExhaustionCost);
			}

			ImGui::TextColored(ImVec4(1, 1, 1, 1), "------AttackExhaustionCost------");
			{
				int attackExhaustionCost = static_cast<uint32_t>(m_emotionParm.exhaustionParm.attackExhaustionCost);
				ImGui::SliderInt("AttackExhaustionCost", &attackExhaustionCost, 1, 100);
				m_emotionParm.exhaustionParm.attackExhaustionCost = static_cast<uint32_t>(attackExhaustionCost);
			}

			ImGui::TextColored(ImVec4(1, 1, 1, 1), "------DamageExhaustionCost------");
			{
				int damageExhaustionCost = static_cast<uint32_t>(m_emotionParm.exhaustionParm.damageExhaustionCost);
				ImGui::SliderInt("DamageExhaustionCost", &damageExhaustionCost, 1, 100);
				m_emotionParm.exhaustionParm.damageExhaustionCost = static_cast<uint32_t>(damageExhaustionCost);
			}
		}

		if (ImGui::CollapsingHeader("Wrath"))
		{
			ImGui::TextColored(ImVec4(1, 1, 1, 1), "------MaxWrathCost------");
			{
				int maxWrathCost = static_cast<uint32_t>(m_emotionParm.wrathParm.maxWrathCost);
				ImGui::SliderInt("MaxWrathCost", &maxWrathCost, 1, 100);
				m_emotionParm.wrathParm.maxWrathCost = static_cast<uint32_t>(maxWrathCost);
			}
		}
	}

	//**************************************
	// Animation
	//**************************************
	if (ImGui::CollapsingHeader("Animation"))
	{
		{
			static float ratio = 0;
			ImGui::SliderFloat("BlendRatio", &ratio, 0.0f, 1.0f);

				m_blendAnimation.animationBlend._blendRatio = ratio;

			static float attackRatio = 0;
			ImGui::SliderFloat("AttackBlendRatio", &attackRatio, 0.0f, 0.5f);
			if (ImGui::Button("Attack BlendRatio"))
				m_blendAnimation.attackBlendRtio = attackRatio;

			static float idleRatio = 0;
			ImGui::SliderFloat("IdleBlendRatio", &idleRatio, 0.0f, 1.0f);
			if (ImGui::Button("Idle BlendRatio"))
				m_blendAnimation.idleBlendRtio = idleRatio;

			static float moveRatio = 0;
			ImGui::SliderFloat("MoveBlendRatio", &moveRatio, 0.0f, 1.0f);
			if (ImGui::Button("Move BlendRatio"))
				m_blendAnimation.moveBlendRatio = moveRatio;
		}

		{
			static float blendValue0 = 0.7f;
			ImGui::SliderFloat("WalkBlendValue", &blendValue0, 0.0f, 1.0f);
			if (ImGui::Button("Set WalkBlendValue"))
				m_blendAnimation.blendValueRange[0] = blendValue0;

			static float blendValue1 = 1.0f;
			ImGui::SliderFloat("RunBlendValue", &blendValue1, 0.0f, 1.0f);
			if (ImGui::Button("Set RunBlendValue"))
				m_blendAnimation.blendValueRange[1] = blendValue1;
			
			float animationSpeed = m_blendAnimation.animationBlend._animationSpeed;
			ImGui::SliderFloat("AnimationSpeed", &animationSpeed, -1.0f, 2.0f);
			m_blendAnimation.animationBlend._animationSpeed = animationSpeed;
		}

		{
			auto animCount = m_model->_resource->_animationTakes.size();

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


			{
				if (ImGui::Button(u8"ChangeAnim"))
				{
					m_blendAnimation.animationBlend.ChangeSampler(0, currentAnim, m_model);
				}

				if (ImGui::Button(u8"AddAnim"))
				{
					m_blendAnimation.animationBlend.AddSampler(currentAnim, m_model);
				}

				if (ImGui::Button(u8"UnLoop"))
				{
					m_blendAnimation.animationBlend.FalseAnimationLoop(0);
				}

				if (ImGui::Button(u8"Reset"))
				{
					m_blendAnimation.animationBlend.ResetAnimationSampler(0);
				}
				ImGui::BulletText("%d", m_blendAnimation.animationBlend.GetAnimationTime(0));
			}
		}
		if (ImGui::Button(u8"Relaes"))
		{
			m_blendAnimation.animationBlend.ReleaseSampler(0);
		}
	}

	//**************************************
	// Attack
	//**************************************
	if (ImGui::CollapsingHeader("Attack"))
	{
		static int current = 0;
		ImGui::RadioButton("CrossPunch", &current, 0); ImGui::SameLine();
		ImGui::RadioButton("TurnAttackLower", &current, 1); ImGui::SameLine();
		ImGui::RadioButton("Hook", &current, 2); 
		ImGui::RadioButton("RightPunchLower", &current, 3); ImGui::SameLine();
		ImGui::RadioButton("TurnAttackHeight", &current, 4); ImGui::SameLine();
		ImGui::RadioButton("FallFlat_edit", &current, 5); 
		ImGui::RadioButton("RightPunchUpper", &current, 6); ImGui::SameLine();
		ImGui::RadioButton("WrathNearAttack", &current,7); ImGui::SameLine();
		ImGui::RadioButton("WrathFarAttack", &current, 8); ImGui::SameLine();
		ImGui::RadioButton("RUN", &current, 9);
		//FrameCount
		{
			 int frameCount = m_attackParm[current].frameCount;
			ImGui::InputInt("FrameCount", &frameCount, 0, 100); 

				m_attackParm[current].frameCount = frameCount;
		}

		//attackPoint
		{
			 float point = m_attackParm[current].attackPoint;
			ImGui::InputFloat("AttackPoint", &point, 0, 100);

				m_attackParm[current].attackPoint = point;
		}
	}

	//**************************************
	// Status
	//**************************************
	if (ImGui::CollapsingHeader("Move"))
	{
		{
			static float  accel0 = m_moveParm.accle.x;

			ImGui::SliderFloat("Accel0", &accel0, 0.0f, 10.0f);

			m_moveParm.accle = { accel0 ,0.0f,accel0 };
		}

		{
			static float  maxSpeed0 = m_moveParm.maxSpeed[0].x;
			static float  maxSpeed1 = m_moveParm.maxSpeed[1].x;
			ImGui::SliderFloat("MaxSpeed0", &maxSpeed0, 0.0f, 100.0f);
			ImGui::SliderFloat("MaxSpeed1", &maxSpeed1, 0.0f, 100.0f);

			m_moveParm.maxSpeed[0] = { maxSpeed0 ,0.0f,maxSpeed0 };
			m_moveParm.maxSpeed[1] = { maxSpeed1 ,0.0f,maxSpeed1 };
		}

		{
			static float decleleration = m_moveParm.decleleration;

			ImGui::SliderFloat("Decleleration", &decleleration, 0.0f, 1.0f);

			m_moveParm.decleleration = decleleration;
		}

		{
			 float turnSpeed = m_moveParm.turnSpeed;

			ImGui::SliderFloat("TurnSpeed", &turnSpeed, 0.0f, 0.1f);

			m_moveParm.turnSpeed = turnSpeed;
		}

		{
			float velocity[] = { m_moveParm.velocity.x,m_moveParm.velocity.y,m_moveParm.velocity.z };
			ImGui::SliderFloat3("Velocity", velocity, -100.0f, 100.0f);

		}

		{
			float dist = ToDistVec3(m_moveParm.velocity);
			ImGui::SliderFloat("Dist", &dist, 0.0f, 200.0f);

		}
		static float  speed = m_moveParm.speed.x;
		ImGui::Text("Speed : %f", &speed);


	}

	//**************************************
	// SelectTask
	//**************************************
	if (ImGui::CollapsingHeader("SelectTask") && m_selectTask)
	{
		ImGui::TextColored(ImVec4(1, 1, 1, 1), "-----ParentNodeName-----");
		{
			std::string currentParentNodeName = m_selectTask->GetParentNodeName();
			ImGui::BulletText("ParentNodeName => %s", currentParentNodeName.data());
		}

		ImGui::TextColored(ImVec4(1, 1, 1, 1), "-----TaskName-----");
		{
			std::string currentTaskName = m_selectTask->GetTaskName();
			ImGui::BulletText("TaskName => %s", currentTaskName.data());
		}

		ImGui::TextColored(ImVec4(1, 1, 1, 1), "-----Priority-----");
		{
			ImGui::BulletText("TaskPriotiry => %d", static_cast<int>(m_selectTask->GetPriority()));
		}

		ImGui::TextColored(ImVec4(1, 1, 1, 1), "-----MoveState-----");
		{
			int state = static_cast<int>(m_selectTask->GetMoveState());
			ImGui::SliderInt("MoveState => %d", &state,0,15);
		}

		ImGui::TextColored(ImVec4(1, 1, 1, 1), "-----JudgeElement-----");
		{
			ImGui::BulletText("AttackCount => %d", static_cast<int>(m_judgeElementPram.attackCount));
			ImGui::BulletText("MoveCount => %d", static_cast<int>(m_judgeElementPram.moveCount));
			ImGui::BulletText("DamageCount => %d", static_cast<int>(m_judgeElementPram.damageCount));
			ImGui::BulletText("AttackHitCount => %d", static_cast<int>(m_judgeElementPram.attackHitCount));
			float ratio = static_cast<float>(m_judgeElementPram.attackHitCount) / static_cast<float>(m_judgeElementPram.attackCount);
			ImGui::BulletText("AttackRatio => %f", ratio);
			ImGui::BulletText("TargetID => %d", static_cast<int>(m_judgeElementPram.targetID));
		}

		if (!m_behaviorTree.GetUseTask().empty())
		{
			auto& useTask = m_behaviorTree.GetUseTask();
			std::pair<std::string, float> useTaskData[7];
			for (int i = 0; i < useTask.size(); ++i)
			{
				useTaskData[i].first = useTask.at(i)->GetTaskName();
				useTaskData[i].second = useTask.at(i)->GetCoolTimer();

			}

			ImGui::BulletText(u8"1番目 taskName -> %s :coolTime -> %f",
				useTaskData[0].first.data(), useTaskData[0].second);
			ImGui::BulletText(u8"2番目 taskName -> %s :coolTime -> %f",
				useTaskData[1].first.data(), useTaskData[1].second);
			ImGui::BulletText(u8"3番目 taskName -> %s :coolTime -> %f",
				useTaskData[2].first.data(), useTaskData[2].second);
			ImGui::BulletText(u8"4番目 taskName -> %s :coolTime -> %f",
				useTaskData[3].first.data(), useTaskData[3].second);
			ImGui::BulletText(u8"5番目 taskName -> %s :coolTime -> %f",
				useTaskData[4].first.data(), useTaskData[4].second);
			ImGui::BulletText(u8"6番目 taskName -> %s :coolTime -> %f",
				useTaskData[5].first.data(), useTaskData[5].second);
			ImGui::BulletText(u8"7番目 taskName -> %s :coolTime -> %f",
				useTaskData[6].first.data(), useTaskData[6].second);

		}
	}

	//**************************************
	// Stone
	//**************************************
	if (ImGui::CollapsingHeader("Stone"))
	{
		m_statusParm.attackPoint = 10.0f;
		FLOAT4X4 world = m_transformParm.world;
		VECTOR3F zAxis = { world._31,world._32,world._33 };
		VECTOR3F xAxis = { world._11,world._12,world._13 };
		zAxis = NormalizeVec3(zAxis);
		xAxis = NormalizeVec3(xAxis);
		if (ImGui::Button("Create"))
		{
			VECTOR3F position = m_transformParm.position;
			position += zAxis * m_stone->GetOffsetZ();
			position += xAxis * m_stone->GetOffsetX();

			m_stone->PrepareForStone(position,VECTOR3F(0.0f, 0.0f, 0.0f), zAxis);
		}


		if (ImGui::ArrowButton("Z", ImGuiDir_Left))
		{
			for (auto& stone :  m_stone->GetStoneParam())
			{
				if (!stone.second.isFlying) continue;
				stone.first.position += zAxis * m_stone->GetOffsetZ();
				stone.first.CreateWorld();
			}
		}
		ImGui::SameLine();
		if (ImGui::ArrowButton("X", ImGuiDir_Right))
		{
			for (auto& stone : m_stone->GetStoneParam())
			{
				if (!stone.second.isFlying) continue;
				stone.first.position += xAxis * m_stone->GetOffsetX();
				stone.first.CreateWorld();
			}
		}

		if (ImGui::CollapsingHeader("Update"))
			m_stone->Update(m_elapsedTime);

		m_stone->ImGui(device);
	}


	float posX = m_transformParm.position.x;
	float posZ = m_transformParm.position.z;
	ImGui::SliderFloat("POS X", &posX, -100.0f, 100.0f);
	ImGui::SliderFloat("POS Z", &posZ, -100.0f, 100.0f);

	m_transformParm.position.x = posX;
	m_transformParm.position.z = posZ;

	m_transformParm.WorldUpdate();



	if (ImGui::Button("ActiveBehaviorTree"))
	{
		m_blendAnimation.animationBlend.ChangeSampler(0, Animation::WRATH, m_model);
		m_active = true;
	}
	if (ImGui::Button("DeActiveBehaviorTree"))
		m_active = false;
	ImGui::End();



#endif
}
