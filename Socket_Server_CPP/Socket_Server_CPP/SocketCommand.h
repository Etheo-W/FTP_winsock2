#pragma once
#ifndef _SocketCommand_H_
#define _SocketCommand_H_
#endif // !_SocketCommand_H_

#include "stdafx.h"
#include "File.h"

#define DEFAULT_COMMAND_BUFLEN 4194304	//4M
#define SERVER_COMMAND_PORT "1621"

typedef FILE_OPERATE_STYLE REQUEST_STYLE;

#define getRequestStyle(REQUEST_STYLE) this->requestStyleName[REQUEST_STYLE]

class SocketCommand
{
private:
	const char* requestStyleName[3] = { "UPLOAD", "DOWNLOAD", "GETALLFILEHASH" };
public:
	WSADATA			wsaData;
	SOCKET			listenSocket;
	SOCKET			commandSocket;
	struct addrinfo *result, hints;

	//char			sendbuf[DEFAULT_COMMAND_BUFLEN];
	//char			recvbuf[DEFAULT_COMMAND_BUFLEN];

	int				iResult;

	//HANDLE		hThread;

public:
	SocketCommand();
	~SocketCommand();

	int		SocketCommand::initWinsock();
	int		SocketCommand::connect2FileServer();
	int		SocketCommand::acceptCommandSocket();

	int		SocketCommand::respons(SOCKET cmdSocket, File *file);
	int		SocketCommand::sendCommandInfo(SOCKET cmdSocket, File *file, REQUEST_STYLE requestStyle);
	int		SocketCommand::receiveCommandInfo(SOCKET cmdSocket, File *file);

	void 	SocketCommand::closeCommandSocket(SOCKET cmdSocket);

private:
	void	SocketCommand::getResponseInfo(File *file, char *response, const char *requestStyle);
	void	SocketCommand::parseRequestInfo(File *file, char *request);
};

