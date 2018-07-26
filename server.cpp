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


//响应客户请求的线程  
DWORD WINAPI clientService(LPVOID lpParameter)
{
	SOCKET clientSocket = (SOCKET)lpParameter;
	
		const int BUF_SIZE = 1024;

		char bufRecv[BUF_SIZE];
		memset(bufRecv, 0, BUF_SIZE); //清空发送缓存，不然可能会产生乱码
		memcpy(bufRecv, &sinfo, sizeof(sinfo)); //(3)结构体转换成字符串
		send(clientSocket, bufRecv, sizeof(sinfo), 0);


	//关闭这个socket  
	    closesocket(clientSocket);

	return 0;
}



void main()
{

	WSADATA wsaData;
	int err;
	



	system("title skeleton source of Server");//设置cmd窗口标题
	system("color 0B");

	//1.加载套接字库  
	err = WSAStartup(MAKEWORD(1, 1), &wsaData);
	if (err != 0)
	{
		cout << "Init Windows Socket Failed::" << GetLastError() << endl;
		return;
	}

	//2.创建socket  
	//套接字描述符,SOCKET实际上是unsigned int  
	SOCKET serverSocket;
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET)
	{
		cout << "Create Socket Failed::" << GetLastError() << endl;
		return;
	}


	//服务器端的地址和端口号  
	struct sockaddr_in serverAddr, clientAdd;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(4399);

	//3.绑定Socket，将Socket与某个协议的某个地址绑定  
	err = bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (err != 0)
	{
		cout << "Bind Socket Failed::" << GetLastError() << endl;
		return;
	}


	//4.监听,将套接字由默认的主动套接字转换成被动套接字  
	err = listen(serverSocket, 10);
	if (err != 0)
	{
		cout << "listen Socket Failed::" << GetLastError() << endl;
		return;
	}

	cout << "服务器端已启动......\n" << endl;

	int addrLen = sizeof(clientAdd);
	HANDLE hThread = NULL;












	SYSTEMTIME local_time = { 0 };
	SYSTEMTIME goal_time = { 0 };

	memset(&sinfo, 0, sizeof(sinfo));//清空缓存
	char str6[] = "请输入连接成功后，要获取的帧数: ";
	printf("%s\n", str6);
	scanf("%d", &sinfo.n);


	cout << "输入开始写入时间" << std::endl;
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
		//5.接收请求，当收到请求后，会将客户端的信息存入clientAdd这个结构体中，并返回描述这个TCP连接的Socket  
		SOCKET sockConn = accept(serverSocket, (struct sockaddr*)&clientAdd, &addrLen);
		if (sockConn == INVALID_SOCKET)
		{
			cout << "Accpet Failed::" << GetLastError() << endl;
			return;
		}



		//6.此时启动一个新的线程调用send和recv这两个函数和客户端进行通信  
		cout << "客户端"<<"(ip)"<< inet_ntoa(clientAdd.sin_addr) << "，" <<"(端口)"<< clientAdd.sin_port <<"接收成功，正在准备执行！"<< endl;
	


		hThread = CreateThread(NULL, 4, clientService, (LPVOID)sockConn, 0, NULL);
		if (hThread == NULL)
		{
			cout << "Create Thread Failed!" << endl;
		}
		CloseHandle(hThread);
	}

	closesocket(serverSocket);
	//清理Windows Socket库  
	WSACleanup();
	
}