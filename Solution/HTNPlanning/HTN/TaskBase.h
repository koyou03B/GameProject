#pragma once
#include <string>


enum TaskType
{
	Primitive,
	Compound,
	Non
};

#pragma region TaskBaseについて
//TaskBaseはPrimitiveTaskやCompoundTaskといった
//Taskの基底にあたるクラス。このクラスを通して
//Plannerにプランニングしてもらう。
//Precondition,Effect,Operatorの各機能を持っている。

//PreconditionはTaskを行うに値すかというかという前提条件の役割
//EffectはTaskを行った際にどういった影響が出るかという役割
//Operatorは実際に行う行動を担っている

//TWorldStateというのは自分の状態もふくめたゲームの世界全体のこと
#pragma endregion
template<class TWorldState, class TChara>
class TaskBase
{
public:
	TaskBase() = default;
	virtual ~TaskBase() = default;
	
	virtual inline TaskType GetTaskType() = 0;
	virtual inline void SetTaskType(TaskType& type) = 0;

	virtual inline std::string GetTaskName() = 0;
	virtual inline void SetTaskName(std::string taskName) = 0;

	virtual bool CheckPreCondition(const TWorldState& state) { return true; }
	virtual void ApplyEffects(TWorldState& state) { return ; }
	virtual bool ExecuteOperator(TChara* chara) { return true; }
};