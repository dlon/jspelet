// FIXME: stop on random position to shoot?

#include "E_Boss2.h"
#include "Jack.h"
#include "Input.h"
#include <assert.h>
#include "TilesetID.h"

extern Engine *eng;
extern Input *input;

static const int		kAttack1DirectionTimer[] = {
	40,
	20,
	15
};
static const int		kAttack1AnimSpeed = 5;
static const float		kAttack1AnimStep = 0.8f;

static const int		kAttack2Timer[] = {
	45,
	38,
	33
};
static const int		kAttack2DirectionTimer = 40;
static const int		kAttack2AnimSpeed = 1;
static const float		kAttack2AnimStep = 1.0f;

//
//	trigger
//
E_Boss2Trigger::E_Boss2Trigger() : player(NULL)
{
	shake = trigger1 = false;
	sndShake = sf::Sound(*eng->res->sounds.Load("data/grnd0.wav"));
}

void E_Boss2Trigger::Step()
{
#if 0
	if (!player) {
		player = static_cast<E_Player*>( eng->jack->map.GetEntities().FindInstanceById(NULL, EID_PLAYER) );
	}
	else {
		if (!destroy &&
			x - player->x < 500 &&
			y - player->y < 100)
		{
			destroy = true;

			// create boss object
			Entity *boss = eng->jack->map.GetEntities().CreateEntity(EID_BOSS2OBJECT);
			E_Boss2 *bb = static_cast<E_Boss2*>(boss);
			bb->x = x;
			bb->y = y;
			bb->Activate(player);

			// fixate cam
			//eng->render->cam.maxY = 688.0f;

			// change the music
			//eng->jack->SetSong("data/hryp.ogg");
			eng->jack->musicPlayer.Free();
			SetTimer
		}
	}
#endif
	Entity::Step();

	if (!player) {
		player = static_cast<E_Player*>( eng->jack->map.GetEntities().FindInstanceById(NULL, EID_PLAYER) );
	}
	else {
		// spooky silence
		if (!trigger1 &&
			x - player->x < 300 &&
			y - player->y < 100)
		{
			trigger1 = true;

			eng->jack->musicPlayer.stop();
			SetTimer(0, 320);

			// create walls
			Entity *ent = 0;
			do {
				ent = eng->jack->map.GetEntities().FindInstanceById(ent, EID_BOSS2DOOR);

				if (ent) {
					static_cast<E_Boss2Door*>(ent)->CreateWall();
				}

			} while (ent != NULL);
		}

		// shake loop
		if (shake && sndShake.getStatus() != sf::SoundSource::Status::Playing) {
			sndShake.play();
			eng->jack->chaseCam.EarthQuake();
		}
	}
}

void E_Boss2Trigger::CallTimer(int num)
{
	switch (num) {
		case 0:
			// shake sound
			shake = true;
			SetTimer(1, 250);
			break;
		case 1:
			{
			// create boss object
			Entity *boss = eng->jack->map.GetEntities().CreateEntity(EID_BOSS2OBJECT);
			E_Boss2 *bb = static_cast<E_Boss2*>(boss);
			bb->x = x;
			bb->y = y;
			bb->Activate(player);

			// music
			eng->jack->SetSong("data/hryp.ogg");

			destroy = true;
			sndShake.stop();
			}
			break;
	}
}

//
//	boss sequencer (choreography)
//
E_Boss2Sequencer::E_Boss2Sequencer()
{
	boss = 0;
}

void E_Boss2Sequencer::SetEntity(E_Boss2 *e)
{
	boss = e;
	if (e)
		SetTimer(0, 30);
}

#define STAGE1_HPf 1600.0f
#define STAGE2_HPf 600.0f
#define STAGE3_HPf 0

