#pragma once
#ifndef JACK_H
#define JACK_H

// main game class

#include "EntityMng.h"
#include "JackHUD.h"
#include "MapMng.h"
#include "OggFile.h"
#include "Groovyizer.h"
#include "Archiver.h"
#include "Font.h"
#include "ChaseCam.h"
#include "UglyMenu.h"
#include "ScoreBoard.h"
#include "JackeGui.h"
#include "CSettings.h"

class Jack
{
	bool loadedGame;

	bool Update();
	void Draw();

	bool postInit;
	char ilikeoverflows[512];
	int overflowDelay;

	int		winrarTimer;

	std::string initialMap;
public:
	JackHUD		hud;
	MapMng		map;
	TextEngine	font;
	Texture		*fontTex;
	ChaseCam	chaseCam;
#ifndef NDEBUG
	bool		drawProfile;
#endif
	OggFile		musicPlayer;
	bool		pause;
	int			pauseReset;
	ScoreBoard	*score;
	JackeGui	gui;

	float		musicVol;
	float		mLowVol;
	float		sfxVol;

	Sound		*sndDoKorVi;
	Sound		*sndBraJobbat;

	void ReadjustVolumes();
	void RewriteConfig();

	void QuitGame(); // return to main menu

	void Load(bool loadState=false);
	bool Frame();
	void PostInit();

	void SaveState(const char *stateFile);
	void LoadState(const char *stateFile);

	void EndLevel(const char *nextLevel);

	void SetInitialMap(const char *map) { initialMap = map; }
	void SetSong(const char *file);
	void SetSongOnce(const char *file);

	void LoadMap(const char *file, int delay=0);
	void SafeLoadMap(const char *file, int delay=0) { LoadMap(file, delay); }

	Jack();
	~Jack();
};

#endif // JACK_H
