// TODO: Change face even if the direction is blocking (change face by key instead of hspeed)

#include "E_Player.h"
#include "Engine.h"
#include "Input.h"
#include <stdio.h>
#include "SolidMap.h"
#include "Jack.h"
#include "E_WoodenBox.h"
#include "E_Barrel.h"
#include "E_MvPlatform.h"
#include "E_LazerBeam.h"

extern Engine *eng;
extern Input *input;
extern bool lInvicParameter;

int		plCheckPointId = -1;
bool	plResetChkPt = true;

static float cpX=0,cpY=0;

E_Player::E_Player()
{
	sPlayer[0].Load("data/jakob1.png", 3, 41, 64);
	sPlayerJmp[0].Load("data/jakob1.png", 2, 41, 64, 0, 64);
	sPlayer[1].Load("data/jakob2.png", 3, 41, 64);
	sPlayerJmp[1].Load("data/jakob2.png", 2, 41, 64, 0, 64);
	sPlayer[2].Load("data/jakob3.png", 3, 41, 64);
	sPlayerJmp[2].Load("data/jakob3.png", 2, 41, 64, 0, 64);
	
	sPlayerDead.Load("data/jakob_ded.png", 1, 0, 0);
	sPlayerDead2.Load("data/ks.PNG", 2, 37, 0);

	aJump		= eng->res->sounds.Load("data/hepp2.wav");
	aPlunge		= eng->res->sounds.Load("data/splash3.wav");
	aBlaster	= eng->res->sounds.Load("data/lazer2.wav");
	aHurt		= eng->res->sounds.Load("data/hurt.wav");
	aDying		= eng->res->sounds.Load("data/dying2.wav");
	aDead		= eng->res->sounds.Load("data/dead.wav");
	aSwim		= eng->res->sounds.Load("data/swim10.wav");
	aCp			= eng->res->sounds.Load("data/cp.wav");

	//aPlunge->SetVolume(-500);
	
	gravity = 0.6f;
	spriteIndex = &sPlayer[0];

	jumpFuelPoint = 0;
	sprint = false;

	/*colRect.SetW(41.0f);
	colRect.SetH(63.0f);
	colRect.lSlopeOff = 15.0f;
	colRect.rSlopeOff = 15.0f;*/
	
	//colRect.SetW(32.0f);
	colRect.SetH(63.0f);
	colRect.SetW(25.0f); // can't be under 32: makes you fall through - I could fix this (08-04: Fall through what? I can't find this problem anywhere)
	//colRect.SetYOff(5.0f);
	//colRect.SetH(58.0f);
	for (int i=0; i<3; i++) {
		sPlayer[i].xofs = -8.0f;
		sPlayerJmp[i].xofs = -8.0f;
	}
	//colRect.lSlopeOff = 8.0f;
	//colRect.rSlopeOff = 15.0f;
	colRect.lSlopeOff = 8.0f;
	colRect.rSlopeOff = 8.0f;

	jump					= false;
	vspeed					= 0;
	swimFuel				= 0;
	blasterTimer			= eng->time; // needs to be eng->time, because it can be negative
	doubleJumpTimer			= eng->time;
	pullThroughWaterTimer	= eng->time;
	healthTimer				= eng->time;
	lockInput				= false;
	dying					= 0;
	onPlatform				= false;
	hurt					= -1;

	emotion					= 0;
	spritesAlpha[0]			= 0;
	spritesAlpha[1]			= 0;
	spritesAlpha[2]			= 255.0f;
	health					= 100.0f;
	jHspeed					= 0;
	//UpdateHealth(health);

	hasPlayedSplash = false;

	// set HUD stuff
	eng->jack->hud.SetPlayer(this);

	// move to checkpoint
	if (plCheckPointId != -1) {
		SetTimer(5,1);
	}
	plResetChkPt = true;
}

void E_Player::UpdateHealth(float newHealth)
{
	float oldHp = health;
	health = newHealth;

	if (health < 75.0f && health >= 25.0f &&
		oldHp >= 75.0f)
	{
		emotion = 0;
		SetTimer(2, 1); // 2 -> 1
	}
	if (health < 25.0f && oldHp >= 25.0f) {
		emotion = 1;
		SetTimer(2, 1); // 1 -> 0
	}
	if (health >= 25.0f && health < 75.0f
		&& oldHp < 25.0f) {
		emotion = 2;
		SetTimer(2, 1); // 0 -> 1
	}
	if (health >= 75.0f && oldHp < 75.0f) {
		emotion = 3;
		SetTimer(2, 1); // 1 -> 2
	}
}

