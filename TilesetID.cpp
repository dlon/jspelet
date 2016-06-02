#include "TilesetID.h"
#include "Engine.h"

extern Engine *eng;

static const char *tilesets[] = {
	"data/tileset0.png",
	"data/tileset1.png",
	"data/tileset2.png",
	"data/tileset3.png",
	"data/tileset4.png",
	"data/tileset5.png",
	"data/tileset6.png",
	"data/biggycool.png",
	"data/txt_black.png",
	"data/vattn.png",
	"data/begevatn.png",
	"data/plattform2.png",
	"data/plattform.png",
	"data/seaset.png"
};

int TilesetID::Size()
{
	return sizeof(tilesets)/sizeof(char*);
}

const char *TilesetID::GetString(int i)
{
	if (i < 0 || i >= sizeof(tilesets)/sizeof(char*))
		return "";
	return tilesets[i];
}

Texture *TilesetID::GetTexture(int i)
{
	//MessageBox(0,GetString(i),0,0);
	return eng->res->textures.Load(GetString(i));
	/*
	Texture *t = eng->res->textures.GetByFile(GetString(i));
	if (!t) {
		t = eng->render->LoadTexture(GetString(i), LF_REPEAT);
		eng->res->textures.Add(GetString(i), t);
	}
	return t;
	*/
}
