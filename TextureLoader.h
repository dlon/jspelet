#pragma once
#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

struct Texture;

// TODO: either move or introduce a new manipulator func (or multi-functional) that is directly fed the pixels (in order to avoid the excessive memory allocation/deallocation)

typedef void (*manipulation_func_t)(unsigned char *&data, unsigned &w, unsigned &h, unsigned &fmt);

class TextureLoader
{
public:
	Texture *LoadTexture(const char *file);
	Texture *LoadTextureEx(const char *file, manipulation_func_t customFunction);
	virtual void FreeTexture(Texture *t) = 0;

	// some built-in manipulation funcs (pass to LoadTextureEx)
	static void SetColorKey(int r, int g, int b);
	static void RemoveColorKey(unsigned char *&data, unsigned &w, unsigned &h, unsigned &fmt); // pass as custom func. note: bitmaps already have a color key (255,0,255)
	static void Swap2Channels(unsigned char *&data, unsigned &w, unsigned &h, unsigned &fmt);

	TextureLoader() {}
	virtual ~TextureLoader() {}
protected:
	virtual Texture *LoadTextureMem(unsigned char *data, unsigned width, unsigned height, unsigned format) = 0;
};

#endif // TEXTURELOADER_H
