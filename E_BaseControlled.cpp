#include "E_BaseControlled.h"
#include "Engine.h"
#include "Input.h"

extern Engine *eng;
extern Input *input;

enum { NONE, RIGHT, LEFT };

void E_BaseControlled::Step()
{
	Entity::Step();

	if (dir == RIGHT && !input->Check(VK_RIGHT))
		dir = 0;
	else if (dir == LEFT && !input->Check(VK_LEFT))
		dir = 0;
	
	if (air && vspeed == .0f)
		air = 0;
	else if (!air && input->CheckPressed(VK_SPACE))
	{
		air = 1;
		Jump();
	}

	if (dir == 0)
	{
		if (input->Check(VK_RIGHT))
			dir = RIGHT;
		else if (input->Check(VK_LEFT))
			dir = LEFT;
	}

	if (dir == LEFT)
		MoveLeft();
	else if (dir == RIGHT)
		MoveRight();
	else
		Idle();
}