void E_Boss2Sequencer::CallTimer(int num)
{
	if (!boss || !eng->jack->hud.GetBoss()) {
		destroy = true;
		return;
	}

	if (boss->GetHealth() > STAGE1_HPf) {
		boss->stage = 0;
	}
	else if (boss->GetHealth() > STAGE2_HPf) {
		boss->stage = 1;
	}
	else {
		boss->stage = 2;
	}

	// events
	switch (num) {
		//	GetHealth() > x
		//			2x(move, fire), shake+shield

		//	GetHealth() > y
		//			shake+fire+shield
		//			move incredifast

		case 0:
			if (boss->stage == 0) {
				boss->SetTimer(0, 1);
				SetTimer(1, 260);
			}
			else if (boss->stage == 1) {
				boss->SetTimer(0, 1);
				SetTimer(1, 130);
			}
			else {
				boss->SetTimer(0, 1);
				int nmove = rand()%5;
				int ntime = 130;
				if (nmove == 1 || nmove == 3)
					ntime -= (rand()%4)*25;
				SetTimer(nmove, ntime);
			}
			break;
		case 1:
			if (boss->stage == 0) {
				boss->SetTimer(1, 1);
				SetTimer(2, 240);
			}
			else if (boss->stage == 1) {
				boss->SetTimer(1, 1);
				SetTimer(2, 130);
			}
			else {
				boss->SetTimer(1, 1);
				int nmove = rand()%5;
				int ntime = 130;
				if (nmove == 1 || nmove == 3)
					ntime -= (rand()%4)*25;
				SetTimer(nmove, ntime);
			}
			break;
		case 2:
			if (boss->stage == 0) {
				boss->SetTimer(0, 1);
				SetTimer(3, 260);
			}
			else if (boss->stage == 1) {
				boss->SetTimer(0, 1);
				SetTimer(3, 130);
			}
			else {
				boss->SetTimer(0, 1);
				int nmove = rand()%5;
				int ntime = 130;
				if (nmove == 1 || nmove == 3)
					ntime -= (rand()%4)*25;
				SetTimer(nmove, ntime);
			}
			break;
		case 3:
			if (boss->stage == 0) {
				boss->SetTimer(1, 1);
				SetTimer(4, 240);
			}
			else if (boss->stage == 1) {
				boss->SetTimer(1, 1);
				SetTimer(4, 130);
			}
			else {
				boss->SetTimer(1, 1);

				int nmove = rand()%5;
				int ntime = 130;
				if (nmove == 1 || nmove == 3)
					ntime -= (rand()%4)*25;
				SetTimer(nmove, ntime);
			}
			break;
		case 4:
			if (boss->stage == 0) {
				boss->SetTimer(2, 1); // shake
				boss->SetTimer(3, 60); // shield
				SetTimer(0, 360);
			}
			else if (boss->stage == 1) {
				boss->SetTimer(2, 1); // shake
				boss->SetTimer(3, 60); // shield
				SetTimer(0, 360);
			}
			else {
				boss->SetTimer(2, 1); // shake
				boss->SetTimer(3, 60); // shield

				int nmove = rand()%5;
				int ntime = 360;
				if (nmove == 1 || nmove == 3)
					ntime = 40;
				SetTimer(nmove, ntime);
			}
			break;
	}
}

//
//	boss
//
E_Boss2::E_Boss2()
{
	maxHealth = 3000.0f;
	health = maxHealth;

	spr1.Load("data/d3.png", 3, 320, 0); // FIXME: free when done - FIXME2: we only need 3 frames! (but there are 4)
	spr2.Load("data/d4.png", 4, 320, 0); // FIXME: free when done

	sndShake = sf::Sound(*eng->res->sounds.Load("data/grnd0.wav")); // FIXME: up frequency towards end?

	spriteIndex = &spr1;

	gravity = 0;

	colRect.SetW(320);
	colRect.SetH((float)spr1.GetHeight());

	activRadius		= 1000.0f;
	playerDamage	= 10.0f;

	attack1 = 0;
	attack2 = 0;
	attack3 = 0;
	attack3_1 = 0;
	stage = 0;

	eng->jack->hud.SetBoss(this);
	
	//seq.SetEntity(this);
	seq = static_cast<E_Boss2Sequencer*>(eng->jack->map.GetEntities().CreateEntity(EID_BOSS2SEQUENCER));
	seq->SetEntity(this);
}

E_Boss2::~E_Boss2()
{
	//seq->SetEntity(NULL);
	//seq->destroy = true; // destroy sequencer
	eng->jack->hud.SetBoss(NULL); // FIXME: memory bug here - closwe in boss fight i

	/*
	// destroy walls
	Entity *ent = 0;
	do {
		ent = eng->jack->map.GetEntities().FindInstanceById(ent, EID_BOSS2DOOR);

		if (ent) {
			static_cast<E_Boss2Door*>(ent)->DestroyWall();
		}

	} while (ent != NULL);
	*/
	//eng->jack->musicPlayer.Stop();
}

