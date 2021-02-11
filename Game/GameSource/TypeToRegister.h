#pragma once

enum class PrimitiveTaskType
{
	FindAttackPoint,
	Move,
	SetArrow,
	ShootArrow,
	Avoiding,
	FindDirectionAvoid,
	PTaskEnd
};

enum class CompoundTaskType
{
	Attack,
	PrepareAttack,
	Avoid,
	CTaskEnd
};

enum class  MethodType
{
	AtkMethod,
	FindAPMethod,
	PrepareAtkMethod,
	AvoidMethod,
	FindDAMethod,
	MethodEnd
};

enum class PreconditionType
{
	AtkPrecondition,
	PrepareAtkPrecondition,
	TruePrecondition,
	AvoidPrecondition,
	FindDAPrecondition,
	PreconditionEnd
};