#pragma once

class Dante : public Actor
{
private:
	enum class Cmd
	{
		MELEE = 'J',
		SHOT = 'I',
		STYLE = 'L',
		JUMP = VK_SPACE,
		LOCKON = VK_LSHIFT,
		SWAP_WEAPON,
		F = 'W',
		B = 'S',
		R = 'D',
		L = 'A',
		TAUNT = 'R',
		CAMERA_RESET = VK_TAB
	};

	enum class PadCmd
	{
		MELEE = XINPUT_GAMEPAD_Y,
		SHOT = XINPUT_GAMEPAD_X,
		STYLE = XINPUT_GAMEPAD_B,
		JUMP = XINPUT_GAMEPAD_A,
		LOCKON = XINPUT_GAMEPAD_RIGHT_SHOULDER,
		SWAP_WEAPON = XINPUT_GAMEPAD_LEFT_SHOULDER,
		TAUNT = XINPUT_GAMEPAD_BACK,
		CAMERA_RESET = XINPUT_GAMEPAD_RIGHT_THUMB
	};

	enum WeaponType
	{
		REB, CER
	};

	enum class Action
	{
		IDLE = 0,
		IDLE_NORMAL,
		IDLE_LOCKON,
		LOCK_WALK_F,
		LOCK_WALK_B,
		LOCK_WALK_R,
		LOCK_WALK_L,
		RUN,
		Dash_F,
		Dash_B,
		Dash_R,
		Dash_L,
		Skystar,
		Evade_R,
		Evade_L,
		Jump_start,
		Jump_loop,
		Jump_end,
		EnemyStep,
		Airtrick_start,
		Airtrick_ground,
		Airtrick_air,
		GetUp,
		TakeHit_Air,
		TakeHit,
		Taunt,
		Death,
		Reb_Melee_A,
		Reb_Melee_B,
		Reb_Melee_C,
		Reb_Melee_D,
		Reb_Melee_E,
		Reb_Air_Melee_A,
		Reb_Air_Melee_B,
		Reb_Air_Melee_C,
		Reb_Air_Melee_D,
		Reb_MillionStab_loop,
		Reb_MillionStab_end,
		Reb_Parring,
		Reb_HighTime,
		Reb_HighTime_jump,
		Reb_HelmBreaker_start,
		Reb_HelmBreaker_loop,
		Reb_HelmBreaker_end,
		Reb_Stinger_start,
		Reb_Stinger_loop,
		Reb_Stinger_end
	};

	struct DASH
	{
	private:
		float maxValue = 2000.0f;
		float curValue;
		float valueStep = 3500.0f;
		Vector3 dir;
		bool overMiddle = false;

	public:
		void Init()
		{
			curValue = 0;
			overMiddle = false;
		}

		Vector3 DashToDir()
		{
			if (!overMiddle)
			{
				curValue += valueStep * DELTA;

				if (curValue > maxValue)
					overMiddle = true;
			}
			else if (curValue > 0)
			{
				curValue -= valueStep * 4.0f * DELTA;
			}
			else
				curValue = 0;

			return dir * curValue * DELTA;
		}
		void SetDir(Vector3 dir) { this->dir = dir; }

	};

	struct JUMP
	{
	public:
		void Init()
		{
			curPower = jumpPower;
			isJump = true;
		}

		void SkystarInit()
		{
			curPower = 0;
			skystarTime = 0.0f;
		}

		void TakeHit()
		{
			curPower = 50.0f;
		}

		void InJump(Vector3& pos)
		{
			curPower -= GRAVITY * DELTA;
			
			pos.y += curPower * DELTA;
		}

		void ActionInJump(Vector3& pos)
		{
			curPower -= GRAVITY * 0.02f * DELTA;

			pos.y += curPower * DELTA;
		}

		void HelmBreakerStart()
		{
			curPower = 0;
		}

		void HelmBreakerLoop(Vector3& pos)
		{
			curPower -= GRAVITY * 3.0f * DELTA;

			pos.y += curPower * DELTA;
		}

