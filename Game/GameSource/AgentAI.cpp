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

	//HTN起動
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
	//6は変える
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

	//信頼度が0.4以下は興味なし
	//それ以上のものを取り出して
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
			//プレイヤーのHP状況を判断
			HitPointLv hpLv = begin->GetHpLv();
			PerceptionOfRecover(worldState, hpLv);
			#pragma region 後で
			//位置がどれだけ動いてるかを判断+敵の最終の距離
			//動いてないならプレイヤーの信頼度を下げる
			//逆なら上げる(上限STARTやから減ってたら上がる)
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
			//エネミーの距離を判断
			DistanceLv distLv = begin->GetDinstanceLv();
			PerceptionOfDistance(worldState, distLv);
		}
	}
	#pragma endregion
	
	#pragma region 現在

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

	//足して8以上(8は適当)なら回復する
	//が、プレイヤーの信頼度が低いなら8から12らへん
	int recoverMaxMeter = mySelf->GetPlayerCreditLv() < 0.5f ? 11 : 7;

	//最終的に回復が8,12以上なら回復
	//攻撃が5以上なら
	//回避が8以上なら
	//で判断もちろん変更はする
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
	//無傷：0 軽傷:1 重傷:3 致命的:5
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
	//敵の距離を過去と現在の距離で遠い攻撃:3 回復:1
	//近いが多いなら回避:2 攻撃:1

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
