#include "JackeGui.h"
#include "Jack.h"
#include "Input.h"

extern Engine *eng;
extern Input *input;

JackeGui::JackeGui()
{
	//menu		= NULL;
	tick		= eng->time;
	menuType	= MENU_UNKWN;
}

void JackeGui::Set(std::auto_ptr<UglyMenu> m)
{
	menu = m;
	menuType = MENU_UNKWN;

	if (dynamic_cast<StartupMenu*>(menu.get())) {
		menuType = MENU_STARTUP;
	}
	else if (dynamic_cast<StartupMenuCont*>(menu.get())) {
		menuType = MENU_STARTUP_CONT;
	}
	else if (dynamic_cast<PauseMenu*>(menu.get())) {
		menuType = MENU_PAUSE;
		eng->jack->pause = true; // pause game
	}
	else if (dynamic_cast<OptionsMenu*>(menu.get())) {
		menuType = MENU_OPTIONS;
	}
	/*else if (dynamic_cast<ControlsInfo*>(menu.get())) {
		menuType = MENU_CONTROLS_INFO;
	}*/
}

void JackeGui::Close()
{
	menu.release();
	menuType = MENU_UNKWN;
}

bool JackeGui::Update()
{
	// FIXME: OK this time loop
	if (menu.get() && eng->time >= tick+40) {
		// navigation
		if (input->CheckPressed(sf::Keyboard::Up))		// Todo: CheckRepeat
			menu->Up();
		if (input->CheckPressed(sf::Keyboard::Down))
			menu->Down();

		bool lrbtn = false;
		if (input->Check(sf::Keyboard::Right)) {
			lrbtn = true;
			menu->Right();
		}
		if (input->Check(sf::Keyboard::Left)) {
			lrbtn = true;
			menu->Left();
		}

		if (lrbtn) {
			// update sfx/music vol
			if (menuType == MENU_OPTIONS) {
				if (menu->GetIndex() == 0)
					eng->jack->musicVol = static_cast<OptionsMenu*>(menu.get())->GetCurVol();
				else if (menu->GetIndex() == 1)
					eng->jack->sfxVol = static_cast<OptionsMenu*>(menu.get())->GetCurVol();
				eng->jack->ReadjustVolumes();
			}
		}

		// enter/esc
		if (input->CheckPressed(sf::Keyboard::Escape)) {
			if (menuType == MENU_PAUSE) {
				Close();
				eng->jack->pause = false; // resume
			}
			else if (menuType == MENU_STARTUP || menuType == MENU_STARTUP_CONT) {
				PostQuitMessage(0);
			}
			else if (menuType == MENU_OPTIONS) {
				Set(prevMenu); // return to prev menu
				eng->jack->RewriteConfig(); // rewrite cfg
			}
		}
		else if (input->CheckPressed(sf::Keyboard::Return)) {
			if (menuType == MENU_STARTUP || menuType == MENU_STARTUP_CONT) {
				int i = menu->Decide();
				i = menuType == MENU_STARTUP ? i+1 : i;

				switch (i) {
					case 0:		// continue
						Close();
						eng->jack->Load(true);
						break;
					case 1:		// load game/map
						Close();
						eng->jack->Load(false);
						break;
					case 2:
						{
						// options
						prevMenu = menu;
						const char *opt = CSettings::GetFullscreen() ? "data/options.PNG" : "data/options2.PNG";
						Set(std::auto_ptr<UglyMenu>(new OptionsMenu(opt, eng->jack->musicVol, eng->jack->sfxVol)));
						}
						break;
				}
			}
			else if (menuType == MENU_PAUSE) {
				switch (menu->Decide()) {
					case 0: // return
						Close();
						eng->jack->pause = false; // resume
						break;
					case 1:
						{
						// options
						prevMenu = menu;
						const char *opt = CSettings::GetFullscreen() ? "data/options.PNG" : "data/options2.PNG";
						Set(std::auto_ptr<UglyMenu>(new OptionsMenu(opt, eng->jack->musicVol, eng->jack->sfxVol)));
						}
						break;
					case 2:
						//PostQuitMessage(0);
						// return to main menu (is "quit" an appropriate label for this?)
						eng->jack->QuitGame();
						break;
				}
			}
			else if (menuType == MENU_OPTIONS) {
				switch (menu->Decide()) {
					case 2:
						{
						// fullscreen
						/*
						int msg = MessageBox(NULL,
							"The game needs to be restarted for the changes to take effect.\nUnsaved progress will be lost. Restart now?",
							"Screen Mode",
							MB_YESNO | MB_ICONEXCLAMATION);
						if (msg == IDYES) {
							CSettings::SetFullscreen( !CSettings::GetFullscreen() );
							eng->jack->RewriteConfig();
							
							char fileBuffer[MAX_PATH]={0};
							GetModuleFileName(NULL,
								reinterpret_cast<LPCH>(&fileBuffer),
								sizeof(fileBuffer));
							fileBuffer[sizeof(fileBuffer)-1] = '\0';
							
							if (fileBuffer[0] != '\0') {
								ShellExecute(NULL,
									"open",
									fileBuffer,
									NULL,
									NULL,
									SW_SHOWDEFAULT);
								PostQuitMessage(0);
							}
						}
						*/ // FIXME-SFML: enable fullscreen option
						}
						break;
					case 3:
						// return to prev screen
						Set(prevMenu);
						// rewrite cfg
						eng->jack->RewriteConfig();
						break;
				}
			}
		}

		// next update
		tick = eng->time;
		return true;
	}
	return false;
}

void JackeGui::Draw()
{
	if (!menu.get())
		return;

	eng->render->BeginFrame();
	menu->Draw();
	eng->render->EndFrame();
}
