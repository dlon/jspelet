#pragma once
#ifndef RESOURCEFACTORY_H
#define RESOURCEFACTORY_H

#include <string.h>
#include <windows.h>

// search in linear time, O(n)

template <class T>
class ResourceFactory {
	struct ResList {
		char *fileName;
		T *t;
		ResList *next;
	} *mResources;
public:
	T *Load(const char *file) {
		T *egg=GetByFile(file);
		if (egg) // already exists?
			return egg;

		T *t = LoadRes(file);
		if (t) {
			// create new element
			ResList *mres = new ResList;
			mres->next = mResources;
			mResources = mres;
			mres->fileName = new char[strlen(file)+1];
			strcpy(mres->fileName, file);
			mres->t = t;
		}
		return t;
	}
	T *Add(const char *accessFilename, T *t) { // accessFilename: used by 'GetByFile'
		T *egg=ExistsID(t);
		if (egg)
			return egg;

		if (t) {
			// create new element
			ResList *mres = new ResList;
			mres->next = mResources;
			mResources = mres;
			mres->fileName = new char[strlen(accessFilename)+1];
			strcpy(mres->fileName, accessFilename);
			mres->t = t;
		}
		return t;
	}
	T *GetByFile(const char *file) {
		ResList *mtl = mResources;
		for (; mtl; mtl = mtl->next) {
			if (!strcmp(file, mtl->fileName))
				return mtl->t;
		}
		return 0;
	}
	T *ExistsID(T *t) {
		ResList *mtl = mResources;
		for (; mtl; mtl = mtl->next) {
			if (mtl->t == t)
				return mtl->t;
		}
		return 0;
	}
	void FreeByFile(const char *file) {
		ResList *mtl = mResources;
		ResList *prev = NULL;
		for (; mtl; mtl = mtl->next) {
			if (!strcmp(file, mtl->fileName))
			{
				FreeRes(mtl->t);
				delete [] mtl->fileName;
				
				if (prev)
					prev->next = mtl->next;
				else
					mResources = mtl->next;

				delete mtl;
				return;
			}
			prev = mtl;
		}
	}
	void FreeByID(T *t) {
		ResList *mtl = mResources;
		ResList *prev = NULL;
		for (; mtl; mtl = mtl->next) {
			if (t == mtl->t)
			{
				FreeRes(mtl->t);
				delete[] mtl->fileName;
				
				if (prev)
					prev->next = mtl->next;
				else
					mResources = mtl->next;

				delete mtl;
				return;
			}
			prev = mtl;
		}
	}
	void FreeAll() {
		while (mResources)
		{
			ResList *next = mResources->next;
			FreeRes(mResources->t);
			delete[] mResources->fileName;
			delete mResources;
			mResources = next;
		}
	}

	ResourceFactory() : mResources(0) {}
	//virtual ~ResourceFactory() { FreeAll(); }
	virtual ~ResourceFactory() {} // FIXME: Can't call pure virtual in destructor! (It leaks now, unless the child class calls it :() - Template inheritance?
protected:
	virtual T *LoadRes(const char *file) = 0;
	virtual void FreeRes(T *t) = 0;
};

#endif // RESOURCEFACTORY_H
