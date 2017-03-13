#include "SocketManger.h"

SocketManger::SocketManger()
{
	this->socketCommond = new SocketCommand();
	this->socketFileTransfer = new SocketFileTransfer();
	this->allFileHashValue = getAllFileHashValue();
}


SocketManger::~SocketManger()
{
	delete this->socketCommond;
	delete this->socketFileTransfer;
	deleteAllFileHashValue(this->allFileHashValue);
}

int SocketManger::init()
{
	this->socketCommond->initWinsock();

	this->socketCommond->connect2FileServer();

	this->socketFileTransfer->initWinsock();

	this->socketFileTransfer->connect2FileServer();

	return 1;
}

int SocketManger::work()
{
	//WaitForSingleObject(this->hThread, INFINITE);

	File *file = new File();

	if (this->socketCommond->acceptCommandSocket())
	{
		this->newThread(file);
		/*
		if (!CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(this->newThread(file)), NULL, 0, NULL))
		{
			delete file;
			return 0;
		}
		*/
	}

	//Sleep(1000);
	//ReleaseMutex(this->hThread);

	//CloseHandle(this->hThread);

	return 1;
}

DWORD WINAPI SocketManger::newThread(LPVOID pParam)
{
	File *tmpFile = (File*)pParam;
	/*此处需改为多线程*/
	if (!this->socketCommond->respons(this->socketCommond->commandSocket, tmpFile)) return 0;
	this->socketCommond->commandSocket = INVALID_SOCKET;
	//socketCommond->hThread = CreateThread(NULL, 0, 
	//	(LPTHREAD_START_ROUTINE)(this->socketCommond->respons(this->socketCommond->commandSocket, file)), NULL, 0, 0);

	//Sleep(1000);
	/*多线程结束*/

	if ((tmpFile->fileOperateStyle == UPLOAD && tmpFile->fileSentSize < tmpFile->fileSize) || (tmpFile->fileOperateStyle == DOWNLOAD && tmpFile->fileReceivedSize < tmpFile->fileSize))
	{
		if (this->socketFileTransfer->acceptFileSocket())
		{
			//*此处需改为多线程
			if (!this->socketFileTransfer->transfer(this->socketFileTransfer->fileSocket, tmpFile)) return 0;
			this->socketFileTransfer->fileSocket = INVALID_SOCKET;
			//socketFileTransfer->hThread = CreateThread(NULL, 0, 
			//	(LPTHREAD_START_ROUTINE)(this->socketFileTransfer->transfer(this->socketFileTransfer->fileSocket, file)), NULL, 0, 0);

			//Sleep(1000);

			//*多线程结束
		}
	}
	

	return 1;
}