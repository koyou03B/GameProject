#include "Enemy.h"
#include "MessengTo.h"
#include "EnemyTaskAll.h"
#include ".\LibrarySource\ModelData.h"
#include ".\LibrarySource\VectorCombo.h"

//CEREAL_CLASS_VERSION(CharacterParameter::Collision, 1);
CEREAL_CLASS_VERSION(Enemy, 3);

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
}

void Enemy::Update(float& elapsedTime)
{
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
			//static bool isCreate = false;
			//if (!isCreate)
			//{
			//	nodeData.CreateNodeData(ENTRY_NODE::WAIT_NODE);		
			//	nodeData.CreateNodeData(ENTRY_NODE::CHASE_NODE);
			//	nodeData.CreateNodeData(ENTRY_NODE::FIGHT_NODE);
			//	nodeData.CreateNodeData(ENTRY_NODE::FIGHT_NEAR_NODE);
			//	nodeData.CreateNodeData(ENTRY_NODE::FIGHT_FAR_NODE);
			//	isCreate = true;
			//}

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
						if (ImGui::Button("RestTask"))
						{
							std::shared_ptr<EnemyRestTask> restTask = std::make_shared<EnemyRestTask>();
							selectBehaviorNode->SetTask(restTask);
						}
					}

					ImGui::TextColored(ImVec4(1, 1, 1, 1), "------Add------");
					{
						if (ImGui::Button("AddNode"))
							m_behaviorTree.AddNode(selectBehaviorNode->GetParentName(), selectBehaviorNode);

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
			}
		}
	}
	ImGui::End();

#endif
}
