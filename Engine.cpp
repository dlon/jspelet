#include "Engine.h"
#include "Jack.h"
#include "SplashImage.h"
#include "Input.h"
#include <SFML/System.hpp>
#include <assert.h>

Engine *	eng = 0;
sf::Texture *	splash = 0;
bool		leaveSplash;
SplashImage	splashObject;
extern Input *input;

bool		allowEnterSplash = true;

extern int tticks;

std::string mapLoadAfterSplash;
std::string songAfterSplash;
std::string endingSplash;

Engine::Engine(sf::RenderWindow& window)
{
	assert(eng == 0); // this should not run more than once
	// todo: error checking
	render = new Renderer(window);
	res = new ResourceMng;

	eng = this;
	time = 0;

	// init game
	jack = new Jack; // 'jack' won't be assigned until after construction!
}

Engine::~Engine()
{
	delete jack;

	delete res;
	render->Close();
	delete render;
}

bool Engine::Frame()
{
	time = clock.getElapsedTime().asMilliseconds();
	if (!splash) {
		return jack->Frame();
	}
	else {
		static sf::Clock pf;

		if (pf.getElapsedTime().asMilliseconds() > 16) {
			splashObject.Update();

			if (!splashObject.IsActive()) {
				splashObject.SetImage(0);
				if (!leaveSplash)
					res->textures.FreeByID(splash);
				splash = 0;

				if (mapLoadAfterSplash != "") {
					jack->LoadMap(mapLoadAfterSplash.c_str());
					mapLoadAfterSplash="";
				}
				if (songAfterSplash != "") {
					jack->SetSong(songAfterSplash.c_str());
					songAfterSplash = "";
				}
				allowEnterSplash = true;

				if (endingSplash != "") {
					allowEnterSplash = false;
					tticks = -1;
					SetSplash(endingSplash.c_str());
					endingSplash = "";
				}
				return true;
			}
			else if (splashObject.alpha == 255.0f) {
				// check input
				if (allowEnterSplash && (input->CheckPressed(sf::Keyboard::Return) || input->CheckPressed(sf::Keyboard::Escape)))
					splashObject.Hide();
			}
			
			// draw splash
			render->BeginFrame();
			render->NoViewBegin();
			splashObject.Draw();
			render->NoViewEnd();
			render->EndFrame();
			
			pf.restart();
			return true;
		}
		return false;
	}
}

void Engine::SetSplash(const char *file)
{
	leaveSplash = false;
	if (splash=res->textures.GetByFile(file)) {
		leaveSplash = true;
	}
	else
		splash = res->textures.Load(file);
	splashObject.SetImage(splash);
	splashObject.Show();
}

