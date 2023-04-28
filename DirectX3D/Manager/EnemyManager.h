#pragma once

class EnemyManager : public Singleton<EnemyManager>
{
public:
	EnemyManager();
	~EnemyManager();

	void Update();
	void Render();
	void PostRender();

	void SpawnEmpusa(UINT roomIndex);
	void AllActivatedEnemyBodyOn();
	void AllActivatedEnemyBodyOff();

	void IsTakeHitCollisionInOut(CapsuleCollider* collider, float dmg, HitType type);
	void IsTakeHitCollisionJust(CapsuleCollider* collider, float dmg, HitType type);
	void EmpusaDead();

	Dungeon* ReturnDungeon() { return dungeon; }
	Dante* ReturnTarget() { return target; }
	Transform* ReturnLockOnTarget(Vector3 playerPos);

	float GetDissolve(UINT index) { return empusa[index]->DissolveRate(); }

	Vector3 TargetPos() { return target->Pos(); }
	const Vector3& LockOnPos() { return lockOnTarget->ReturnLockOnPos(); }
	void LockOff();

	void SetDungeon(Dungeon* dungeon) { this->dungeon = dungeon; }
	void SetTarget(Dante* dante) { target = dante; }
	void SetEmpusaAmount(UINT roomSize);

private:
	void SetEmpusas();

private:
	DissolveModelAnimatorInstancing* empusaInstancing;
	vector<Empusa*> empusa;
	vector<int> empusaSpawnAmount;
	
	Dante* target;
	Dungeon* dungeon;

	Enemy* lockOnTarget = nullptr;
};