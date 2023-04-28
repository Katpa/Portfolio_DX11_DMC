#pragma once

class UIManager : public Singleton<UIManager>
{
private:
	struct PlayerUISet
	{
		float maxHP;
		const float* curHP;
		float prevHP;

		ProgressBar* hpBar;

		PlayerUISet(float* curHP)
			:curHP(curHP)
		{
			maxHP = *(this->curHP);
			prevHP = *(this->curHP);
			hpBar = new ProgressBar(L"Textures/UI/hp_front.png", L"Textures/UI/hp_back.png");
			Vector2 barSize = { 600, 40 };
			hpBar->Pos().x = barSize.x * 0.5f + 20.0f;
			hpBar->Pos().y = WIN_HEIGHT - (barSize.y * 0.5f + 5.0f);
			hpBar->UpdateWorld();
		}

		~PlayerUISet()
		{
			delete hpBar;
		}

		void Update()
		{	
			prevHP = Lerp(prevHP, *curHP, DELTA);
			hpBar->SetAmount(CurRate(), PrevRate());
		}

		float CurRate() { return *curHP / maxHP; }
		float PrevRate() { return prevHP / maxHP; }
	};

public:
	UIManager();
	~UIManager();

	void Update();
	void PostRender();

	void SetPlayerUI(float* curHP) { player = new PlayerUISet(curHP); }

private:
	PlayerUISet* player;

};