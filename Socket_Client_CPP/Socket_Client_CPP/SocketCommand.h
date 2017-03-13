#pragma once
#ifndef _SocketCommand_H_
#define _SocketCommand_H_
#endif // !_SocketCommand_H_

#include "CommonHeader.h"
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
	SOCKET			commandSocket;
	struct addrinfo *result, *ptr, hints;
	char			sendbuf[DEFAULT_COMMAND_BUFLEN];
	char			recvbuf[DEFAULT_COMMAND_BUFLEN];
	int				iResult;

public:
	SocketCommand();
	~SocketCommand();

	int		SocketCommand::initWinsock(char *IPAddress);
	int		SocketCommand::connect2FileServer();

	int		SocketCommand::sendCommandInfo(File *file, REQUEST_STYLE requestStyle);
	int		SocketCommand::receiveCommandInfo(File *file);

	void 	SocketCommand::closeCommandSocket();

private:
	void	SocketCommand::getRequestInfo(File *file, char *request, const char *requestStyle);
	void	SocketCommand::parseResponseInfo(File *file, char *response);
};

