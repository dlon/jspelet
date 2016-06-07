// todo: explosion sound
// todo: uber-damage sound

#include "AllEntities.h"
#include "Engine.h"
#include "Jack.h"
#include "Input.h"

extern Input *input;
extern Engine *eng;

// emitter templates
static EmitterProperties rocketP = {
	0.0f, 0.0f, // distribution
	5.0f, 5.0f, // velocity
	0.0f, 360.0f, // direction
	1.0f, 0.2f, 0.001f, 1.0f, // color
	1.0f, 0.2f, 0.001f, 1.0f,
	1.0f, 1.5f, // lifespan
	1.0f, 1.5f, // scale
	false, // tail
	50, // rate
	1,
	false,
	0, // texture: set later
	EMP_NONE,
	0.0f, 0.0f // gravity
};
static EmitterProperties rocketPIdle = { // idle
	0.0f, 0.0f, // distribution
	1.2f, 1.2f, // velocity
	180.0f, 360.0f, // direction
	1.0f, 0.2f, 0.001f, 1.0f, // color
	1.0f, 0.2f, 0.001f, 1.0f,
	2.0f, 2.5f, // lifespan
	0.1f, 0.5f, // scale
	false, // tail
	50, // rate
	1,
	false,
	0, // texture: set later
	EMP_NONE,
	0.0f, 0.0f // gravity
};
static EmitterProperties smokeP = {
	0.0f, 0.0f, // distribution
	3.0f, 3.0f, // velocity
	0.0f, 360.0f, // direction
	0.1f, 0.1f, 0.1f, 0.9f, // color
	0.1f, 0.1f, 0.1f, 0.9f,
	2.0f, 2.0f, // lifespan
	2.0f, 2.0f, // scale
	false, // tail
	30, // rate
	0,
	false,
	0, // texture: set later
	EMP_NONE,
	0.0f, 0.0f // gravity
};
static EmitterProperties smokePIdle = {
	0.0f, 0.0f, // distribution
	1.0f, 1.0f, // velocity
	180.0f, 360.0f, // direction
	0.2f, 0.2f, 0.2f, 0.1f, // color
	0.2f, 0.2f, 0.2f, 0.1f,
	2.0f, 2.0f, // lifespan
	0.6f, 0.6f, // scale
	false, // tail
	40, // rate 60
	0,
	false,
	0, // texture: set later
	EMP_NONE,
	0.0f, 0.0f // gravity
};
static EmitterProperties bloodP = {
	10.0f, 3.0f, // distribution
	6.0f, 9.0f, // velocity ( 4
	250.0f, 290.0f, // direction
	0.9f, 0.1f, 0.1f, 0.8f, // color
	0.9f, 0.1f, 0.1f, 0.95f,
	3.0f, 4.0f, // lifespan
	0.4f, 0.5f, // scale
	false, // tail
	60, // rate
	0,
	false,
	0, // texture: set later
	EMP_NONE,
	0.25f, 0.3f // gravity
};

// barrel

E_Barrel::E_Barrel()
{
	sprite.Load("data/kg.PNG", 2, 64,64);
	spriteIndex = &sprite;
	spriteIndex->imgSpeed = 0;

	colRect.SetW(39.0f);
	colRect.SetH(63.0f);

	deactivated = pl = false;
	solid = true;
	em1 = em2 = em3 = em4 = curSmoke = curFire = em5 = 0;

	dmg = 5.0f;

	SetTimer(2, 180);

	player = 0;

	sndFire = Sound(*eng->res->sounds.Load("data/fire2.wav"));
	sndExplo = Sound(*eng->res->sounds.Load("data/adl.wav"));

	onPlatform = false;
	platformV = platformH = 0;
}

E_Barrel::~E_Barrel()
{
	//delete em1;
	//delete em2;
}

