#include "CommonHeader.h"
#include "SocketManger.h"

int main(int argc, char * argv[])
{
	cout << " ----------------------------- " << endl;
	cout << "|         �û������ֲ�        |" << endl;
	cout << "|            0.�ϴ�           |" << endl;
	cout << "|            1.����           |" << endl;
	cout << " ----------------------------- " << endl;

	if (argc < 2)
	{
		cout << "�����������IP��ַ��";
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
		cout << "��ѡ�������";
		cin >> operate;

		socketManger->work(argv[1], (REQUEST_STYLE)operate);

		//cout << "�Ƿ����(��1����0)��";
		//cin >> isEnd;
	}

	socketManger->stop();

	delete socketManger;

	free(argv[1]);
	
	return 0;
}