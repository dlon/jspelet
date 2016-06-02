#pragma once
#ifndef ENGINE_H
#define ENGINE_H

#include "ResourceMng.h"
#include "Renderer.h"
#include "Audio.h"

#ifdef NDEBUG
#define puts(x) {}
#define printf(fmt,...) {}
#endif

class Jack;

class Engine
{
public:
	//Console		*con;
	ResourceMng	*res;
	Renderer	*render;
	int			time;
	Jack		*jack;

	bool Frame();

	void SetSplash(const char *file);

	Engine(HWND wnd);
	~Engine();
};

//extern Engine *eng;

#endif // ENGINE_H
