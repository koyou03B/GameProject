#pragma once

#pragma region PreconditionBase�ɂ���
//Precondition�̊�ꕔ��
//Precondition�͑O������̖����������Ă��܂��B

//Q.PrimitiveTask�Ŏg���Ė�����ˁH
//A.�����Method�N�Ɏg�����̂Ȃ�

//CompoundTask��Precondition�����̂łȂ�
//Method�������߁A������K�v�Ȃ̂ł��B
//CompoundTask��...�B
#pragma endregion
template<class TWorldState>
class PreconditionBase
{
public:
	PreconditionBase() = default;
	virtual ~PreconditionBase() = default;

	virtual bool CheckPreCondition(TWorldState& state) { return true; };
};