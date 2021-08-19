#pragma once
#include "TaskBase.h"
#include "Method.h"

#pragma region CompoundTaskについて
//CompoundTaskはMethodを複数持ちます。
//MethodというのはPreconditionとTaskを持ちます。

//仮想関数から必要なものだけ記載します。
//Precondition,Effect,OperatorはCompoundTaskには必要ありません。
//であれば継承するなと言いたくなると思いますが、プランニングのときに
//少々困るので継承させます。

//CompoundTaskは複数の動きの提案(Method)を
//もつクラスといえばわかりやすいでしょうか
#pragma endregion

template<class TWorldState,class TChara>
class CompoundTask : public TaskBase<TWorldState, TChara>
{
public:
	CompoundTask() = default;
	virtual ~CompoundTask() = default;

	inline TaskType GetTaskType() override { return m_taskType; }
	inline void SetTaskType(TaskType& type) override { m_taskType = type; }

	inline std::string GetTaskName() override { return m_taskName; }
	inline void SetTaskName(std::string taskName) override { m_taskName = taskName; }

	std::vector<std::shared_ptr<Method<TWorldState, TChara>>>& GetMethod() { return m_methods; }
	void AddMethod(std::shared_ptr<Method<TWorldState, TChara>> method) { m_methods.push_back(method); }

protected:
	std::vector<std::shared_ptr<Method<TWorldState, TChara>>> m_methods;
	std::string m_taskName;
	TaskType m_taskType = TaskType::Non;
};