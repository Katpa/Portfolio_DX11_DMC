#include "Framework.h"
#include "Enemy.h"

Enemy::Enemy(Transform* transform, DissolveModelAnimatorInstancing* instancing, float maxHP)
	:transform(transform), instancing(instancing), state(EnemyState(maxHP))
{
	lockOnImage = new ProgressBar(L"Textures/UI/LockOn.png", L"Textures/UI/NULL.png");
	lockOnImage->SetShader(L"UI/CircleProgressBar.hlsl");
	lockOnImage->Scale() *= 0.1f;
}

Enemy::~Enemy()
{
	delete lockOnImage;
}

void Enemy::Update()
{
	ExcuteEvent();
	Gravity();

	if (!isLockedOn) return;

	Vector3 imagePos = transform->Pos() + Vector3(0, 100.0f, 0);

	lockOnImage->Pos() = CAM->WorldToScreen(imagePos);
	lockOnImage->Rot().z += DELTA;
	lockOnImage->UpdateWorld();
}

void Enemy::PostRender()
{
	if (!isLockedOn) return;

	lockOnImage->Render();
}

void Enemy::GetPath(UINT index)
{
	//pathRefindTime += DELTA;
	//if (pathRefindTime < pathInterval) return;
	//
	//pathRefindTime -= pathInterval;

	POINT start, end;

	start = PATH_FIND->FindCloseNode(transform->GlobalPos());
	end = PATH_FIND->FindCloseNode(ENEMY->TargetPos());

	PATH_FIND->AskPath(start, end, &path, index);

	pathIndex = 0;
}

void Enemy::Rotate(Vector3 forward, Vector3 dir)
{
	Vector3 cross = Cross(forward, dir);
	if (cross.y < 0)
		transform->Rot().y += rotSpeed * DELTA;
	else if (cross.y > 0)
		transform->Rot().y -= rotSpeed * DELTA;
}

void Enemy::MoveCollision(CapsuleCollider* body)
{
	ENEMY->ReturnDungeon()->CheckPlayer(ENEMY->TargetPos());
	BoxCollider* collidingWall = ENEMY->ReturnDungeon()->IsWallCollision(body);

	if (collidingWall)
	{
		Vector3 dir = { 0, 0, 0 };

		if (collidingWall->R() < body->GlobalPos().x)
			dir.x = 1;
		else if (collidingWall->L() > body->GlobalPos().x)
			dir.x = -1;
		else if (collidingWall->F() < body->GlobalPos().z)
			dir.z = 1;
		else if (collidingWall->B() > body->GlobalPos().z)
			dir.z = -1;

		transform->Pos() += dir * body->CollisionDepth();
	}
}

void Enemy::Gravity()
{
	if (!isInAir) return;

	blowDirection.y -= GRAVITY * DELTA;
	transform->Pos().y += blowDirection.y * DELTA;
}

void Enemy::RotateDirectly()
{
	Vector3 dir = (ENEMY->TargetPos() - transform->Pos()).GetNormalized();
	float angle = atan2(dir.x, dir.z);

	transform->Rot().y = angle + XM_PI;
}

void Enemy::ExcuteEvent()
{
	int index = curAnim;

	if (totalEvent[index].empty()) return;
	if (eventIters[index] == totalEvent[index].end()) return;

	float ratio = motion->runningTime / motion->duration;

	if (eventIters[index]->first > ratio) return;

	eventIters[index]->second();
	eventIters[index]++;
}