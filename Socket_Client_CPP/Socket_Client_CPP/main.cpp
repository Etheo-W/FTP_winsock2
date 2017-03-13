#include "CommonHeader.h"
#include "SocketManger.h"

int main(int argc, char * argv[])
{
	cout << " ----------------------------- " << endl;
	cout << "|         用户操作手册        |" << endl;
	cout << "|            0.上传           |" << endl;
	cout << "|            1.下载           |" << endl;
	cout << " ----------------------------- " << endl;

	if (argc < 2)
	{
		cout << "请输入服务器IP地址：";
		string ipAddress = "";
		cin >> ipAddress;
		argv[1] = (char*)malloc(16 * sizeof(char));
		for (int i = 0; i < 16; i++)
		{
			argv[1][i] = '\0';
		}

		strcpy(argv[1], ipAddress.c_str());
	}

	//int isEnd = 0;
	int operate = 0;

	SocketManger *socketManger = new SocketManger();

	if (!socketManger->init(argv[1])) return 0;

	//while (!isEnd)
	{
		cout << "请选择操作：";
		cin >> operate;

		socketManger->work(argv[1], (REQUEST_STYLE)operate);

		//cout << "是否结束(是1，否0)：";
		//cin >> isEnd;
	}

	socketManger->stop();

	delete socketManger;

	free(argv[1]);
	
	return 0;
}