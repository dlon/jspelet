#include "JackHUD.h"
#include "Engine.h"
#include "Input.h"
#include "Jack.h"

extern Engine *eng;
extern Input *input;

extern int tticks;
extern std::string mapLoadAfterSplash;

JackHUD::JackHUD() : player(0), boss(0)
{
	healthMax = 100.0f;
	health = 60.0f;
	
	healthBarX = healthBarY = 5.0f;
	healthBarW = 100.0f;
	healthBarH = 25.0f;
	healthBarCur = 0.0f;

	lowHealthSign = false;
	lowHealthA = 0;
	lowHealthFrames = 0;
	lowSubtract = false;
	lowHealthMeansCheckPoint = false;

	/*
	heartContainers = 5;
	hierta.Load("data/hierta.png", 2, 36, 0);
	*/
	heartContainers = 2; // extra lives
	hierta.Load("data/hrt.PNG", 1, 0,0);
}

int JackHUD::GetLives()
{
	return heartContainers;
}
void JackHUD::SetLives(int lives)
{
	heartContainers = lives;
}

void JackHUD::OneUp()
{
	heartContainers++;
}

void JackHUD::OneDown()
{
	heartContainers--;
	if (heartContainers < 0) {
		// game over
		heartContainers = 2;

		eng->jack->SetSongOnce("data/over.ogg");
		int x = tticks;
		tticks = -1;
		eng->SetSplash("data/over.png");
		tticks = x;

		mapLoadAfterSplash = eng->jack->map.MapFile();
	}
}

void JackHUD::Step()
{
	if (player) {
		float prevHealth = health;
		health = player->GetHealth();

		if (prevHealth >= 25.0f && health < 25.0f) {
			lowHealthSign = true;
			lowHealthMeansCheckPoint = false;
			lowHealthFrames = 360;
			lowHealthA = 0;
			lowSubtract = false;
		}
	}

	if (lowHealthSign) {
		lowHealthA += lowSubtract ? -10.0f : 10.0f;
		if (lowHealthA <= 0.0f) {
			lowSubtract = false;
			lowHealthA = 0.0f;
		}
		if (lowHealthA >= 255.0f) {
			lowSubtract = true;
			lowHealthA = 255.0f;
		}

		if (--lowHealthFrames <= 0)
			lowHealthSign = false;
	}

	if (healthBarCur < health)
		healthBarCur += 1.0f;
	else if (healthBarCur > health)
		healthBarCur -= 1.0f;

	if (healthBarCur < 0.0f)
		healthBarCur = 0;
	else if (healthBarCur > healthMax)
		healthBarCur = healthMax;

	if (boss)
		boss->HudStep();
}

void JackHUD::CheckPoint()
{
	lowHealthSign = true;
	lowHealthMeansCheckPoint = true;
	lowHealthFrames = 360;
	lowHealthA = 0;
	lowSubtract = false;
}

void JackHUD::Draw()
{
	eng->render->NoViewBegin();
	{
		eng->render->SetColor(0,0,0);
		eng->render->DrawRect(healthBarX, healthBarY, healthBarW, healthBarH);
		eng->render->SetColor(255,255,255);
		eng->render->DrawRect(healthBarX+3.0f, healthBarY+3.0f, healthBarW-6.0f, healthBarH-6.0f);
		eng->render->SetColor(255,0,0);
		eng->render->DrawRect(healthBarX+5.0f, healthBarY+5.0f, (healthBarW-10.0f)*(healthBarCur/healthMax), healthBarH-10.0f);
		/*
		eng->render->SetColor(255,255,255);
		for (int i=0; i<heartContainers; i++)
			eng->render->DrawSubImage(hierta[1], healthBarX + (float)i*36.0f, healthBarY);
		for (int i=0; i<(int)(health/healthMax*(float)heartContainers); i++)
			eng->render->DrawSubImage(hierta[0], healthBarX + (float)i*36.0f, healthBarY);
		*/

		// lives
		eng->render->SetColor(255,255,255);
		hierta.x = healthBarX;
		hierta.y = healthBarY + 33.0f;
		hierta.Draw();
		
		eng->jack->font.SetScale(1.0f);
		eng->render->SetColor(0,0,0);
		eng->jack->font.Printf(healthBarX+32.0f, healthBarY+38.0f, "%d", heartContainers);
		eng->render->SetColor(255,255,255);
		eng->jack->font.Printf(healthBarX+31.0f, healthBarY+37.0f, "%d", heartContainers);

		// health warning
		if (lowHealthSign) {
			eng->jack->font.SetScale(1.0f);
			if (!lowHealthMeansCheckPoint) {
				eng->render->SetColor(255,255,0,(unsigned char)lowHealthA);
				eng->jack->font.Printf(healthBarX+healthBarW+5.0f, healthBarY+5.0f, "Low Health!");
			}
			else {
				eng->render->SetColor(0,0,255,(unsigned char)lowHealthA);
				eng->jack->font.Printf(healthBarX+healthBarW+5.0f, healthBarY+5.0f, "Checkpoint!");
			}
		}

		if (boss)
			boss->HudDraw();
	}
	eng->render->NoViewEnd();
}

void JackHUD::SetBoss(BossInfo *bi)
{
	boss = bi;
}

//
// boss hud
//
BossInfo::BossInfo()
{
	healthBarX = 535.0f;
	healthBarY = 5.0f;
	healthBarW = 100.0f;
	healthBarH = 25.0f;
	healthBarCur = 0;
}
void BossInfo::HudStep()
{
	if (healthBarCur < GetHealth())
		healthBarCur += floor(GetMaxHealth()/100.0f);
	else if (healthBarCur > GetHealth())
		healthBarCur -= floor(GetMaxHealth()/100.0f);

	if (fabs(healthBarCur - GetHealth()) <= floor(GetMaxHealth()/100.0f)) { // precision fix
		healthBarCur = GetHealth();
	}

	if (healthBarCur < 0.0f)
		healthBarCur = 0;
	else if (healthBarCur > GetMaxHealth())
		healthBarCur = GetMaxHealth();
}
void BossInfo::HudDraw()
{
	eng->render->SetColor(0,0,0);
	eng->render->DrawRect(healthBarX, healthBarY, healthBarW, healthBarH);
	eng->render->SetColor(255,255,255);
	eng->render->DrawRect(healthBarX+3.0f, healthBarY+3.0f, healthBarW-6.0f, healthBarH-6.0f);
	eng->render->SetColor(0,0,255);
	eng->render->DrawRect(healthBarX+5.0f, healthBarY+5.0f, (healthBarW-10.0f)*(healthBarCur/GetMaxHealth()), healthBarH-10.0f);
}
