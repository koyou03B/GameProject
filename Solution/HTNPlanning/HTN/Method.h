#pragma once
#include <vector>
#include <memory>
#include "PreconditionBase.h"
#include "TaskBase.h"

#pragma region Methodについて
//Methodといのは複数のTaskとPreconditionを所持しています
//これは「俺この条件(Precondition)満たしてたら
//これ(Task)できますよ?」とCompoundTaskにアピールしている
//って考え方をイメージしてくれたらわかりやすいかなと思ってます。

//TaskはPrimitiveTaskでもCompoundTaskでもいいです
//このときに基底が同じだと助かるんですよ～
#pragma endregion
template<class TWorldState,class TChara>
class Method
{
public:
	Method() = default;
	~Method() = default;

	//Preconditionを満たしているかを判断します
	//見たいていないのであればこのMethodは使用不可
	bool CheckPreCondition(TWorldState& state)
	{
		size_t count = m_preconditions.size();
		for (size_t i = 0; i < count; ++i)
		{
			if (m_preconditions.at(i)->CheckPreCondition(state))
				return true;
		}
		return false;
	}
	std::vector<std::weak_ptr<TaskBase<TWorldState, TChara>>>& GetSubTask() { return m_subTasks; }

	inline void AddSubTask(std::shared_ptr<TaskBase<TWorldState, TChara>> subTask) { m_subTasks.push_back(subTask); }
	inline void AddPrecondition(std::shared_ptr<PreconditionBase<TWorldState>> precondition) { m_preconditions.push_back(precondition); }

	inline void SetSubTasks(std::vector<std::shared_ptr<TaskBase<TWorldState, TChara>>> subTasks)
	{
		m_subTasks = subTasks;
	}

	inline void SetPreconditions(std::vector<std::shared_ptr<PreconditionBase<TWorldState>>> preconditions)
	{
		m_preconditions = preconditions;
	}

protected:
	std::vector<std::weak_ptr<TaskBase<TWorldState, TChara>>> m_subTasks;
	std::vector<std::shared_ptr<PreconditionBase<TWorldState>>> m_preconditions;
};