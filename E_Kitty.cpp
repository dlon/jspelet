#include "E_Kitty.h"
#include "Engine.h"
#include "Jack.h"

extern Engine *eng;

#define KITTY_SPEED	4.0f
#define TURN_DIST	100.0f	

E_Kitty::E_Kitty() : E_BaseEnemy(30.0f)
{
	//mainSpr.Load("data/unchat.png", 3, 75, 64);
	//jumpSpr.Load("data/unchat.png", 2, 67, 64, 0, 64);

	//TextureLoader::SetColorKey(255, 0, 255); // magneta

	//Texture *mT = eng->res->textures.LoadEx("data/kattmjau.png", TextureLoader::RemoveColorKey); // load texture + remove chroma
	// FIXME:	The above code sometimes causes other textures to be loaded using the chroma key...
	//			I think it has to do with the "switch" (bool) in TextureFactory
	sf::Texture *mT = eng->res->textures.Load("data/kattmjau.png");

	mainSpr.Load(mT, 3, 79, 40);
	jumpSpr.Load(mT, 2, 75, 56, 0, 40);

	gravity = 0.6f;
	spriteIndex = &mainSpr;
	//spriteIndex->imgSpeed = .25f;
	spriteIndex->imgSpeed = .25f;

	meowSnd = Sound(*eng->res->sounds.Load("data/kattmjau.wav"));

	colRect.SetX(15.0f); // FIXME: isn't this supposed to be XOffs?
	colRect.SetW(70.0f);
	colRect.SetH(40.0f);
	colRect.lSlopeOff = 15.0f;
	colRect.rSlopeOff = 15.0f;

	foundTarget = false;

	//activRadius = 500.0f;
}

void E_Kitty::Step()
{
	E_BaseEnemy::Step();

	if (!active)
		return;

	//Entity *pl = eng->jack->map.GetEntities().FindClosestInstance(x, y, (!foundTarget ? 300 : 800), EID_PLAYER);
	Entity *pl = player;

	if (pl) {
		foundTarget = true; // bigger search area

		float l = pl->x - x;

		/*if (pl->y - y > 10.0f) {
			// try to get down
			hspeed = -KITTY_SPEED;
		}
		else*/
		{
			if (l < -TURN_DIST)
				hspeed = -KITTY_SPEED;
			else if (l > TURN_DIST)
				hspeed = KITTY_SPEED;

			// jump attack
			if (vspeed == 0.0f && abs(l) < 80.0f) {
				//meowSnd->Play(true);
				//meowSnd->PlayLoc(x - pl->x, y - pl->y, true);
				meowSnd.play(); // FIXME-SFML: pan
				vspeed = -10.0f;
			}
		}
	}
	else {
		hspeed = 0;
		foundTarget = false;
	}
}

void E_Kitty::PostStep()
{
	// jumping?
	if (vspeed < -5.0f) {
		spriteIndex = &jumpSpr;
		jumpSpr.imgInd = 0.0f;
	}
	else if (vspeed < 0.0f) {
		spriteIndex = &jumpSpr;
		jumpSpr.imgInd = 1.0f;
	}
	else {
		spriteIndex = &mainSpr;
	}

	// set face
	if (hspeed > 0.0f)
		spriteIndex->MirrorFlip();
	else if (hspeed < 0.0f)
		spriteIndex->NormalFlip();

	E_BaseEnemy::PostStep();
}