void E_Player::Step()
{
	Entity::Step();

	if (jumpFuelPoint && eng->time >= jumpFuelPoint)
	{
		if (!lockInput && !input->Check(VK_SPACE))
			vspeed += 4.0f;
		jumpFuelPoint = 0;
	}

	if (eng->time >= swimFuel)
		swimFuel = 0;

	if (!lockInput && vspeed == .0f)
	{
		if (input->Check(VK_SHIFT))
			sprint = true;
		else
			sprint = false;
		jHspeed = 0;
	}

	if (inWater)
		sprint = false;

	// lazer
	if (!lockInput && eng->time >= blasterTimer && input->CheckPressed('D')) {
		//aBlaster->Stop();
		//aBlaster->SetCurrentPosition(0);
		//aBlaster->Play(0,0,0);
		aBlaster->Stop();
		aBlaster->Play();

		// beam
		Entity *lazer = eng->jack->map.GetEntities().CreateEntity(EID_LAZERBEAM); // add(new E_LazerBeam)
		lazer->x = (spriteIndex->Flipped() ? x-15.0f : x+20.0f);
		lazer->y = y+30.0f;
		lazer->hspeed = (spriteIndex->Flipped() ? -20.0f : 20.0f);
		static_cast<E_LazerBeam*>(lazer)->ShortenLife(5);

		blasterTimer = eng->time + 100;
	}

	// movement
	float speed = sprint ? 5.0f : 2.5f;
	
	if (inWater && !swimFuel)
		speed = vspeed; // or 1.25
	if (inWater && !speed && !vspeed) // sea floor
		speed = 1.0f;

	if (!lockInput) {
		if (!input->Check(VK_LEFT) && !input->Check(VK_RIGHT))
			hspeed = 0;
		if (input->Check(VK_LEFT) && !input->Check(VK_RIGHT))
			hspeed = -speed;
		if (input->Check(VK_RIGHT) && !input->Check(VK_LEFT))
			hspeed = speed;
	}

	// dev shit
#ifdef JSDEBUG
	if (input->Check('F'))
		vspeed = -5.0f;
#endif

	// jump
#if 0
	if (doubleJumpTimer && doubleJumpTimer > eng->time && input->CheckPressed(VK_SPACE)) {
		doubleJumpTimer = 0;
		pullOutOfWater = true;
		pullThroughWaterTimer = eng->time + 75;
	}
	else if (input->CheckPressed(VK_SPACE)) {
		doubleJumpTimer = eng->time + 300; // msec to jump out of water
	}
	if (eng->time >= pullThroughWaterTimer /*|| vspeed > 0.0f*/) {
		pullOutOfWater = false;
		pullThroughWaterTimer = 0;
	}
#endif

	if (!lockInput && (!vspeed || (inWater)) && input->CheckPressed(VK_SPACE))
	{
		if (inWater || !input->Check(VK_DOWN))
		{
#if 0
			if (!inWater /*|| pullOutOfWater*/)
				vspeed = !sprint ? -12.8f : -14.3f;
			else {
				vspeed = -5.0f; // inWater
				swimFuel = eng->time + 500;
			}
#endif
			float speed = (!sprint || inWater) ? 12.8f : 14.3f;
			/*
			speed *= health/100.0f; // run slower as you get hurt
			if (speed < 1.0f)
				speed = 1.0f;
			*/

			if (inWater)
				swimFuel = eng->time + 500;
			//vspeed = (!sprint || inWater) ? -12.8f : -14.3f;
			vspeed = -speed;

			if (onPlatform)
				jHspeed = platformHs;

			jump = true;
			if (inWater) {
				aSwim->Rewind();
				aSwim->Play();
			}
			else
				aJump->Play();
		}
		else {
			fallThroughTile = true;
		}
	}

	// release jump
	if (jump && ((!inWater && !input->Check(VK_SPACE)) || vspeed > .0f) )
	{
		if (vspeed < -3.0f)
			vspeed += 7.0f;
		jump = false;
	}

	// low health
	if (health < 30.0f) {
		if (!aDying->Playing())
			aDying->Play(false);
	}
	else
		aDying->Stop();

	// die (out of camera)
	if (eng->render->cam.maxY && y > eng->render->cam.maxY+500.0f) {
		if (!dying)
			vspeed = -20.0f;
		PlayDeathSequence();
	}

	// die (out of health)
	if (health <= 0.0f && !lInvicParameter) {
		PlayDeathSequence2();
	}

	if (dying == 1)
		sPlayerDead.rot = fmod(sPlayerDead.rot + 10.0f, 360.0f);

	// platform hack
	onPlatform = false;

	// checkpoint
	CheckPointInfo *cpi;
	if (cpi = eng->jack->map.CheckPointAt(x, y, colRect.GetW(), colRect.GetH())) {
		plCheckPointId = cpi->id;
		aCp->Play();
		eng->jack->hud.CheckPoint();
		//hud.TextPopUp();
	}
}

