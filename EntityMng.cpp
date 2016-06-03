#include "EntityMng.h"
#include "Engine.h"
#include "CollisionObject.h"
#include "AllEntities.h"
#include <sstream>

extern Engine *eng;
extern bool lMapParameter;
extern bool lPosParameter;

/* pos param */
struct PlPosParam {
	float x;
	float y;

	PlPosParam(const char *str) {
		if (!str) {
			x = y = 0;
			return;
		}
		const char *p = strchr(str, ',');
		if (p) {
			std::stringstream	bx,by;
			bx.write(str, p-str);
			by.write(p, p-str);
			bx >> x;
			by >> y;
			return;
		}
		x = y = 0;
	}
};
extern PlPosParam initialPos;
/* pos param */

EntityMng::EntityMng() : timerSpawner(this)
{
	//prevUpdate = eng->time;
	prevUpdate	= 0;
	boss		= NULL;
	player		= NULL;
}

EntityMng::~EntityMng()
{
	/*
	std::list<Entity*>::iterator it,next;
	for (it = ents.begin(); it != ents.end(); it++)
		delete *it;
	ents.clear();
	*/
	Free();
}

void EntityMng::Free()
{
	std::list<Entity*>::iterator it,next;
	for (it = ents.begin(); it != ents.end(); it++)
		delete *it;
	ents.clear();

	player = 0;
	boss = 0;

	prevUpdate = 0;
}

Entity *EntityMng::CreateEntity(int id)
{
	// FIXME: Idea. For future stuff, couldn't this be solved with templates somehow? Alternatively, macros or dynamically
	Entity *e = 0;
	switch (id)
	{
	case EID_PLAYER:
		e = new E_Player;
		break;
	case EID_KITTY:
		e = new E_Kitty;
		break;
	case EID_SKJUTARN:
		e = new E_Skjutarn;
		break;
	case EID_KATTN:
		e = new E_Kattn;
		break;
	case EID_WOODENBOX:
		e = new E_WoodenBox(WOOD_NORMAL);
		break;
	case EID_WOODENBOX_RESPAWN:
		e = new E_WoodenBox(WOOD_RESPAWN);
		break;
	case EID_BARREL:
		e = new E_Barrel;
		break;
	case EID_CATFISH:
		e = new E_CatFish;
		break;
	case EID_BOSSTRIGGER:
		e = new E_BossTrigger;
		break;
	case EID_ENDMAP:
		e = new E_EndMap;
		break;

	case EID_MV_PLATFORM_U:
		e = new E_MvPlatform(PLDIR_UP);
		break;
	case EID_MV_PLATFORM_D:
		e = new E_MvPlatform(PLDIR_DOWN);
		break;
	case EID_MV_PLATFORM_L:
		e = new E_MvPlatform(PLDIR_LEFT);
		break;
	case EID_MV_PLATFORM_R:
		e = new E_MvPlatform(PLDIR_RIGHT);
		break;

	case EID_BOSS2TRIGGER:
		e = new E_Boss2Trigger;
		break;
	case EID_COPYCAT:
		e = new E_CopyCat;
		break;
	case EID_PIGGATTACK:
		e = new E_PiggAttack;
		break;
	case EID_ONEUP:
		e = new E_OneUp;
		break;
	case EID_PIGGFACTORY:
		e = new E_PiggFactory;
		break;
	case EID_BOSS2DOOR:
		e = new E_Boss2Door;
		break;

	case EID_LAZERBEAM:
		e = new E_LazerBeam;
		break;
	case EID_BOSSROCKET:
		e = new E_BossRocket;
		break;
	case EID_BOSSSURPRISE:
		e = new E_BossSurprise;
		break;
	case EID_BOSSOBJECT:
		e = new E_Boss;
		break;
	case EID_BOSS2OBJECT:
		e = new E_Boss2;
		break;
	case EID_BOSS2FIREBALL:
		e = new E_Boss2Fireball;
		break;
	case EID_BOSS2GRNDFIRE:
		e = new E_Boss2GrndFire;
		break;
	case EID_BOSS2SHIELD:
		e = new E_Boss2Shield;
		break;
	case EID_BOSS2SEQUENCER:
		e = new E_Boss2Sequencer;
		break;
	}

	if (e) {
		ents.push_back(e);
		if (e->IsPlayer())
			player = e;
	}
	return e;
}

