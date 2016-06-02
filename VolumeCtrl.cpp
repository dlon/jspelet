#include "VolumeCtrl.h"
#include "Renderer.h"
#include "Engine.h"
#include "Jack.h"
#include "Input.h"

#define CURSOR_SPEED 2.0f

extern Engine	*eng;
extern Input	*input;

unsigned short VolumeCtrl::refT = 0;
sf::Texture *VolumeCtrl::volT = 0;

VolumeCtrl::VolumeCtrl(float min, float max, float def)
{
	m_vol = def;
	m_maxVol = max;
	m_minVol = min;
	m_selected = false;

	// load texture
	if (!refT) // if (!refT++)
		volT = new sf::Texture;
		volT->loadFromFile("data/volctrl.png");
	refT++;

	// load sprites
	volMeter.LoadHorizontal(volT, 2, 136, 48, 8, 0);
	//volCur.Load(volT, 1, 48, 48, 2, 96);
	volCur.Load(volT, 1, 16, 45, 2, 96);
}
VolumeCtrl::~VolumeCtrl()
{
	// free textures
	if (!(--refT)) {
		delete volT;
		volT = NULL;
	}
}

void VolumeCtrl::SetVol(float nv)
{
	m_vol = nv;
}

float VolumeCtrl::GetVol()
{
	return m_vol;
}

void VolumeCtrl::Left()
{
	if (m_vol - CURSOR_SPEED >= m_minVol) m_vol -= CURSOR_SPEED;
	else m_vol = m_minVol;
}
void VolumeCtrl::Right()
{
	if (m_vol + CURSOR_SPEED <= m_maxVol) m_vol += CURSOR_SPEED;
	else m_vol = m_maxVol;
}

void VolumeCtrl::Tick()
{
	if (!m_selected)
		return;

	if (input->Check(sf::Keyboard::Right)) { // TODO: 'CheckRepeat'
		if (m_vol + CURSOR_SPEED <= m_maxVol) m_vol += CURSOR_SPEED;
		else m_vol = m_maxVol;
	}
	if (input->Check(sf::Keyboard::Left)) {
		if (m_vol - CURSOR_SPEED >= m_minVol) m_vol -= CURSOR_SPEED;
		else m_vol = m_minVol;
	}
}

void VolumeCtrl::Draw(float x, float y)
{
	float ccv = m_vol;
	if (m_vol == 0.0f)
		ccv = 0.001f; // div by 0

	volCur.xofs = ccv / (m_maxVol - m_minVol) * (float)volMeter.GetWidth(); // fixme
	
	// draw potential area
	volMeter.imgInd = 0.0f;
	volMeter.x = x; volMeter.y = y;
	volMeter.Draw();

	// draw shaded area (selected)
	volMeter.imgInd = 1.0f;
	volMeter.DrawEx(x, y, 0.0f, 0.0f, volCur.xofs, (float)volMeter.GetHeight()); // draw till cursor

	// draw cur
	volCur.x = x - 7.0f;
	volCur.y = y;
	volCur.Draw();
}

void VolumeCtrl::Select()
{
	m_selected = true;
}
void VolumeCtrl::Deselect()
{
	m_selected = false;
}
