#include "Jack.h"
#include "Engine.h"
#include "Input.h"

//#undef NDEBUG

extern Engine *eng;
extern Input *input;

void OpenMapDialog(char *buffer, int size) {
	buffer[0] = 0;

	OPENFILENAME ofd;
	ofd.lStructSize = sizeof(ofd);
	ofd.hwndOwner = NULL;
	//ofd.hInstance;
	ofd.lpstrFilter = "Jakob Maps\0*.jkm\0";
	ofd.lpstrCustomFilter = 0;
	ofd.nFilterIndex = 0;
	ofd.lpstrFile = buffer;
	ofd.nMaxFile = size;
	ofd.lpstrFileTitle = NULL;
	ofd.lpstrInitialDir = ".\\data";
	ofd.lpstrTitle = "Load map";
	ofd.Flags = OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_NOCHANGEDIR |
		OFN_NODEREFERENCELINKS | OFN_PATHMUSTEXIST;
	ofd.lpstrDefExt = "jkm";

	GetOpenFileName(&ofd);
}

Jack::Jack() : loadedGame(false),
		postInit(false),
#ifndef NDEBUG
		drawProfile(false),
#endif
		pause(false), pauseReset(0)
{
	// init vars
	score				= NULL;
	overflowDelay		= 0;
	initialMap			= "data/Lvl1.jkm"; // default map
	ilikeoverflows[0]	= '\0';
	winrarTimer			= 0;

	// load 'continue' screen or 'new' screen
	if (CSettings::GameStateExists())
		gui.Set(std::auto_ptr<UglyMenu>(new StartupMenuCont));
	else
		gui.Set(std::auto_ptr<UglyMenu>(new StartupMenu));

	// read config
	musicVol			= 88.0f;
	mLowVol				= 1.0f;
	sfxVol				= 90.0f;
	
	CSettings::ReadSettings(this);

	Sound::SetGlobalVolume(sfxVol);
	//SetSong("data/asaszszx3.ogg");
	//SetSong("data/shjak.ogg");
}

Jack::~Jack()
{
	RewriteConfig();
}

void Jack::ReadjustVolumes()
{
	// sfx vol
	//Sound::SetGlobalVolume(sfxVol); // FIXME-SFML

	// music vol
	/*
	if (musicVol > 0.0f) {
		long mvol = -5000 + long(musicVol * mLowVol * 50.0f);
		musicPlayer.SetVolume(mvol);
	}
	else
		musicPlayer.SetVolume(-10000); // dead 0
	*/ // FIXME-SFML: adjust music volume based on what's going on

	//CSettings::SaveSettings(this);
}

void Jack::RewriteConfig()
{
	CSettings::SaveSettings(this);
}

void Jack::Load(bool loadState)
{
	if (loadedGame)
		return;

	//map.Load(initialMap.c_str());
	if (!loadState || !CSettings::GameStateExists()) {
		LoadMap(initialMap.c_str());
		musicPlayer.stop();
		eng->SetSplash("data/Cathorel2.png");
	}
	else {
		LoadState(CSettings::GameStateFile());
		sndDoKorVi.play();
	}

	loadedGame = true;
}

void Jack::SetSong(const char *file)
{
	if (file == 0 || file[0] == 0)
		return;
	// play song
	if (!musicPlayer.openFromFile(file))
		return;
	musicPlayer.setLoop(true);
	musicPlayer.play();

	ReadjustVolumes();
}

void Jack::SetSongOnce(const char *file)
{
	if (file == 0 || file[0] == 0)
		return;

	// play song
	if (!musicPlayer.openFromFile(file))
		return;
	musicPlayer.setLoop(true);
	musicPlayer.play();

	ReadjustVolumes();
}

void Jack::PostInit()
{
	// load font
	fontTex = eng->res->textures.Load("data/font.png");
	font.SetFont(fontTex, 8, 128);

	postInit = true;

	// audio stuff
	sndDoKorVi		= sf::Sound(*eng->res->sounds.Load("data/v_dakorvi.wav"));
	sndBraJobbat	= sf::Sound(*eng->res->sounds.Load("data/v_brajobat.wav"));

	//Load(); // added
}

