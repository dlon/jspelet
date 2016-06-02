#pragma once
#include "Entity.h"

enum PLATFORM_DIR
{
	PLDIR_UP,
	PLDIR_LEFT,
	PLDIR_RIGHT,
	PLDIR_DOWN,
};

class E_MvPlatform : public Entity
{
public:
	int		mTimerDelay;

	E_MvPlatform(PLATFORM_DIR dir);

	int GetID() {
		switch (dir)
		{
		case PLDIR_DOWN: return EID_MV_PLATFORM_D;
		case PLDIR_LEFT: return EID_MV_PLATFORM_L;
		case PLDIR_RIGHT: return EID_MV_PLATFORM_R;
		}
		return EID_MV_PLATFORM_U;
	}
	bool		IsSolid()					{ return true; }
	bool		IsMovingPlatform()			{ return true; }
	void		SetDir(PLATFORM_DIR dir);
	void		CallTimer(int num);
	void		CheckTiles(SolidMap *tileMng);
	void		ResetTimer();

	void IEditorSettings(int lifeSpan,
		int activeRadius,
		bool manualActivation,
		bool initiallyActive);

	Sprite			spr;
	PLATFORM_DIR	dir;
};
