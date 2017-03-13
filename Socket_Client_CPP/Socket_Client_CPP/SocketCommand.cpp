#include "SocketCommand.h"



SocketCommand::SocketCommand()
{
	for (int i = 0; i < DEFAULT_COMMAND_BUFLEN; i++)
	{
		this->sendbuf[i] = '\0';
		this->recvbuf[i] = '\0';
	}

	this->commandSocket = INVALID_SOCKET;
	this->result = NULL;
	this->ptr = NULL;
}


SocketCommand::~SocketCommand()
{
}

int SocketCommand::initWinsock(char *IPAddress)
{
	// Initialize Winsock
	this->iResult = WSAStartup(MAKEWORD(2, 2), &this->wsaData);
	if (this->iResult != 0) {
		printf("WSAStartup failed with error: %d\n", this->iResult);
		return 0;
	}

	ZeroMemory(&this->hints, sizeof(this->hints));
	this->hints.ai_family = AF_UNSPEC;
	this->hints.ai_socktype = SOCK_STREAM;
	this->hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	this->iResult = getaddrinfo(IPAddress, SERVER_COMMAND_PORT, &this->hints, &this->result);
	if (this->iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", this->iResult);
		WSACleanup();
		return 0;
	}

	return 1;
}

int	SocketCommand::connect2FileServer()
{
	// Attempt to connect to an address until one succeeds
	for (this->ptr = this->result; this->ptr != NULL; this->ptr = this->ptr->ai_next)
	{
		// Create a SOCKET for connecting to fileServer
		this->commandSocket = socket(this->ptr->ai_family, this->ptr->ai_socktype, this->ptr->ai_protocol);
		if (this->commandSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 0;
		}

		// Connect to server.
		this->iResult = connect(this->commandSocket, this->ptr->ai_addr, (int)this->ptr->ai_addrlen);
		if (this->iResult == SOCKET_ERROR) {
			closesocket(this->commandSocket);
			this->commandSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(this->result);

	if (this->commandSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 0;
	}

	return 1;
}

int	SocketCommand::sendCommandInfo(File *file, REQUEST_STYLE requestStyle)
{
	for (int i = 0; i < DEFAULT_COMMAND_BUFLEN; i++)
	{
		this->sendbuf[i] = '\0';
	}

	this->getRequestInfo(file, this->sendbuf, getRequestStyle(requestStyle));

	// Send an initial buffer
	this->iResult = send(this->commandSocket, this->sendbuf, (int)strlen(this->sendbuf), 0);
	if (this->iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(this->commandSocket);
		WSACleanup();
		return 0;
	}

	return 1;
}

int	SocketCommand::receiveCommandInfo(File *file)
{
	for (int i = 0; i < DEFAULT_COMMAND_BUFLEN; i++)
	{
		this->recvbuf[i] = '\0';
	}

	this->iResult = recv(this->commandSocket, this->recvbuf, DEFAULT_COMMAND_BUFLEN, 0);
	if (this->iResult > 0)
	{
		this->parseResponseInfo(file, this->recvbuf);
	}
	else if (this->iResult == 0)
		printf("Connection closed\n");
	else
		printf("recv failed with error: %d\n", WSAGetLastError());

	return 1;
}

void SocketCommand::closeCommandSocket()
{
	// cleanup
	closesocket(this->commandSocket);
	WSACleanup();
}

void SocketCommand::getRequestInfo(File *file, char *request, const char *requestStyle)
{
	//请求信息格式：请求类型|文件名|hash值|文件大小
	//若请求类型为DOWNLOAD，则文件大小为0
	if (!strcmp(requestStyle, getRequestStyle(GETALLFILEHASH)))
	{
		strcat(request, requestStyle);
		return;
	}

	strcat(request, requestStyle);
	strcat(request, "|");

	strcat(request, file->fileHashSha256);
	strcat(request, "|");

	strcat(request, file->fileHashSha256);
	strcat(request, "|");

	string fileSizeString = "";
	//int2String((int)file->fileSize, fileSizeString, 10);
	fileSizeString = to_string(file->fileSize);
	strcat(request, fileSizeString.c_str());
}

void SocketCommand::parseResponseInfo(File *file, char *response)
{
	//响应信息格式：请求类型|文件已上传大小|文件已下载大小|文件大小
	//若请求类型为UPLOAD，则文件大小为请求消息中文件大小
	//若请求类型为DOWNLOAD，则文件已上传大小为文件大小
	char *token = NULL;
	
	token = strtok(response, "|");

	if (!strcmp(token, getRequestStyle(GETALLFILEHASH)))
	{
		AFHV allFHV = getAllFileHashValue();

		token = strtok(NULL, "|");

		while (token)
		{
			addHashValue(allFHV, token);
			token = strtok(NULL, "|");
		}

		return;
	}

	if (!strcmp(token, getRequestStyle(UPLOAD)))
	{
		token = strtok(NULL, "|");
		file->fileSentSize = (uint32_t)atof(token);
		//file->fileSize = file->fileSentSize;
	}

	if (!strcmp(token, getRequestStyle(DOWNLOAD)))
	{
		token = strtok(NULL, "|");
		token = strtok(NULL, "|");
		//file->fileReceivedSize = (uint32_t)atof(token);

		token = strtok(NULL, "|");
		file->fileSize = (uint32_t)atof(token);
	}
}