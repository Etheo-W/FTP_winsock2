#include "SocketManger.h"



SocketManger::SocketManger()
{
	this->socketCommond = new SocketCommand();

	this->socketFileTransfer = new SocketFileTransfer();
}


SocketManger::~SocketManger()
{
	delete this->socketCommond;

	delete this->socketFileTransfer;
}

int SocketManger::init(char *IPAddress)
{
	if (!this->socketCommond->initWinsock(IPAddress)) return 0;

	if (!this->socketFileTransfer->initWinsock(IPAddress)) return 0;

	return 1;
}

int SocketManger::work(char *IPAddress, REQUEST_STYLE requestStyle)
{
	File *file = new File(requestStyle);

	if (requestStyle == DOWNLOAD)
	{
		if (this->socketCommond->connect2FileServer())
		{
			this->socketCommond->sendCommandInfo(file, GETALLFILEHASH);

			this->socketCommond->receiveCommandInfo(file);

			AFHV allFHV = getAllFileHashValue();

			cout << "所有文件如下：\n";
			for (int i = 0; i < allFHV->length; i++)
			{
				printf("%d. %s\n", i + 1, allFHV->allFileHashValue[i]);
			}

			int fileHash = 0;
			cout << "请选择文件：";
			cin >> fileHash;
			strcpy(file->fileHashSha256, allFHV->allFileHashValue[fileHash - 1]);

			this->socketCommond->sendCommandInfo(file, requestStyle);

			this->socketCommond->receiveCommandInfo(file);
		}
	}
	
	if (requestStyle == UPLOAD)
	{
		file->getFileHashSha256();

		if (this->socketCommond->connect2FileServer())
		{
			this->socketCommond->sendCommandInfo(file, requestStyle);

			this->socketCommond->receiveCommandInfo(file);
		}
	}
	

	if ((requestStyle == UPLOAD && file->fileSentSize < file->fileSize) || (requestStyle == DOWNLOAD && file->fileReceivedSize < file->fileSize))
	{
		/*此处需改为多线程*/

		if (this->socketFileTransfer->connect2FileServer())
		{
			if (requestStyle == UPLOAD)
			{
				this->socketFileTransfer->sendFile(file);
			}

			if (requestStyle == DOWNLOAD)
			{
				this->socketFileTransfer->receiveFile(file);
			}
		}
		/*多线程结束*/
	}

	delete file;

	return 1;
}

int SocketManger::stop()
{
	this->socketCommond->closeCommandSocket();

	this->socketFileTransfer->closeFileSocket();

	return 1;
}