void E_Boss2::Step()
{
	E_BaseBoss::Step();

	if (input->CheckPressed(VK_NUMPAD1))
		SetTimer(0, 1);
	if (input->CheckPressed(VK_NUMPAD2))
		SetTimer(1, 1);
	if (input->CheckPressed(VK_NUMPAD3))
		SetTimer(2, 1);
	if (input->CheckPressed(VK_NUMPAD4))
		SetTimer(3, 1);

	if (attack3 && sndShake.getStatus()!=sf::SoundSource::Status::Playing) {
		sndShake.play();
		eng->jack->chaseCam.EarthQuake();
	}
}

void E_Boss2::PostStep()
{
	E_BaseBoss::PostStep();
}

void E_Boss2::CallTimer(int num)
{
	E_BaseBoss::CallTimer(num);

	switch (num)
	{
	case 0: // attack 1 (move up and down)
		spriteIndex = &spr1;

		if (++attack1 % 2)
			vspeed = -(stage ? 6.0f : 4.0f);
		else
			vspeed = (stage ? 6.0f : 4.0f);
		SetTimer(10, kAttack1AnimSpeed);

		if (attack1 < 7)
			SetTimer(0, kAttack1DirectionTimer[stage]);
		else {
			//SetTimer(1, 30);
			attack1 = 0;
			spriteIndex->imgInd = 0;
			spriteIndex->imgSpeed = 0;
			vspeed = 0;
		}
		break;

	case 10: // attack animation
		if (attack1 % 2) {
			if (spriteIndex->imgInd + kAttack1AnimStep < (float)(spriteIndex->GetNumFrames()))
				spriteIndex->imgInd += kAttack1AnimStep;
			else
				spriteIndex->imgInd = (float)(spriteIndex->GetNumFrames()-1);
		}
		else {
			if (spriteIndex->imgInd - kAttack1AnimStep > 0.0f)
				spriteIndex->imgInd -= kAttack1AnimStep;
			else
				spriteIndex->imgInd = 0;
		}

		if (attack1 > 0)
			SetTimer(10, kAttack1AnimSpeed);
		break;

	case 1: // attack 2: fireball
		spriteIndex = &spr2;
		if (attack2++ < 5) {
			E_Boss2Fireball *fb = static_cast<E_Boss2Fireball*>(eng->jack->map.GetEntities().CreateEntity(EID_BOSS2FIREBALL));
			if (fb) {
				fb->pl = player;
				fb->x = x;
				fb->y = y + 64.0f;
				fb->hspeed = (stage ? -13.0f : -10.0f);
			}
			SetTimer(1, kAttack2Timer[stage]);
		}
		else
			attack2 = 0;
		SetTimer(11, kAttack2AnimSpeed);
		break;

	case 11: // attack 2 anim
		if (attack2 > 0) {
			if (spriteIndex->imgInd + kAttack2AnimStep < (float)(spriteIndex->GetNumFrames()))
				spriteIndex->imgInd += kAttack2AnimStep;
			else
				spriteIndex->imgInd = (float)(spriteIndex->GetNumFrames()-1);

			SetTimer(11, kAttack2AnimSpeed);
		}
		else {
			if (spriteIndex->imgInd - kAttack2AnimStep > 0.0f) {
				spriteIndex->imgInd -= kAttack2AnimStep;
				SetTimer(11, kAttack2AnimSpeed);
			}
			else {
				spriteIndex->imgInd = 0;
			}
		}
		//puts("tim11");
		break;

	case 2: // ground fire
		// spawn a bunch of them in random locations
		if (++attack3 <= 6) {
			/*
			float px = x - float( rand()%321 ); // proximate x
			for (int i=0; i<3; i++) {
				E_Boss2GrndFire *gf = static_cast<E_Boss2GrndFire*>(eng->jack->map.GetEntities().CreateEntity(EID_BOSS2GRNDFIRE));
				if (gf) {
					gf->x = px + float( rand()%5 ); // set x; y is automated
					gf->frAlpha[0] = (255.0f/3.0f)*float(i+1); // set alpha: highest = highest
					//gf->frBlend[0] = (i > 0 ? 1 : 0); // set blend mode
				}
			}
			*/
			E_Boss2GrndFire *gf = static_cast<E_Boss2GrndFire*>(eng->jack->map.GetEntities().CreateEntity(EID_BOSS2GRNDFIRE));
			//if (gf)
			//	gf->x = x - float( (rand()%18)*16 ); // set x; y is automated
			SetTimer(2, 30);
		}
		else {
			attack3 = 0;

			// (re)play some extra shake sound
			sndShake.play();
			eng->jack->chaseCam.EarthQuake();
		}
		break;

	case 3: // shield
		E_Boss2Shield *bs = static_cast<E_Boss2Shield*>(eng->jack->map.GetEntities().CreateEntity(EID_BOSS2SHIELD));
		bs->x = x - 50.0f;
		bs->y = y - 60.0f;
		break;
	}
}