void E_Barrel::CallTimer(int num)
{
	if (num == 0) {
		// find or create emitters
		em1 = eng->jack->map.partMng.GetEmitter("barrelFire");
		if (!em1)
			em1 = eng->jack->map.partMng.NewEmitter("barrelFire", 50, x, y, 50, &rocketP);
		em2 = eng->jack->map.partMng.GetEmitter("barrelSmoke");
		if (!em2)
			em2 = eng->jack->map.partMng.NewEmitter("barrelSmoke", 49, x, y, 80, &smokeP);
		em3 = eng->jack->map.partMng.GetEmitter("barrelFireIdle");
		if (!em3)
			em3 = eng->jack->map.partMng.NewEmitter("barrelFireIdle", 50, x, y, 50, &rocketPIdle);
		em4 = eng->jack->map.partMng.GetEmitter("barrelSmokeIdle");
		if (!em4)
			em4 = eng->jack->map.partMng.NewEmitter("barrelSmokeIdle", 49, x, y, 80, &smokePIdle);

		// set texture
		sf::Texture *t1 = eng->res->textures.GetByFile("data/fire.tga");
		sf::Texture *t2 = eng->res->textures.GetByFile("data/smoke.tga");

		em1->SetTexture(t1);
		em2->SetTexture(t2);
		em3->SetTexture(t1);
		em4->SetTexture(t2);
		
		eng->jack->chaseCam.Explode();

		curFire = em1;
		curSmoke = em2;

		//dmg = 50.0f;

		SetTimer(1, 20); // set exploded state
		SetTimer(2, 20); // Find player: needed for fire sound

		// FIXME: Shouldn't this be called *immediately* from the collision event?
	}
	else if (num == 1) {
		spriteIndex->imgInd = 1.0f;
		curFire = em3;
		curSmoke = em4;
		colRect.SetH(10.0f);

		dmg = 5.0f;

		solid = false;
	}
	else if (num == 2) {
		// deactivate particles if player is out of reach
		if ( (player=eng->jack->map.GetEntities().FindClosestInstance(x, y, 800.0f, EID_PLAYER)) == NULL) {
			deactivated = true;
		}
		else
			deactivated = false;
		SetTimer(2, 60); // FIXME: optimal value?
	}
	else if (num == 10) {
		em5 = 0; // reset blood
	}
}

void E_Barrel::Step()
{
	Entity::Step();

	onPlatform = false;

	// blood experiment
	if (em5) {
		em5->Emit();
		em5->Emit();
	}

	if (curSmoke && curFire) {
		if (!deactivated) {
			curSmoke->x = x+19.5f;
			curSmoke->y = y;
			curFire->x = x+19.5f;
			curFire->y = y;

			curSmoke->Emit();
			curFire->Emit();
		}

		// FIXME-SFML: add back panning based on player position
		if (deactivated || !player)
			sndFire.stop();
	}
}

void E_Barrel::Collision(Entity *with)
{
	Entity::Collision(with); // FIXME: Test

	if (pl) {
		if (with->Life() > 0.0f) {
			// burn 'em
			// TODO: bigger range for massive damage - gotta use distance find function
			with->Hurt(dmg); // FIXME: ?

			if (dmg == 50.0f && !em5 && with->Life() <= 0.0f) {
				em5 = eng->jack->map.partMng.GetEmitter("testBlood");
				if (!em5)
					em5 = eng->jack->map.partMng.NewEmitter("testBlood", -5, x, y, 150, &bloodP);
				if (!em5->GetTexture()) {
					//Texture *t = eng->res->textures.LoadEx("data/part3.tga", TextureLoader::Swap2Channels); // FIXME (note): on the fly load
					sf::Texture *t = eng->res->textures.Load("data/part3.tga"); // FIXME: is this freed?
					em5->SetTexture(t);
				}
				em5->x = with->x + with->colRect.GetW() / 2.0f;
				em5->y = with->y + with->colRect.GetH();
				SetTimer(10, 30);
			}
		}

		return;
	}

	if (!with->IsEnemy() && with->GetID() == EID_LAZERBEAM) {
		pl = true;

		Entity *e = eng->jack->map.GetEntities().FindInstanceById(NULL, EID_PLAYER);
		if (e)
			sndExplo.play(); // FIXME-SFML: set panning based on Player position
		sndFire.setLoop(true); // FIXME-SFML: set and update panning based on Player position
		sndFire.play();

		dmg = 50.0f;
		SetTimer(0, 1);
	}
}
