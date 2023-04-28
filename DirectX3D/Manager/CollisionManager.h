#pragma once

class CollisionManager : public Singleton<CollisionManager>
{
private:
	struct CCInputDesc
	{
		Float3 aUp;
		Float3 aPos;
		float aHeight;
		float aRadius;

		Float3 bUp;
		Float3 bPos;
		float bHeight;
		float bRadius;
	};

	struct CCOutputDesc
	{
		int isCollision;
	};

	struct CBInputDesc
	{
	};

	struct CBOutputDesc
	{
		int isCollision;
	};

	struct CSInputDesc
	{
	};

	struct CSOutputDesc
	{
		int isCollision;
	};

public:
	CollisionManager();
	~CollisionManager();

	void Update();
	void Render();

	bool IsCollision(Collider* a, Collider* b);

	void SetPlayerBody(CapsuleCollider* collider) { playerBody = collider; }

	void AddEnemy(CapsuleCollider* collider);
	void ClearEnemies();

	void AddWall(BoxCollider* collider);
	void ClearWalls();
	

private:
	ComputeShader* cc;		// capsule - capsule
	ComputeShader* cb;		// capsule - box
	ComputeShader* cs;		// capsule - sphere

	StructuredBuffer* ccStructuredBuffer;
	StructuredBuffer* cbStructuredBuffer;
	StructuredBuffer* csStructuredBuffer;

	vector<CCInputDesc> ccInputs;
	vector<CCOutputDesc> ccOutputs;

	vector<CCInputDesc> cbInputs;
	vector<CCOutputDesc> cbOutputs;

	vector<CCInputDesc> csInputs;
	vector<CCOutputDesc> csOutputs;

	CapsuleCollider* playerBody = nullptr;
	vector<CapsuleCollider*> enemyBodies;
	vector<BoxCollider*> walls;

	// 무기 혹은 공격 같은 경우엔 IN/OUT을 체크해야하는데...
};