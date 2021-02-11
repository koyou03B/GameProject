#pragma once

#pragma region PrimitiveTask
#include "FindAPTask.h"
#include "MoveTask.h"
#include "SetArrowTask.h"
#include "ShootArrowTask.h"

#include "AvoidingTask.h"
#include "FindDATask.h"
#pragma endregion

#pragma region CompoundTask
#include "AttackTask.h"
#include "PrepareAttackTask.h"
#include "AvoidTask.h"
#pragma endregion

#pragma region Method
#include "AttackMethod.h"
#include "FindAPMethod.h"
#include "PrepareAttackMethod.h"
#include "AvoidMethod.h"
#include "FindDAMethod.h"
#pragma endregion

#pragma region Precondition
#include "AttackPrecondition.h"
#include "PrepareAttackPrecondition.h"
#include "AvoidPrecondition.h"
#include "FindDAPrecondition.h"
#pragma endregion