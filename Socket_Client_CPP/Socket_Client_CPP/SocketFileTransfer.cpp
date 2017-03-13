#include "SocketFileTransfer.h"



SocketFileTransfer::SocketFileTransfer()
{
	for (int i = 0; i < DEFAULT_FILE_BUFLEN; i++)
	{
		this->sendbuf[i] = '\0';
		this->recvbuf[i] = '\0';
	}

	this->fileSocket = INVALID_SOCKET;
	this->result = NULL;
	this->ptr = NULL;
}


SocketFileTransfer::~SocketFileTransfer()
{
}


int SocketFileTransfer::initWinsock(char *IPAddress)
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
	this->iResult = getaddrinfo(IPAddress, SERVER_FILE_PORT, &this->hints, &this->result);
	if (this->iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", this->iResult);
		WSACleanup();
		return 0;
	}

	return 1;
}

int SocketFileTransfer::connect2FileServer()
{
	// Attempt to connect to an address until one succeeds
	for (this->ptr = this->result; this->ptr != NULL; this->ptr = this->ptr->ai_next) 
	{
		// Create a SOCKET for connecting to fileServer
		this->fileSocket = socket(this->ptr->ai_family, this->ptr->ai_socktype, this->ptr->ai_protocol);
		if (this->fileSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 0;
		}

		// Connect to server.
		this->iResult = connect(this->fileSocket, this->ptr->ai_addr, (int)this->ptr->ai_addrlen);
		if (this->iResult == SOCKET_ERROR) {
			closesocket(this->fileSocket);
			this->fileSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(this->result);

	if (this->fileSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 0;
	}

	return 1;
}

int SocketFileTransfer::sendFile(File *file)
{
	int sendbufLength = 0;

	if (!file->openFile(R)) return 0;

	if (!file->seekFile(UPLOAD)) return 0;

	do {
		// Send an initial buffer
		sendbufLength = file->readFile(this->sendbuf, DEFAULT_FILE_BUFLEN);
		if (!sendbufLength) break;

		this->iResult = send(this->fileSocket, this->sendbuf, sendbufLength, 0);
		if (this->iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(this->fileSocket);
			WSACleanup();
			return 0;
		}

		file->fileSentSize += sendbufLength;

		printf(file->fileSentSize >= file->fileSize ? "文件上传完毕！\n" : "文件已上传:%5.2f%%\n", (double)file->fileSentSize * 100 / file->fileSize);

	} while (this->iResult > 0 && file->fileSentSize < file->fileSize);

	file->closeFile();
	
	return 1;
}

int SocketFileTransfer::receiveFile(File *file)
{
	int recvbufLength = 0;

	if (!file->openFile(A))
	{
		if (!file->openFile(W)) return 0;
	}

	if (!file->seekFile(DOWNLOAD)) return 0;

	// Receive until the peer closes the connection
	do {

		this->iResult = recv(this->fileSocket, this->recvbuf, DEFAULT_FILE_BUFLEN, 0);
		if (this->iResult > 0)
		{
			recvbufLength = this->iResult;

			if (!file->writeFile(this->recvbuf, recvbufLength)) return 0;

			file->fileReceivedSize += recvbufLength;

			printf(file->fileReceivedSize >= file->fileSize ? "文件下载完毕！\n" : "文件已下载:%5.2f%%\n", (double)file->fileReceivedSize * 100 / file->fileSize);
			
		}
		else if (this->iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

	} while (this->iResult > 0 && file->fileReceivedSize < file->fileSize);

	file->closeFile();

	return 1;
}

void SocketFileTransfer::closeFileSocket()
{
	// cleanup
	closesocket(this->fileSocket);
	WSACleanup();
}