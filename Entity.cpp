#include "Entity.h"
#include "Engine.h"
#include "CollisionObject.h"
#include <stdio.h>
//#include "TileMng.h"
#include "SolidMap.h"
#include <assert.h>
#include "E_WoodenBox.h"
#include "E_Barrel.h"

#include "prof.h"

extern Engine *eng;

#define DRAW_COLRECTS

Entity::Entity() :
	x(0), y(0),
	inWater(false),
	//pullOutOfWater(false),
	waterBlockTimer(0),
	prevx(0), prevy(0),
	prevHspeed(0), prevVspeed(0),
	gravity(0.6f), 
	hspeed(0), vspeed(0), 
	destroy(false), 
	spriteIndex(0), 
	fallThroughTile(0)
{
	memset(timers, 0, sizeof(timers)); // reset all timers
}

void Entity::Step()
{
	// update timers
	for (int i=0; i<sizeof(timers)/sizeof(etimer_t); i++) {
		if (timers[i] >= 0) {
			if (--timers[i] == 0)
				CallTimer(i);
		}
	}
}

void Entity::CheckTiles(SolidMap *tileMng)
{
	Prof(Entities_CheckTiles);

	CollisionObject actualPos(x,y,colRect.GetW(),colRect.GetH());

	if (vspeed+gravity < GRAVITY_LIMIT)
		vspeed += gravity;
	
	colRect.SetY(y+vspeed);
	colRect.SetX(x+hspeed);

	bool firstYTest = false;
	bool firstWasSouth = false;

	// north/south + slope
	if (int f=tileMng->Collision(colRect, actualPos, TFLAG_BLOCK_S|TFLAG_BLOCK_N|TFLAG_IMPENETRABLE)) // has to be checked!
	{
		if (f & TFLAG_BLOCK_S)
		{
			firstYTest = true; // do nothing and stop vspeed?
			firstWasSouth = true;
		}
		else if ( !fallThroughTile || (f & TFLAG_IMPENETRABLE) )
		{
			CollisionObject screwUpFix(colRect.GetX()-hspeed, colRect.GetY(), colRect.GetW(), colRect.GetH());
			
			if (tileMng->Collision(screwUpFix, actualPos, TFLAG_BLOCK_N)) // wall fix (might've introduced a floor bug!)
			{
				do {
					y = colRect.SetY(colRect.GetY() - 0.1f);
				} while (tileMng->Collision(colRect, actualPos, TFLAG_BLOCK_N));
				y = colRect.SetY(ceilf(colRect.GetY())); // FIXME
			}

			firstYTest = true;
			actualPos.SetY(y);
		}
	}

	// walking down a slope?
#if 0
	if (hspeed > 0.0f) {
		float yBack = colRect.GetY();
		colRect.SetY(colRect.GetY() + 5.0f);
		int n=tileMng->Collision(colRect, actualPos, TFLAG_SLOPE_E|TFLAG_SLOPE_HE | TFLAG_BLOCK_N);
		if (!(n & (TFLAG_SLOPE_E|TFLAG_SLOPE_HE)))
			colRect.SetY(yBack);
	}
	else if (hspeed < 0.0f) {
		float yBack = colRect.GetY();
		colRect.SetY(colRect.GetY() + 5.0f);
		int n=tileMng->Collision(colRect, actualPos, TFLAG_SLOPE_W|TFLAG_SLOPE_HW | TFLAG_BLOCK_N);
		if (!(n & (TFLAG_SLOPE_W|TFLAG_SLOPE_HW)))
			colRect.SetY(yBack);
	}
#endif
	// slope fix 2009-08-18
	if (hspeed > 0.0f) {
		float yBack = colRect.GetY();
		colRect.SetY(colRect.GetY() + 16.0f);
		int n=tileMng->Collision(colRect, actualPos, TFLAG_SLOPE_E|TFLAG_SLOPE_HE /*| TFLAG_BLOCK_N*/);
		if (!(n & (TFLAG_SLOPE_E|TFLAG_SLOPE_HE)))
			colRect.SetY(yBack);
	}
	else if (hspeed < 0.0f) {
		float yBack = colRect.GetY();
		colRect.SetY(colRect.GetY() + 16.0f);
		int n=tileMng->Collision(colRect, actualPos, TFLAG_SLOPE_W|TFLAG_SLOPE_HW /*| TFLAG_BLOCK_N*/);
		if (!(n & (TFLAG_SLOPE_W|TFLAG_SLOPE_HW)))
			colRect.SetY(yBack);
	}

	// slope test
	if (int f=tileMng->Collision(colRect, actualPos, TFLAG_SLOPE_E|TFLAG_SLOPE_W|TFLAG_SLOPE_HE|TFLAG_SLOPE_HW))
	{
		do {
			y = colRect.SetY(colRect.GetY() - 0.1f);
		} while (tileMng->Collision(colRect, actualPos, TFLAG_SLOPE_E|TFLAG_SLOPE_W|TFLAG_SLOPE_HE|TFLAG_SLOPE_HW));

		/*
		// v 07-18: _HW & _HE fix: going down too steep in the ground
		if (colRect.GetY() > actualPos.GetY()) {
			while (tileMng->Collision(colRect, actualPos, TFLAG_BLOCK_N)) {
				y = colRect.SetY(colRect.GetY() - 0.1f);
			}
		}
		// ^ 07-18: _HW & _HE fix: going down too steep in the ground
		*/
		// 08-18:	do the same but only on the last (lower-most) block
		float xBack = colRect.GetX();
		if ( (f & (TFLAG_SLOPE_W|TFLAG_SLOPE_HW)) != 0)
			colRect.SetX( colRect.GetX() + 32.0f );
		else if ( (f & (TFLAG_SLOPE_E|TFLAG_SLOPE_HE)) != 0)
			colRect.SetX( colRect.GetX() - 32.0f );
		if (tileMng->Collision(colRect, actualPos, TFLAG_SLOPE_E|TFLAG_SLOPE_W|TFLAG_SLOPE_HE|TFLAG_SLOPE_HW)) {
			colRect.SetX(xBack);
			while (tileMng->Collision(colRect, actualPos, TFLAG_BLOCK_N)) {
				y = colRect.SetY(colRect.GetY() - 0.1f);
			}
		}
		else
			colRect.SetX(xBack);

		firstYTest = true;
		actualPos.SetY(y);
	}

	// east, west
	int oFlags = TFLAG_BLOCK_W|TFLAG_BLOCK_E;

	if (int f=tileMng->Collision(colRect, actualPos, oFlags))
	{
		if (f & TFLAG_BLOCK_W)
		{
			hspeed = 0;
			do {
				x = colRect.SetX(colRect.GetX() - 0.1f);
			} while (tileMng->Collision(colRect, actualPos, TFLAG_BLOCK_W));
		}
		else if (f & TFLAG_BLOCK_E) {
			hspeed = 0;
			do {
				x = colRect.SetX(colRect.GetX() + 0.1f);
			} while (tileMng->Collision(colRect, actualPos, TFLAG_BLOCK_E));
		}
	}

	// we have to put an additional y-test here...
	// that's because the x-coordinate is not verified in the first test (likely to change in the following test).
	actualPos.SetX(x);
	colRect.SetY(y+vspeed);
	if (firstYTest && tileMng->Collision(colRect, actualPos, TFLAG_BLOCK_N|TFLAG_BLOCK_S | TFLAG_SLOPE_E|TFLAG_SLOPE_W|TFLAG_SLOPE_HE|TFLAG_SLOPE_HW)) {
		if (!firstWasSouth)
			vspeed = 0;
		else
			vspeed = 0.1f; // to prevent 0 in air
	}

	// water
	int t;
	if ((t=tileMng->Collision(colRect, actualPos, TFLAG_WATER) || inWater))
	{
		bool setInWater = true;
		
		float yBack = colRect.GetY();
		//float hBack = colRect.GetH();

		//colRect.SetH(colRect.GetH() - 20.0f);

		if (t && vspeed > 0.0f) {
			colRect.SetY(colRect.GetY() + colRect.GetH());
			if (tileMng->Collision(colRect, actualPos, TFLAG_WATER))
				//vspeed = 1.5f; // sink
				vspeed = 1.0f; // sink
			else
				vspeed = 0;
		}
		else if (vspeed <0.0f) {
			CollisionObject ca(actualPos);
			float bakVspeed = vspeed;
			
			ca.SetY(ca.GetY() - 16.0f);
			if (tileMng->Collision(ca, actualPos, TFLAG_WATER))
				vspeed *= 0.8f; // should be safe

			// TODO: improve jump-out-of-water function if desired
			if (!waterBlockTimer || waterBlockTimer <= eng->time) {
				waterBlockTimer = 0;

				// TODO: freeze-bug fix
				CollisionObject ffBug(colRect.GetX() + hspeed, colRect.GetY(),
					colRect.GetW(), colRect.GetH());
				if (!tileMng->Collision(ffBug, actualPos, TFLAG_WATER)) {
					vspeed = bakVspeed;
					setInWater = false;
					waterBlockTimer = 0;
					puts("Water Freeze Bug");
				}
				// water emerge timer
				else {
					colRect.SetY(colRect.GetY() + vspeed);
					while (!tileMng->Collision(colRect, actualPos, TFLAG_WATER)) {
						y = yBack = colRect.SetY(colRect.GetY() + 0.1f);
						vspeed = 0;
						waterBlockTimer = eng->time + 1000;
					}
				}
			}
			else {
				vspeed = bakVspeed;
				waterBlockTimer = 0;
				setInWater = false;
			}
		}

		colRect.SetY(yBack);
		//colRect.GetH() = hBack;
		inWater = setInWater;
	}
	else {
		inWater = false;
	}
}

