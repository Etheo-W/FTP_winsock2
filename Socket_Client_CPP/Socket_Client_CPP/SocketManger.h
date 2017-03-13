#pragma once
#ifndef _SocketManger_H_
#define _SocketManger_H_
#endif // !_SocketManger_H_

#include "CommonHeader.h"
#include "SocketCommand.h"
#include "SocketFileTransfer.h"

class SocketManger
{
public:
	SocketCommand		*socketCommond;
	SocketFileTransfer	*socketFileTransfer;

public:
	SocketManger();
	~SocketManger();

	int SocketManger::init(char *IPAddress);
	int SocketManger::work(char *IPAddress, REQUEST_STYLE requestStyle);
	int SocketManger::stop();
};

