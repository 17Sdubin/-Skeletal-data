#include <stdio.h>  
#include <stdlib.h>  
#include <WinSock2.h>  
#include <iostream>  
#include<atlstr.h>  
#pragma comment(lib, "ws2_32.lib")  
#define _WINSOCK_DEPRECATED_NO_WARNINGS

using namespace std;

bool CmpTime(SYSTEMTIME &local_time, SYSTEMTIME &goal_time)
{

	if (local_time.wHour == goal_time.wHour &&  local_time.wMinute == goal_time.wMinute && local_time.wSecond == goal_time.wSecond && local_time.wMilliseconds == goal_time.wMilliseconds)
		return true;
	else
		return false;
}
 
struct SendInfo {
	int n;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;
}sinfo;


//��Ӧ�ͻ�������߳�  
DWORD WINAPI clientService(LPVOID lpParameter)
{
	SOCKET clientSocket = (SOCKET)lpParameter;
	
		const int BUF_SIZE = 1024;

		char bufRecv[BUF_SIZE];
		memset(bufRecv, 0, BUF_SIZE); //��շ��ͻ��棬��Ȼ���ܻ��������
		memcpy(bufRecv, &sinfo, sizeof(sinfo)); //(3)�ṹ��ת�����ַ���
		send(clientSocket, bufRecv, sizeof(sinfo), 0);


	//�ر����socket  
	    closesocket(clientSocket);

	return 0;
}



void main()
{

	WSADATA wsaData;
	int err;
	



	system("title skeleton source of Server");//����cmd���ڱ���
	system("color 0B");

	//1.�����׽��ֿ�  
	err = WSAStartup(MAKEWORD(1, 1), &wsaData);
	if (err != 0)
	{
		cout << "Init Windows Socket Failed::" << GetLastError() << endl;
		return;
	}

	//2.����socket  
	//�׽���������,SOCKETʵ������unsigned int  
	SOCKET serverSocket;
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET)
	{
		cout << "Create Socket Failed::" << GetLastError() << endl;
		return;
	}


	//�������˵ĵ�ַ�Ͷ˿ں�  
	struct sockaddr_in serverAddr, clientAdd;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(4399);

	//3.��Socket����Socket��ĳ��Э���ĳ����ַ��  
	err = bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (err != 0)
	{
		cout << "Bind Socket Failed::" << GetLastError() << endl;
		return;
	}


	//4.����,���׽�����Ĭ�ϵ������׽���ת���ɱ����׽���  
	err = listen(serverSocket, 10);
	if (err != 0)
	{
		cout << "listen Socket Failed::" << GetLastError() << endl;
		return;
	}

	cout << "��������������......\n" << endl;

	int addrLen = sizeof(clientAdd);
	HANDLE hThread = NULL;












	SYSTEMTIME local_time = { 0 };
	SYSTEMTIME goal_time = { 0 };

	memset(&sinfo, 0, sizeof(sinfo));//��ջ���
	char str6[] = "���������ӳɹ���Ҫ��ȡ��֡��: ";
	printf("%s\n", str6);
	scanf("%d", &sinfo.n);


	cout << "���뿪ʼд��ʱ��" << std::endl;
	cin >> goal_time.wHour >> goal_time.wMinute >> goal_time.wSecond >> goal_time.wMilliseconds;
	GetLocalTime(&local_time);

	_tprintf(_T("The local time is\t: %02d:%02d:%02d.%03d\n"), local_time.wHour, local_time.wMinute, local_time.wSecond, local_time.wMilliseconds);
	_tprintf(_T("The goal time is\t: %02d:%02d:%02d.%03d\n"), goal_time.wHour, goal_time.wMinute, goal_time.wSecond, goal_time.wMilliseconds);
	sinfo.wHour = goal_time.wHour;
	sinfo.wMinute = goal_time.wMinute;
	sinfo.wSecond = goal_time.wSecond;
	sinfo.wMilliseconds = goal_time.wMilliseconds;



	while (true)
	{
		//5.�������󣬵��յ�����󣬻Ὣ�ͻ��˵���Ϣ����clientAdd����ṹ���У��������������TCP���ӵ�Socket  
		SOCKET sockConn = accept(serverSocket, (struct sockaddr*)&clientAdd, &addrLen);
		if (sockConn == INVALID_SOCKET)
		{
			cout << "Accpet Failed::" << GetLastError() << endl;
			return;
		}



		//6.��ʱ����һ���µ��̵߳���send��recv�����������Ϳͻ��˽���ͨ��  
		cout << "�ͻ���"<<"(ip)"<< inet_ntoa(clientAdd.sin_addr) << "��" <<"(�˿�)"<< clientAdd.sin_port <<"���ճɹ�������׼��ִ�У�"<< endl;
	


		hThread = CreateThread(NULL, 4, clientService, (LPVOID)sockConn, 0, NULL);
		if (hThread == NULL)
		{
			cout << "Create Thread Failed!" << endl;
		}
		CloseHandle(hThread);
	}

	closesocket(serverSocket);
	//����Windows Socket��  
	WSACleanup();
	
}