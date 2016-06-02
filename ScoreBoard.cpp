#include "ScoreBoard.h"
#include "Jack.h"
#include "Input.h"

extern Engine *eng;
extern Input *input;

ScoreBoard::ScoreBoard()
{
	x = 60.0f;
	y = 96.0f;

	xw = 512.0f;
	yw = 278.0f;

	allowCursor = false;
	cursorPos = 0;
	pick = -1;

	screen = b = a = 0;

	x_fw = y_fw = 0.0f;

	//screenTime = eng->time + 5000;
	screenTime = 0;
	prev = 0;
}

static const int timeStep = 16;

bool ScoreBoard::Update()
{
	if (!prev)
		prev = eng->time;

	static int pp = 0;
	pp += eng->time - prev;

	bool ret = false;

	if (pp >= timeStep) {
		if (pp > 500) pp = timeStep; // freeze fix

		for (; pp >= timeStep; pp-=timeStep) {
			if (x_fw < xw)
				x_fw+=7.0f;
			if (y_fw < yw)
				y_fw+=7.0f;
			if (a < 255)
				a = (a+7<255?a+7:255);
			else if (b < 255)
				b = (b+7<255?b+7:255);
			else if (!allowCursor)
				allowCursor = true;

			if (allowCursor && pick == -1) {
				if (input->CheckPressed(sf::Keyboard::Up) && cursorPos > 0)
					cursorPos--;
				if (input->CheckPressed(sf::Keyboard::Down) && cursorPos < 1)
					cursorPos++;

				if (input->CheckPressed(sf::Keyboard::Return)) {
					pick = cursorPos;
					screenTime = eng->time + 50;
				}
			}

			if (screenTime && eng->time >= screenTime) {
				screen = screen+7<255?screen+7:255;
			}
		}
		ret = true;
	}
	prev = eng->time;
	return ret;
}

int ScoreBoard::Select()
{
	int ret = pick;
	pick = -1;
	return ret;
}

void ScoreBoard::Draw()
{
	eng->render->NoViewBegin();

	if (screen < 255) {
		eng->render->SetColor(0,0,0);
		eng->render->DrawRect(x, y, x_fw, y_fw);

		eng->render->SetColor(255,255,255,(unsigned char)b);
		eng->jack->font.SetScale(2.0f);
		eng->jack->font.Draw(x + 50.0f, y + 75.0f, "Nice!");
		//eng->jack->font.Draw(x + 50.0f, y + 100.0f, "Statistics here and whatnot");

		eng->jack->font.SetScale(1.0f);

		if (allowCursor) {
			eng->render->SetColor(255,255,0,(unsigned char)b);
			eng->jack->font.Draw(x + 50.0f, y + 160.0f + float(cursorPos * 25), ">");
		}

		eng->render->SetColor(255,255,255,(unsigned char)b);
		eng->jack->font.Draw(x + 75.0f, y + 160.0f, "Save and continue");
		eng->jack->font.Draw(x + 75.0f, y + 185.0f, "Continue without saving");
	}

	if (screen) {
		eng->render->SetColor(0,0,0,screen);
		eng->render->DrawRect(0,0,640.0f,480.0f);
	}

	eng->render->NoViewEnd();
}
