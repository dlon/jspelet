#include "E_OneUp.h"
#include "Engine.h"
#include "Input.h"
#include <stdio.h>
#include "SolidMap.h"
#include "Jack.h"
#include "AllEntities.h"

extern Engine *eng;
extern Input *input;

E_OneUp::E_OneUp()
{
	sprite.Load("data/1up.PNG", 1, 0,0);
	spriteIndex = &sprite;
	colRect.FromSprite(spriteIndex);
	snd = Sound(*eng->res->sounds.Load("data/oneup3.wav"));
}

void E_OneUp::Collision(Entity *with)
{
	Entity::Collision(with); 
	if (destroy)
		return;

	if (with->IsPlayer()) {
		destroy = true;
		eng->jack->hud.OneUp();
		snd.play();
	}
}
