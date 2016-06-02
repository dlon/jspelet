#include "E_Boss.h"
#include "Jack.h"
#include "Engine.h"

extern Engine *eng;

//
//	trigger
//
E_BossTrigger::E_BossTrigger() : player(NULL)
{
}

void E_BossTrigger::Step()
{
	if (!player) {
		player = static_cast<E_Player*>( eng->jack->map.GetEntities().FindInstanceById(NULL, EID_PLAYER) );
	}
	else {
		if (!destroy &&
			x - player->x < 230 &&
			y - player->y < 100)
		{
			destroy = true;

			// create boss object
			Entity *boss = eng->jack->map.GetEntities().CreateEntity(EID_BOSSOBJECT);
			E_Boss *bb = static_cast<E_Boss*>(boss);
			bb->x = x;
			bb->y = y;
			bb->Activate(player);

			// fixate cam
			//eng->jack->chaseCam.minX = eng->render->cam.maxX - 800.0f;
			//eng->jack->chaseCam.minY = eng->render->cam.maxY - 800.0f;
			eng->render->cam.maxY = 688.0f;

			// change the music
			eng->jack->SetSong("data/boss1.ogg");
		}
	}
}

//
//	boss
//
//E_Boss::E_Boss() : maxHealth(3000.0f), E_BaseEnemy(3000.0f)
E_Boss::E_Boss() : maxHealth(3000.0f), E_BaseBoss()
{
	spr.Load("data/boss.png", 2, 155, 173);
	sprShoot.Load("data/boss.png", 1, 155, 173, 310, 0);

	sndStep = sf::Sound(*eng->res->sounds.Load("data/boss1steg.wav"));

	attack2 = false;
	shooting = false;
	attack2Swift = false;
	attack2Swift2 = false;

	spriteIndex = &spr;

	// 40, 50
	// 115, 123
	spr.xofs = -40.0f;
	spr.yofs = -40.0f;
	sprShoot.xofs = -40.0f;
	sprShoot.yofs = -40.0f;
	colRect.SetW(115.0f);
	colRect.SetH(133.0f);

	/*
	colRect.SetW(155.0f);
	colRect.SetH(173.0f);
	*/

	eng->jack->hud.SetBoss(this);

	SetTimer(2, 1);

	activRadius		= 1000.0f;
	playerDamage	= 15.0f;
	health			= maxHealth;

	osc = 0;
	oscS = false;
	oscB = false;
}

void E_Boss::Step()
{
	E_BaseEnemy::Step();

	if (!active)
		return;

	if (!attack2 && // unless we're in the air...
		hspeed != 0.0f) {
		if (!sndStep->Playing()) {
			//sndStep->Play(false);
			//sndStep->PlayLoc(x - player->x, y - player->y);
			// FIXME-SFML: pan
			sndStep.play();
			eng->jack->chaseCam.EarthQuake();
		}
	}

	// fly to the left
	if (attack2 && attack2Swift && vspeed >= 0.0f) {
		// move left and drop stuff
		attack2Swift = false;
		hspeed = -5.0f;

		// drop stuff thrice
		dropN = 9;
		SetTimer(1, 1);
	}
	if (attack2 && attack2Swift2 && vspeed >= 0.0f) {
		attack2Swift2 = false;
		hspeed = 3.0f;
	}
	if (attack2) {
		// oscillate
		if (oscS) {
			osc -= (hspeed==3.0f ? 0.25f : 0.5f);
			if (osc < (hspeed==3.0f ? -3.0f : -5.0f))
				oscS = !oscS;
		}
		else {
			osc += (hspeed==3.0f ? 0.25f : 0.5f);
			if (osc > (hspeed==3.0f ? 3.0f : 5.0f))
				oscS = !oscS;
		}
	}
}

void E_Boss::PostStep()
{
	if (!active)
		return;

	if (!attack2) {
		spriteIndex = &spr;
		if (!shooting) {
			if (hspeed == 0.0f) {
				spriteIndex->imgSpeed = 0;
				spriteIndex->imgInd = 0;
			}
			else {
				spriteIndex->imgSpeed = 0.05f;
			}
		}
		else {
			spriteIndex->imgSpeed = 0.5f;
		}
	}
	else {
		spriteIndex = &sprShoot;
		if (!oscB && vspeed > 0.0f) {
			vspeed = 0;
			oscB = true;
		}

		// oscillate vspeed
		if (oscB)
			vspeed = osc;
	}

	E_BaseEnemy::PostStep();
}

E_Boss::~E_Boss()
{
	eng->jack->hud.SetBoss(NULL);

	// leave token
	Entity *e = eng->jack->map.GetEntities().CreateEntity(EID_ENDMAP);
	e->x = x;
	e->y = y-100.0f;
	e->gravity = 0.6f;

	// reset cam
	//eng->jack->chaseCam.minX = 0;
	//eng->jack->chaseCam.minY = 0;
}

void E_Boss::Attack()
{
}

