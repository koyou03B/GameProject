#include "AgentAI.h"
#include "MessengTo.h"
#include "ArcherCharacter.h"

std::vector<TaskBase<ArcherWorldState, Archer>*> AgentAI::Init(Archer* mySelf)
{
	m_gameMaker.Init();
	m_gameMaker.SetRootTask(CompoundTaskType::Attack);
	ArcherWorldState worldState = mySelf->GetWorldState();
	return m_gameMaker.SearchBehaviorTask(worldState);
}

std::vector<TaskBase<ArcherWorldState, Archer>*> AgentAI::OperationFlowStart(Archer* mySelf)
{
	CreatePerception(mySelf);

	//HTN�N��
	ArcherWorldState worldState = mySelf->GetWorldState();
	return m_gameMaker.SearchBehaviorTask(worldState);
}

void AgentAI::SavePerception(CharacterAI* mySelf, CharacterAI* target)
{
	m_worldAnalyser.Analyze(mySelf, target);
	int id = static_cast<int>(target->GetID());
	m_blackboard.m_memoryLog[id].m_longTermMemory.push_back(m_worldAnalyser);
	size_t count = m_blackboard.m_memoryLog[id].m_longTermMemory.size();

	//6�͕ς���
	if (count > 6)
	{
		auto data = m_blackboard.m_memoryLog[id].m_longTermMemory.begin();
		m_blackboard.m_memoryLog[id].m_longTermMemory.erase(data);
	}
}

void AgentAI::Release()
{
	m_gameMaker.Release();
}

void AgentAI::ImGui(Archer* archer)
{
	m_gameMaker.ImGui(archer);
}

void AgentAI::CreatePerception(Archer* mySelf)
{
	CharacterAI* target = MESSENGER.CallEnemyInstance(EnemyType::Boss);
	m_worldAnalyser.Analyze(mySelf, target);
	int id = static_cast<int>(target->GetID());
	int targetID = target->GetJudgeElement().targetID;
	m_blackboard.m_memoryLog[id].m_workingMemory = m_worldAnalyser;
	Blackboard::MemoryLog enemyLog = m_blackboard.m_memoryLog[id];
	m_blackboard.m_memoryLog[id].m_longTermMemory.push_back(m_worldAnalyser);

	target = MESSENGER.CallPlayerInstance(PlayerType::Fighter);
	m_worldAnalyser.Analyze(mySelf, target);
	id = static_cast<int>(target->GetID());
	m_blackboard.m_memoryLog[id].m_workingMemory = m_worldAnalyser;
	Blackboard::MemoryLog playerLog = m_blackboard.m_memoryLog[id];
	m_blackboard.m_memoryLog[id].m_longTermMemory.push_back(m_worldAnalyser);

	//�M���x��0.4�ȉ��͋����Ȃ�
	//����ȏ�̂��̂����o����
	for (int i = 0; i < 2; ++i)
	{
		auto begin = i == 0 ? playerLog.m_longTermMemory.begin() : enemyLog.m_longTermMemory.begin();
		auto end   = i == 0 ? playerLog.m_longTermMemory.end() : enemyLog.m_longTermMemory.end();

		while (begin != end)
		{
			if (begin->GetCreditLv() <= 0.4f)
			{
				if (i == 0)
					begin = playerLog.m_longTermMemory.erase(begin);
				else
					begin = enemyLog.m_longTermMemory.erase(begin);
			}
			else
				++begin;
		}
	}

	ArcherWorldState worldState = mySelf->GetWorldState();
	worldState.MeterReset();
	#pragma region Player
	{
		auto begin = playerLog.m_longTermMemory.begin();
		auto end = playerLog.m_longTermMemory.end();
		for (begin; begin != end; ++begin)
		{
			//�v���C���[��HP�󋵂𔻒f
			HitPointLv hpLv = begin->GetHpLv();
			PerceptionOfRecover(worldState, hpLv);
			#pragma region ���
			//�ʒu���ǂꂾ�������Ă邩�𔻒f+�G�̍ŏI�̋���
			//����Ă邩�����ĂȂ��Ȃ�v���C���[�̐M���x��������
			//�t�Ȃ�グ��(���START�₩�猸���Ă���オ��)
			#pragma endregion
		}


	}
	#pragma endregion

	#pragma region Enemy
	{
		auto begin = enemyLog.m_longTermMemory.begin();
		auto end = enemyLog.m_longTermMemory.end();
		for (begin; begin != end; ++begin)
		{
			//�G�l�~�[�̋����𔻒f
			DistanceLv distLv = begin->GetDinstanceLv();
			PerceptionOfDistance(worldState, distLv);
		}
	}
	#pragma endregion
	
	#pragma region ����

	HitPointLv hpLv = playerLog.m_workingMemory.GetHpLv();
	PerceptionOfRecover(worldState, playerLog.m_workingMemory.GetHpLv());
	DistanceLv distLv = enemyLog.m_workingMemory.GetDinstanceLv();
	PerceptionOfDistance(worldState, distLv,true);

	#pragma endregion

	//������8�ȏ�(8�͓K��)�Ȃ�񕜂���
	//���A�v���C���[�̐M���x���Ⴂ�Ȃ�8����12��ւ�
	int recoverMaxMeter = mySelf->GetPlayerCreditLv() < 0.5f ? 12 : 8;

	//�ŏI�I�ɉ񕜂�8,12�ȏ�Ȃ��
	//�U����5�ȏ�Ȃ�
	//�����8�ȏ�Ȃ�
	//�Ŕ��f�������ύX�͂���
	if (worldState._recoverMeter > recoverMaxMeter)
	{
		if (mySelf->GetCanRecover())
		{
			m_gameMaker.SetRootTask(CompoundTaskType::Recover);
			return;
		}
	}

	int avoidMaxMeter = 8;
	if (worldState._avoidMeter > avoidMaxMeter)
		m_gameMaker.SetRootTask(CompoundTaskType::Avoid);

	int atkMaxMeter = 5;
	if (worldState._attackMeter > atkMaxMeter)
		m_gameMaker.SetRootTask(CompoundTaskType::Attack);

}

void AgentAI::PerceptionOfRecover(ArcherWorldState& worldState, const HitPointLv& hpLv)
{
	//�����F0 �y��:1 �d��:3 �v���I:5
	switch (hpLv)
	{
	case HitPointLv::UNINJURED:
		worldState._recoverMeter += m_recoverPoint[0];
		break;
	case HitPointLv::MINOR_INJURY:
		worldState._recoverMeter += m_recoverPoint[1];
		break;
	case HitPointLv::SEVERE_WOUND:
		worldState._recoverMeter += m_recoverPoint[2];
		break;
	case HitPointLv::FATAL:
		worldState._recoverMeter += m_recoverPoint[3];
		break;
	case HitPointLv::END:
		break;
	}

}

void AgentAI::PerceptionOfDistance(ArcherWorldState& worldState, const DistanceLv& hpLv, bool isCurrent)
{
	//�G�̋������ߋ��ƌ��݂̋����ŉ����U��:3 ��:1
	//�߂��������Ȃ���:2 �U��:1

	switch (hpLv)
	{
	case DistanceLv::NEAR_AWAY:
		worldState._avoidMeter += 2;
		worldState._attackMeter += 1;
		break;
	case DistanceLv::FAR_WAY:
		worldState._attackMeter += 3;
		worldState._recoverMeter += 1;
		worldState._avoidMeter += 1;

		break;
	}
}
