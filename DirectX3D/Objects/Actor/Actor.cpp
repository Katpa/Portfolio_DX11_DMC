#include "Framework.h"

Actor::Actor()
	:status(Status(30.0f)), move(MoveHandler(30))
{
}

Actor::Actor(float maxHP, float moveSpeed)
	: status(Status(maxHP)), move(MoveHandler((moveSpeed)))
{
}

Actor::~Actor()
{
}

void Actor::SetAnim(UINT state, float scale, float takeTime)
{
	if (this->state == state) return;

	this->state = state;
	maBody->PlayClip(state, scale, takeTime);
}
