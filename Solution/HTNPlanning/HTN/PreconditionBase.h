#pragma once

#pragma region PreconditionBaseについて
//Preconditionの基底部分
//Preconditionは前提条件の役割を持っています。

//Q.PrimitiveTaskで使って無いよね？
//A.これはMethod君に使うものなの

//CompoundTaskがPreconditionを持つのでなく
//Methodがもつため、こやつが必要なのです。
//CompoundTaskめ...。
#pragma endregion
template<class TWorldState>
class PreconditionBase
{
public:
	PreconditionBase() = default;
	virtual ~PreconditionBase() = default;

	virtual bool CheckPreCondition(TWorldState& state) { return true; };
};