#include <iostream>
#include "winsock2.h" 
#include <string>  
#include<stdio.h>
#include <vector>  
//#include <sstream>  
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <Kinect.h>  
#include<stdlib.h>
#include<atlstr.h>
#include <windows.h>  
#include <fstream>
#pragma comment(lib, "ws2_32.lib")  




typedef struct point {
	double x = 0;
	double y = 0;
	double z = 0;
	int TrackingState = 0;
}Csvpoint;

using namespace cv;
using namespace std;

vector<vector<Csvpoint>> csvpoint;
vector<Csvpoint> pp;

char filename_skeleton[50];

int a = 0;
int b = 0;


template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

bool CmpTime(SYSTEMTIME &local_time, SYSTEMTIME &goal_time)
{

	if (local_time.wHour == goal_time.wHour &&  local_time.wMinute == goal_time.wMinute && local_time.wSecond == goal_time.wSecond && local_time.wMilliseconds == goal_time.wMilliseconds)
		return true;
	else
		return false;
}

void DrawBone(JointType b, JointType c, ICoordinateMapper*coordinatemapper, Joint joint[], Mat&a)
{
	DepthSpacePoint d1, d2;
	coordinatemapper->MapCameraPointToDepthSpace(joint[b].Position, &d1);
	coordinatemapper->MapCameraPointToDepthSpace(joint[c].Position, &d2);

	if (d1.X > 0 && d1.X < 512 && d1.Y>0 && d1.Y < 424 && d2.X>0 && d2.X < 512 && d2.Y>0 && d2.Y < 424)
		line(a, Point(d1.X, d1.Y), Point(d2.X, d2.Y), Scalar(0, 255, 0, 255), 3);
	else
		line(a, Point(d1.X, d1.Y), Point(d2.X, d2.Y), Scalar(255, 255, 255, 255), 1);
	circle(a, Point(d1.X, d1.Y), 2, Scalar(255, 255, 255, 255), 4);
	circle(a, Point(d2.X, d2.Y), 2, Scalar(255, 255, 255, 255), 4);
}

struct RevInfo {
	int n;//帧数
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;
}rinfo;





