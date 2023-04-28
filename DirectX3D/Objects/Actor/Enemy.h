#pragma once

class Enemy
{
public:
	struct Dissolve
	{
	public:
		float duration;
		float curTime = 0.0f;

		void Set() { curTime += DELTA; }
		float DissolveValue() { return (duration - curTime) / duration; }
	};

protected:
	const UINT MAX_CNT = 30;
	const float pathInterval = 5.0f;

	const float moveLimit = 150.0f;
	const float attackLimit = 175.0f;

	enum State
	{
		SPAWN,
		IDLE,
		ATK,
		RUN,
		DEATH,
		HIT,
		GET_DOWN
	};

	struct EnemyState
	{
		State curState = IDLE;
		State prevState;
		Status status;
		bool isAir = false;

		HitType hitType;
		float dmg = 8.0f;

		EnemyState(float maxHP) : status(Status(maxHP))
		{}
	};

public:
	Enemy(Transform* transform, DissolveModelAnimatorInstancing* instancing, float maxHP);
	~Enemy();

	void Update();
	void PostRender();
	
	virtual void TakeHit(float dmg, HitType type) = 0;

	void GetPath(UINT index);

	void RotateDirectly();

	void SetLockOn() { isLockedOn = true; }
	void SetLockOff() { isLockedOn = false; }
	
	Transform* ReturnTransform() { return transform; }
	Vector3 ReturnLockOnPos() { return lockOnImage->Pos(); }

	bool Active() { return transform->Active(); }
	void SetActive(bool value) { transform->SetActive(value); }

	CapsuleCollider* ReturnBody() { return body; }
	bool& InBodyCollider() { return inBodyCollider; }

protected:
	void Rotate(Vector3 forward, Vector3 dir);
	void MoveCollision(CapsuleCollider* body);

	void Gravity();


private:
	void ExcuteEvent();

protected:
	Transform* transform;
	ProgressBar* lockOnImage;
	DissolveModelAnimatorInstancing* instancing;
	DissolveModelAnimatorInstancing::Motion* motion;

	vector<map<float, Event>> totalEvent;
	vector<map<float, Event>::iterator> eventIters;

	CapsuleCollider* body;

	bool inBodyCollider = false;

	bool isLockedOn = false;
	bool isMove = false;

	EnemyState state;
	float moveSpeed = 300.0f;
	float rotSpeed = 10.0f;
	UINT curAnim;

	float pathRefindTime = 0.0f;
	vector<Vector3> path;
	UINT pathIndex = 0;

	float idleTime = 2.0f;

	bool isBlow = false;
	Vector3 blowDirection;

	bool isInAir = false;

	Dissolve dissolve;
};