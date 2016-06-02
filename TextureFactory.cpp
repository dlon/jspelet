#include "TextureFactory.h"

Texture *TextureFactory::LoadEx(const char *file, manipulation_func_t custom)
{
	useCustomFunc = true;
	customFunc = custom;
	return Load(file); // call base method
}

Texture *TextureFactory::LoadRes(const char *file)
{
	if (useCustomFunc) {
		useCustomFunc = false;
		return _loader->LoadTextureEx(file, customFunc); // pixel manipulation stuff
	}
	return _loader->LoadTexture(file);
}

void TextureFactory::FreeRes(Texture *t)
{
	_loader->FreeTexture(t);
}
