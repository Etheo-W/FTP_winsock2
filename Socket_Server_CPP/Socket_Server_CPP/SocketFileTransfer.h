#pragma once
#ifndef _SocketFileTransfer_H_
#define _SocketFileTransfer_H_
#endif // !_SocketFileTransfer_H_

#include "stdafx.h"
#include "File.h"

#define DEFAULT_FILE_BUFLEN 4194304 //4M
#define SERVER_FILE_PORT "1620"

class SocketFileTransfer
{
public:
	WSADATA			wsaData;
	SOCKET			listenSocket;
	SOCKET			fileSocket;
	struct addrinfo *result, hints;

	int				iResult;

	//char			sendbuf[DEFAULT_FILE_BUFLEN];
	//char			recvbuf[DEFAULT_FILE_BUFLEN];
	
	//HANDLE		hThread;

public:
	SocketFileTransfer();
	~SocketFileTransfer();

	int		SocketFileTransfer::initWinsock();
	int		SocketFileTransfer::connect2FileServer();
	int		SocketFileTransfer::acceptFileSocket();

	int		SocketFileTransfer::transfer(SOCKET FTSocket, File *file);
	int		SocketFileTransfer::sendFile(SOCKET FTSocket, File *file);
	int		SocketFileTransfer::receiveFile(SOCKET FTSocket, File *file);

	void	SocketFileTransfer::closeFileSocket(SOCKET FTSocket);
};