bool Jack::Frame()
{
	bool tick = true;

	if (!postInit)
		PostInit();

#if 0
	if (startupMenu) {
		if (pause) {
			if (input->CheckPressed(VK_ESCAPE)) { // resume
				delete startupMenu; startupMenu = 0;
				pause = false;
			}
		}
		else {
			// VK_ESC = quit
			if (input->CheckPressed(VK_ESCAPE))
				PostQuitMessage(0);
		}

		if (input->CheckPressed(VK_UP))		// Todo: CheckRepeat
			startupMenu->Up();
		if (input->CheckPressed(VK_DOWN))
			startupMenu->Down();
		if (input->Check(VK_RIGHT))
			startupMenu->Right();
		if (input->Check(VK_LEFT))
			startupMenu->Left();

		if (input->CheckPressed(VK_RETURN)) {
			if (!pause) { // startup menu
				switch (startupMenu->Decide()) {
					case 0:		// TODO: continue. NOTE: Automatic save from checkpoint?
						break;
					case 1:		// play
						delete startupMenu; startupMenu = 0;
						Load();
						break;
				}
			} else { // pause menu
				switch (startupMenu->Decide()) {
					case 0: // return
						delete startupMenu; startupMenu = 0;
						pause = false;
						break;
					case 1: // TODO: load
						break;
					case 2: // TODO: save
						break;
					case 3: // quit
						PostQuitMessage(0);
						break;
				}
			}
		}

		if (startupMenu) { // if not deleted
			eng->render->BeginFrame();
			startupMenu->Draw();
			eng->render->EndFrame();
		}
	}
#endif
	if (gui.HasMenu()) {
		tick = gui.Update();
		gui.Draw();
	}
	else if (loadedGame) {
		// update game
		tick = Update();
		Draw();
	}
	return tick;
}

bool Jack::Update()
{
	bool tick = true;
	if (!pause)
		tick = map.Update();
	else if (score) {
		tick = score->Update();

		int i=score->Select();
		if (i != -1) {
			if (i == 0) { // save
				Archiver arc(CSettings::GameStateFile(), false);
				arc << "StateFile" << score->nextMap;
			}
		}
		if (score->Done()) {
			const char *nextMap = score->nextMap;
			delete score; score = NULL;
			LoadMap(nextMap);
		}
	}

	if (tick && !score)
	{
		if (!pause) {
			hud.Step();
			chaseCam.Update();
		}

#ifdef JSDEBUG
		// test stuff
		if (input->Check(VK_CONTROL) && input->CheckPressed('O'))
			OpenMapDialog(ilikeoverflows, sizeof(ilikeoverflows));

		if (input->CheckPressed('T')) {
			EndLevel("data/random.jkm");
		}

		if (input->CheckPressed(VK_F4)) {
			SaveState(CSettings::GameStateFile());
		}
		else if (input->CheckPressed(VK_F5)) {
			LoadState(CSettings::GameStateFile());
		}

		if (input->CheckPressed('S'))
			eng->SetSplash("data/Cathorel2.png");

#ifndef NDEBUG
		if (input->CheckPressed(VK_F2)) // profiler
			drawProfile = !drawProfile;
#endif
#endif

		// user pause
		if (!pauseReset && input->CheckPressed(VK_ESCAPE)) {
			pause = true;
			//startupMenu = new UglyMenu("data/pause.PNG", 4, 80);
			gui.Set(std::auto_ptr<UglyMenu>(new PauseMenu));

			if (gui.MenuType() == MENU_STARTUP || gui.MenuType() == MENU_STARTUP_CONT) { // user decided to quit
				return true;
			}
		}
	}

	// lower music vol while th snds are playing
	if (mLowVol==1.0f && (sndDoKorVi.getStatus()==sf::SoundSource::Status::Playing || sndBraJobbat.getStatus()==sf::SoundSource::Status::Playing)) {
		mLowVol = 0.7f;
		ReadjustVolumes();
	}
	else if (mLowVol==0.7f && (!sndDoKorVi.getStatus()==sf::SoundSource::Status::Playing && !sndBraJobbat.getStatus()==sf::SoundSource::Status::Playing)) {
		mLowVol = 1.0f;
		ReadjustVolumes();
	}

	// load map (safe?)
	if (ilikeoverflows[0] != 0)
	{
		if (eng->time >= overflowDelay) {
			delete score; score = NULL;
			pause = false;
			pauseReset = 0;
			//overflowDelay = 0;

			hud.SetPlayer(NULL);

			chaseCam.Reset();
			map.Load(ilikeoverflows);

			sndDoKorVi.play();
			
			ilikeoverflows[0] = '\0'; // safe here?
		}
	}

	// unpause timer
	if (pause && pauseReset) {
		if (eng->time >= pauseReset) {
			pause = false;
			pauseReset = 0;
		}
	}

	// play winner song
	if (winrarTimer && eng->time >= winrarTimer) {
		SetSongOnce("data/winlyfe.ogg");
		winrarTimer = 0;
	}

	return tick;
}

