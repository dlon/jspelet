#pragma once
#include "UglyMenu.h"
#include <memory>

enum {
	MENU_UNKWN,
	MENU_STARTUP,
	MENU_STARTUP_CONT,
	MENU_OPTIONS,
	MENU_PAUSE,
	MENU_CONTROLS_INFO
};

class JackeGui {
	std::auto_ptr<UglyMenu>		menu;
	std::auto_ptr<UglyMenu>		prevMenu;
	int							tick;
	int							menuType;
public:
	JackeGui();

	void Set(std::auto_ptr<UglyMenu> m);
	void Close();

	bool HasMenu() { return menu.get() != NULL; }
	int  MenuType() { return menuType; }

	bool Update();
	void Draw();
};