		bool SkystarInJump(Vector3& pos)
		{
			skystarTime += DELTA;
			if (SKYSTAR_FALL_START > skystarTime) return false;

			curPower -= GRAVITY * 0.5f * DELTA;
			pos.y += curPower * DELTA;

			return true;
		}

		void EndJump() { isJump = false; }
		bool IsJump() { return isJump; }

	private:
		const float jumpPower = 1000.0f;
		float curPower = 0.0f;
		bool isJump = false;
		
		const float SKYSTAR_FALL_START = 0.6f;
		float skystarTime;
	};

	struct ComboHandler
	{
	public:
		void SwapState()
		{
			if (normalCombo)
			{
				normalCombo = false;
				uniqueCombo = true;
			}
			else if (uniqueCombo)
			{
				normalCombo = false;
				uniqueCombo = false;
			}
			else
			{
				normalCombo = true;
				uniqueCombo = false;
			}
		}

		void SetInit()
		{
			normalCombo = false;
			uniqueCombo = false;
		}

		void ComboStart() { isCombo = true; }
		void ComboEnd() { isCombo = false; }

		void OpenNCombo() { normalCombo = true; }
		void CloseNCombo() { normalCombo = false; }
		void OpenUCombo() { uniqueCombo = true; }
		void CloseUCombo() { uniqueCombo = false; }

		const bool& NCombo() { return normalCombo; }
		const bool& UCombo() { return uniqueCombo; }
		const bool& IsCombo() { return isCombo; }

	private:
		bool normalCombo = false;
		bool uniqueCombo = false;
		bool isCombo = false;
	};

	struct Combo
	{
		UINT normalAnim = 0;
		UINT uniqueAnim = 0;

		UINT Normal() { return normalAnim; }
		UINT Unique() { return uniqueAnim; }
	};

public:
	Dante();
	~Dante();

	// Actor을(를) 통해 상속됨
	virtual void Update() override;
	virtual void Render() override;

	void SetDungeon(Dungeon* dungeon) { this->dungeon = dungeon; }

	void SetTakeHit(float dmg);

	CapsuleCollider* ReturnBody() { return body; }

private:
	virtual void Control() override;
	virtual void CreateModels() override;
	void CreateWeaponLight();
	virtual void ReadAnims() override;

	void Input();
	void ActionUpdate();

	void ActionMove();

	void Jump();
	void Dash();
	virtual void Move() override;
	void LockOn();
	void AttackControl();

	void MoveCollision();
	bool IsMoveCollision();

	void SetLookTarget();

	void SetAbleInput0() { ableInput = 0; }
	void SetAbleInput1() { ableInput = 1; }
	void SetAbleInput2() { ableInput = 2; }
	void SetAbleInput3() { ableInput = 3; }

	void SetIdle();
	void SetJumpLoop();
	void SetLanding();
	void MillionStabLoop();
	void MillionStabEnd();
	void SetAirTrick();
	void HighTime();
	void HelmBreaker();
	void StingerToLoop();
	void OnAllEnemyBody();
	void OffAllEnemyBody();

	void WeaponSetIdle();
	void WeaponSetRightHand();
	void WeaponSetLeftHand();

	void SetStateInit();

private:
	Model* mHead;
	Model* mHair;

	//==============Weapon=================
	Model* reb;
	CapsuleCollider* rebCollider;
	LightBuffer::Light* rebLight;
	Transform* rebTrailStart;
	Transform* rebTrailEnd;

	Trail* trail;

	ModelAnimator* cer;
	//=====================================

	Transform* btHead;
	Transform* btRightHand;
	Transform* btLeftHand;
	Transform* btWeaponIdle;

	DASH dash;
	JUMP jump;
	ComboHandler combo;
	map<UINT, Combo> combos;

	WeaponType wType = REB;
	UINT ableInput;

	Dungeon* dungeon;

	CapsuleCollider* body;
	BoxCollider* collidingWall = nullptr;

	Transform* lockOnTarget = nullptr;

	bool isAirTrickAir = false;

	float rebDmg = 10.0f;
	float cerDmg = 2.0f;
};