// Socket_Server_CPP.cpp : �������̨Ӧ�ó������ڵ㡣
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

