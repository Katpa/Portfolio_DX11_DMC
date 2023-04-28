#include "Framework.h"

XBoxPad::XBoxPad()
{
}

XBoxPad::~XBoxPad()
{
}

void XBoxPad::Update()
{
	//첫번째 파라미터에 해당하는 순서의 컨트롤러의 버튼 상태를 받아온다
	result = XInputGetState(0, &state);

	if (!Connected()) return;

	for (pair<UINT, XBoxButton> p : buttons)
	{
		buttons[p.first].Pass();

		if (state.Gamepad.wButtons & p.first)
			buttons[p.first].cur = true;
	}

	ls.Update(state);
	rs.Update(state, false);
}