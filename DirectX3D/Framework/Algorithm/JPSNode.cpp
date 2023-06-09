#include "Framework.h"

JPSNode::JPSNode(Vector2 pos, POINT index, float radius)
	: index(index)
{
	Pos() = Vector3(pos.x, 0, pos.y);

	collider = new SphereCollider(radius);
	collider->SetParent(this);

	SetInit();
}

JPSNode::~JPSNode()
{
	delete collider;
}

void JPSNode::Render()
{

	switch (state)
	{
	case JPSNode::NONE:
		collider->SetColor(1.0f, 1.0f, 1.0f);
		break;
	case JPSNode::OPEN:
		collider->SetColor(0, 0, 1.0f);
		break;
	case JPSNode::CLOSE:
		collider->SetColor(1.0f, 0, 0);
		break;
	case JPSNode::OBSTACLE:
		collider->SetColor(0, 0, 0);
		break;
	case JPSNode::PATH:
		collider->SetColor(0, 1.0f, 1.0f);
		break;
	default:
		break;
	}

	collider->Render();
}

void JPSNode::UpdateWorld()
{
	Transform::UpdateWorld();
	collider->UpdateWorld();
}

void JPSNode::SetInit()
{
	if (state == State::OBSTACLE) return;

	state = State::NONE;
	dir = DIR_N;
	parent = nullptr;
	g = 0;
	h = 0;
	f = 0;
}

void JPSNode::SetNode(JPSNode* parent, Vector2 dir)
{
	this->dir = dir;
	if (parent != this)
	{
		this->parent = parent;
		//f = parent->GetCost() + (parent->Pos() - Pos()).Length();
		SetCost();
	}
	if(state != OBSTACLE)
		state = OPEN;
}

void JPSNode::SetCost()
{
	if (!parent) return;

	Vector2 temp = parent->Vec2Pos() - Vec2Pos();

	float x = abs(temp.x);
	float y = abs(temp.y);

	float minSize = min(x, y);
	float maxSize = max(x, y);

	g = parent->g + temp.Length();
	h = sqrt(minSize * minSize * 2) + (maxSize - minSize);

	f = g + h;
}
