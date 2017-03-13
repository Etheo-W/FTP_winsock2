#include "SocketCommand.h"

SocketCommand::SocketCommand()
{
	this->listenSocket = INVALID_SOCKET;
	this->commandSocket = INVALID_SOCKET;
	this->result = NULL;
}


SocketCommand::~SocketCommand()
{
}

int SocketCommand::initWinsock()
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
	this->iResult = getaddrinfo(NULL, SERVER_COMMAND_PORT, &this->hints, &this->result);
	if (this->iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", this->iResult);
		WSACleanup();
		return 0;
	}

	return 1;
}

int	SocketCommand::connect2FileServer()
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
	// closesocket(this->listenSocket);

	return 1;
}
int SocketCommand::acceptCommandSocket()
{
	// Accept a client socket
	this->commandSocket = accept(this->listenSocket, NULL, NULL);
	
	if (this->commandSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(this->listenSocket);
		WSACleanup();
		return 0;
	}
	return 1;
}

int	SocketCommand::respons(SOCKET cmdSocket,File *file)
{
	//WaitForSingleObject(this->hThread, INFINITE);

	if (!this->receiveCommandInfo(cmdSocket, file)) return 0;

	if (!this->sendCommandInfo(cmdSocket, file, file->fileOperateStyle == GETALLFILEHASH ? GETALLFILEHASH : (file->fileOperateStyle == UPLOAD ? DOWNLOAD : UPLOAD))) return 0;

	if (file->fileOperateStyle == GETALLFILEHASH)
	{
		if (!this->receiveCommandInfo(cmdSocket, file)) return 0;

		if (!this->sendCommandInfo(cmdSocket, file, file->fileOperateStyle == UPLOAD ? DOWNLOAD : UPLOAD)) return 0;
	}

	//Sleep(1000);
	//ReleaseMutex(this->hThread);

	//CloseHandle(this->hThread);
	//this->closeCommandSocket(cmdSocket);

	return 1;
}

int	SocketCommand::sendCommandInfo(SOCKET cmdSocket, File *file, REQUEST_STYLE requestStyle)
{
	char *sendbuf = new char[DEFAULT_COMMAND_BUFLEN];

	for (int i = 0; i < DEFAULT_COMMAND_BUFLEN; i++)
	{
		sendbuf[i] = '\0';
	}

	this->getResponseInfo(file, sendbuf, getRequestStyle(requestStyle));

	// Send an initial buffer
	this->iResult = send(cmdSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (this->iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(this->commandSocket);
		WSACleanup();
		return 0;
	}

	delete(sendbuf);

	return 1;
}

int	SocketCommand::receiveCommandInfo(SOCKET cmdSocket, File *file)
{
	char *recvbuf = new char[DEFAULT_COMMAND_BUFLEN];

	for (int i = 0; i < DEFAULT_COMMAND_BUFLEN; i++)
	{
		recvbuf[i] = '\0';
	}

	this->iResult = recv(cmdSocket, recvbuf, DEFAULT_COMMAND_BUFLEN, 0);
	if (this->iResult > 0)
	{
		this->parseRequestInfo(file, recvbuf);
		

		if (file->fileOperateStyle == UPLOAD)
		{
			file->getFileFullPath();

			file->getFileSize(UPLOAD);
			file->fileReceivedSize = file->fileSize;
		}

		if (file->fileOperateStyle == DOWNLOAD)
		{
			file->getFileFullPath();

			uint32_t fileSize = file->fileSize;
			file->getFileSize(DOWNLOAD);
			file->fileReceivedSize = file->fileSize;
			file->fileSize = fileSize;
			file->fileSentSize = file->fileSize;
		}
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

	delete(recvbuf);

	return 1;
}

void SocketCommand::closeCommandSocket(SOCKET cmdSocket)
{
	// cleanup
	closesocket(cmdSocket);
	//WSACleanup();
}

void SocketCommand::getResponseInfo(File *file, char *response, const char *requestStyle)
{
	//响应信息格式：请求类型|文件已接受大小|文件已发送大小|文件大小
	//若请求类型为UPLOAD，则文件大小为请求消息中文件大小
	//若请求类型为DOWNLOAD，则文件已上传大小为文件大小
	
	if (!strcmp(requestStyle, getRequestStyle(GETALLFILEHASH)))
	{
		strcat(response, requestStyle);
		
		AFHV allFHV = getAllFileHashValue();

		for (int i = 0; i < allFHV->length; i++)
		{
			strcat(response, "|");
			strcat(response, allFHV->allFileHashValue[i]);
		}

		return;
	}

	strcat(response, requestStyle);
	strcat(response, "|");

	string fileReceivedSizeString = to_string(file->fileReceivedSize);
	//int2String((int)file->fileReceivedSize, fileReceivedSizeString, 10);
	strcat(response, fileReceivedSizeString.c_str());
	strcat(response, "|");

	string fileSentSizeString = to_string(file->fileSentSize);
	//int2String((int)file->fileSentSize, fileSentSizeString, 10);
	strcat(response, fileSentSizeString.c_str());
	strcat(response, "|");

	string fileSizeString = to_string(file->fileSize);
	//int2String((int)file->fileSize, fileSizeString, 10);
	strcat(response, fileSizeString.c_str());
}

void SocketCommand::parseRequestInfo(File *file, char *request)
{
	//请求信息格式：请求类型|文件名|hash值|文件大小
	//若请求类型为UPLOAD，则文件大小为客户端已上传文件大小
	//若请求类型为DOWNLOAD，则文件大小为客户端已下载文件大小

	char *token = NULL;
	
	token = strtok(request, "|");

	if (!strcmp(token, getRequestStyle(GETALLFILEHASH)))
	{
		file->fileOperateStyle = GETALLFILEHASH;
		return;
	}

	if (!strcmp(token, getRequestStyle(UPLOAD)))
	{
		file->fileOperateStyle = DOWNLOAD;
	}

	if (!strcmp(token, getRequestStyle(DOWNLOAD)))
	{
		file->fileOperateStyle = UPLOAD;
	}

	token = strtok(NULL, "|");
	token = strtok(NULL, "|");
	strcpy(file->fileHashSha256, token);

	if (file->fileOperateStyle == UPLOAD)
	{
		token = strtok(NULL, "|");
		file->fileSentSize = (uint32_t)atof(token);
	}

	if (file->fileOperateStyle == DOWNLOAD)
	{
		token = strtok(NULL, "|");
		file->fileSize = (uint32_t)atof(token);
	}
}