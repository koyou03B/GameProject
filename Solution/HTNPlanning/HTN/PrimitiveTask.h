#pragma once
#include "TaskBase.h"

#pragma region PrimitiveTaskについて
//PrimitiveTaskはTaskBaseに記載している
//Precondition,Effect,Operatorで構成されている

//TWorldStateはTWorldStateを継承したクラスを指定してください
//そうすることで各キャラクターごとのTWorldStateを見ることができます

//TCharaではキャラクターのクラスを指定してください
//そうすることで各キャラクターの行動を実行できます
#pragma endregion
template<class TWorldState,class TChara>
class PrimitiveTask : public TaskBase<TWorldState,TChara>
{
public:
	PrimitiveTask() = default;
	virtual ~PrimitiveTask() = default;

	//タスクを実行できるか
	bool CheckPreCondition(const TWorldState& state) override	
	{ return IsSatisfiedPreConditions(state);}

	//タスクを実行した場合の影響は
	void ApplyEffects(TWorldState& state) override
	{ ApplyEffectsToWorldState(state); }

	//タスクを実行する
	bool ExecuteOperator(TChara* chara) override
	{ return Execute(chara); }

	inline TaskType GetTaskType() override { return m_taskType; }
	inline void SetTaskType(TaskType& type) override { m_taskType = type; }

	inline std::string GetTaskName() override { return m_taskName; }
	inline void SetTaskName(std::string taskName) override { m_taskName = taskName; }

	//型名ActFuncで引数TWorldStateの関数ポインタ
	typedef bool (TChara::* ActFunc)();
	inline void SetOperator(ActFunc func) { m_func = func; }

protected:
	//このクラスを継承したクラスで詳しい前提条件を記載してもらいます
	//また、Effectも同様です。
	virtual bool IsSatisfiedPreConditions(const TWorldState& currentState) { return true; }
	virtual void ApplyEffectsToWorldState(TWorldState& previousState) { return; };

	std::string m_taskName;
	TaskType m_taskType = TaskType::Non;
private:
	bool Execute(TChara* chara)
	{
		return (chara->*m_func)();
	}
	ActFunc m_func;
};


