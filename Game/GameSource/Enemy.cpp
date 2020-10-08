#include "Enemy.h"
#include "MessengTo.h"
#include "EnemyTaskAll.h"
#include ".\LibrarySource\ModelData.h"
#include ".\LibrarySource\VectorCombo.h"

//CEREAL_CLASS_VERSION(CharacterParameter::Collision, 1);
CEREAL_CLASS_VERSION(Enemy, 6);

void Enemy::Init()
{
	m_transformParm.position = { -10.0f,0.0f,80.0f };
	m_transformParm.angle = { 0.0f * 0.01745f, 180.0f * 0.01745f,0.0f * 0.017454f };
	m_transformParm.scale = { 1.25f,1.25f,1.25f };
	m_transformParm.WorldUpdate();

	m_model = Source::ModelData::fbxLoader().GetActorModel(Source::ModelData::ActorModel::ENEMY);
	//Source::ModelData::fbxLoader().SaveActForBinary(Source::ModelData::ActorModel::ENEMY);

	m_statusParm.life = 12000.0f;

	m_blendAnimation.animationBlend.Init(m_model);
	m_collision.resize(4);
	m_attackParm.resize(13);
	//SerialVersionUpdate(1);

	m_behaviorTree.CreateRootNode();
	if (PathFileExistsA((std::string("../Asset/Binary/Enemy/Parameter") + ".bin").c_str()))
	{
		std::ifstream ifs;
		ifs.open((std::string("../Asset/Binary/Enemy/Parameter") + ".bin").c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);
	}
	
	m_behaviorTree.SetRootNodeChild();
	m_behaviorTree.SetTaskToNode();
	m_isAction = false;
}

void Enemy::Update(float& elapsedTime)
{
	m_elapsedTime = elapsedTime;
	if (m_isAction)
	{
		if (!m_selectTask)
			m_selectTask = m_behaviorTree.SearchOfActiveTask(m_id);

		m_selectTask->Run(this);

		if (m_selectTask->GetTaskState() == EnemyBehaviorTask::TASK_STATE::END)
		{
			m_selectTask = m_behaviorTree.SearchOfActiveTask(m_id);
		}
	}
	m_blendAnimation.animationBlend.Update(m_model, elapsedTime);

	m_collision[0].position[0] = m_transformParm.position;
}

