// Socket_Server_CPP.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "SocketManger.h"

HANDLE hMutex;

int main()
{
	SocketManger *socketManger = new SocketManger();

	socketManger->init();

	while (1)
	{
		socketManger->work();
		//socketManger->hThread = CreateThread(NULL, 0, 
		//	(LPTHREAD_START_ROUTINE)(socketManger->work()), NULL, 0, 0 );

		//Sleep(1000);
	}

	delete socketManger;

	return 0;
}

