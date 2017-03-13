#pragma once
#ifndef _SocketManger_H_
#define _SocketManger_H_
#endif // !_SocketManger_H_

#include "stdafx.h"
#include "SocketCommand.h"
#include "SocketFileTransfer.h"

class SocketManger
{
public:
	SocketCommand		*socketCommond;
	SocketFileTransfer	*socketFileTransfer;
	AFHV				allFileHashValue;

	//HANDLE			hThread;

public:
	SocketManger();
	~SocketManger();

	int SocketManger::init();
	int SocketManger::work();
	DWORD WINAPI SocketManger::newThread(LPVOID pParam);
};