void Enemy::Render(ID3D11DeviceContext* immediateContext)
{
	auto& localTransforms = m_blendAnimation.animationBlend._blendLocals;
	//	auto& localTransforms = m_blendAnimation.partialBlend._blendLocals;
	VECTOR4F color{ 1.0f,1.0f,1.0f,1.0f };
	m_model->Render(immediateContext, m_transformParm.world, color, localTransforms);
	//m_debugObjects.debugObject.Render(immediateContext, m_debugObjects.scrollValue,false);
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

	if (ImGui::Button("ASK_ATTACK"))
	{
		MESSENGER.MessageFromEnemy(m_id, MessengType::ASK_ATTACK);
	}


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
					if (current == 0)
					{
						geomtry.position = m_transformParm.position;
					}
					else
					geomtry.position = bonePosition;
				}
				//Scale
				{
					static float scale = m_collision[current].scale;
					if (scale <= 0.0f) scale = 1.0f;
					ImGui::SliderFloat("Scale", &scale, 0.1f, 100.0f);
					geomtry.scale = { scale ,scale ,scale };
					m_collision[current].scale = scale;
				}

				//Radius
				{
					static float radius = m_collision[current].radius;
					if (radius <= 0.0f) radius = 1.0f;
					ImGui::SliderFloat("Radius", &radius, 1.0f, 20.0f);

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
			"FightFarNode"
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

			if (ImGui::CollapsingHeader("FightNearNode"))
			{
				std::shared_ptr<EnemyFightNearNode> selectNearNode = nodeData.fightNearNode;

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
					auto& players = MESSENGER.CallPlayersInstance();
					static int selectPlayer = 0;
					int playerCount = static_cast<int>(players.size());
					for (int i = 0; i < playerCount; ++i)
					{
						if (players[i]->GetChangeComand().isPlay)
							selectPlayer = i;
					}
					VECTOR3F playerPosition = players[selectPlayer]->GetWorldTransform().position;
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
				if (ImGui::Button("Chase"))
					selectBehaviorNode = nodeData.chaseNode;
				ImGui::SameLine();
				if (ImGui::Button("Fight"))
					selectBehaviorNode = nodeData.fightNode;
				ImGui::SameLine();
				if (ImGui::Button("FightFar"))
					selectBehaviorNode = nodeData.fightFarNode;

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
			"ChaseTask",
			"NearAttack0Task",
			"NearAttack1Task",
			"NearAttack2Task",
			"NearSpecialAttack0Task",
			"FarAttack0Task",
			"FarAttack1Task",
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
			ImGui::SameLine();
			if (ImGui::Button("ChaseTask"))
			{
				taskData.CreateTaskData(ENTRY_TASK::CHASE_TASK);
				selectBehaviorTask = taskData.chaseTask;
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
			ImGui::SameLine();
			if (ImGui::Button("NearAttack2Task"))
			{
				taskData.CreateTaskData(ENTRY_TASK::NEAR_ATTACK2_TASK);
				selectBehaviorTask = taskData.fightNearTask2;
			}
			ImGui::SameLine();
			if (ImGui::Button("NearSpecialAttack0"))
			{
				taskData.CreateTaskData(ENTRY_TASK::Near_Special_TASK);
				selectBehaviorTask = taskData.fightNearSpecialTask0;

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
					ImGui::SliderInt("SelectTaskPriority", &priority, 1, 4);
					if (ImGui::Button("SetTaskPriority"))
						selectBehaviorTask->SetPriority(static_cast<uint32_t>(priority));
				}

				ImGui::TextColored(ImVec4(1, 1, 1, 1), "------CoolTime------");
				{
					static int  coolTime = static_cast<int>(selectBehaviorTask->GetCoolTimer());
					ImGui::BulletText("CoolTime -> %d", static_cast<int>(selectBehaviorTask->GetCoolTimer()) / 60);
					ImGui::SliderInt("CurrentCoolTime", &coolTime, 0, 6000);

					if (ImGui::Button("SetCoolTime"))
						selectBehaviorTask->SetCoolTimer(static_cast<uint32_t>(coolTime));
				}
				ImGui::TextColored(ImVec4(1, 1, 1, 1), "------SelectTask------");
				{
					if (ImGui::Button("Set ActiveTask"))
						m_selectTask = selectBehaviorTask;

					if (m_selectTask)
					{
						m_selectTask->Run(this);
						if (m_selectTask->GetTaskState() == EnemyBehaviorTask::TASK_STATE::END)
							m_selectTask.reset();

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

		ImGui::TextColored(ImVec4(1, 1, 1, 1), "------AttackHitCountValue------");
		{
			int attackHitCountValue = static_cast<int>(m_standardValuePram.attackHitCountValue);
			ImGui::SliderInt("AttackHitCountValue", &attackHitCountValue, 1, 20);
			m_standardValuePram.attackHitCountValue = static_cast<uint32_t>(attackHitCountValue);
		}

		ImGui::TextColored(ImVec4(1, 1, 1, 1), "------ViewFrontValue------");
		{
			auto& players = MESSENGER.CallPlayersInstance();
			int targetID = m_judgeElementPram.targetID;
			VECTOR3F playerPosition = players[targetID]->GetWorldTransform().position;
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
			ImGui::SliderFloat("AnimationSpeed", &animationSpeed, 1.0f, 2.0f);
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
		ImGui::RadioButton(u8"右なぎ払い前行動", &current, 0); ImGui::SameLine();
		ImGui::RadioButton(u8"右なぎ払い", &current, 1); ImGui::SameLine();
		ImGui::RadioButton(u8"右なぎ払い後行動", &current, 2); 
		ImGui::RadioButton(u8"左なぎ払い前行動", &current, 3); ImGui::SameLine();
		ImGui::RadioButton(u8"左なぎ払い", &current, 4); ImGui::SameLine();
		ImGui::RadioButton(u8"左なぎ払い後行動", &current, 5); 
		ImGui::RadioButton(u8"スタンプ", &current, 6); ImGui::SameLine();
		ImGui::RadioButton(u8"回転", &current,7); ImGui::SameLine();
		ImGui::RadioButton(u8"高速回転", &current, 8);

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

			ImGui::SliderFloat("TurnSpeed", &turnSpeed, 0.0f, 0.05f);

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

	if (ImGui::Button("ActiveBehaviorTree"))
		m_isAction = true;
	if (ImGui::Button("DeActiveBehaviorTree"))
		m_isAction = false;
	ImGui::End();



#endif
}
