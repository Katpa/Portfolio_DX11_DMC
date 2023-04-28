#pragma once

class Urizen : public Transform
{
private:
	enum Action
	{
		 Idle,
		 Death,
		 Walk,
		 Battle_start,
		 Attack1,
		 Attack1_end,
		 Attack2,
		 Attack2_end,
		 Assult_start,
		 Assult_loop,
		 Assult_end,
		 NONE
	};

	class DissolveBuffer : public ConstBuffer
	{
	public:
		struct Data
		{
			Float3 color;
			float dissolves;
		};

	public:
		DissolveBuffer() : ConstBuffer(&data, sizeof(Data))
		{
			Get().color = {1, 1, 1};
			Get().dissolves = 1.0f;
		}

		Data& Get() { return data; }

	private:
		Data data;
	};

private:
	const float moveSpeed = 200.0f;
	const float MAX_MOVE_SPEED = 300.0f;

public:
	Urizen();
	~Urizen();

	void Update();
	void Render();

	void Spawn(const Vector3& spawnPos);

	void TakeHit(float dmg, HitType type);

	CapsuleCollider* ReturnBody() { return body; }

private:
	void CreateModels();
	void CreateAnimations();

	void AIHandler();
	void StateUpdate();

	void Dead();
	//void GetHit(float dmg);
	void Move();
	void Rotate(Vector3 dir);
	void ATtack();

	void SetIdle();
	void SetDeath();

	void SetAnim(Action action);

	void PlayAttackEnd();
	void PlayAssultLoop();

	void MoveCollision();

	void ArmOn() { armCollider->SetActive(true); }
	void ArmOff() { armCollider->SetActive(false); }
	void FootOn() { footCollider->SetActive(true); }
	void FootOff() { footCollider->SetActive(false); }

private:
	ModelAnimator* urizen;

	Status status;
	Action curAnim = NONE;

	float curMoveSpeed = 0;

	CapsuleCollider* moveCollider;
	CapsuleCollider* body;

	Transform* rightHand;
	Transform* rightFoot;

	CapsuleCollider* armCollider;
	CapsuleCollider* footCollider;

	Enemy::Dissolve dissolve;
	DissolveBuffer* dissolveBuffer;
	Texture* dissolveTexture;
};