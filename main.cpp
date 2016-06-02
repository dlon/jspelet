#include <windows.h>
#include "Engine.h"
#include "Jack.h"
#include "Input.h"
#include <string>
#include <sstream>
#include "resource.h"

#define GAME_CAPTION "Jackespele!"
#define GAME_CLASS_NAME "jjkbspele"

bool lWindowedMode = true;
bool lMapParameter = false;
bool lPosParameter = false;
bool lInvicParameter = false;

extern std::string songAfterSplash;
extern int tticks;

// globals
//Engine	*eng = 0;
extern Engine *eng;
Input	*input = 0;

struct PlPosParam {
	float x;
	float y;

	PlPosParam(const char *str) {
		if (!str) {
			x = y = 0;
			return;
		}
		const char *p = strchr(str, ',');
		if (p) {
			std::stringstream	bx,by;
			bx.write(str, p-str);
			by.write(p+1, strlen(p+1));
			bx >> x;
			by >> y;
			return;
		}
		x = y = 0;
	}
};

static std::string		initialMap;
PlPosParam				initialPos(NULL);

inline void SpawnPos(int id, float x, float y)
{
	Entity *e = eng->jack->map.GetEntities().CreateEntity(id);
	e->x = eng->render->cam.x + x;
	e->y = eng->render->cam.y + y;
}

LRESULT CALLBACK WndProc(HWND hWnd,
						 UINT Msg,
						 WPARAM wParam,
						 LPARAM lParam)
{
#ifdef JSDEBUG
	static POINT mousePos = { 0, 0 };
#endif
	switch (Msg)
	{
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		case WM_KEYDOWN:
			/*
			if (wParam == VK_ESCAPE)
				PostQuitMessage(0);
			*/
			if (wParam == VK_F9)
				SpawnPos(EID_ENDMAP, eng->render->cam.x, eng->render->cam.y);
			input->HitKey((int)wParam);
			break;
		case WM_KEYUP:
			input->RelKey((int)wParam);
			break;
		case WM_KILLFOCUS:
			if (!lWindowedMode)
			{
				ChangeDisplaySettings(0, 0); // reset res on lost focus
				ShowCursor(TRUE);
			}
			break;
		case WM_SETFOCUS:
			{
				if (!lWindowedMode)
				{
					DEVMODE dm;
					dm.dmSize = sizeof(dm);
					dm.dmDriverExtra = 0;
					dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
					dm.dmPelsWidth = 640;
					dm.dmPelsHeight = 480;
					dm.dmDisplayFrequency = 60;
					ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
					ShowCursor(FALSE);
				}
			}
			break;
		case WM_MOUSEMOVE:
			input->SetMousePosition( (float)LOWORD(lParam), (float)HIWORD(wParam) );
			break;
#ifdef JSDEBUG
		case WM_COMMAND:
			// spawn menu
			switch (LOWORD(wParam))
			{
			case ID_PLATFORMS_UP:
				SpawnPos(EID_MV_PLATFORM_U, mousePos.x, mousePos.y);
				break;
			case ID_PLATFORMS_RIGHT:
				SpawnPos(EID_MV_PLATFORM_R, mousePos.x, mousePos.y);
				break;
			case ID_PLATFORMS_LEFT:
				SpawnPos(EID_MV_PLATFORM_L, mousePos.x, mousePos.y);
				break;
			case ID_PLATFORMS_DOWN:
				SpawnPos(EID_MV_PLATFORM_D, mousePos.x, mousePos.y);
				break;
			case ID_MISC_BARREL:
				SpawnPos(EID_BARREL, mousePos.x, mousePos.y);
				break;
			case ID_MISC_ENDMAP:
				SpawnPos(EID_ENDMAP, mousePos.x, mousePos.y);
				break;
			case ID_MISC_BOSS2WALL:
				SpawnPos(EID_BOSS2DOOR, mousePos.x, mousePos.y);
				break;
			case ID_PUSHABLE_BOX:
				SpawnPos(EID_WOODENBOX, mousePos.x, mousePos.y);
				break;
			case ID_PUSHABLE_RESPAWNABLEBOX:
				SpawnPos(EID_WOODENBOX_RESPAWN, mousePos.x, mousePos.y);
				break;
			case ID_ENEMIES_KATTN:
				SpawnPos(EID_KATTN, mousePos.x, mousePos.y);
				break;
			case ID_ENEMIES_COPYCAT:
				SpawnPos(EID_COPYCAT, mousePos.x, mousePos.y);
				break;
			case ID_ENEMIES_PIGGATTACK:
				SpawnPos(EID_PIGGATTACK, mousePos.x, mousePos.y);
				break;
			default:
				return DefWindowProc(hWnd, Msg, wParam, lParam);
			}
			break;
		case WM_RBUTTONDOWN:
			{
				HMENU Popup;
				Popup = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU1));
				Popup = GetSubMenu(Popup, 0); 
				GetCursorPos(&mousePos);
				TrackPopupMenuEx(Popup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, mousePos.x, mousePos.y, hWnd, NULL);
				DestroyMenu(Popup);

				RECT rc;
				GetWindowRect(hWnd, &rc);
				mousePos.x -= rc.left;
				mousePos.y -= rc.top;
			}
			break;
