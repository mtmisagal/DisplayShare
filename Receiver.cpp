#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>

#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <cstdlib>
#include <Windows.h>

#define HEIGHT 800
#define WIDTH 800
#pragma comment (lib,"ws2_32.lib")

using namespace cv;
using namespace std;

int main() {
	WSADATA WsData;
	int ret = WSAStartup(MAKEWORD(2, 2), &WsData);
	if (ret != 0) {
		printf("Can't initialize Winsock! Error: %d", ret);
		return -1;
	}

	//Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listening == INVALID_SOCKET) {
		printf("Can't create a socket! Error: %d", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	//Bind the ip and port to a socket
	sockaddr_in hint = {};
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.s_addr = INADDR_ANY; //Could also use inet_pton

	ret = ::bind(listening, (sockaddr*)& hint, sizeof(hint));
	if (ret == SOCKET_ERROR) {
		printf("Can't bind socket! Error: %d", WSAGetLastError());
		closesocket(listening);
		WSACleanup();
		return -1;
	}

	//Tell winsock the socket is for listening
	ret = listen(listening, SOMAXCONN);
	if (ret == SOCKET_ERROR) {
		printf("Can't listen on socket! Error: %d", WSAGetLastError());
		closesocket(listening);
		WSACleanup();
		return -1;
	}

	//Wait for a connection
	sockaddr_in client = {};
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)& client, &clientSize);
	if (clientSocket == INVALID_SOCKET) {
		printf("Can't accept a client! Error: %d", WSAGetLastError());
		closesocket(listening);
		WSACleanup();
		return -1;
	}

	char host[NI_MAXHOST] = {};  //Client's remote name

	ret = getnameinfo((sockaddr*)& client, sizeof(client), host, NI_MAXHOST, NULL, 0, 0);
	if (ret != 0) {
		printf("Can't get client name info! Error: %d", ret);
		inet_ntop(AF_INET, &(client.sin_addr), host, NI_MAXHOST);
	}

	printf("Client: %s, Connected on port: %hu", host, ntohs(client.sin_port));

	//Close listening socket - we don't need it anymore - later on we'll learn how to accept multiple client
	closesocket(listening);

	//while loop: accept and echo message back to client
	char buf[4096];
	int bytesReceived;

	Mat  img = Mat::zeros(HEIGHT, WIDTH, CV_8UC3);
	int  imgSize = img.total() * img.elemSize();
	uchar* sockData = new uchar[imgSize];
	cvNamedWindow("Filters", WINDOW_AUTOSIZE);// Create a window for display.

	while (true)
	{
		int bytes = 0;

		//Receive data here
		for (int i = 0; i < imgSize; i += bytes) {
			bytes = recv(clientSocket, (char*)sockData + i, imgSize - i, 0);
		}
		Mat my_mat(Size(HEIGHT, WIDTH), CV_8UC3, sockData);

		// Display in window
	cv:imshow("Filters", my_mat);
		cv::waitKey(1);
		//break;
	}

	//Close the socket
	closesocket(clientSocket);

	//Cleanup winsock
	WSACleanup();

	return 0;
}
