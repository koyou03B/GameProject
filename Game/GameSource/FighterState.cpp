#include "FighterState.h"
#include "FighterCharacter.h"

void MovekState::Execute(Fighter* fighter)
{
	fighter->GetWorldTransform().position.x += 10;
	fighter->GetWorldTransform().WorldUpdate();
}
