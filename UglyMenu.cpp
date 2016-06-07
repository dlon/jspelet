#include "UglyMenu.h"
#include "Engine.h"
#include "Jack.h"

extern Engine *eng;

UglyMenu::UglyMenu(const char *sFile, int nargs, int spacing, float xo, float yo)
{
	aOpen = Sound(*eng->res->sounds.Load("data/mmopen.wav"));
	aClose = Sound(*eng->res->sounds.Load("data/mmclose.wav"));
	aSelect = Sound(*eng->res->sounds.Load("data/mmselect2.wav"));
	aDecide = Sound(*eng->res->sounds.Load("data/kesh.wav"));

	tScreen = eng->res->textures.Load(sFile);
	tCursor = eng->res->textures.Load("data/screencur.PNG");

	i = 0;
	this->nargs = nargs; // 2
	this->spacing = spacing; // 120
	this->xo = xo; // 87
	this->yo = yo; // 117

	//aOpen->Play(false);
}

UglyMenu::~UglyMenu()
{
	// TODO: Free sounds - use ref. counter?
	// FIXME-SFML: Why can't I free all resources upon destruction?

	//aClose->Play(false); // can't free this now, though
}

void UglyMenu::Draw()
{
	eng->render->NoViewBegin();

	//eng->render->SetColor(200, 20, 20);
	//eng->render->DrawRect(0.0f, 0.0f, 620.0f, 440.0f);
	eng->render->SetColor();
	eng->render->DrawTexture(tScreen, 0, 0);
	eng->render->DrawTexture(tCursor, xo, yo + float(i*spacing));

	eng->render->NoViewEnd();
}

void UglyMenu::Up()
{
	if (i) {
		aSelect.play();
		i--;
	}
}

void UglyMenu::Down()
{
	if (i+1 < nargs) {
		aSelect.play();
		i++;
	}
}

int UglyMenu::Decide()
{
	aDecide.play();
	return i;
}


//
// options menu
//
void OptionsMenu::Draw()
{
	//UglyMenu::Draw();

	eng->render->NoViewBegin();
	
	eng->render->SetColor();
	eng->render->DrawTexture(tScreen, 0, 0);
	eng->render->DrawTexture(tCursor, xo, yo + float(i*spacing)); // don't draw if vol ctrl is selected

	musicVol.Draw(150.0f, 80.0f);
	sfxVol.Draw(150.0f, 180.0f);
	
	eng->render->NoViewEnd();
}

int OptionsMenu::Decide()
{
	return UglyMenu::Decide();
}

void OptionsMenu::Left()
{
	if (i == 0)
		musicVol.Left();
	if (i == 1)
		sfxVol.Left();
}
void OptionsMenu::Right()
{
	if (i == 0)
		musicVol.Right();
	if (i == 1)
		sfxVol.Right();
}

void OptionsMenu::Up()
{
	UglyMenu::Up();

	musicVol.Deselect();
	sfxVol.Deselect();
	
	if (i == 0)
		musicVol.Select();
	else if (i == 1)
		sfxVol.Select();
}
void OptionsMenu::Down()
{
	UglyMenu::Down();

	musicVol.Deselect();
	sfxVol.Deselect();
	
	if (i == 0)
		musicVol.Select();
	else if (i == 1)
		sfxVol.Select();
}

float OptionsMenu::GetCurVol()
{
	if (i == 0)
		return musicVol.GetVol();
	else if (i == 1)
		return sfxVol.GetVol();
	return -1.0f;
}
