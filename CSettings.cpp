#include "CSettings.h"
#include <sstream>
#include <string>
#include <stdlib.h> // getenv

#ifdef _WIN32
#include <windows.h> // CreateDirectory
#define APPDIR_APPNAME "JackeSpelet"
#else
#include <sys/stat.h> // mkdir
#define APPDIR_APPNAME ".JackeSpelet"
#endif

std::string CSettings::appDir;
bool CSettings::fullscreen = true;			// default

const char *CSettings::GetAppDir()
{
	if (appDir.empty()) {
		const char* appData = 0;
		std::stringstream path;

#ifdef _WIN32
		path << getenv("APPDATA") << "/" << APPDIR_APPNAME;
		appDir = path.str();
		CreateDirectory(appDir, NULL); // create directory if necessary
#elif __linux__
		path << getenv("HOME") << "/" << APPDIR_APPNAME;
		appDir = path.str();
		mkdir(appDir.c_str(), 0755); // create directory if necessary. FIXME: handle errors
#else
		#warning "Don't know what environment variable to use. Using working directory to store data."
		appDir = "./data/user";
		mkdir(appDir.c_str(), 0755); // this may not work
#endif
	}
	return appDir.c_str();
}

void CSettings::ReadSettings(Jack *jack)
{
	std::string s(GetAppDir());
	s += "/jack.jfg";

	if (!FileExists(s.c_str()))
		return;

	Archiver arc(s.c_str(), true);

	// read cfg
	while (!arc.EndOfFile()) {
		std::string s;
		arc >> s;

		if (!arc.EndOfFile()) {
			if (s == "MusicVol") {
				arc >> jack->musicVol;
			}
			else if (s == "SfxVol") {
				arc >> jack->sfxVol;
			}
			else if (s == "Fullscreen") {
				arc >> fullscreen;
			}
		}
	}
}

bool CSettings::ReadFullscreen()
{
	std::string s(GetAppDir());
	s += "/jack.jfg";

	if (!FileExists(s.c_str()))
		return fullscreen;

	Archiver arc(s.c_str(), true);

	// read cfg
	while (!arc.EndOfFile()) {
		std::string s;
		arc >> s;

		if (!arc.EndOfFile()) {
			if (s == "MusicVol") {
				float f;
				arc >> f;
			}
			else if (s == "SfxVol") {
				float f;
				arc >> f;
			}
			else if (s == "Fullscreen") {
				arc >> fullscreen;
				break;
			}
		}
	}
	return fullscreen;
}

void CSettings::SaveSettings(Jack *jack)
{
	std::string s(GetAppDir());
	s += "/jack.jfg";

	Archiver arc(s.c_str(), false);

	// write cfg
	arc << "MusicVol" << jack->musicVol <<
		"SfxVol" << jack->sfxVol <<
		"Fullscreen" << fullscreen;
}

const char * CSettings::GameStateFile()
{
	static std::string sv;
	if (sv.empty()) {
		sv = GetAppDir();
		sv += "/state.jks";
	}
	return sv.c_str();
}

bool CSettings::GameStateExists()
{
	FILE *fp = fopen(GameStateFile(), "rb");
	if (fp) {
		fclose(fp);
		return true;
	}
	return false;
}
