#include "File.h"
#include <stdlib.h>

#define TMP_BUFFER_SIZE 1024

File::File()
{
	for (int i = 0; i < 65; i++)
	{
		this->fileHashSha256[i] = '\0';
	}

	for (int i = 0; i < MAX_PATH; i++)
	{
		this->fileFullPath[i] = '\0';
	}

	this->fileSentSize = 0;
	this->fileReceivedSize = 0;
	this->fileSize = 0;
}


File::~File()
{
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
	
	this->fileSize = ftell(this->fp);	//ftell ���ڵõ��ļ�λ��ָ�뵱ǰλ��������ļ��׵�ƫ���ֽ�����
										//2.1G���ϵ��ļ�����ʱ���ܳ���
	this->closeFile();

	return 1;
}

void File::getFileFullPath()
{
	GetCurrentDirectory(MAX_PATH, this->fileFullPath);
	strcat(this->fileFullPath, "\\SocketServer\\File\\");
	strcat(this->fileFullPath, this->fileHashSha256);
}

int File::openFile(FILE_USAGE_MODE mode)
{
	this->fp = fopen(this->fileFullPath, getFileUsageMode(mode));
	if (this->fp == NULL)
	{
		printf("�޷��򿪴��ļ���\n");
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
	return fclose(this->fp) + 1;	//fclose�ɹ�������0��ʧ�ܣ�����EOF��-1��
}

int File::seekFile(FILE_OPERATE_STYLE style)
{
	if (style)
	{
		if (!fseek(this->fp, 0, 2)) return  1;	//DOWNLOAD ��λ���ļ�ĩβ
	}
	
	if (!fseek(this->fp, this->fileSentSize, 0)) return  1;	//UPLOAD ��λ���ļ��ѷ�����ɲ��ֵ�ĩβ

	return 0;	//��λ�ɹ�������1��ʧ�ܣ�����0
}