void E_Player::PostStep()
{
	if (x < 0.0f)
		x = 0;

	// update sprite
	if (vspeed < .0f)
	{
		spriteIndex = &sPlayer[0];
		spriteIndex->imgInd = 0.0f;
	}
	else if (vspeed > .0f) {
		spriteIndex = &sPlayerJmp[0];
		spriteIndex->imgInd = 1.0f;
	}
	else {
		spriteIndex = &sPlayer[0];
		if (hspeed) {
			if (inWater) // sea floor
				spriteIndex->imgSpeed = 0.2f;
			else
				spriteIndex->imgSpeed = 0.3f;
		}
		else {
			spriteIndex->imgSpeed = 0;
			spriteIndex->imgInd = 0;
		}
	}

	/*
	if (hspeed > .0f || input->Check(VK_RIGHT)) // moonwalk bug
	{
		for (int i=0; i<3; i++)
		{
			sPlayer[i].NormalFlip();
			sPlayerJmp[i].NormalFlip();
		}
	}
	else if (hspeed < .0f || input->Check(VK_LEFT)) {
		for (int i=0; i<3; i++)
		{
			sPlayer[i].MirrorFlip();
			sPlayerJmp[i].MirrorFlip();
		}
	}
	*/
	// intentional moonwalk
	if (hspeed > .0f) {
		if (!input->Check(VK_LEFT)) {
			for (int i=0; i<3; i++) {
				sPlayer[i].NormalFlip();
				sPlayerJmp[i].NormalFlip();
			}
		}
		else {
			for (int i=0; i<3; i++) {
				sPlayer[i].MirrorFlip();
				sPlayerJmp[i].MirrorFlip();
			}
		}
	}
	if (hspeed < .0f) {
		if (!input->Check(VK_RIGHT)) {
			for (int i=0; i<3; i++) {
				sPlayer[i].MirrorFlip();
				sPlayerJmp[i].MirrorFlip();
			}
		}
		else {
			for (int i=0; i<3; i++) {
				sPlayer[i].NormalFlip();
				sPlayerJmp[i].NormalFlip();
			}
		}
	}

	// splash
	if (inWater && !hasPlayedSplash && prevVspeed > 6.0f) {
		aPlunge->Play();
		hasPlayedSplash = true;
	}
	else if (!inWater && hasPlayedSplash)
		hasPlayedSplash = false;

	hspeed += jHspeed;
	Entity::PostStep();
	hspeed -= jHspeed;

	// have the camera follow
	/*
	eng->render->cam.x = x+floor(colRect.w/2) - 320.0f;
	eng->render->cam.y = y+floor(colRect.h/2) - 240.0f;
	if (eng->render->cam.x < 0) eng->render->cam.x = 0;
	if (eng->render->cam.y < 0) eng->render->cam.y = 0;
	*/
	eng->jack->chaseCam.MoveTo(x+floor(colRect.GetW()/2) - 320.0f,
		y+floor(colRect.GetH()/2) - 240.0f);

	fallThroughTile = false;

	// update hurt blink
	if (hurt != -1) {
		hurt = (hurt + 10) % 510;
	}
}

