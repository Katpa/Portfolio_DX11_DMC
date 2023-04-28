#pragma once

struct Vector2;

class XBoxPad : public Singleton<XBoxPad>
{
private:
	struct XBoxButton
{
	bool prev = false;
	bool cur = false;

	void Pass()
	{
		prev = cur;
		cur = false;
	}
};

struct Stick
{
	Vector2 output = {};
	Vector2 input = {};

	void Update(XINPUT_STATE& state, bool isLeft = true)
	{
		if(isLeft)
			input = { static_cast<float>(state.Gamepad.sThumbLX), static_cast<float>(state.Gamepad.sThumbLY) };
		else
			input = { static_cast<float>(state.Gamepad.sThumbRX), static_cast<float>(state.Gamepad.sThumbRY) };

		Output();
	}

	void Output()
	{
		/////////////////
		// 0 - FORWARD //
		// 1 - LEFT	   //
		// 2 - 
		/////////////////

		SetValue(input.x, output.x);
		SetValue(input.y, output.y);
	}

	void SetValue(float& inputValue, float& outputValue)
	{
		if (inputValue > STICK_DEADZONE || inputValue < -STICK_DEADZONE)
			outputValue = inputValue / MAX_STICK_RANGE;
		else
			outputValue = 0.0f;
	}
};

public:
	XBoxPad();
	~XBoxPad();

	void Update();

	bool PadDown(UINT button) { return (buttons[button].cur && !buttons[button].prev); }
	bool PadPress(UINT button) { return (buttons[button].cur && buttons[button].prev); }
	bool PadUp(UINT button) { return (!buttons[button].cur && buttons[button].prev); }
	bool Connected() { return result == ERROR_SUCCESS; }
	Vector2 LS() { return ls.output; }
	Vector2 RS() { return rs.output; }

private:
	map<UINT, XBoxButton> buttons;
	Stick ls, rs;

	XINPUT_STATE state;
	DWORD result;
};