void EntityMng::GenerateCollisions(Entity *e, SolidMap *tiles, std::list<Entity*>::iterator o)
{
	// 2009-04-25 (commented)
	//if (!e->spriteIndex)
	//	return;

	// collisions with the map
	if (e->spriteIndex) // 2009-04-25
		e->CheckTiles(tiles);

	// collisions with other entities
	std::list<Entity*>::iterator it;
	for (it = ents.begin(); it != ents.end(); it++)
	{
		Entity *other = *it;

		/*
		if (other != e && other->spriteIndex)
		{
			if (e->spriteIndex->Collision(other->spriteIndex))
				e->Collision(other);
		}
		*/

		// 2009-04-25
		if (other != e && other->colRect.GetW() && other->colRect.GetH())
		{
			if ((other->GetID() == EID_WOODENBOX || other->GetID() == EID_WOODENBOX_RESPAWN) ||
				other->IsMovingPlatform())
			{ // moving platforms defy gravity (not really)
				CollisionObject co(e->colRect.GetX(), e->colRect.GetY(), e->colRect.GetW(), e->colRect.GetH() + 5.0f); // 5.0f

				/*
				if (other->GetID() == EID_WOODENBOX || other->GetID() == EID_WOODENBOX_RESPAWN) {
					if (!static_cast<E_WoodenBox*>(other)->onPlatform)
						co.SetH(co.GetH() - 5.0f);
					else {
						if (static_cast<E_WoodenBox*>(other)->platformH <= 0.0f)
							co.SetH(co.GetH() - 5.0f);
					}
				}
				*/

				if (co.Test(other->colRect))
					e->Collision(other);
			}
			else if (e->colRect.Test(other->colRect))
				e->Collision(other);
		}
	}
#if 0
	// 2009-07-05
	// removed unnecessary loops - FIXME: problems due to expectations of order
	// But the savings seem neglegible anyhow
	for (; it != ents.end(); it++)
	{
		Entity *other = *it;

		if (other != e && other->colRect.GetW() && other->colRect.GetH())
		{
			if (other->IsMovingPlatform()) { // moving platforms defy gravity (not really)
				
				CollisionObject co(e->colRect.GetX(), e->colRect.GetY(), e->colRect.GetW(), e->colRect.GetH() + 5.0f);
				if (co.Test(other->colRect)) {
					other->Collision(e);
					e->Collision(other);
				}
			}
			else if (e->colRect.Test(other->colRect)) {
				other->Collision(e);
				e->Collision(other);
			}
		}
	}
#endif
}

bool EntityMng::Update(/*TileMng*/SolidMap *tiles)
{
	if (!prevUpdate)	// should say prevFrame!
		prevUpdate = eng->time;

	static int pp = 0;
	pp += eng->time - prevUpdate;

	bool ret = false;

	if (pp >= 16) {
		if (pp > 500) pp = 16; // freeze fix

		for (; pp >= 16; pp-=16)
		{
			// spawn timed entities
			timerSpawner.SpawnDelayedEntities();
			
			// update entities
			std::list<Entity*>::iterator it,next;
			for (it = ents.begin(); it != ents.end(); )
			{
				next = it;
				++next;
				
				Entity *e = *it;
				if (e->destroy) {
					delete e;
					ents.erase(it);
				}
				else {
					e->Step();
					//e->UpdateCollisionRect();
					//GenerateCollisions(e, tiles); // generate collision events with others
					GenerateCollisions(e, tiles, next);
					e->PostStep();
				}
				it = next;
			}
		}
		ret = true;
	}
	prevUpdate = eng->time;
	return ret;
}

void EntityMng::Draw()
{
	std::list<Entity*>::iterator it;
	for (it = ents.begin(); it != ents.end(); it++)
	{
		Entity *e = *it;
		e->Draw();
	}
}

void EntityMng::Serialize(Archiver &arc)
{
	std::list<Entity*>::iterator it;

	// save/load the entity map (general information)
	if (arc.Writable())
	{
		arc << (int)ents.size();
		for (it = ents.begin(); it != ents.end(); it++)
		{
			arc << (*it)->GetID();
			arc << (*it)->x << (*it)->y; // note: float!
		}
	}
	else {
		Free();

		int num;
		arc >> num;
		for (int i=0; i<num; i++)
		{
			int id;
			float x,y; // note: float!
			arc >> id >> x >> y;

			// spawn instance
			Entity *e = CreateEntity(id);
			if (e) {
				e->x = x;
				e->y = y;
			}
		}
	}

	// serialize specific information (should be in the same order)
	for (it = ents.begin(); it != ents.end(); it++)
	{
		// save specific info
		(*it)->Serialize(arc);
	}
}