//
//	fire
//
E_Boss2Fireball::E_Boss2Fireball()
{
	this->pl = 0;

	//hspeed = -10.0f;
	gravity = 0;

	snd1 = sf::Sound(*eng->res->sounds.Load("data/fball3.wav"));
	snd1.play();
	// FIXME-SFML: Stop the old fireball sound

	sprFireball.Load("data/d_klot.png", 2, 41, 0);
	spriteIndex = &sprFireball;

	sprFireball.imgSpeed = 0.5f;

	colRect.FromSprite(&sprFireball);

	SetTimer(0, 120);
}

void E_Boss2Fireball::Collision(Entity *with)
{
	if (with->IsPlayer())
	{
		with->Hurt(15.0f);
	}
}

void E_Boss2Fireball::CallTimer(int num)
{
	switch (num)
	{
	case 0:
		destroy = true;
		break;
	}
}

//
//	ground fire attack
//

static const int		grndFireMW = 35;
static const int		grndFireMH = 178;
static const int		grndFireT0 = 1;
static const int		grndFireDmvI = 10;
static const int		grndFireT1 = 1;
static const float		grndFireRmA = 20.0f; // remove alpha

E_Boss2GrndFire::E_Boss2GrndFire()
{
	gravity = 0;

	spr1.Load("data/d_eld.png", 2, grndFireMW, grndFireMH);
	spr1.imgSpeed = 0.1f;
	frHeight[0] = 1;
	
	frAlpha[1] = 100.0f;
	frAlpha[0] = 255.0f;
	frBlend[0] = 0;

	// clone, alpha, different speeds

	// a bit of black magic to get things done
	for (int i=0; i<spr1.GetNumFrames(); i++)
	{
		spr1.GetFrame(i)->h = frHeight[0];
		//spr1.GetFrame(i)->ty = grndFireMH - spr1.GetFrame(i)->h;
	}

	// snap to ground
	Entity *boss = eng->jack->map.GetEntities().FindInstanceById(0, EID_BOSS2OBJECT);
	if (boss) {
		x = boss->x - float( (rand()%33)*16 );
		y = boss->y - 50.0f;
	}

	colRect.FromSprite(&spr1);
	colRect.SetX(x); colRect.SetY(y);

	CollisionObject xRect(colRect);
	xRect.SetY(xRect.GetY() - 10.0f);

	while (!eng->jack->map.GetSolids().Collision(colRect, xRect, TFLAG_BLOCK_N) &&
		y < 10000.0f)
	{ // TODO: create evil macro that assumes pointer names, etc?
		y += 1.0f;
		colRect.SetY(y);
		xRect.SetY(y-10.0f);
	}

	// TODO: shake ground

	snd1 = sf::Sound(*eng->res->sounds.Load("data/grnd2.wav"));

	// animate fire
	spr1.imgInd = (float)(rand()%2);
	//SetTimer(0, grndFireT0 + rand()%3);
	SetTimer(3, 1);
}

