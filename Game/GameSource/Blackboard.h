#pragma once
#include <vector>
#include "WorldAnalyzer.h"

//�^�X�N�Ƃ�WorldState�Ƃ���
//�����Ă����������H
class Blackboard
{
public:
	Blackboard() = default;
	~Blackboard() = default;

	void ImGui();

public:
	struct MemoryLog
	{
		WorldAnalyzer m_workingMemory;
		std::vector<WorldAnalyzer> m_longTermMemory;

		void CreditDown();
	};

	MemoryLog m_memoryLog[2];
};