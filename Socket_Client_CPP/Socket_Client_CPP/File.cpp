#include "File.h"

#define TMP_BUFFER_SIZE 419430


File::File(FILE_OPERATE_STYLE requestStyle)
{
	for (int i = 0; i < MAX_PATH; i++)
	{
		this->filePath[i] = '\0';
		this->fileName[i] = '\0';
		this->fileFullPath[i] = '\0';
	}

	for (int i = 0; i < 65; i++)
	{
		this->fileHashSha256[i] = '\0';
	}

	string fileTmpPath, fileTmpName;
	cout << "请输入文件路径:";
	cin >> fileTmpPath;

	cout << "请输入文件名:";
	cin >> fileTmpName;

	strcpy(this->filePath, fileTmpPath.c_str());
	strcpy(this->fileName, fileTmpName.c_str());

	this->getFileFullPath();

	this->fileSentSize = 0;
	this->getFileSize(requestStyle);
	this->fileReceivedSize = this->fileSize;
}


File::~File()
{
}

void File::getFileHashSha256()
{
	if (!this->openFile(R)) return;

	int tmpBufferLength = 0;

	char *tmpBuffer = (char*)malloc(TMP_BUFFER_SIZE * sizeof(char));

	sha256_ctx context;

	sha256_init(&context);

	while (tmpBufferLength = this->readFile(tmpBuffer, TMP_BUFFER_SIZE))
	{
		sha256_process(&context, (uint8_t*)tmpBuffer, tmpBufferLength);
	}

	uint8_t out[32]; //b93d e54b 9f3e fd86 
					 //7f9d d6ed 5331 1de0 
					 //2e5e 36b5 094b 7ef8 
					 //401b 75b2 f8cd df68

	sha256_final(&context, out);

	char tmpString[3] = { '\0' };

	for (int i = 0; i < 32; i++)
	{
		//int2String((int)out[i], &tmpString, 16);
		sprintf(tmpString, "%02x", out[i]);
		//tmpString = to_string(out[i]);
		strcat(this->fileHashSha256, tmpString);
	}

	free(tmpBuffer);

	this->closeFile();
}

int File::getFileSize(FILE_OPERATE_STYLE style)
{
	if (style)
	{
		if (!this->openFile(A))
		{
			if (!this->openFile(W))
			{
				this->closeFile();
				return 0;
			}
		}
	}
	else
	{
		if (!this->openFile(R))
		{
			this->closeFile();
			return 0;
		}
	}
	
	if (!this->seekFile(DOWNLOAD)) 
	{
		this->closeFile();
		return 0;
	}
	
	this->fileSize = ftell(this->fp);	//ftell 用于得到文件位置指针当前位置相对于文件首的偏移字节数；
										//2.1G以上的文件操作时可能出错
	this->closeFile();

	return 1;
}

void File::getFileFullPath()
{
	strcat(this->fileFullPath, this->filePath);
	strcat(this->fileFullPath, "\\");
	strcat(this->fileFullPath, this->fileName);
}

int File::openFile(FILE_USAGE_MODE mode)
{
	this->fp = fopen(this->fileFullPath, getFileUsageMode(mode));
	if (this->fp == NULL)
	{
		printf("无法打开此文件！\n");

		return 0;
	}
	return 1;
}

int File::readFile(char* str, int n)
{
	return fread(str, sizeof(char), n, this->fp);
}

int File::writeFile(char* str, int n)
{
	return fwrite(str, sizeof(char), n, this->fp);
}

int File::closeFile()
{
	return fclose(this->fp) + 1;	//fclose成功，返回0；失败，返回EOF（-1）
}

int File::seekFile(FILE_OPERATE_STYLE style)
{
	if (style)
	{
		if (!fseek(this->fp, 0, SEEK_END)) return  1;	//DOWNLOAD 定位到文件末尾
	}
	
	if (!fseek(this->fp, this->fileSentSize, 0)) return  1;	//UPLOAD 定位到文件已发送完成部分的末尾

	return 0;	//定位成功，返回1；失败，返回0
}