void E_Player::CallTimer(int num)
{
	switch (num) {
		case 0:
			UnlockInput();
			//dying = false;
			eng->jack->SafeLoadMap(eng->jack->map.MapFile());
			plResetChkPt = false;
			// TODO: Disable previous checkpoints
			break;
		case 2:
			// fade emotion (sprite)
			{
				int from,to;
				switch (emotion) {
					case 0:
						from = 2;
						to = 1;
						spritesAlpha[0] = 0;
						break;
					case 1:
						from = 1;
						to = 0;
						spritesAlpha[2] = 0;
						break;
					case 2:
						from = 0;
						to = 1;
						spritesAlpha[2] = 0;
						break;
					case 3:
						from = 1;
						to = 2;
						spritesAlpha[0] = 0;
						break;
				}
				spritesAlpha[to] = 255.0f;
				if (spritesAlpha[from] > 0.0f) {
					spritesAlpha[from] -= 20.0f;
					spritesAlpha[from] = spritesAlpha[from] < 0.0f ? 0.0f : spritesAlpha[from];
					SetTimer(2, 1); // fade
				}
			}
			break;
		case 3: // death 2 animation
			sPlayerDead2.imgInd = 1.0f;
			sPlayerDead2.yofs = 14.0f;
			y -= 14.0f;
			break;

		case 4:
			// hurt timer
			hurt = -1;
			break;
		
		case 5:
			// set cpoint position
			CheckPointInfo *cpi = eng->jack->map.CheckPointId(plCheckPointId);
			if (cpi) {
				x = cpi->x;
				y = cpi->y;

				eng->jack->chaseCam.MoveTo(x-320.0f, y-240.0f);
				eng->jack->chaseCam.Reset();
			}
			// disable taken checkpoints
			for (int i=0; i<=plCheckPointId; i++) {
				CheckPointInfo *cpi=eng->jack->map.CheckPointId(i);
				if (cpi) {
					cpi->taken = true;
				}
			}
			break;
	}
}

void E_Player::LockInput()
{
	lockInput = true;
	hspeed = 0;
}
void E_Player::UnlockInput()
{
	lockInput = false;
}
void E_Player::PlayDeathSequence()
{
	if (dying)
		return;

	LockInput();
	//vspeed = -20.0f;
	SetTimer(0, 120);
	dying = 1;

	aDead->Play(false);
	eng->jack->hud.OneDown();
}
void E_Player::PlayDeathSequence2()
{
	if (dying)
		return;
	LockInput();
	SetTimer(0, 120);
	dying = 2;

	vspeed = -7.5f;

	// dead 2 sprite offset
	SetTimer(3, 10);
	sPlayerDead2.yofs = 5.0f;
	sPlayerDead2.xofs = -8.0f;

	if (spriteIndex->Flipped())
		sPlayerDead2.MirrorFlip();
	else
		sPlayerDead2.NormalFlip();

	eng->jack->hud.OneDown();
}

void E_Player::Draw()
{
#ifndef NDEBUG /* draw colRect */ // FIXME: JSDEBUG
	Sprite *ss = spriteIndex;
	spriteIndex = 0;
	Entity::Draw();
	spriteIndex = ss;
#endif
	/*
	if (spriteIndex)
	{
		spriteIndex->x = x;
		spriteIndex->y = y;
		spriteIndex->Draw();
	}
	*/
	eng->render->SetBlendMode(RBLEND_NORMAL, true);

	if (dying == 1) {
		sPlayerDead.x = x;
		sPlayerDead.y = y;
		sPlayerDead.Draw();
	}
	else if (dying == 2) {
		sPlayerDead2.x = x;
		sPlayerDead2.y = y;
		sPlayerDead2.Draw();
	}
	else if (vspeed != .0f || (inWater && vspeed))
	{
		if (vspeed < .0f)
			sPlayerJmp[0].imgInd = 0.0f;
		else
			sPlayerJmp[0].imgInd = 1.0f;

		for (int i=0; i<3; i++) {
			sPlayerJmp[i].x = x;
			sPlayerJmp[i].y = y;
			sPlayerJmp[i].imgInd = sPlayerJmp[0].imgInd;

			/*
			if (hurt == -1 || i < 2)
				eng->render->SetColor(255, 255, 255, (unsigned char)spritesAlpha[i]);
			else
			*/
			{
				int a = hurt;
				if (a > 255) a = 255 - a;
				eng->render->SetColor(255, (unsigned char)a, (unsigned char)a, (unsigned char)spritesAlpha[i]);
			}
			sPlayerJmp[i].Draw();
		}
	}
	else {
		for (int i=0; i<3; i++) {
			sPlayer[i].x = x;
			sPlayer[i].y = y;
			sPlayer[i].imgInd = sPlayer[0].imgInd;
			/*
			if (hurt == -1 || i < 2)
				eng->render->SetColor(255, 255, 255, (unsigned char)spritesAlpha[i]);
			else
			*/
			{
				int a = hurt;
				if (a > 255) a = 255 - a;
				eng->render->SetColor(255, (unsigned char)a, (unsigned char)a, (unsigned char)spritesAlpha[i]);
			}
			sPlayer[i].Draw();
		}
	}
	eng->render->SetBlendMode(RBLEND_NORMAL);
}