void E_Boss::CallTimer(int num)
{
	E_BaseBoss::CallTimer(num);

	switch (num) {
		// repeat cycle
		case 2:
			hspeed = 0;
			SetTimer(3, 60*4);	// fly attack
			SetTimer(5, 40);	// shoot attack
			break;

		// walk - 8,9,10
		case 8:
			SetTimer(9, 60*9);
			hspeed = -1.0f;
			break;
		case 9:
			SetTimer(10, 60*9);
			hspeed = 1.0f;
			break;
		case 10:
			SetTimer(2, 120); // repeat
			break;

		// attack 2
		case 3:
			// fly and drop things
			attack2 = true;
			attack2Swift = true;
			oscB = false; // oscillate switch
			vspeed = -18.0f;
			SetTimer(0, 150);
			break;
		case 0:
			// fly back
			hspeed = 0;
			attack2Swift2 = true;
			SetTimer(4, 190);
			break;
		case 4:
			// stop dropping things and descend
			hspeed = 0;
			vspeed = 0;
			attack2Swift2 = false;
			attack2 = false;
			//SetTimer(2, 60*2); // repeat
			SetTimer(8, 60*2); // walk
			break;

		// drop stuff (attack 2)
		case 1:
			{
				Entity *e = eng->jack->map.GetEntities().CreateEntity(EID_BOSSSURPRISE);
				e->x = x+40.0f; e->y = y;
			}
			// repeat
			if (--dropN)
				SetTimer(1, 12);
			break;

		// shoot
		case 5:
			{
				shooting = true;
				Entity *e = eng->jack->map.GetEntities().CreateEntity(EID_BOSSROCKET);
				//e->x = x+10.0f; e->y = y+70.0f;
				e->x = x-30.0f; e->y = y+30.0f;
				SetTimer(6, 30);
			}
			break;
		case 6:
			{
				Entity *e = eng->jack->map.GetEntities().CreateEntity(EID_BOSSROCKET);
				//e->x = x+10.0f; e->y = y;
				e->x = x-30.0f; e->y = y;
				SetTimer(7, 30);
			}
			break;
		case 7:
			{
				Entity *e = eng->jack->map.GetEntities().CreateEntity(EID_BOSSROCKET);
				//e->x = x+10.0f; e->y = y+130.0f;
				e->x = x-30.0f; e->y = y+90.0f;
				shooting = false;
			}
			break;
	}
}

//
//	rocket
//
E_BossRocket::E_BossRocket()
{
	sprRocket.Load("data/boss1raket.png", 2, 64, 0);
	spriteIndex = &sprRocket;
	sprRocket.imgSpeed = 0.1f;

	sndRocket = sf::Sound(*eng->res->sounds.Load("data/explosion.wav"));
	sndShoot = sf::Sound(*eng->res->sounds.Load("data/sht.wav"));

	pl = eng->jack->map.GetEntities().FindInstanceById(NULL, EID_PLAYER); // FIXME: Slow?

	gravity = 0;
	hspeed = -25.0f;

	SetTimer(0, 60);
	SetTimer(1, 1);		// let the parent init some stuff

	colRect.SetW(64.0f);
	colRect.SetH(24.0f);
}

void E_BossRocket::CallTimer(int num)
{
	if (num == 0)
		destroy=true;
	else if (num == 1) {
		// first exec, after pos is initialized
		if (pl)
			//sndShoot->PlayLoc(x - pl->x, y - pl->y, true);
			sndShoot.play(); // FIXME-SFML: fix pan
	}
}

void E_BossRocket::Collision(Entity *with)
{
	//Entity::Collision(with);

	if (spriteIndex && with->IsPlayer()) {
		if (pl)
			//sndRocket->PlayLoc(x - pl->x, y - pl->y, true);
			sndRocket.play();
		spriteIndex = NULL;
		// todo: explosion sprite

		with->Hurt(30.0f);
	}
}

void E_BossRocket::CheckTiles(SolidMap *sldMap)
{
	colRect.SetY(y+vspeed);
	colRect.SetX(x+hspeed);
}

//
//	surprise
//
E_BossSurprise::E_BossSurprise()
{
	sprSurprise.Load("data/boss1raket.png", 1, 64, 0);
	spriteIndex = &sprSurprise;

	sndDrop		= sf::Sound(*eng->res->sounds.Load("data/boss1drop.wav"));
	sndExplode	= sf::Sound(*eng->res->sounds.Load("data/explosion.wav"));

	pl = eng->jack->map.GetEntities().FindInstanceById(NULL, EID_PLAYER); // FIXME: Slow?

	firstExec = false;

	vspeed = gravity + 0.1f;
	colRect.SetW(64.0f);
	colRect.SetH(24.0f);
}

void E_BossSurprise::Step()
{
	Entity::Step();

	if (!firstExec) {
		if (pl)
			//sndDrop->PlayLoc(x - pl->x, y - pl->y, true);
			sndDrop.play(); // FIXME-SFML: pan
		firstExec = true;
	}

	if (spriteIndex && vspeed <= gravity) {
		// hit the ground
		if (pl)
			//sndExplode->PlayLoc(x - pl->x, y - pl->y, true);
			sndExplode.play(); // FIXME-SFML: pan
		spriteIndex = NULL;
		SetTimer(0, 60);
	}
}

void E_BossSurprise::CallTimer(int num)
{
	if (num == 0)
		destroy = true;
}

void E_BossSurprise::Collision(Entity *with)
{
	//Entity::Collision(with);

	// todo: explosion sprite
	if (spriteIndex && with->IsPlayer()) {
		if (pl)
			//sndExplode->PlayLoc(x - pl->x, y - pl->y, true);
			sndExplode.play(); // FIXME-SFML: pan
		
		spriteIndex = NULL;
		SetTimer(0, 60);
		
		// todo: explosion sprite
		with->Hurt(30.0f);
	}
}
