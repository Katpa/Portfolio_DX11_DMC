#include "Framework.h"

CollisionManager::CollisionManager()
{
    ccStructuredBuffer = new StructuredBuffer(ccInputs.data(), sizeof(CCInputDesc), 0, sizeof(ccOutputs), 0);
    cbStructuredBuffer = new StructuredBuffer(cbInputs.data(), sizeof(CBInputDesc), 0, sizeof(cbOutputs), 0);
    csStructuredBuffer = new StructuredBuffer(csInputs.data(), sizeof(CSInputDesc), 0, sizeof(csOutputs), 0);

    cc = Shader::AddCS(L"Compute/CapsuleCollision.hlsl");
}

CollisionManager::~CollisionManager()
{
    delete ccStructuredBuffer;
    delete cbStructuredBuffer;
    delete csStructuredBuffer;
}

void CollisionManager::Update()
{
    ccStructuredBuffer->UpdateInput(ccInputs.data());
}

bool CollisionManager::IsCollision(Collider* a, Collider* b)
{
    return false;
}

void CollisionManager::AddEnemy(CapsuleCollider* collider)
{
    if (!collider->Active()) return;

    enemyBodies.push_back(collider);
}

void CollisionManager::ClearEnemies()
{
    bool isAllDead = true;

    FOR(enemyBodies.size())
    {
        if (enemyBodies[i] == nullptr)
        {
            continue;
        }

        if (enemyBodies[i]->Active())
        {
            isAllDead = false;
            continue;
        }

        enemyBodies[i] = nullptr;
    }

    if (isAllDead)
        enemyBodies.clear();
}

void CollisionManager::AddWall(BoxCollider* collider)
{
    walls.push_back(collider);
}

void CollisionManager::ClearWalls()
{
    walls.clear();
}