#endif
		default:
			return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	return 0;
}

HWND CreateGameWindow(HINSTANCE hInstance)
{
	WNDCLASS wc;
	wc.style			= CS_OWNDC;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= GAME_CLASS_NAME;
	RegisterClass(&wc);

	HWND hWnd;
	if (lWindowedMode)
	{
		DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		RECT WRect = {0, 0, 640, 480};
		AdjustWindowRect(&WRect, dwStyle, 0);
		
		hWnd = CreateWindow(GAME_CLASS_NAME,
			GAME_CAPTION,
			dwStyle,
			CW_USEDEFAULT, CW_USEDEFAULT,
			WRect.right - WRect.left,
			WRect.bottom - WRect.top,
			NULL,
			NULL,
			hInstance,
			NULL);
	}
	else
	{
		hWnd = CreateWindow(GAME_CLASS_NAME,
			GAME_CAPTION,
			WS_POPUP,
			CW_USEDEFAULT, CW_USEDEFAULT,
			640, 480,
			NULL,
			NULL,
			hInstance,
			NULL);
	}
	if (hWnd)
	{
		ShowWindow(hWnd, SW_SHOWNORMAL);
		UpdateWindow(hWnd);
	}
	return hWnd;
}

// Process command-line
static
const char *SlowTok(const char *s, int tok, int i) {
	for (int c=0; c<i; c++) {
		for (;*s != 0 && *s != tok; s++) {}
	}
	return s;
}

static
void ProcessCmdLine(LPSTR lpCmdLine) {
	if (!lpCmdLine)
		return;

	char *str = strtok((char*)lpCmdLine," ");
	if (!str)
		return;
	
	bool mapArg = false;
	bool startArg = false;

	do {
		if (!mapArg && !startArg /* && ... */) {
			if (strcmp("-map", str) == 0) {
				mapArg = true;
				continue;
			}
			if (strcmp("-startpos", str) == 0) {
				startArg = true;
				continue;
			}
			if (strcmp("-invic", str) == 0) {
				lInvicParameter = true;
				continue;
			}
		}

		// args
		if (mapArg) {
			initialMap = str;
			lMapParameter = true;
		}
		else if (startArg) {
			initialPos = PlPosParam(str);
			lPosParameter = true;
		}

		// reset params
		mapArg = false;
		startArg = false;
	} while (str = strtok(NULL, " "));
}

//
// WinMain function
//
int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int CmdShow)
{
	lWindowedMode = !CSettings::ReadFullscreen();

	// create window
	HWND hWnd = CreateGameWindow(hInstance);
	if (!hWnd)
	{
		MessageBox(NULL, "Could not create window.", 0, MB_ICONERROR);
		return 1;
	}
	
	input = new Input;
	/*eng =*/ new Engine(hWnd);

	if (!lWindowedMode)
		eng->render->SetVsync(true); // NOTE: always vsync in fullscreen?

	ProcessCmdLine(lpCmdLine);

	if (initialMap != "")
		eng->jack->SetInitialMap(initialMap.c_str());

	// intro
	tticks = 180;
	songAfterSplash = "data/shjak.ogg";
	eng->SetSplash("data/Pres.png");
	tticks = 420;

	// main loop
	MSG msg;
	while (1) {
		// Windows events
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				// Exit program
				break;
			}
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			if (eng->Frame())
				input->UnPress();
			//Sleep(20);
		}
	}
	delete eng;
	delete input;
	return 0;
}

int main(int argc, char **argv)
{
	// concatenate argv to str
	int len = 0;
	char *cmd = 0;
	for (int i=0; i<argc; i++)
		len += (int)strlen(argv[i])+1;

	if (len) {
		int pos = 0;
		cmd = new char[len];

		for (int i=0; i<argc; i++) {
			strcpy(cmd+pos, argv[i]);

			pos += (int)strlen(argv[i]) + 1;
			cmd[pos-1] = ' '; // replace null char with space
		}
		cmd[pos-1] = 0; // replace null space with null char
	} else {
		// empty string
		cmd = new char[1];
		cmd[0] = 0;
	}

	// run game
	int res = WinMain(GetModuleHandle(NULL), 0, cmd, 0);

	delete [] cmd;
	return res;
}
