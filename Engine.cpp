#include "Engine.h"
#include "Audio.h"
#include "Jack.h"
#include "SplashImage.h"
#include "Input.h"

Engine *	eng = 0;
Texture *	splash = 0;
bool		leaveSplash;
SplashImage	splashObject;
extern Input *input;

bool		allowEnterSplash = true;

extern int tticks;

std::string mapLoadAfterSplash;
std::string songAfterSplash;
std::string endingSplash;

Engine::Engine(HWND wnd)
{
	timeBeginPeriod(1);
	time = timeGetTime();

	// todo: error checking
	render = new Renderer;
	render->Init(wnd);
	AudioDevice::Init(wnd);
	res = new ResourceMng(render);

	eng = this;

	// init game
	jack = new Jack; // 'jack' won't be assigned until after construction!
}

Engine::~Engine()
{
	delete jack;

	delete res;
	AudioDevice::Close();
	render->Close();
	delete render;

	timeEndPeriod(1);
}

bool Engine::Frame()
{
	time = timeGetTime();
	if (!splash) {
		return jack->Frame();
	}
	else {
		static int pf = 0;
		if (!pf) pf = time;

		if (time-pf > 16) {
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
					allowEnterSplash = true;
					tticks = -1;
					SetSplash(endingSplash.c_str());
					endingSplash = "";
				}
				return true;
			}
			else if (splashObject.alpha == 255.0f) {
				// check input
				if (allowEnterSplash && (input->CheckPressed(VK_RETURN) || input->CheckPressed(VK_ESCAPE)))
					splashObject.Hide();
			}
			
			// draw splash
			render->BeginFrame();
			render->NoViewBegin();
			splashObject.Draw();
			render->NoViewEnd();
			render->EndFrame();
			
			pf = time;
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

