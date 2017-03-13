#include "SocketFileTransfer.h"

SocketFileTransfer::SocketFileTransfer()
{
	this->listenSocket = INVALID_SOCKET;
	this->fileSocket = INVALID_SOCKET;
	this->result = NULL;
}


SocketFileTransfer::~SocketFileTransfer()
{
}


int SocketFileTransfer::initWinsock()
{
	// Initialize Winsock
	this->iResult = WSAStartup(MAKEWORD(2, 2), &this->wsaData);
	if (this->iResult != 0) {
		printf("WSAStartup failed with error: %d\n", this->iResult);
		return 0;
	}

	ZeroMemory(&this->hints, sizeof(this->hints));
	this->hints.ai_family = AF_INET;
	this->hints.ai_socktype = SOCK_STREAM;
	this->hints.ai_protocol = IPPROTO_TCP;
	this->hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	this->iResult = getaddrinfo(NULL, SERVER_FILE_PORT, &this->hints, &this->result);
	if (this->iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", this->iResult);
		WSACleanup();
		return 0;
	}

	return 1;
}

int SocketFileTransfer::connect2FileServer()
{
	// Create a SOCKET for connecting to server
	this->listenSocket = socket(this->result->ai_family, this->result->ai_socktype, this->result->ai_protocol);
	if (this->listenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(this->result);
		WSACleanup();
		return 0;
	}

	// Setup the TCP listening socket
	this->iResult = bind(this->listenSocket, this->result->ai_addr, (int)this->result->ai_addrlen);
	if (this->iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(this->result);
		closesocket(this->listenSocket);
		WSACleanup();
		return 0;
	}

	freeaddrinfo(this->result);

	this->iResult = listen(this->listenSocket, SOMAXCONN);
	if (this->iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(this->listenSocket);
		WSACleanup();
		return 0;
	}

	

	// No longer need server socket
	//closesocket(this->listenSocket);

	return 1;
}

int SocketFileTransfer::acceptFileSocket()
{
	// Accept a client socket
	this->fileSocket = accept(this->listenSocket, NULL, NULL);
	
	if (this->fileSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(this->listenSocket);
		WSACleanup();
		return 0;
	}

	return 1;
}

int	SocketFileTransfer::transfer(SOCKET FTSocket, File *file)
{
	//WaitForSingleObject(this->hThread, INFINITE);

	if (file->fileOperateStyle == DOWNLOAD)
	{
		if (!this->receiveFile(FTSocket, file)) return 0;
	}

	if (file->fileOperateStyle == UPLOAD)
	{
		if (!this->sendFile(FTSocket, file)) return 0;
	}

	//this->closeFileSocket(FTSocket);

	delete file;

	//Sleep(1000);
	//ReleaseMutex(this->hThread);

	//CloseHandle(this->hThread);

	return 1;
}

int SocketFileTransfer::sendFile(SOCKET FTSocket, File *file)
{
	int sendbufLength = 0;

	if (!file->openFile(R)) return 0;

	if (!file->seekFile(UPLOAD)) return 0;

	char *sendbuf = new char[DEFAULT_FILE_BUFLEN];

	for (int i = 0; i < DEFAULT_FILE_BUFLEN; i++)
	{
		sendbuf[i] = '\0';
	}

	do {
		// Send an initial buffer
		sendbufLength = file->readFile(sendbuf, DEFAULT_FILE_BUFLEN);
		if (!sendbufLength) break;		

		this->iResult = send(FTSocket, sendbuf, sendbufLength, 0);
		if (this->iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(this->fileSocket);
			WSACleanup();
			return 0;
		}

		file->fileSentSize += sendbufLength;

		printf(file->fileSentSize >= file->fileSize ? "文件发送完毕！\n" : "文件已发送:%5.2f%%\n", (double)file->fileSentSize * 100 / file->fileSize);

	} while (this->iResult > 0 && file->fileSentSize < file->fileSize);

	file->closeFile();

	delete sendbuf;
	
	return 1;
}

int SocketFileTransfer::receiveFile(SOCKET FTSocket, File *file)
{
	int recvbufLength = 0;

	if (!file->openFile(A))
	{
		if (!file->openFile(W)) return 0;
	}

	if (!file->seekFile(DOWNLOAD)) return 0;

	char *recvbuf = new char[DEFAULT_FILE_BUFLEN];

	for (int i = 0; i < DEFAULT_FILE_BUFLEN; i++)
	{
		recvbuf[i] = '\0';
	}

	// Receive until the peer closes the connection
	do {

		this->iResult = recv(FTSocket, recvbuf, DEFAULT_FILE_BUFLEN, 0);
		if (this->iResult > 0)
		{
			recvbufLength = this->iResult;

			if (!file->writeFile(recvbuf, recvbufLength)) return 0;

			file->fileReceivedSize += recvbufLength;

			printf(file->fileReceivedSize >= file->fileSize ? "文件接收完毕！\n" : "文件已接收:%5.2f%%\n", (double)file->fileReceivedSize * 100 / file->fileSize);
			
		}
		else if (this->iResult == 0)
		{
			printf("Connection closed\n");
			return 0;
		}
		else
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			return 0;
		}

	} while (this->iResult > 0 && file->fileReceivedSize < file->fileSize);

	if (file->fileReceivedSize >= file->fileSize)
	{
		AFHV allFHV = getAllFileHashValue();
		addHashValue(allFHV, file->fileHashSha256);
	}

	file->closeFile();

	delete recvbuf;

	return 1;
}

void SocketFileTransfer::closeFileSocket(SOCKET FTSocket)
{
	// cleanup
	closesocket(FTSocket);
	//WSACleanup();
}