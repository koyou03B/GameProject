#pragma once

#pragma region PrimitiveTask
#include "FindAPTask.h"
#include "MoveTask.h"
#include "SetArrowTask.h"
#include "ShootArrowTask.h"

#include "AvoidingTask.h"
#include "FindDATask.h"

#include "HealTask.h"
#pragma endregion

#pragma region CompoundTask
#include "AttackTask.h"
#include "PrepareAttackTask.h"
#include "AvoidTask.h"
#include "RecoverTask.h"
#pragma endregion

#pragma region Method
#include "AttackMethod.h"
#include "FindAPMethod.h"
#include "PrepareAttackMethod.h"
#include "AvoidMethod.h"
#include "FindDAMethod.h"
#include "HealMethod.h"
#pragma endregion

#pragma region Precondition
#include "AttackPrecondition.h"
#include "PrepareAttackPrecondition.h"
#include "AvoidPrecondition.h"
#include "FindDAPrecondition.h"
#include "HealPrecondition.h"
#pragma endregion