void E_Boss2GrndFire::CallTimer(int num)
{
	switch (num)
	{
	case 0: // change frame offset + height ("ascend")

		if (frHeight[0] == 1) {
			snd1.play(); // FIRST FRAME
		}

		frHeight[0] += grndFireDmvI;
		y -= float(grndFireDmvI);

		colRect.SetH((float)frHeight[0]); // NOTE

		// more black magic
		for (int i=0; i<spr1.GetNumFrames(); i++)
		{
			spr1.GetFrame(i)->h = frHeight[0];
			//spr1.GetFrame(i)->ty = grndFireMH - spr1.GetFrame(i)->h;
		}

		if (frHeight[0] < grndFireMH)
			SetTimer(0, grndFireT0);
		else
			SetTimer(1, grndFireT1);
		break;
	case 1: // fade out and die
		frAlpha[0]-=grndFireRmA;
		if (frAlpha[0] <= 0.0f) {
			frAlpha[0] = 0;
			destroy = true;
		}
		else {
			SetTimer(1, grndFireT1);
		}
		break;

	case 3: // fade in warning
		frAlpha[1]+=5.0f;
		if (frAlpha[1] >= 200.0f) {
			frAlpha[1] = 200.0f;
			SetTimer(0, 50); // actual fire
			SetTimer(4, 50); // fade out warning
		}
		else
			SetTimer(3, 1); // keep fading in
		break;
	case 4: // fade out warning
		frAlpha[1]-=5.0f;
		if (frAlpha[1] <= 0.0f) {
			frAlpha[1] = 0;
		}
		else
			SetTimer(4, 1);
		break;
	}
}

void E_Boss2GrndFire::Collision(Entity *with)
{
	if (with->IsPlayer())
	{
		if (frHeight[0] >= 3)
			with->Hurt(15.0f);
	}
}

void E_Boss2GrndFire::PostStep()
{
	spr1.Update();

	colRect.SetX(x);
	colRect.SetY(y);
}

void E_Boss2GrndFire::Draw()
{
	if (frBlend[0] == 0)
		eng->render->SetBlendMode(RBLEND_NORMAL, true);
	else
		eng->render->SetBlendMode(RBLEND_ADDITIVE, true);

	// draw marker
	eng->render->SetColor(255,128,0,(unsigned char)frAlpha[1]);
	eng->render->DrawRect(x,y + (float)frHeight[0], 35.0f, 4.0f);

	// draw fire
	eng->render->SetColor(255,255,255,(unsigned char)(frAlpha[0]));
	spr1.x = x;
	spr1.y = y;
	spr1.Draw();

	eng->render->SetBlendMode(RBLEND_NORMAL, false);
}

//
//	shield
//
static const float		kShieldAlphaStep = 20.0f;

E_Boss2Shield::E_Boss2Shield()
{
	spr1.Load("data/d_shield.png", 2, 34, 0);
	a = 0;
	fMode = 0; // 0=fade in,1=standby,2=fade out

	snd1 = sf::Sound(*eng->res->sounds.Load("data/tp.wav"));
	snd2 = sf::Sound(*eng->res->sounds.Load("data/tp2.wav"));

	snd1.play();

#if 0
	// find ground
	Entity *boss = eng->jack->map.GetEntities().FindInstanceById(0, EID_BOSS2OBJECT);
	if (boss) {
		x = boss->x - 50.0f;
		y = boss->y - 50.0f;
	}

	colRect.FromSprite(&spr1);
	colRect.SetX(x); colRect.SetY(y);
	CollisionObject xRect(colRect);
	xRect.SetY(xRect.GetY() - 10.0f);

	while (!eng->jack->map.GetSolids().Collision(colRect, xRect, TFLAG_BLOCK_N) &&
		y < 10000.0f)
	{ // TODO: create evil macro that assumes pointer names, etc?
		y += 1.0f;
		colRect.SetY(y);
		xRect.SetY(y-10.0f);
	}

	y += 10.0f; // roughly accurate
	colRect.SetY(y);
#endif
	colRect.FromSprite(&spr1);

	solid = true;
}

void E_Boss2Shield::Step()
{
	Entity::Step();

	if (fMode == 0) {
		a += kShieldAlphaStep;
		if (a >= 255.0f) {
			a = 255.0f;
			fMode = 1;

			SetTimer(0, 120); // set fade out
		}
	}
	else if (fMode == 2) {
		a -= kShieldAlphaStep;
		if (a <= 0.0f) {
			a = 0;
			SetTimer(1, 30); // periodically check whether sound is playing and then destroy
			fMode = 1;
		}
		else if (a < 50.0f) {
			solid = false;
		}
	}
}

void E_Boss2Shield::CallTimer(int num)
{
	switch (num)
	{
	case 0:// begin fading out
		snd2.play();
		SetTimer(5, 40); // give sound a head start
		break;
	case 5:
		fMode = 2;
		break;
	case 1:
		if (snd2.getStatus() != sf::SoundSource::Status::Playing) {
			destroy = true;
		}
		else {
			SetTimer(1, 30);
		}
		break;
	}
}

