#pragma once
#ifndef JACKRESOURCES_H
#define JACKRESOURCES_H

#include "SoundFactory.h"
#include "TextureFactory.h"

class ResourceMng
{
public:
	SoundFactory		sounds;
	TextureFactory		textures;

	ResourceMng(TextureLoader *textureLoader) : textures(textureLoader) {}
};

#endif // JACKRESOURCES_H