void EntityMng::FromFile(FILE *fp, int len, bool versionFour, bool versionFive)
{
	Free();

	Entity *xplayer = 0;
	float movePlayerX = -1.0f;
	float movePlayerY = -1.0f;

	if (versionFive) versionFour = true;

	int ents;
	if (versionFive)
		ents = len/30;
	else if (versionFour)
		ents = len/26;
	else
		ents = len/16;
	for (int i=0; i<ents; i++)
	{
		int id;
		int x,y;
		int timer;
		fread(&id, sizeof(int), 1, fp);
		fread(&x, sizeof(int), 1, fp);
		fread(&y, sizeof(int), 1, fp);
		fread(&timer, sizeof(int), 1, fp);

		int lifeSpan, activeRadius;
		bool manualActivation, initiallyActive;
		if (versionFour) {
			fread(&lifeSpan, sizeof(lifeSpan), 1, fp);
			fread(&activeRadius, sizeof(activeRadius), 1, fp);
			manualActivation = fgetc(fp);
			initiallyActive = fgetc(fp);
		}

		int piggSpeed;
		if (versionFive)
			fread(&piggSpeed, sizeof(piggSpeed), 1, fp);

		if (id == EID_DUMMYPLAYER) {
			movePlayerX = (float)x;
			movePlayerY = (float)y;
		} else {
			if (timer <= 0) {
				Entity *e = CreateEntity(id);
				if (e)
				{
					e->x = (float)x;
					e->y = (float)y;

					if (id == EID_PLAYER)
						xplayer = e;

					if (versionFour)
						e->IEditorSettings(lifeSpan, activeRadius, manualActivation, initiallyActive);
					if (versionFive && id == EID_PIGGATTACK)
						static_cast<E_PiggAttack*>(e)->SetSpeed((float)piggSpeed);
				}
			}
			else {
				// hack: use delay as timer for moving platforms
				if (id == EID_MV_PLATFORM_U ||
					id == EID_MV_PLATFORM_D ||
					id == EID_MV_PLATFORM_L ||
					id == EID_MV_PLATFORM_R)
				{
					Entity *e = CreateEntity(id);
					if (e) {
						e->x = (float)x;
						e->y = (float)y;
						static_cast<E_MvPlatform*>(e)->mTimerDelay = timer;
						static_cast<E_MvPlatform*>(e)->ResetTimer();

						if (versionFour)
							e->IEditorSettings(lifeSpan, activeRadius, manualActivation, initiallyActive);
					}
				}
				// hack 2: use delay (& life as lifespan for piggs) as spawn interval for pigg factory
				else if (id == EID_PIGGFACTORY) {
					Entity *e = CreateEntity(id);
					if (e) {
						e->x = (float)x;
						e->y = (float)y;
						static_cast<E_PiggFactory*>(e)->mPiggSpawnInterval = timer;
						static_cast<E_PiggFactory*>(e)->ResetTimer();

						if (versionFour)
							e->IEditorSettings(lifeSpan, activeRadius, manualActivation, initiallyActive);
						if (versionFive)
							static_cast<E_PiggFactory*>(e)->mPiggSpeed = (float)piggSpeed;
					}
				}
				else {
					// delay spawning of instance
					TimerSpawner::einfo ei;
					ei.id		= id;
					ei.frames	= timer;
					ei.x		= x;
					ei.y		= y;
					
					ei.versionFour = versionFour;
					if (versionFour) {
						ei.lifeSpan = lifeSpan;
						ei.activeRadius = activeRadius;
						ei.manualActivation = manualActivation;
						ei.initiallyActive = initiallyActive;
						ei.piggSpeed = 16;
					}
					if (versionFive)
						ei.piggSpeed = piggSpeed;
					
					timerSpawner.m.push_back(ei);
				}
			}
		}
	}
	if (lPosParameter) {
		if (!xplayer)// spawn
			xplayer = CreateEntity(EID_PLAYER);
		xplayer->x = initialPos.x;
		xplayer->y = initialPos.y;
		lPosParameter = false;
	}
}

Entity *EntityMng::FindInstanceById(Entity *pos, int id)
{
	std::list<Entity*>::iterator it = ents.begin();

	// TODO: make this faster
	if (pos) {
		for (; it != ents.end(); it++) {
			if ((*it) == pos) {
				it++;
				break;
			}
		}
		if (it == ents.end())
			return NULL;
	}

	// find instance
	for (; it != ents.end(); it++) {
		if (id == -1 || id == (*it)->GetID()) {
			return *it;
		}
	}
	return NULL;
}

Entity *EntityMng::FindClosestInstance(float x, float y, float maxDist, int id) {
	std::list<Entity*>::iterator it;
	for (it = ents.begin(); it != ents.end(); it++)
	{
		Entity *e = *it;

		if (id == -1 || id == e->GetID()) {
			float xx = x - e->x;
			float yy = y - e->y;

			if (id == EID_PLAYER)
				printf("dwzzjiowd: %f %f\n", xx, yy);
		
			//if (sqrt(xx*xx + yy*yy) <= maxDist)
			if (xx*xx + yy*yy <= maxDist*maxDist)
				return e;
		}
	}
	return 0;
}

#if 0
Entity *EntityMng::CollisionAtPos(Entity *me, float x, float y)
{
}
#endif

//
//	timer spawner
//
void TimerSpawner::SpawnDelayedEntities()
{
	std::list< einfo >::iterator it = m.begin();
	for (; it != m.end(); ) {
		if (--it->frames <= 0) {
			// spawn instance
			Entity *e = entMng->CreateEntity(it->id);
			if (e) {
				e->x = it->x;
				e->y = it->y;
				if (it->versionFour)
					e->IEditorSettings(it->lifeSpan, it->activeRadius, it->manualActivation, it->initiallyActive);
				if (e->GetID() == EID_PIGGATTACK)
				{
					static_cast<E_PiggAttack*>(e)->SetSpeed((float)it->piggSpeed);
				}
				else if (e->GetID() == EID_PIGGFACTORY) {
					static_cast<E_PiggFactory*>(e)->mPiggSpeed = (float)it->piggSpeed;
				}
			}
			it = m.erase(it);
		}
		else
			++it;
	}
}