void E_Player::Hurt(float hp)
{
	if (dying || healthTimer > eng->time)
		return;

	float newHealth = (health - hp) > 0.0f ? (health - hp) : 0.0f;
	
	aHurt->Play();
	healthTimer = eng->time + 750;

	hurt = 0;
	SetTimer(4, 51);

	UpdateHealth(newHealth);
}

void E_Player::CheckTiles(SolidMap *sldMap)
{
	if (dying != 1)
		Entity::CheckTiles(sldMap);
	else {
		if (vspeed+gravity < GRAVITY_LIMIT)
			vspeed += gravity;
	}
}

void E_Player::Collision(Entity *with)
{
	if (dying)
		return;

	//Entity::Collision(with);

	// ENTITY:COLLISION START
	if (with->IsSolid()) {
		// block above with
		if (vspeed > 0.0f && prevy + colRect.GetH() - 5.0f <= with->y) { // fix here         bckp: vspeed > 0.0f
			y = floorf(with->colRect.GetY() - colRect.GetH() - 1.0f); // TODO: Lådfix. Ok?

			if (with->IsMovingPlatform()) {
				if (with->vspeed > 0.0f) {
					y += 1.5f;
				}
				if (with->hspeed != 0.0f) {
					x += with->hspeed;
				}
				platformHs = with->hspeed;
				onPlatform = true;
			}
			else if (with->GetID() == EID_WOODENBOX || with->GetID() == EID_WOODENBOX_RESPAWN) {
				// we're on a box on a moving platform
				E_WoodenBox *wb = static_cast<E_WoodenBox*>(with);
				if (wb->onPlatform) {
					x += wb->platformH;
					platformHs = wb->platformH;

					if (wb->platformV > 0.0f)
						y += 1.5f;
					else if (wb->platformV < 0.0f)
						y -= 2.0f;
					onPlatform = true;
				}
			}
			else if (with->GetID() == EID_BARREL) {
				// we're on a box on a moving platform
				E_Barrel *wb = static_cast<E_Barrel*>(with);
				if (wb->onPlatform) {
					x += wb->platformH;
					platformHs = wb->platformH;

					if (wb->platformV > 0.0f)
						y += 1.5f;
					else if (wb->platformV < 0.0f)
						y -= 2.0f;
					onPlatform = true;
				}
			}
			vspeed = 0;
		}
		// block below with
		else if (!with->IsMovingPlatform() && vspeed < 0.0f && prevy + 5.0f >= with->y+with->colRect.GetH()) {
			y = with->colRect.GetY()+with->colRect.GetH() - 1.0f; // Varför -1? Bugfix: Hoppa och falla genom marken
			vspeed = 0;
		}
		
		//if ((!with->IsMovingPlatform() || !IsPlayer()) && (!with->IsPushable() || !IsPlayer()) )
		if (!IsPlayer() || (!with->IsMovingPlatform() && !with->IsPushable()))
		{
			// block left of with
			if (hspeed > 0.0f && prevx + colRect.GetW() - 5.0f <= with->x) {
				x = with->colRect.GetX() - colRect.GetW();
				hspeed = 0;
			}
			// block right of with
			else if (hspeed < 0.0f && prevx + 5.0f >= with->x+with->colRect.GetW()) {
				x = with->colRect.GetX()+with->colRect.GetW();
				hspeed = 0;
			}
		}
	}
	// ENTITY:COLLISION END

	if (with->IsEnemy() && with->GetID() == EID_LAZERBEAM) {
		Hurt(10.0f);
	}
	else if (with->IsPushable()) {
		E_WoodenBox *wb = 0;
		if (with->GetID() == EID_WOODENBOX || with->GetID() == EID_WOODENBOX_RESPAWN) {
			wb = static_cast<E_WoodenBox*>(with);
		}

		// PushFriction()

		if (wb) {
			if (y+colRect.GetH()-5.0f > wb->y) {
				//hspeed /= 5.0f;
				hspeed /= wb->Friction();
				wb->x += hspeed;
			}
		}
	}
}
