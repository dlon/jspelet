#include "E_EndMap.h"
#include "Engine.h"
#include "Jack.h"

extern Engine *eng;

extern int tticks;
extern bool allowEnterSplash;
extern std::string endingSplash;
extern std::string songAfterSplash;

E_EndMap::E_EndMap()
{
	spr.Load("data/handborjare.png", 6, 94,0);
	spr.imgSpeed = 0.25f;

	gravity = 0;

	spriteIndex = &spr;

	colRect.SetW(100.0f);
	colRect.SetH(68.0f);
}

void E_EndMap::Collision(Entity *with)
{
	if (destroy || !spriteIndex)
		return;

	if (with->IsPlayer()) {
		spriteIndex = 0;
		destroy = true;
		
		// find next map
		const char *cm = eng->jack->map.MapFile();
		const char *next = NULL;

		if (!strcmp(cm, "data/Lvl1.jkm"))
			next = "data/Lvl2.jkm";
		else if (!strcmp(cm, "data/Lvl2.jkm"))
			next = "data/Lvl3.jkm";
		else if (!strcmp(cm, "data/Lvl3.jkm"))
			next = "data/Lvl4.jkm";
		else if (!strcmp(cm, "data/Lvl4.jkm"))
			next = "data/Lvl5.jkm";
		else if (!strcmp(cm, "data/Lvl5.jkm"))
			next = "data/Lvl6.jkm";
		else if (!strcmp(cm, "data/Lvl6.jkm"))
			next = "data/Lvl7.jkm";
		else if (!strcmp(cm, "data/Lvl7.jkm"))
			next = "data/Lvl9.jkm";

		/*
		else if (!strcmp(cm, "data/Lvl7.jkm"))
			next = "data/Lvl8.jkm";
		else if (!strcmp(cm, "data/Lvl8.jkm"))
			next = "data/Lvl9.jkm";
		*/

		if (strcmp(cm, "data/Lvl9.jkm")) {
			// transition, etc
			if (next)
				eng->jack->EndLevel(next);
		}
		else {
			// the end
			// TODO: slower transition + music transition
			//eng->jack->map.Free();
			tticks = 1200;
			allowEnterSplash = false;
			
			endingSplash = "data/cred.PNG";
			//songAfterSplash = "data/credits.ogg";
			eng->SetSplash("data/basdas.PNG");
			
			eng->jack->SetSongOnce("data/creds.ogg");
		}
	}
}
