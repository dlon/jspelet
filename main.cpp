#include <SFML/Graphics.hpp>
#include "Engine.h"
#include "Jack.h"
#include "Input.h"
#include <string>
#include <sstream>
#include <assert.h>

#define GAME_CAPTION "Jackespelet"

bool lWindowedMode = true;
bool lMapParameter = false;
bool lPosParameter = false;
bool lInvicParameter = false;

extern std::string songAfterSplash;
extern int tticks;

// globals
//Engine	*eng = 0;
extern Engine *eng;
Input	*input = 0;

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
			by.write(p+1, strlen(p+1));
			bx >> x;
			by >> y;
			return;
		}
		x = y = 0;
	}
};

static std::string		initialMap;
PlPosParam				initialPos(NULL);

inline void SpawnPos(int id, float x, float y)
{
	Entity *e = eng->jack->map.GetEntities().CreateEntity(id);
	e->x = eng->render->cam.x + x;
	e->y = eng->render->cam.y + y;
}

// Process command-line
static
const char *SlowTok(const char *s, int tok, int i) {
	for (int c=0; c<i; c++) {
		for (;*s != 0 && *s != tok; s++) {}
	}
	return s;
}

static
void ProcessCmdLine(const char* lpCmdLine) {
	if (!lpCmdLine)
		return;

	char *str = strtok((char*)lpCmdLine," ");
	if (!str)
		return;
	
	bool mapArg = false;
	bool startArg = false;

	do {
		if (!mapArg && !startArg /* && ... */) {
			if (strcmp("-map", str) == 0) {
				mapArg = true;
				continue;
			}
			if (strcmp("-startpos", str) == 0) {
				startArg = true;
				continue;
			}
			if (strcmp("-invic", str) == 0) {
				lInvicParameter = true;
				continue;
			}
		}

		// args
		if (mapArg) {
			initialMap = str;
			lMapParameter = true;
		}
		else if (startArg) {
			initialPos = PlPosParam(str);
			lPosParameter = true;
		}

		// reset params
		mapArg = false;
		startArg = false;
	} while (str = strtok(NULL, " "));
}

int RunGame(const char *cmdLine)
{
	lWindowedMode = !CSettings::ReadFullscreen();
	sf::RenderWindow window(sf::VideoMode(640,480), GAME_CAPTION);
	//assert(lWindowedMode); // FIXME-SFML: implement fullscreen mode
	
	input = new Input;
	/*eng =*/ new Engine(window);

	if (!lWindowedMode)
		eng->render->SetVsync(true); // NOTE: always vsync in fullscreen?

	ProcessCmdLine(cmdLine);

	if (initialMap != "")
		eng->jack->SetInitialMap(initialMap.c_str());

	// intro
	tticks = 180;
	songAfterSplash = "data/shjak.ogg";
	eng->SetSplash("data/Pres.png");
	tticks = 420;

	// main loop
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == -1 || event.key.code == sf::Keyboard::D) // HACKY BUGFIX
					break;
				input->HitKey(event.key.code);
				break;
			case sf::Event::KeyReleased:
				if (event.key.code == -1 || event.key.code == sf::Keyboard::D) // HACKY BUGFIX
					break;
				input->RelKey(event.key.code);
				break;
			}
		}

		// HACKY BUGFIX: SFML would not give the correct key event for D when shift was held
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			input->HitKey(sf::Keyboard::D);
		else if (input->Check(sf::Keyboard::D))
			input->RelKey(sf::Keyboard::D);

		if (eng->Frame())
			input->UnPress();
	}
	delete eng;
	delete input;
	return 0;
}

int main(int argc, char **argv)
{
	// concatenate argv to str
	int len = 0;
	char *cmd = 0;
	for (int i=0; i<argc; i++)
		len += (int)strlen(argv[i])+1;

	if (len) {
		int pos = 0;
		cmd = new char[len];

		for (int i=0; i<argc; i++) {
			strcpy(cmd+pos, argv[i]);

			pos += (int)strlen(argv[i]) + 1;
			cmd[pos-1] = ' '; // replace null char with space
		}
		cmd[pos-1] = 0; // replace null space with null char
	} else {
		// empty string
		cmd = new char[1];
		cmd[0] = 0;
	}

	// run game
	int res = RunGame(cmd);

	delete [] cmd;
	return res;
}