void Entity::PostStep()
{
	x += hspeed;
	y += vspeed;

	if (spriteIndex)
		spriteIndex->Update();

	prevx = x;
	prevy = y;
	prevHspeed = hspeed;
	prevVspeed = vspeed;
}

void Entity::Draw()
{
#if !defined(NDEBUG) && defined(DRAW_COLRECTS)
	col4_t cbak;
	eng->render->GetColor4c(&cbak);
	eng->render->SetColor(0,0,255,150);
	eng->render->DrawRectW(colRect.GetXnO(),colRect.GetYnO(),colRect.GetW(),colRect.GetH());
	eng->render->SetColor4c(cbak);
#endif
	Prof(Entity_Draw);

	if (spriteIndex)
	{
		spriteIndex->x = x;
		spriteIndex->y = y;
		spriteIndex->Draw();
	}
}

void Entity::Serialize(Archiver &arc)
{
}

void Entity::Collision(Entity *with)
{
	if (IsMovingPlatform())
		return;

	if (with->IsSolid()) {
		// block above with
		if (vspeed > 0.0f && prevy + colRect.GetH() - 5.0f <= with->y) { // fix here         bckp: vspeed > 0.0f
			y = with->colRect.GetY() - colRect.GetH();

			if (with->IsMovingPlatform()) {
				if (with->vspeed > 0.0f)
					y += 1.5f;
				if (with->hspeed != 0.0f)
					x += with->hspeed;

				// wooden box ON a platform
				if (GetID() == EID_WOODENBOX || GetID() == EID_WOODENBOX_RESPAWN)
				{
					static_cast<E_WoodenBox*>(this)->onPlatform = true;
					static_cast<E_WoodenBox*>(this)->platformH = with->hspeed;
					static_cast<E_WoodenBox*>(this)->platformV = with->vspeed;
				}
				else if (GetID() == EID_BARREL)
				{
					static_cast<E_Barrel*>(this)->onPlatform = true;
					static_cast<E_Barrel*>(this)->platformH = with->hspeed;
					static_cast<E_Barrel*>(this)->platformV = with->vspeed;
				}
			}
			else if (with->GetID() == EID_WOODENBOX || with->GetID() == EID_WOODENBOX_RESPAWN) {
				// we're on a box on a moving platform
				E_WoodenBox *wb = static_cast<E_WoodenBox*>(with);
				if (wb->onPlatform) {
					x += wb->platformH;
					if (wb->platformV > 0.0f)
						y += 1.5f;
					else if (wb->platformV < 0.0f)
						y -= 2.0f;
				}
			}
			else if (with->GetID() == EID_BARREL) {
				// we're on a box on a moving platform
				E_Barrel *wb = static_cast<E_Barrel*>(with);
				if (wb->onPlatform) {
					x += wb->platformH;
					if (wb->platformV > 0.0f)
						y += 1.5f;
					else if (wb->platformV < 0.0f)
						y -= 2.0f;
				}
			}
			vspeed = 0;
		}
		// block below with
		else if (!with->IsMovingPlatform() && vspeed < 0.0f && prevy + 5.0f >= with->y+with->colRect.GetH()) {
			y = with->colRect.GetY()+with->colRect.GetH(); // FIXME: sync with player code
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
}

void Entity::SetTimer(int num, int frames)
{
	assert(num >= 0);
	assert(num < MAX_TIMERS);

	timers[num] = frames;
}

int Entity::GetTimer(int num)
{
	assert(num >= 0);
	assert(num < MAX_TIMERS);

	return timers[num];
}
