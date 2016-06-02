#pragma once
#include "Sprite.h"

class VolumeCtrl {
	float	m_vol;
	float	m_maxVol,m_minVol;
	bool	m_selected;

	static sf::Texture *volT;
	static unsigned short refT;

	Sprite	volMeter;
	Sprite	volCur;
public:
	VolumeCtrl(float min, float max, float def=0.0f);
	~VolumeCtrl();

	// use auto_ptr texture?

	void	SetVol(float nv);
	float	GetVol();

	void	Select();
	void	Deselect();
	void	Left();		// alternative to Tick()
	void	Right();

	void	Tick();
	void	Draw(float x, float y);
};
