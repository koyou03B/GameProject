#pragma once
#include <queue>

#pragma region Operatorについて
//本来ならOperatorクラスを作り
//queueにはOperatorが入るのだが
//今回は別仕様となっている
//Operatorの中身は行動処理が入っている
//PlanListには実行するOperatorを管理する
//役割がある。
#pragma endregion
class PlanList
{
public:
	PlanList() = default;
	~PlanList() = default;

	inline void Clear()
	{
		size_t count = m_operatorID.size();
		for (size_t i = 0; i < count; ++i)
		{
			m_operatorID.pop();
		}
	}
	
	inline void AddOperator(const uint32_t& OperatorID)
	{
		m_operatorID.push(OperatorID);
	}

	inline std::queue<uint32_t>& GetOperators() { return m_operatorID; }

protected:
	std::queue<uint32_t> m_operatorID;
};