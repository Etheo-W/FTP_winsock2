#pragma once
#ifndef _SocketFileTransfer_H_
#define _SocketFileTransfer_H_
#endif // !_SocketFileTransfer_H_

#include "CommonHeader.h"
#include "File.h"

#define DEFAULT_FILE_BUFLEN 4194304 //4M
#define SERVER_FILE_PORT "1620"

class SocketFileTransfer
{
public:
	WSADATA			wsaData;
	SOCKET			fileSocket;
	struct addrinfo *result, *ptr, hints;
	char			sendbuf[DEFAULT_FILE_BUFLEN];
	char			recvbuf[DEFAULT_FILE_BUFLEN];
	int				iResult;

public:
	SocketFileTransfer();
	~SocketFileTransfer();

	int		SocketFileTransfer::initWinsock(char *IPAddress);
	int		SocketFileTransfer::connect2FileServer();

	int		SocketFileTransfer::sendFile(File *file);
	int		SocketFileTransfer::receiveFile(File *file);

	void	SocketFileTransfer::closeFileSocket();
};

