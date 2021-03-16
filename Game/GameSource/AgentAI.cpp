#include "AgentAI.h"
#include "MessengTo.h"
#include "ArcherCharacter.h"
#include "WorldAnalyzer.h"

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
	m_blackboard.m_memoryLog[id].m_longTermMemory.erase(std::remove_if(m_blackboard.m_memoryLog[id].m_longTermMemory.
		begin(), m_blackboard.m_memoryLog[id].m_longTermMemory.end(),
		[](WorldAnalyzer& memory)
		{
			float mCredit = memory.GetCreditLv();
			float credit = mCredit - (1.0f / 5.0f);

			if (credit <= 0.0f)
			{
				return true;
			}
			memory.SetCreditLv(credit);

			return false;

		}), m_blackboard.m_memoryLog[id].m_longTermMemory.end());

	m_blackboard.m_memoryLog[id].m_longTermMemory.push_back(m_worldAnalyser);
	int count = static_cast<int>(m_blackboard.m_memoryLog[id].m_longTermMemory.size());
	//6�͕ς���
	if (count > 5)
	{
		count -= 5;
		for (int i = 0; i < count; ++i)
		{
			m_blackboard.m_memoryLog[id].m_longTermMemory.pop_back();
		}
	}
}

void AgentAI::Release()
{
	m_gameMaker.Release();
}

void AgentAI::ImGui(Archer* archer)
{
	m_gameMaker.ImGui(archer);
	m_blackboard.ImGui();
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
		auto& select = i == 0 ? playerLog.m_longTermMemory : enemyLog.m_longTermMemory;

		select.erase(std::remove_if(select.begin(),select.end(),
			[](WorldAnalyzer& memory)
			{
				float mCredit = memory.GetCreditLv();
				if (mCredit <= 0.4f)
					return true;
				return false;

			}), select.end());
	}

	ArcherWorldState worldState = mySelf->GetWorldState();
	worldState.MeterReset();
	if(playerLog.m_longTermMemory.size() == 0 || enemyLog.m_longTermMemory.size() == 0)
		m_gameMaker.SetRootTask(CompoundTaskType::Attack);


	#pragma region Player
	{

		auto begin = playerLog.m_longTermMemory.begin();
		auto end = playerLog.m_longTermMemory.end();
		VECTOR3F position = begin->GetPosition();
		float movePosition = 0.0f;
		for (begin; begin != end; ++begin)
		{
			//�v���C���[��HP�󋵂𔻒f
			HitPointLv hpLv = begin->GetHpLv();
			PerceptionOfRecover(worldState, hpLv);
			#pragma region ���
			//�ʒu���ǂꂾ�������Ă邩�𔻒f+�G�̍ŏI�̋���
			//�����ĂȂ��Ȃ�v���C���[�̐M���x��������
			//�t�Ȃ�グ��(���START�₩�猸���Ă���オ��)
			movePosition = ToDistVec3(begin->GetPosition() - position);
			position = begin->GetPosition();
			#pragma endregion
		}

		if (movePosition < 2.5f)
		{
			float& cred = mySelf->GetPlayerCreditLv();
			cred-=0.1f;
		}
		else
		{
			float& cred = mySelf->GetPlayerCreditLv();
			cred += 0.05f;
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

	int myHpLv = 0;
	float targetHp = mySelf->GetStatus().life;
	float targetMaxHp = mySelf->GetStatus().maxLife;
	float hpRatio = targetHp / targetMaxHp;
	int maxHpLv = static_cast<int>(HitPointLv::END);
	const float divisionHp = 1.0f / maxHpLv;
	float minHp = 0.0f;
	for (int i = 0; i < maxHpLv; ++i)
	{
		if (minHp < hpRatio && hpRatio <= minHp + divisionHp)
			myHpLv = i;

		minHp += divisionHp;
	}

	#pragma endregion

	//������8�ȏ�(8�͓K��)�Ȃ�񕜂���
	//���A�v���C���[�̐M���x���Ⴂ�Ȃ�8����12��ւ�
	int recoverMaxMeter = mySelf->GetPlayerCreditLv() < 0.5f ? 11 : 7;

	//�ŏI�I�ɉ񕜂�8,12�ȏ�Ȃ��
	//�U����5�ȏ�Ȃ�
	//�����8�ȏ�Ȃ�
	//�Ŕ��f�������ύX�͂���
	if (worldState._recoverMeter > recoverMaxMeter)
	{
		if (mySelf->GetCanRecover())
		{
			m_gameMaker.SetRootTask(CompoundTaskType::Recover);
			mySelf->GetRecoverParm().isPlayer = true;
			mySelf->GetWorldState()._canRecover = true;
			return;
		}
	}
	else if (myHpLv < static_cast<int>(HitPointLv::MINOR_INJURY))
	{
		if (mySelf->GetCanRecover())
		{
			m_gameMaker.SetRootTask(CompoundTaskType::Recover);
			mySelf->GetRecoverParm().isPlayer = false;
			mySelf->GetWorldState()._canRecover = true;
			return;
		}
	}

	int avoidMaxMeter = 7;
	if (worldState._avoidMeter > avoidMaxMeter)
	{
		m_gameMaker.SetRootTask(CompoundTaskType::Avoid);
	}

	int atkMaxMeter = 6;
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
		worldState._avoidMeter += 3;
		worldState._attackMeter += 1;
		break;
	case DistanceLv::FAR_WAY:
		worldState._attackMeter += 2;
		worldState._recoverMeter += 1;
		worldState._avoidMeter += 1;

		break;
	}
}
