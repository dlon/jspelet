#include "Archiver.h"
#include <assert.h>
#include <iostream>
#include <windows.h>

Archiver& Archiver::operator<<(int i)
{
	assert(write);
	fwrite(&i, sizeof(int), 1, fp);
	return *this;
}

Archiver& Archiver::operator<<(unsigned int i)
{
	assert(write);
	fwrite(&i, sizeof(int), 1, fp);
	return *this;
}

Archiver& Archiver::operator <<(float i)
{
	assert(write);
	fwrite(&i, sizeof(float), 1, fp);
	return *this;
}

Archiver& Archiver::operator <<(double i)
{
	assert(write);
	fwrite(&i, sizeof(double), 1, fp);
	return *this;
}

Archiver& Archiver::operator <<(const char *str)
{
	assert(write);
	size_t len = strlen(str);
	(*this) << (unsigned int)len;
	fwrite(str, 1, len, fp);
	return *this;
}

Archiver& Archiver::operator<<(bool b)
{
	assert(write);
	fwrite(&b, sizeof(b), 1, fp);
	return *this;
}

Archiver& Archiver::operator >>(int &x)
{
	assert(!write);
	fread(&x, sizeof(int), 1, fp);
	return *this;
}

Archiver& Archiver::operator >>(float &x)
{
	assert(!write);
	fread(&x, sizeof(float), 1, fp);
	return *this;
}

Archiver& Archiver::operator >>(double &x)
{
	assert(!write);
	fread(&x, sizeof(double), 1, fp);
	return *this;
}

Archiver& Archiver::operator >>(bool &x)
{
	assert(!write);
	fread(&x, sizeof(x), 1, fp);
	return *this;
}

Archiver& Archiver::operator >>(std::string &x)
{
	assert(!write);
	unsigned int len = 0;
	fread(&len, sizeof(int), 1, fp);

	x.clear();
	x.reserve(len);
	for (unsigned int i=0; i<len; i++)
		x += ((char)fgetc(fp));
	return *this;
}

Archiver& Archiver::WriteByte(int ch)
{
	assert(write);
	fputc(ch, fp);
	return *this;
}

int Archiver::ReadByte()
{
	assert(!write);
	return fgetc(fp);
}

static char *FindAppStr(char *env)
{
#ifdef _WIN32
	char *ptr = env;
	while (*ptr != 0)
	{
		size_t len = strlen(ptr);
		if (len > 7 && !strncmp(ptr, "APPDATA", 7))
			return ptr+8;
		ptr += len+1;
	}
#endif
	return 0;
}

std::string& Archiver::GetAppDir(std::string &str)
{
	char *env = GetEnvironmentStrings();
	char *ret = FindAppStr(env);
	if (ret)
		str.assign(ret);
	FreeEnvironmentStrings(env);
	return str;
}
