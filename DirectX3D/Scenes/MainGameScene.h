#pragma once

class MainGameScene : public Scene
{
public:
	MainGameScene();
	~MainGameScene();

	// Scene을(를) 통해 상속됨
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void GUIRender() override;

private:
	Dungeon* dungeon;
	Dante* dante;

	Skybox* sky;
};