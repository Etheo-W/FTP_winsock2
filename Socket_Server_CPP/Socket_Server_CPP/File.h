#pragma once
#ifndef _File_H_
#define _File_H_
#endif // !_File_H_

#include "stdafx.h"
extern "C" {
#include "AllFileHashValue.h"
}
#include <fstream>

typedef enum fileUsageMode { R = 0, W, A } FILE_USAGE_MODE;

typedef enum { UPLOAD = 0, DOWNLOAD, GETALLFILEHASH } FILE_OPERATE_STYLE;

#define getFileUsageMode(FILE_USAGE_MODE) this->fileUsageModeName[FILE_USAGE_MODE]

class File
{
private:
	const char* fileUsageModeName[3] = { "rb", "wb", "ab" };
public:
	char fileFullPath[MAX_PATH];

	uint32_t fileSize;
	uint32_t fileSentSize;
	uint32_t fileReceivedSize;
	char fileHashSha256[65];

	FILE_OPERATE_STYLE fileOperateStyle;

	FILE *fp;
public:
	File();
	~File();

	int		File::openFile(FILE_USAGE_MODE mode);
	int		File::readFile(char* str, int n);
	int		File::writeFile(char* str, int n);
	int		File::closeFile();
	int		File::seekFile(FILE_OPERATE_STYLE style);
	int		File::getFileSize(FILE_OPERATE_STYLE style);
	void	File::getFileFullPath();
};

