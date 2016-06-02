#pragma once

//
//	show scoreboard, wait, and fade
//

class ScoreBoard
{
	float x,y;
	float xw,yw;
	float x_fw,y_fw;
	int a,b;

	bool allowCursor;
	int screenTime;
	int screen;
	int cursorPos;
	int pick;
	int prev;
public:
	ScoreBoard();

	void Draw();
	bool Update();

	int Select();

	bool Done() { return screen == 255; }

	const char *nextMap; // hack
};
