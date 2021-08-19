#pragma once
#include <vector>
#include <memory>
#include "CompoundTask.h"
#include "TaskBase.h"

#pragma region HTNPlannerについて
//てめぇ(HTNPlanner)は俺を困らせた
//ここが鬼門だった。上限クラスの強さ。時間かかった。
//ここではTWorldStateとrootTaskからプランを練り上げる
//ように作られている。

//詳しい説明は中で...
#pragma endregion

#pragma region PlannerStateについて
//HTNPlannerでしか使用しないため中にいれてます
//こいつにはプランニング必要なものを纏めています
//TWorldState:「現在」のTWorldState
//finalPlanList:思考の末たどり着いた結果のTask
//taskListToProcess:思考するTaskの一覧
//nextMethodNumber:同じmethodは見なくて結構です
#pragma endregion

template<class TWorldState,class TChara>
class HTNPlanner
{
public:
	HTNPlanner() = default;
	~HTNPlanner() = default;

	//名前長いんで変更
	using PlanList = std::vector<TaskBase<TWorldState, TChara>*>;
	
	struct PlannerState
	{
		TWorldState worldState;
		PlanList finalPlanList;
		PlanList taskListToProcess;
		int nextMethodNumber = 0;

		void Clear()
		{
			finalPlanList.clear();
			taskListToProcess.clear();
			nextMethodNumber = 0;
		}
	};

	PlanList& Planning(TWorldState& currentState, TaskBase<TWorldState, TChara>* rootTask)
	{
		//初期設定部分です
		{
			m_historyState.clear();
			m_plannerState.Clear();
			m_plannerState.worldState = currentState;
			m_plannerState.taskListToProcess.push_back(rootTask);
			m_plannerState.nextMethodNumber = 0;
		}

		//思考するタスクリストが無くなり次第終了です
		while (m_plannerState.taskListToProcess.size() > 0)
		{
			TaskBase<TWorldState, TChara>* currentTask = m_plannerState.taskListToProcess[0];
			//タスクがPrimitiveTaskなのかCompoundTaskなのかで
			//処理が変わるので、まずはどちらかを確かめます。
			if (currentTask->GetTaskType() == TaskType::Compound)
			{
				//ここでMethodがあるかないかで処理が変わります
				Method<TWorldState, TChara>* satisfiedMethod = FindSatisfiedMethod(currentTask);
				if (satisfiedMethod)
				{
					//ここでは三つの作業をします
					//1.historyStateに記録させる
					//2.taskListToProcessの0番目(今見ているTask)を消去
					//3.subTaskをtaskListToProcessに追加
					RecordDecompositionOfTask(currentTask);
					m_plannerState.taskListToProcess.erase(m_plannerState.taskListToProcess.begin());
					AddToTaskLiast(satisfiedMethod);
				}
				else
				{
					RestoreToLastDecomposedTask();
				}
			}
			else
			{
				if (currentTask->CheckPreCondition(m_plannerState.worldState))
				{
					m_plannerState.taskListToProcess.erase(m_plannerState.taskListToProcess.begin());
					m_plannerState.finalPlanList.push_back(currentTask);
					currentTask->ApplyEffects(m_plannerState.worldState);
				}
			}
		}

		return m_plannerState.finalPlanList;
	}

	//使用可能なMethodを探します
	Method<TWorldState, TChara>* FindSatisfiedMethod(
		TaskBase<TWorldState, TChara>* currentCompoundTask);

	//PlannerStateを段階的に記録させる
	void RecordDecompositionOfTask(
		TaskBase<TWorldState, TChara>* nextCompoundTask);

	//subTaskを追加していく
	void AddToTaskLiast(Method<TWorldState, TChara>* currentMethod);

	//アクシデント発生。最新の記録を取り出せ
	void RestoreToLastDecomposedTask();

private:
	PlannerState m_plannerState;
	std::vector<PlannerState> m_historyState;
};

template<class TWorldState, class TChara>
inline Method<TWorldState, TChara>* HTNPlanner<TWorldState, TChara>::FindSatisfiedMethod(
	TaskBase<TWorldState, TChara>* currentCompoundTask)
{
	//はいここ私ポイント! 絶対にこれって決まっている時はstatic_castでもダウンキャスト可能なの
	//ここではMethodを持つクラスなんてCompoundTaskしかないからこれでいいのぉよぉ
	auto& methods = static_cast<CompoundTask<TWorldState, TChara>*>(currentCompoundTask)->GetMethod();

	//この中で使用できるMethodを探し出しているのぉよぉ
	while (m_plannerState.nextMethodNumber < static_cast<int>(methods.size()))
	{
		auto& method = methods[m_plannerState.nextMethodNumber];
		++m_plannerState.nextMethodNumber;
		if (method->CheckPreCondition(m_plannerState.worldState))
		{
			return method.get();
		}
	}

	return nullptr;
}

template<class TWorldState, class TChara>
inline void HTNPlanner<TWorldState, TChara>::RecordDecompositionOfTask(
	TaskBase<TWorldState, TChara>* nextCompoundTask)
{
	//現在のプランニング状態(PlannerState)を記録させておく
	//これによりアクシデントがあっても一つ前の記録を下ろしてきて
	//再プランニングが可能になる
	m_historyState.push_back(m_plannerState);
	m_plannerState.nextMethodNumber = 0;
	//m_plannerState.taskListToProcess.push_back(nextCompoundTask);
}

template<class TWorldState, class TChara>
inline void HTNPlanner<TWorldState, TChara>::AddToTaskLiast(
	Method<TWorldState, TChara>* currentMethod)
{
	//はいここ私ポイント! taskListToProcessにはsubTaskの後ろから(2->1->0)
	//taskListToProcessの前に挿入していくわぁ。そうすることでtaskListToProcessには
	//0->1->2の順で入るからよぉ。順番がとても大事なのよここ。
	//subTaskを前から(0->1->2)の順だと2->1->0の順でtaskListToProcessに入るから
	//「走る->走る方向を決める->準備する」の順だと変だということがわかるわよねぇ
	auto& subTasks = currentMethod->GetSubTask();
	int  select = static_cast<int>(subTasks.size());
	for (int i = select; i > 0; --i)
	{
		std::shared_ptr<TaskBase<TWorldState, TChara>> ptr = subTasks[i - 1].lock();
		m_plannerState.taskListToProcess.insert(
			m_plannerState.taskListToProcess.begin(), ptr.get());
	}
}

template<class TWorldState, class TChara>
inline void HTNPlanner<TWorldState, TChara>::RestoreToLastDecomposedTask()
{
	//記録がないなら諦めよう！
	size_t size = m_historyState.size();
	if (size == 0)
	{
		m_plannerState.Clear();
		return;
	}

	//最新の記録を渡す。そして消す
	m_plannerState = m_historyState.at(size - 1);
	m_historyState.pop_back();
}