void main()
{
	HRESULT hResult = S_OK;
	IKinectSensor *kinect;
	GetDefaultKinectSensor(&kinect);
	kinect->Open();
	IBodyFrameSource*bodysource;
	kinect->get_BodyFrameSource(&bodysource);
	IBodyFrameReader*bodyreader;
	bodysource->OpenReader(&bodyreader);
	ICoordinateMapper* coordinatemapper;
	kinect->get_CoordinateMapper(&coordinatemapper);

	system("title skeleton source of Client3");//设置cmd窗口标题
	system("color 0B");

	SYSTEMTIME local_time = { 0 };
	SYSTEMTIME goal_time = { 0 };

	WSADATA wsaData;
	int err;

	//1.首先执行初始化Windows Socket库  
	err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (err != 0)
	{
		cout << "Init Socket Failed::" << GetLastError() << endl;
		return;
	}

	//2.创建Socket  
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in addrServer;
	addrServer.sin_addr.s_addr = inet_addr("101.76.216.113");
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(4399);

	//3.连接Socket，第一个参数为客户端socket，第二个参数为服务器端地址  
	err = connect(sockClient, (struct sockaddr *)&addrServer, sizeof(addrServer));
	if (err != 0)
	{
		cout << "Connect Error::" << GetLastError() << endl;
		return;
	}
	else
	{
		cout << "连接成功!" << endl;
	}


	const int BUF_SIZE = 1024;
	char recvBuf[BUF_SIZE];
	ZeroMemory(recvBuf, BUF_SIZE);// 初始化
	memset(&rinfo, 0, sizeof(rinfo));//清空结构体
	recv(sockClient, recvBuf, BUF_SIZE, 0); // 接收数据


	memcpy(&rinfo, recvBuf, sizeof(rinfo));//(3)把接收到的信息转换成结构体
	goal_time.wHour = rinfo.wHour;
	goal_time.wMinute = rinfo.wMinute;
	goal_time.wSecond = rinfo.wSecond;
	goal_time.wMilliseconds = rinfo.wMilliseconds;

	_tprintf(_T("从服务端获取的目标时间\t: %02d:%02d:%02d.%03d\n"), goal_time.wHour, goal_time.wMinute, goal_time.wSecond, goal_time.wMilliseconds);
	cout << endl << "从服务器接收到要获取帧数：" << rinfo.n << endl;
	cout << "接收成功，等待目标时间开始执行" << endl;

	while (1)
	{
		GetLocalTime(&local_time);
		if (CmpTime(local_time, goal_time)) break;
	}


	while (a < rinfo.n)

	{
		Mat asd(424, 512, CV_8UC4);
		IBodyFrame* bodyframe = nullptr;
		hResult = bodyreader->AcquireLatestFrame(&bodyframe);
		if (SUCCEEDED(hResult))
		{
			IBody* body[BODY_COUNT] = { 0 };
			hResult = bodyframe->GetAndRefreshBodyData(BODY_COUNT, body);
			if (SUCCEEDED(hResult))
			{
				for (int i = 0; i < BODY_COUNT; i++)
				{
					BOOLEAN tracked = false;
					hResult = body[i]->get_IsTracked(&tracked);
					if (SUCCEEDED(hResult) && tracked)
					{

						Joint joint[JointType_Count];
						hResult = body[i]->GetJoints(JointType_Count, joint);
						DepthSpacePoint depthspacepoint;

						if (SUCCEEDED(hResult))
						{                            //可以记录骨架数据流csv

													 // Torso
							DrawBone(JointType_Head, JointType_Neck, coordinatemapper, joint, asd);
							DrawBone(JointType_Neck, JointType_SpineShoulder, coordinatemapper, joint, asd);
							DrawBone(JointType_SpineShoulder, JointType_SpineMid, coordinatemapper, joint, asd);
							DrawBone(JointType_SpineMid, JointType_SpineBase, coordinatemapper, joint, asd);
							DrawBone(JointType_SpineShoulder, JointType_ShoulderRight, coordinatemapper, joint, asd);
							DrawBone(JointType_SpineShoulder, JointType_ShoulderLeft, coordinatemapper, joint, asd);
							DrawBone(JointType_SpineBase, JointType_HipRight, coordinatemapper, joint, asd);
							DrawBone(JointType_SpineBase, JointType_HipLeft, coordinatemapper, joint, asd);

							// Right Arm    
							DrawBone(JointType_ShoulderRight, JointType_ElbowRight, coordinatemapper, joint, asd);
							DrawBone(JointType_ElbowRight, JointType_WristRight, coordinatemapper, joint, asd);
							DrawBone(JointType_WristRight, JointType_HandRight, coordinatemapper, joint, asd);
							DrawBone(JointType_HandRight, JointType_HandTipRight, coordinatemapper, joint, asd);
							DrawBone(JointType_WristRight, JointType_ThumbRight, coordinatemapper, joint, asd);

							// Left Arm
							DrawBone(JointType_ShoulderLeft, JointType_ElbowLeft, coordinatemapper, joint, asd);
							DrawBone(JointType_ElbowLeft, JointType_WristLeft, coordinatemapper, joint, asd);
							DrawBone(JointType_WristLeft, JointType_HandLeft, coordinatemapper, joint, asd);
							DrawBone(JointType_HandLeft, JointType_HandTipLeft, coordinatemapper, joint, asd);
							DrawBone(JointType_WristLeft, JointType_ThumbLeft, coordinatemapper, joint, asd);

							// Right Leg
							DrawBone(JointType_HipRight, JointType_KneeRight, coordinatemapper, joint, asd);
							DrawBone(JointType_KneeRight, JointType_AnkleRight, coordinatemapper, joint, asd);
							DrawBone(JointType_AnkleRight, JointType_FootRight, coordinatemapper, joint, asd);

							// Left Leg
							DrawBone(JointType_HipLeft, JointType_KneeLeft, coordinatemapper, joint, asd);
							DrawBone(JointType_KneeLeft, JointType_AnkleLeft, coordinatemapper, joint, asd);
							DrawBone(JointType_AnkleLeft, JointType_FootLeft, coordinatemapper, joint, asd);

							DepthSpacePoint d1, d2;
							coordinatemapper->MapCameraPointToDepthSpace(joint[JointType_HandLeft].Position, &d1);
							coordinatemapper->MapCameraPointToDepthSpace(joint[JointType_HandRight].Position, &d2);
							HandState left;
							body[i]->get_HandLeftState(&left);
							HandState right;
							body[i]->get_HandRightState(&right);
							switch (left)
							{
							case HandState_Closed:
								circle(asd, Point(d1.X, d1.Y), 10, Scalar(0, 0, 255, 1), 20); break;
							case HandState_Open:
								circle(asd, Point(d1.X, d1.Y), 10, Scalar(0, 255, 0, 1), 20); break;
							case HandState_Lasso:
								circle(asd, Point(d1.X, d1.Y), 10, Scalar(255, 0, 0, 1), 20); break;
							default:
								break;
							}
							switch (right)
							{
							case HandState_Closed:
								circle(asd, Point(d2.X, d2.Y), 10, Scalar(0, 0, 255, 1), 20); break;
							case HandState_Open:
								circle(asd, Point(d2.X, d2.Y), 10, Scalar(0, 255, 0, 1), 20); break;
							case HandState_Lasso:
								circle(asd, Point(d2.X, d2.Y), 10, Scalar(255, 0, 0, 1), 20); break;
							default:
								break;
							}
							//一帧存入一维向量
							std::vector<Csvpoint> pp;
							for (int i = 0; i < 25; ++i) {
								Csvpoint p;
								p.x = joint[i].Position.X;
								p.y = joint[i].Position.Y;
								p.z = joint[i].Position.Z;
								p.TrackingState = joint[i].TrackingState;
								//cout << joint[i].Position.X<< joint[i].Position.Y<<joint[i].Position.Z;
								pp.push_back(p);
							}
							csvpoint.push_back(pp);
							pp.clear();
						}

					}

				}

			}

			for (int count = 0; count < BODY_COUNT; count++)

			{

				SafeRelease(body[count]);

			}
		}
		SafeRelease(bodyframe);

		imshow("skeleton", asd);
		sprintf(filename_skeleton, "ske//skeletonImg%d.jpg", a++);
		imwrite(filename_skeleton, asd);
		if (waitKey(33) == VK_ESCAPE)
		{
			break;
		}
	}
	closesocket(sockClient);

	WSACleanup();


	std::cout << csvpoint.size() << std::endl;

	std::ofstream ofile;

	ofile.open("clent3.csv", std::ios::out | std::ios::trunc);
	for (int i = 0; i < csvpoint.size(); i++)
	{//外循环帧数

		for (int j = 0; j < csvpoint[i].size(); j++)
		{
			ofile << "\"" << csvpoint[i][j].x << "," << csvpoint[i][j].y << "," << csvpoint[i][j].z << "," << csvpoint[i][j].TrackingState << "\"" << ",";
		}
		ofile << std::endl;
	}
	ofile.close();
	csvpoint.clear();

	SafeRelease(bodysource);
	SafeRelease(bodyreader);
	SafeRelease(coordinatemapper);
	if (kinect)
	{
		kinect->Close();
	}
	SafeRelease(kinect);
	destroyAllWindows();

	//退出  
	system("pause");
}