#include "SplashImage.h"
#include "Renderer.h"
#include "Engine.h"
#include "Input.h"

extern Engine *eng;
extern Input *input;

int tticks = 420;

void SplashImage::Show() {
	if (!isActive && bg != NULL) {
		//MapPauseStatePrev = g_MapPaused;
		//g_MapPaused = 2;
		isActive=1;
		alpha = 0.0f;
		timeOutTicks = tticks;
		hide = false;
	}
}

void SplashImage::Hide() {
	if (isActive && bg != NULL) {
		//isActive = 0;
		timeOutTicks = -1;
		hide = true;
	}
}

void SplashImage::Toggle() {
	if (isActive)
		Hide();
	else
		Show();
}

void SplashImage::Update() {
	if (isActive && bg != NULL) {
		if (!hide) {
			alpha = (alpha+10.0f < 255.0f ? alpha+10.0f : 255.0f);

			//if (KeyPressed(VK_RETURN))
			//	Hide();

			if (timeOutTicks > -1)
				if ((--timeOutTicks) == 0)
					Hide();
		}
		else {
			alpha = (alpha-10.0f > 0.0f ? alpha-10.0f : 0.0f);
			if (alpha == 0.0f)
				isActive = false;
		}
	}
}

void SplashImage::Draw() {
	if (isActive && bg != NULL) {
		eng->render->SetColor(0, 0, 0, 255);
		eng->render->DrawRect(0, 0, SPLASH_W, SPLASH_H);

		eng->render->SetBlendMode(RBLEND_NORMAL, 1);
		eng->render->SetColor(255, 255, 255, (unsigned char)alpha);
		eng->render->DrawTexture(bg, float(x), float(y));
		eng->render->SetBlendMode(RBLEND_NORMAL);
	}
}

void SplashImage::SetImage(sf::Texture *tex) {
	bg = tex;
	sf::Vector2u sz = tex->getSize();

	if (bg) {
		x=320.0f - sz.x/2.0f;
		y=240.0f - sz.y/2.0f;
	}
}

SplashImage::~SplashImage() {
	if (isActive)
		Hide();
}

SplashImage::SplashImage() {
	alpha=0.0f;
	isActive=0;
	bg = NULL;
	hide = false;
}

SplashImage::SplashImage(sf::Texture *tex) {
	alpha=0.0f;
	isActive=0;
	bg = NULL;
	hide = false;
	SetImage(tex);
}
