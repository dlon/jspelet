#pragma once
#ifndef TILESETID_H
#define TILESETID_H

struct Texture;

class TilesetID
{
	TilesetID();
public:
	static const char *GetString(int i);
	static Texture *GetTexture(int i);
	static int Size();
};

#endif // TILESETID_H
