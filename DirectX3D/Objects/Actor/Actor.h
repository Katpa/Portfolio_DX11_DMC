#pragma once

enum class HitType
{
	NORMAL,
	UPPER,
	SLAM,
	LAUNCH
};

struct MoveHandler
{
public:
	MoveHandler(float moveSpeed) :moveSpeed(moveSpeed)
	{
		actionMoveSpeed = moveSpeed * 0.2f;
	}

	bool Input()
	{
		input = {};

		if (PAD_CONNECT)
		{
			input.x = PAD_LS.x;
			input.y = PAD_LS.y;
		}
		else
		{
			if (KEY_PRESS('W')) input.y += 1;
			if (KEY_PRESS('D')) input.x += 1;
			if (KEY_PRESS('S')) input.y -= 1;
			if (KEY_PRESS('A')) input.x -= 1;
		}
		
		if (input.x || input.y)
			return true;

		return false;
	}

	void SetDirection()
	{
		direction = 0;

		forward = CAM->Forward();
		forward.y = 0;

		right = CAM->Right();
		right.y = 0;

		if (input.x)
			direction += right * input.x;

		if (input.y)
			direction += forward * input.y;

		direction.Normalize();
	}

	void Move(Vector3& pos, bool isWalk = false)
	{
		if(isWalk)
			pos += direction * moveSpeed * 0.3f * DELTA;
		else
			pos += direction * moveSpeed * DELTA;
	}

	void ActionMove(Vector3& pos, Vector3 forward)
	{
		pos += forward * actionMoveSpeed * DELTA;
	}

	void PowerActionMove(Vector3& pos, Vector3 forward)
	{
		pos += forward * moveSpeed * DELTA;
	}

	void AirMove(Vector3& pos)
	{
		pos += direction * moveSpeed * DELTA * 0.6f;
	}

	void Rotate(Vector3& rot, Vector3 forward)
	{
		Vector3 cross = Cross(forward, direction);
		
		if (cross.y < 0)
			rot.y += rotSpeed * DELTA;
		else if (cross.y > 0)
			rot.y -= rotSpeed * DELTA;
	}

	void SetDirectRotate(Vector3& rot)
	{
		targetRotateY = atan2(direction.x, direction.z);
		rot.y = targetRotateY;
	}

	//-------------------------------------------------------
	// Code: UINT IsLockDirFB(Vector3& playerPos, Vector3& enemyPos)
	// Desc: return 0 = nothing
	//         return 1 = Input Forward dir
	//         return 2 = Input Back dir
	//         return 3 = Input Right dir
	//         return 4 = Input Left dir
	//-------------------------------------------------------
	UINT IsLockTightDir(Vector3 playerPos, Vector3 enemyPos, Vector3 playerRight)
	{
		Vector3 dirToEnemy = Vector3(enemyPos.x - playerPos.x, 0, enemyPos.z - playerPos.z).GetNormalized();
		float dist = (direction - dirToEnemy).Length();

		if (dist < 0.15f)
			return 1;
		else if (dist > 0.85f)
			return 2;
		else
		{
			dist = (direction - playerRight).Length();

			if (dist < 0.15f)
				return 3;
			else if (dist > 0.85f)
				return 4;
		}

		return 0;
	}

	//-------------------------------------------------------
	// Code: UINT IsLockDirFB(Vector3& playerPos, Vector3& enemyPos)
	// Desc: return 1 = Input Forward dir
	//         return 2 = Input Back dir
	//         return 3 = Input Right dir
	//         return 4 = Input Left dir
	//-------------------------------------------------------
	UINT LockInputDir()
	{
		float dist = (direction - CAM->Forward()).Length();

		if (fabs(input.y) >= fabs(input.x))
		{
			if (input.y > 0)
				return 1;
			else
				return 2;
		}
		else
		{
			if (input.x > 0)
				return 3;
			else
				return 4;
		}
	}

	void ActionOn() { isAction = true; }
	void ActionOff() { isAction = false; }
	void PowerActionOn() { isPowerAction = true; }
	void PowerActionOff() { isPowerAction = false; }

	Vector3 ReturnDirection() { return direction; }
	Vector2 ReturnInput() { return input; }
	Vector3& DIR() { return direction; }
	float ReturnTargetRotateY() { return targetRotateY; }
	
	const bool IsAction() { return isAction; }
	const bool IsPowerAction() { return isPowerAction; }

private:
	Vector3 direction;
	float targetRotateY;
	float moveSpeed;
	float actionMoveSpeed;
	const float rotSpeed = 10.0f;
	Vector2 input;

	Vector3 forward;
	Vector3 right;

	bool isAction = false;
	bool isPowerAction = false;
};

struct Status
{
public:
	Status(float maxHP)
		:maxHP(maxHP), curHP(maxHP)
	{}

	void SetInit()
	{
		curHP = maxHP;
	}

	void Update()
	{
		hpRate = curHP / maxHP;
	}

	// 피격 및 사망 애니메이션을 위해 bool형으로 정의함
	bool GetDmg(float dmg)
	{
		curHP -= dmg;
		if (curHP < 1)
		{
			curHP = 0;
			return false;
		}

		return true;
	}

	const float HPRate() { return hpRate; }
	float* CurHP() { return &curHP; }

private:
	const float maxHP;
	float curHP;

	float hpRate = 1.0f;
};

class Actor : public Transform
{
public:
	Actor();
	Actor(float maxHP, float moveSpeed);
	~Actor();

	virtual void Update() = 0;
	virtual void Render() = 0;

protected:
	virtual void Control() = 0;
	virtual void Move() = 0;

	void SetAnim(UINT state, float scale = 1.5f, float takeTime = 0.1f);

	virtual void CreateModels() = 0;
	virtual void ReadAnims() = 0;

protected:
	MoveHandler move;
	Status status;

	ModelAnimator* maBody;

	UINT state;
};