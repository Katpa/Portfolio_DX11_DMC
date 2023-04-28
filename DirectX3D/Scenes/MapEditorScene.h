#pragma once

class MapEditorScene : public Scene
{
private:
	enum Obsjactile
	{
		NONE,
		STATUE,
		HESCO
	};

public:
	MapEditorScene();
	~MapEditorScene();

	// Scene을(를) 통해 상속됨
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void GUIRender() override;

private:
	void SetRoom();

	void Picking();

	void Save();
	void Load();
	void LoadAll();

	void SetModels();

private:
	int roomIndex = 0;

	UINT objIndex = 0;
	
	Dungeon* dungeon;
	Room* curRoom = nullptr;
	Vector3 curRoomPos;

	vector<Model*> statues;
	vector<Model*> hescoes;
};