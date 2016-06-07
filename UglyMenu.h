#pragma once

#include <vector>
#include "Engine.h"
#include "VolumeCtrl.h"
#include <SFML/Audio.hpp>
#include "Sound.h"

//
// base
//
class UglyMenu
{
protected:
	int i;
	int nargs;
	int spacing;
	float xo,yo;
public:
	UglyMenu(const char *sFile, int nargs, int spacing, float xo=87.0f, float yo=117.0f);
	virtual ~UglyMenu();

	Sound		aOpen;
	Sound		aClose;
	Sound		aSelect;
	Sound		aDecide;
	sf::Texture*	tScreen;
	sf::Texture*	tCursor;

	virtual void	Up();
	virtual void	Down();
	virtual void	Left() {}
	virtual void	Right() {}
	virtual int		Decide();

	virtual int		GetIndex() { return i; }

	virtual void	Draw();
};

//
//asd
//
class PauseMenu : public UglyMenu {
public:
	PauseMenu() : UglyMenu("data/pause.PNG", 3, 85, 130.0f, 138.0f) {
		aOpen.play();
	}
};

class StartupMenu : public UglyMenu {
public:
	StartupMenu() : UglyMenu("data/screen2.PNG", 2, 110, 87.0f, 160.0f) {}
};

class StartupMenuCont : public UglyMenu {
public:
	StartupMenuCont() : UglyMenu("data/screen.PNG", 3, 110, 87.0f, 100.0f) {}
};

class OptionsMenu : public UglyMenu
{
	VolumeCtrl musicVol;
	VolumeCtrl sfxVol;
public:
	OptionsMenu(const char *file, float mvol, float svol) : UglyMenu(file, 4, 90, 80.0f, 100.0f), sfxVol(0.0f, 100.0f, svol), musicVol(0.0f, 100.0f, mvol)
	{
		aOpen.play();
	}

	void	Up();
	void	Down();
	void	Left();
	void	Right();
	int		Decide();
	void	Draw();

	float	GetCurVol();
};
