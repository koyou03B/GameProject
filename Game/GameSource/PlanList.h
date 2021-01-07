#pragma once
#include <queue>

#pragma region Operator�ɂ���
//�{���Ȃ�Operator�N���X�����
//queue�ɂ�Operator������̂���
//����͕ʎd�l�ƂȂ��Ă���
//Operator�̒��g�͍s�������������Ă���
//PlanList�ɂ͎��s����Operator���Ǘ�����
//����������B
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