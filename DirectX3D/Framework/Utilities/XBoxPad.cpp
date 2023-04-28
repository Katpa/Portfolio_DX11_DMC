#include "Framework.h"

XBoxPad::XBoxPad()
{
}

XBoxPad::~XBoxPad()
{
}

void XBoxPad::Update()
{
	//ù��° �Ķ���Ϳ� �ش��ϴ� ������ ��Ʈ�ѷ��� ��ư ���¸� �޾ƿ´�
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