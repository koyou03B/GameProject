#pragma once

enum class PrimitiveTaskType
{
	FindAttackPoint,
	Move,
	SetArrow,
	ShootArrow,
	Avoiding,
	FindDirectionAvoid,
	Recover,
	PTaskEnd
};

enum class CompoundTaskType
{
	Attack,
	PrepareAttack,
	Avoid,
	Recover,//�܂�����ĂȂ�
	CTaskEnd
};

enum class  MethodType
{
	AtkMethod,
	FindAPMethod,
	PrepareAtkMethod,
	AvoidMethod,
	FindDAMethod,
	RecoverMethod,
	MethodEnd
};

enum class PreconditionType
{
	AtkPrecondition,
	PrepareAtkPrecondition,
	TruePrecondition,
	AvoidPrecondition,
	FindDAPrecondition,
	RecoverPrecondition,
	PreconditionEnd
};