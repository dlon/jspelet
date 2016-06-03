#pragma once
#ifndef ENGINE_H
#define ENGINE_H

#include "ResourceMng.h"
#include "Renderer.h"
#include <SFML/System.hpp>

class Jack;

class Engine
{
	sf::Clock	clock;
public:
	//Console		*con;
	ResourceMng	*res;
	Renderer	*render;
	Jack		*jack;
	int		time;

	bool Frame();

	void SetSplash(const char *file);

	Engine(sf::RenderWindow& window);
	~Engine();
};

//extern Engine *eng;

#endif // ENGINE_H
