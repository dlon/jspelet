#pragma once

#include <stdio.h>
#include <string>

// should it save the WHOLE map? I mean, to avoid spawning entities and have altering tiles etc. might be a good idea...

class FileExists
{
	bool exists;
public:
	FileExists(const char *f) : exists(false) {
		FILE *fp = fopen(f, "rb");
		if (fp) {
			exists = true;
			fclose (fp);
		}
	}

	operator bool() const { return exists; }
};

class Archiver
{
	FILE *fp;
	bool write;
public:
	Archiver(const char *file, bool read) { fp=fopen(file, read?"rb":"wb"); write=!read; }
	~Archiver() { if (fp) fclose(fp); }

	bool Readable() { return !write; }
	bool Writable() { return write; }
	bool EndOfFile() { return feof(fp) != 0; }

	Archiver&	WriteByte(int ch);
	int			ReadByte();
	
	Archiver& operator<<(bool);
	Archiver& operator<<(int);
	Archiver& operator<<(unsigned int);
	Archiver& operator<<(float);
	Archiver& operator<<(double);
	Archiver& operator<<(const char*); // prefixed
	
	Archiver&	operator>>(bool&);
	Archiver&	operator>>(int&);
	Archiver&	operator>>(float&);
	Archiver&	operator>>(double&);
	Archiver&	operator>>(std::string&);
};
