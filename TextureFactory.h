#pragma once
#ifndef TEXTUREFACTORY_H
#define TEXTUREFACTORY_H

#include "TextureLoader.h"
#include "ResourceFactory.h"

class TextureFactory : public ResourceFactory<Texture>
{
	TextureLoader *			_loader;

	bool					useCustomFunc;
	manipulation_func_t		customFunc;
public:
	TextureFactory(TextureLoader *loader) : _loader(loader), useCustomFunc(false), customFunc(NULL) {}
	~TextureFactory() { FreeAll(); }

	Texture *LoadEx(const char *file, manipulation_func_t custom);
protected:
	Texture *LoadRes(const char *file);
	void FreeRes(Texture *t);
};

#endif // TEXTUREFACTORY_H
