#include "Framework.h"

EnemyManager::EnemyManager()
{
	SetEmpusas();
}

EnemyManager::~EnemyManager()
{
	delete empusaInstancing;

	for (auto emp : empusa)
		delete emp;
}

void EnemyManager::Update()
{
	empusaInstancing->Update();

	for (auto emp : empusa)
		emp->Update();
}

void EnemyManager::Render()
{
	empusaInstancing->Render();

	for (auto emp : empusa)
		emp->Render();
}

void EnemyManager::PostRender()
{
	for (auto emp : empusa)
		emp->PostRender();
}

void EnemyManager::SpawnEmpusa(UINT roomIndex)
{
	vector<vector<JPSNode*>> nodes = dungeon->ReturnRoom(roomIndex)->ReturnNodes();
	// Prevent to spawn on edge of a room
	UINT width = nodes[0].size() - 1;
	UINT height = nodes.size() - 1;

	// Reset enemies
	for (int i = 0; i < empusa.size(); i++)
	{
		empusa[i]->SetActive(false);
	}

	for (int i = 0; i < empusaSpawnAmount[roomIndex]; i++)
	{
		while (true)
		{
			UINT spawnHeight = Random(1, height);
			UINT spawnWidth = Random(1, width);

			if (nodes[spawnHeight][spawnWidth]->GetState() == JPSNode::OBSTACLE) continue;

			for (int j = 0; j < empusa.size(); j++)
			{
				if (empusa[i]->ReturnTransform()->Active()) continue;

				empusa[i]->Spawn(nodes[spawnHeight][spawnWidth]->GlobalPos());
				break;
			}
			break;
		}
	}
}

void EnemyManager::AllActivatedEnemyBodyOn()
{
	FOR(empusa.size())
	{
		if (!empusa[i]->Active()) continue;
		if (empusa[i]->ReturnBody()->Active()) continue;

		empusa[i]->ReturnBody()->SetActive(true); 
	}
}

void EnemyManager::AllActivatedEnemyBodyOff()
{
	FOR(empusa.size())
	{
		if (!empusa[i]->Active()) continue;
		if (!empusa[i]->ReturnBody()->Active()) continue;

		empusa[i]->ReturnBody()->SetActive(false); 
	}
}

void EnemyManager::IsTakeHitCollisionInOut(CapsuleCollider* collider, float dmg, HitType type)
{
	FOR(empusa.size())
	{
		if (!empusa[i]->Active()) continue;
		if (!empusa[i]->ReturnBody()->Active()) continue;

		if (empusa[i]->ReturnBody()->IsCollision(collider))
		{
			if (empusa[i]->InBodyCollider()) continue;

			empusa[i]->InBodyCollider() = true;
		}
		else if (empusa[i]->InBodyCollider())
		{
			empusa[i]->InBodyCollider() = false;
			ParticleManager::Get()->Play("Blood", empusa[i]->ReturnBody()->GlobalPos());

			empusa[i]->TakeHit(dmg, type);
		}
	}
}

void EnemyManager::IsTakeHitCollisionJust(CapsuleCollider* collider, float dmg, HitType type)
{
	FOR(empusa.size())
	{
		if (!empusa[i]->Active()) continue;
		if (!empusa[i]->ReturnBody()->Active()) continue;

		if (empusa[i]->ReturnBody()->IsCollision(collider))
		{
			ParticleManager::Get()->Play("Blood", empusa[i]->ReturnBody()->GlobalPos());
			empusa[i]->ReturnBody()->SetActive(false);
			empusa[i]->TakeHit(dmg, type);
		}
	}
}

void EnemyManager::EmpusaDead()
{
	UINT index = dungeon->ReturnCurRoomIndex();
	if (index < 0) return;

	empusaSpawnAmount[index]--;

	if (empusaSpawnAmount[index] > 0) return;

	dungeon->ReturnRoom(index)->OpenGate();
}

Transform* EnemyManager::ReturnLockOnTarget(Vector3 playerPos)
{
	lockOnTarget = nullptr;
	float minDist = FLT_MAX;

	FOR(empusa.size())
	{
		if (!empusa[i]->ReturnTransform()->Active()) continue;
		if (empusa[i]->IsDead()) continue;

		float dist = Distance(playerPos, empusa[i]->ReturnTransform()->GlobalPos());

		if (dist > 5000.0f) continue;

		if (minDist > dist)
		{
			minDist = dist;
			lockOnTarget = empusa[i];
		}
	}

	if (lockOnTarget)
	{
		lockOnTarget->SetLockOn();
		return lockOnTarget->ReturnTransform();
	}
	else
		return nullptr;

}

void EnemyManager::LockOff()
{
	if (!lockOnTarget) return;

	lockOnTarget->SetLockOff();
	lockOnTarget = nullptr;
	return;
}

void EnemyManager::SetEmpusaAmount(UINT roomSize)
{
	FOR(roomSize)
	{
		empusaSpawnAmount.push_back(Random(3, 6));
	}
}

void EnemyManager::SetEmpusas()
{
	empusaInstancing = new DissolveModelAnimatorInstancing("Empusa");
	empusaInstancing->Load();
	empusaInstancing->Rot().y += XM_PI;
	empusa.resize(MAX_UNIT_CNT);

	empusaInstancing->ReadClip("Attack");
	empusaInstancing->ReadClip("Death");
	empusaInstancing->ReadClip("Death_downward");
	empusaInstancing->ReadClip("GetUp");
	empusaInstancing->ReadClip("Idle_A");
	empusaInstancing->ReadClip("Idle_B");
	empusaInstancing->ReadClip("Idle_C");
	empusaInstancing->ReadClip("Run");
	empusaInstancing->ReadClip("Spawn_A");
	empusaInstancing->ReadClip("Spawn_B");
	empusaInstancing->ReadClip("TakeHit_air");
	empusaInstancing->ReadClip("TakeHit_F");
	empusaInstancing->ReadClip("TakeHit_B");
	empusaInstancing->ReadClip("TakeHit_R");
	empusaInstancing->ReadClip("TakeHit_L");
	empusaInstancing->ReadClip("TakeHit_falling");
	empusaInstancing->ReadClip("TakeHit_falling_Landing");
	empusaInstancing->ReadClip("TakeHit_fallingFloat");
	empusaInstancing->ReadClip("TakeHit_Launcing_start");
	empusaInstancing->ReadClip("TakeHit_Launcing_loop");
	empusaInstancing->ReadClip("TakeHit_Launcing_end");
	empusaInstancing->ReadClip("TakeHit_Slam_start");
	empusaInstancing->ReadClip("TakeHit_Slam_loop");
	empusaInstancing->ReadClip("TakeHit_Slam_end");

	FOR(MAX_UNIT_CNT)
	{
		Transform* transform = empusaInstancing->Add();
		transform->SetActive(false);

		empusa[i] = new Empusa(transform, empusaInstancing, i);
	}
}
