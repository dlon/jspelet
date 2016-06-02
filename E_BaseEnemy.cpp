// TODO: On moving platforms, activate gravity?

#include "E_BaseEnemy.h"
#include "Engine.h"
#include "Jack.h"

#define HFLASH_DUR 800

extern Engine *eng;

Sound E_BaseEnemy::sndSpltr;

E_BaseEnemy::E_BaseEnemy(float maxHealth) : active(false) {
	health		= maxHealth;
	hurtTimer	= eng->time;
	blendMp		= 1.0f;
	player		= NULL;
	playerDamage= 10.0f;
	//activRadius	= 550.0f;
	activRadius	= 800.0f;
	manualActivation = false;
	lifeSpan	= -1;

	if (!sndSpltr.getBuffer())
		sndSpltr = sf::Sound(*eng->res->sounds.Load("data/spltr.wav"));
}
void E_BaseEnemy::PostCreate()
{
}

void E_BaseEnemy::IEditorSettings(int lifeSpan,
		int activeRadius,
		bool manualActivation,
		bool initiallyActive)
{
	this->lifeSpan = lifeSpan;
	this->activRadius = activeRadius;
	this->manualActivation = manualActivation;
	this->active = initiallyActive;
}

/*
void E_BaseEnemy::CallTimer(int num)
{
}
*/
void E_BaseEnemy::Step() {
	if (active) // NOTE: This is just as long as we're only updating timers in this event
		Entity::Step();

	if (!player)
		player = static_cast<E_Player*>( eng->jack->map.GetEntities().FindInstanceById(NULL, EID_PLAYER) );

	if (active) {
		if (hurtTimer > eng->time) {
			if ((hurtTimer-eng->time) % 400 < 200) {
				blendMp = 1.0f - float(hurtTimer-eng->time)/200.0f;
			}
			else {
				blendMp = float(hurtTimer-eng->time-200)/200.0f;
			}
		}
		else
			blendMp = 1.0f;

		if (health <= 0.0f)
			destroy = true;
		
		// NOTE: Lifespan decrease only when active
		if (lifeSpan > -1) {
			if (!lifeSpan--)
				destroy = true;
		}

		// deactivate if we're outside of the area again
		if (!manualActivation) {
			if (player) {
				float xx = x-player->x;
				float yy = y-player->y;
				if (xx*xx + yy*yy > activRadius*activRadius)
					active = false;
			}
			else
				active = false;
		}
	}
	else if (!manualActivation) {
		// activate within range
		if (player) {
			float xx = x-player->x;
			float yy = y-player->y;
			if (xx*xx + yy*yy <= activRadius*activRadius) {
				active = true;
			}
		}
	}
}

void E_BaseEnemy::Draw() {
	eng->render->SetBlendMode(RBLEND_NORMAL, true);
	eng->render->SetColor(255,(int)(255.0f*blendMp),(int)(255.0f*blendMp));

	Entity::Draw();
	eng->render->SetBlendMode(RBLEND_NORMAL);
}

void E_BaseEnemy::Hurt(float hp) {
	if (hurtTimer - 700 < eng->time) // -x?
	{
		hurtTimer = eng->time + HFLASH_DUR;
		health -= hp;
	}
}

void E_BaseEnemy::Collision(Entity *with)
{
	Entity::Collision(with);

	/*
	// FIXME: Bug - unactivated objects are all activated!
	if (!active && with->IsMovingPlatform()) {
		// FIXME: Moving platform!
		if (colRect.GetX() == 0.0f && colRect.GetY() == 0.0f) {
			colRect.SetX(x); // initial pos: accuracy not extremely important but nevertheless we need one
			colRect.SetY(y);
			return;
		}

		manualActivation = true;
		active = true;
		if (!player)
			player = static_cast<E_Player*>( eng->jack->map.GetEntities().FindInstanceById(NULL, EID_PLAYER) );

		if (GetID() == EID_KITTY) {
			printf("erwesio: %f,%f\n", colRect.GetX(), colRect.GetY());
		}
	}
	*/

	/*
	if (!active && with->IsMovingPlatform()) {
		// FIXME: untested (funkar ej)
		vspeed = 0.1f;
		colRect.SetX(x);
		colRect.SetY(y);
	}
	Entity::Collision(with);
	if (!active && with->IsMovingPlatform())
		vspeed = 0;
	*/

	if (!active)
		return;

	if (with->GetID() == EID_LAZERBEAM) {
		if (!with->IsEnemy())
			Hurt(10);
	}
	else if (with->GetID() == EID_PLAYER) {
		with->Hurt(playerDamage);
	}
	else if (with->GetID() == EID_WOODENBOX) {
		// splatter
		if (with->vspeed > 0.0f) {
			if (with->y + with->colRect.GetH() < y + colRect.GetH() /*&&
				x < with->x+with->colRect.GetW() && x+colRect.GetW() >= with->x - always true*/) {
				// we're beneath it
				sndSpltr.play();
				destroy = true;
			}
		}
	}
}

void E_BaseEnemy::PostStep() {
	if (active)
		Entity::PostStep();
	else {
		colRect.SetX(x); // FIXME: is this sufficient for finding initial collisions (e.g. moving platforms)?
		colRect.SetY(y); // FIXME: bug: activates all objects immediately!
	}
}

void E_BaseEnemy::CheckTiles(SolidMap *solidMap) {
	if (active) {
		Entity::CheckTiles(solidMap);
	}
}
