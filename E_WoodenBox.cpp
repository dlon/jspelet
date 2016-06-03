// todo: flash respawn

#include "E_WoodenBox.h"
#include "Engine.h"
#include "Jack.h"

extern Engine *eng;

E_WoodenBox::E_WoodenBox(WoodenType type)
{
	sprite.Load("data/tileset3.png", 1, 64,64, 128,64);
	spriteIndex = &sprite;

	colRect.SetW(64.0f);
	colRect.SetH(64.0f);

	colliding = false;
	mOther = 0;
	mHspeed = 0;
	
	// for other entities:
	onPlatform = false;
	platformH = 0;

	orgY = orgX = -1.0f;

	switch (type)
	{
	case WOOD_RESPAWN:
		id = EID_WOODENBOX_RESPAWN;
		break;
	case WOOD_NORMAL:
	default:
		id = EID_WOODENBOX;
		break;
	}
}

void E_WoodenBox::Step()
{
	if (orgX == -1.0f) {
		orgX = x;
		orgY = y;
	}

#if 0
	// set speed/block player
	if (colliding && mOther)
		hspeed = mOther->hspeed / friction;

	//
	Entity::Step();

	colliding = false;
	friction = 5.0f;

	// change friction if stacked
	Entity *e = eng->jack->map.GetEntities().FindClosestInstance(x, y, colRect.GetH() + 5.0f, GetID()); // FIXME: Different types of boxes
	if (e && e->y+e->colRect.GetH() <= y + 3.0f)
		friction = 5000.0f;
#endif

	// new
	Entity::Step();
	colliding = false;
	friction = 5.0f;
	onPlatform = false;

	// falling outside map
	if (id == EID_WOODENBOX_RESPAWN && y > eng->render->cam.maxY+480.0f && eng->render->cam.maxY != 0.0f) {
		x = orgX;
		y = orgY;
	}
}

void E_WoodenBox::Collision(Entity *with)
{
#if 0
	if (with->IsPlayer()) {
		mOther = 0;

		if ((with->hspeed > 0.0f && with->x+with->colRect.GetW() < x+5.0f) ||
			(with->hspeed < 0.0f && with->x < x+colRect.GetW()-5.0f))
		{
			if (with->y+with->colRect.GetH() > y+5.0f) {
				//hspeed = with->hspeed / friction;
				//with->x -= with->hspeed - hspeed;

				// set hspeed before tile test
				mOther = with;
			}
		}
		colliding = true;
	}
#endif


	// stacked box
	if (with->GetID() == EID_WOODENBOX ||
		with->GetID() == EID_WOODENBOX_RESPAWN)
	{
		if (y+colRect.GetH()-2.0f > with->y)
		{
			friction = 5000.0f;
		}
	}

	Entity::Collision(with);
}

void E_WoodenBox::PostStep()
{
#if 0
	// move player
	if (colliding && mOther)
		mOther->x -= mOther->hspeed - hspeed;

	//
	Entity::PostStep();

	if (!colliding)
		hspeed = 0;
#endif
	Entity::PostStep();
}
