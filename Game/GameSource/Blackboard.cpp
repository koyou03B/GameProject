#include "Blackboard.h"

void Blackboard::MemoryLog::CreditDown()
{
	for (auto& it : m_longTermMemory)
	{
		float credit = it.GetCreditLv();
		credit -= (1.0f / 6.0f);
		it.SetCreditLv(credit);
	}
}