static float p_t_width(char *str)
{
	return (float)8*strlen(str);
}
static void p_t_draw(float x, float y, char *str)
{
	eng->render->SetColor(255,255,255);
	eng->jack->font.SetScale(1.0f);
	eng->jack->font.Draw(x,y,str);
}

void Jack::Draw()
{
	eng->render->BeginFrame();
	map.Draw();
	hud.Draw();

	// scoreboard
	if (score)
		score->Draw();

#ifdef JSDEBUG
	// fps
	static int fp = 0;
	if (!fp)
		fp = eng->time;
	if (eng->time - fp == 0)
		fp--; // div by 0

	eng->render->SetColor(255,255,0);

	eng->render->NoViewBegin();
	font.SetScale(1.5f);
	font.Printf(5.0f, 450.0f, "FPS: %f", 1000.0f / (float)(eng->time-fp));
	eng->render->NoViewEnd();

#ifndef NDEBUG
	// profiler
	if (drawProfile) {
		eng->render->NoViewBegin();
		Prof_draw_gl(5.0f, 5.0f,
			640.0f, 480.0f,
			24.0f,
			2,
			p_t_draw,
			p_t_width);
		eng->render->NoViewEnd();
	}
#endif

	fp = eng->time;
#endif

	eng->render->EndFrame();
}

void Jack::QuitGame()
{
	//SetSong("data/asaszszx3.ogg");
	SetSong("data/shjak.ogg");
	map.Free();

	pause = false;
	loadedGame = false;

	if (CSettings::GameStateExists())
		gui.Set(std::auto_ptr<UglyMenu>(new StartupMenuCont));
	else
		gui.Set(std::auto_ptr<UglyMenu>(new StartupMenu));
}

void Jack::SaveState(const char *stateFile)
{
	Archiver arc(stateFile, false);
	arc << "StateFile";
#if 0
	map.Serialize(arc);
	// TODO: crc-check map (if I'm not feeling lazy)
#endif
	// good bye, oh elaborate scheme for nothing!
	
	arc << map.MapFile();
	arc << hud.GetLives();
}

void Jack::LoadState(const char *stateFile)
{
	Archiver arc(stateFile, true);
	std::string sig;
	arc >> sig;

	if (sig.compare("StateFile")) {
		MessageBox(0, "Not a state file.", stateFile, 0);
		return;
	}
	//map.Serialize(arc); // good bye, oh elaborate scheme for nothing!

	std::string mf;
	arc >> mf;
	LoadMap(mf.c_str()); // EndLevel? Beware of string pointer!

	int lives = 3;
	arc >> lives;
	hud.SetLives(lives);
}

void Jack::EndLevel(const char *nextMap)
{
	if (score)
		return;

#if 0
	// show some scoreboard and play winrar song
	// pause for a while
	// fade and/or do something fancy with the player (like rotate - wow!)
	pause = true;
	pauseReset = eng->time + 7000;

	score = new ScoreBoard;

	sndBraJobbat->Play(false);
	//SetSong("data/winlyfe.ogg");
	winrarTimer = eng->time + 1000;

	LoadMap(nextMap, 7000);
#endif
	pause = true;
	score = new ScoreBoard;
	score->nextMap = nextMap; // can't believe I'm doing this!
	sndBraJobbat.play();
	winrarTimer = eng->time + 1000;
}

void Jack::LoadMap(const char *file, int delay)
{
	//printf("LoadMap: %s - %d\n", file, delay);
	overflowDelay = eng->time + delay;
	strcpy(ilikeoverflows, file);
}
