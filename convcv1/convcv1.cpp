// convcv1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <new>
#include <list>
#include <string>
#include <boost/scoped_array.hpp>

void process_file (std::string input)
{
	unsigned char a = 0x8b;
	unsigned char b = 0x71;

	FILE* i = fopen (input.c_str(), "rb");
	std::string output = input.substr(0, input.length() - 3) + "cv1";
	FILE* o = fopen (output.c_str(), "wb");

	int c;
	while ((c = fgetc(i)) != EOF) {
		c ^= a;
		fputc(c, o);
		a += b;
		b -= 0x6b;
	}

	fclose (i);
	fclose (o);

	DeleteFile (input.c_str());
}

void find_file (std::string path)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile ((path + "*.tx1").c_str(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		int i = GetLastError();
		if (i != 2) printf ("FindFirstFile file failed %s (%d)\n", path.c_str(), i);
		return;
	}

	do {
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			continue;
		}

		process_file (path + std::string(FindFileData.cFileName));
	} while (FindNextFile(hFind, &FindFileData));
	FindClose(hFind);
}

void find_directory (std::string path)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile ((path + "*.*").c_str(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		printf ("FindFirstFile dir failed %s (%d)\n", path.c_str(), GetLastError());
		return;
	}

	do {
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (strcmp(FindFileData.cFileName, ".") == 0 || strcmp(FindFileData.cFileName, "..") == 0)
				continue;

			find_file(path + std::string(FindFileData.cFileName) + '\\');
			find_directory(path + std::string(FindFileData.cFileName) + '\\');
		}
	} while (FindNextFile(hFind, &FindFileData));
	FindClose(hFind);
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc <= 1) {
		printf ("Usage : %s directory\n", argv[0]);
		return -1;
	}

	if (!SetCurrentDirectory(argv[1])) {
		printf ("SetCurrentDirectory error : %d\n", GetLastError());
		return -1;
	}

	find_file (std::string());
	find_directory (std::string());

	return 0;
}
