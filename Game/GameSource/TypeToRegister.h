#pragma once

enum PrimitiveTaskType
{
	FindAttackPoint,
	Move,
	SetArrow,
	ShootArrow,
	PTaskEnd
};

enum CompoundTaskType
{
	Attack,
	PrepareAttack,
	CTaskEnd
};

enum MethodType
{
	AtkMethod,
	FindAPMethod,
	PrepareAtkMethod,
	MethodEnd
};

enum PreconditionType
{
	AtkPrecondition,
	PrepareAtkPrecondition,
	TruePrecondition,
	PreconditionEnd
};