void E_Boss2Shield::Collision(Entity *with)
{
	if (with->GetID() == EID_LAZERBEAM) {
		with->destroy = true;
		// TODO: sound effect - gunshot in barrel
	}
}

void E_Boss2Shield::PostStep()
{
	spr1.x = x;
	spr1.y = y;
	spr1.Update();

	colRect.SetX(x);
	colRect.SetY(y);

	Entity::PostStep();
}

void E_Boss2Shield::Draw()
{
	eng->render->SetColor(255, 255, 255, (unsigned char)a);
	eng->render->SetBlendMode(RBLEND_NORMAL, true);
	spr1.Draw();
	eng->render->SetBlendMode(RBLEND_NORMAL, false);
}

//
// magic door
//

#define WALL_N 9

static int nSndRef = 0;

E_Boss2Door::E_Boss2Door()
{
	tset = TilesetID::GetTexture(0);

	nSndRef++;

	snd = sf::Sound(*eng->res->sounds.Load("data/shitclose.wav"));
	sndOpen = sf::Sound(*eng->res->sounds.Load("data/shitopen.wav"));

	//CreateWall();
}
E_Boss2Door::~E_Boss2Door()
{
	--nSndRef;
	if (!nSndRef) {
		eng->res->sounds.FreeByFile("data/shitclose.wav");
		eng->res->sounds.FreeByFile("data/shitopen.wav");
	}
}

void E_Boss2Door::CreateWall()
{
	player = static_cast<E_Player*>(eng->jack->map.GetEntities().FindInstanceById(NULL, EID_PLAYER));
	if (player) {
		player->LockInput();

		//if (player->x <= x+32.0f)
		//	player->x = x+40.0f;
	}

	wallN = WALL_N;
	y += 32.0f;
	wallHeight = 0;
	SetTimer(0, 30);

	SetTimer(10, 1000); // periodically check whether boss object exists
}

void E_Boss2Door::DestroyWall()
{
	if (player) {
		//player->LockInput();

		//if (player->x <= x+32.0f)
		//	player->x = x+40.0f;
	}

	wallN = WALL_N;
	//y -= 32.0f;
	SetTimer(1, 30);
}

void E_Boss2Door::CallTimer(int num)
{
	switch (num)
	{
		// create wall
	case 0:
		snd.play();
		
		wallHeight += 32.0f;
		y -= 32.0f;
		colRect.SetW(32.0f);
		colRect.SetH(wallHeight);
		colRect.SetX(x);
		colRect.SetY(y);

		if (!(--wallN)) {
			player->UnlockInput();
			//SetTimer(5, 180); // test
		}
		else
			SetTimer(0, 30);
		break;
	case 1: // destroy wall
		sndOpen.play();
		
		wallHeight -= 32.0f;
		y += 32.0f;
		colRect.SetW(32.0f);
		colRect.SetH(wallHeight);
		colRect.SetX(x);
		colRect.SetY(y);

		if (!(--wallN)) {
			player->UnlockInput();
			
			colRect.SetW(0);
			colRect.SetH(0);

			SetTimer(8, 120); // destroy ghost object
		}
		else
			SetTimer(1, 30);
		break;

		/*
	case 5:
		DestroyWall();
		break;
		*/

	case 8:
		destroy = true;
		break;

	case 10: // periodically check whether boss object exists
		Entity *boss=eng->jack->map.GetEntities().FindInstanceById(0, EID_BOSS2OBJECT);
		if ((int)wallHeight/32 == WALL_N && !boss)
		{
			DestroyWall();
			eng->jack->musicPlayer.stop(); // FIXME: This should be triggered when boss 2 is destr, but it's not safe in the destructor
		}
		else
			SetTimer(10, 240);
		break;
	}
}

void E_Boss2Door::Draw()
{
	// Entity::Draw();
	if (wallHeight) {
		SubImage sub;
		sub.t = tset;
		sub.h = 32;
		sub.w = 32;
		sub.tx = 128;
		sub.ty = 0;

		for (int i=0; i < (int)wallHeight/32; i++) {
			eng->render->DrawSubImage(&sub, x, y + 32*i);
		}
	}
}
