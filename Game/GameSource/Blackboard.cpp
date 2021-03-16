#include "Blackboard.h"
#ifdef _DEBUG
#include "..\External_libraries\imgui\imgui.h"
#include "..\External_libraries\imgui\imgui_impl_dx11.h"
#include "..\External_libraries\imgui\imgui_impl_win32.h"
#include "..\External_libraries\imgui\imgui_internal.h"
#endif
void Blackboard::MemoryLog::CreditDown()
{
	for (auto& it : m_longTermMemory)
	{
		float credit = it.GetCreditLv();
		credit -= (1.0f / 5.0f);
		it.SetCreditLv(credit);
	}
}

void Blackboard::ImGui()
{
	ImGui::Begin("Blackboard");
	int count = static_cast<int>(m_memoryLog[0].m_longTermMemory.size());
	for (int i = 0; i < count; ++i)
	{
		float credit = m_memoryLog[0].m_longTermMemory.at(i).GetCreditLv();
		ImGui::Text(u8"ƒvƒŒƒCƒ„[ %d”Ô–Ú %f", i, credit);
	}
	count = static_cast<int>(m_memoryLog[1].m_longTermMemory.size());
	for (int i = 0; i < count; ++i)
	{
		float credit = m_memoryLog[1].m_longTermMemory.at(i).GetCreditLv();

		ImGui::Text(u8"“G %d”Ô–Ú %f", i, credit);
	}
	ImGui::End();
}
