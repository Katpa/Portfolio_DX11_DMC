#pragma once

class Empusa : public Enemy
{
private:
	enum Action
	{
		Attack,

		Death,
		Death_downward,

		GetUp,

		Idle_A,
		Idle_B,
		Idle_C,

		Run,

		Spawn_A,
		Spawn_B,

		TakeHit_air,
		TakeHit_F,
		TakeHit_B,
		TakeHit_R,
		TakeHit_L,
		TakeHit_falling,
		TakeHit_falling_Landing,
		TakeHit_fallingFloat,
		TakeHit_Launcing_start,
		TakeHit_Launcing_loop,
		TakeHit_Launcing_end,
		TakeHit_Slam_start,
		TakeHit_Slam_loop,
		TakeHit_Slam_end
	};

public:
	Empusa(Transform* transform, DissolveModelAnimatorInstancing* instancing, UINT index);
	~Empusa();

	void Update();
	void Render();

	void Spawn(const Vector3& spawnPos);
	// Ãæµ¹°Ë»ç ¸öÃ¼¶û / °ø°Ý

	virtual void TakeHit(float dmg, HitType type) override;
	float DissolveRate() { return dissolve.DissolveValue(); }

	bool IsDead() { return curAnim == Action::Death; }

private:
	void AIHandler();

	void Dead();
	void GetHit(float dmg);
	void Move();
	void ATtack();
	void BlowAway();

	void SetAnimSpawn();
	void SetAnimIdle();
	void SetAnimAtk();
	void SetAnimRun();
	void SetAnimDeath();
	void SetAnimTakeHit();

	void SetIdle();
	void SetFalling();
	void SetFallingLanding();
	void SetLaunchingLoop();
	void SetLaunchingEnd();
	void GetUP();
	void SetDeath();
	void SetEvents();

	void SetAnim(Empusa::State action);
	void SetEvent(int clip, Event event, float timeRatio);

	void ArmOn() { arm->SetActive(true); }
	void ArmOff() { arm->SetActive(false); }
	void ParryOn() { parry->SetActive(true); }
	void ParryOff() { parry->SetActive(false); }
	void BlowOn() { isBlow = true; }
	void BlowOff() { isBlow = false; }

private:
	UINT index;

	Transform* btBody;

	CapsuleCollider* moveCollider;
	BoxCollider* arm;
	BoxCollider* parry;

	float newPathTime = 0.0f;
};