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
#include <cstring>

#define HEIGHT 800
#define WIDTH 800
#pragma comment (lib,"ws2_32.lib")

using namespace cv;
using namespace std;

Mat hwnd2mat(HWND hwnd, int x, int y) {
	HDC hwindowDC, hwindowCompatibleDC;

	int height, width, srcheight, srcwidth;
	HBITMAP hbwindow;
	BITMAPINFOHEADER  bi;
	hwindowDC = GetDC(hwnd);
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);
	//SetStretchBltMode(hwindowCompatibleDC, HALFTONE);

	RECT windowsize;    // get the height and width of the screen
	GetClientRect(hwnd, &windowsize);

	srcheight = windowsize.bottom;
	srcwidth = windowsize.right;
	//height = windowsize.bottom / 4;  //change this to whatever size you want to resize to
	//width = windowsize.right / 4;
	height = HEIGHT;  //change this to whatever size you want to resize to
	width = WIDTH;
	//src.create(height, width, CV_8UC3); //8UC3 results in gray, 8SC3 darker grey,  16UC3 black and grey, 16SC3 grey and darkgrey, 32FC3 black, 64FC3 black
	Mat src(height, width, CV_8UC3, Scalar(120, 70, 240));
	//trial: check if this matrix will be changed
	//imshow("Filters", src);
	//waitKey(500);

	//default for OpenCV is 8UC3 for 8bit BGR
	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	//bi.biBitCount = 32;
	bi.biBitCount = 24;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	unsigned char* buffer = (unsigned char*)malloc(width * height * 3); //temporary buffer :: window changes color from initial pink to gray
	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	// copy from the window device context to the bitmap device context
	// handle destination DC: hwindowCompatibleDC :: handle source DC: hwindowDC :: raster operation: SRCCOPY
	//StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, x, y, width, height, SRCCOPY); //changed width and height
	//GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, buffer, (BITMAPINFO*)& bi, DIB_RGB_COLORS);
	src.data = buffer;
	// avoid memory leak
	DeleteObject(hbwindow); DeleteDC(hwindowCompatibleDC); ReleaseDC(hwnd, hwindowDC);

	return src;
}


int main() {
	string ipAddress = "127.0.0.1"; // IP Address of the server
	int port = 54000; //Listening port # on the server

	// Initialize winsock
	WSADATA WSData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &WSData);
	if (wsOk != 0)
	{
		cerr << "Can't Initialize winsock! Err #" << wsOk << endl;
		return -1;
	}

	// Create a socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Err #" << WSAGetLastError() << endl;
		WSACleanup();
		return -1;
	}

	// Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	// Connect to server
	int connResult = connect(sock, (sockaddr*)& hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		cerr << "Can't connect to server, Err #" << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return -1;
	}
  
	char buf[4096];
	string userInput;
	Mat image(600, 600, CV_8UC3, Scalar(255, 255, 0)); //temporary test buffer: you can change the scalar content
	HWND hwnd = NULL;
	int x = 0;
	int y = 0;

	Mat tmp, dst;
	tmp = image;
	dst = tmp;
	double flux = 0.5;
	int q;
	do
	{
		image = hwnd2mat(hwnd, x, y);
		if (!image.data)                              // Check for invalid input
		{
			cout << "Could not open or find the image" << std::endl;
			return -1;
		}
		
		q = waitKey(1000);
		if ((char)q == 27) //esc
		{
			break;
		}
		else if ((char)q == 'b')  //blue light filter
		{
			image = image = hwnd2mat(hwnd, x, y);
			Mat blf_image = Mat::zeros(image.size(), image.type());
			for (int y = 0; y < image.rows; y++) {
				for (int x = 0; x < image.cols; x++) {
					for (int c = 0; c < image.channels(); c++) {
						if (c == 2)
						{
							blf_image.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(flux * image.at<Vec3b>(y, x)[c]);
						}
						else
						{
							blf_image.at<Vec3b>(y, x)[c] = image.at<Vec3b>(y, x)[c];
						}
					}
				}
			}
			image = blf_image;
		}
		else if ((char)q == 'g') //grayscale
		{
			//filter grayscale
			image = image = hwnd2mat(hwnd, x, y);
			Mat gsf_image = Mat::zeros(image.size(), image.type());
			for (int y = 0; y < image.rows; y++) {
				for (int x = 0; x < image.cols; x++) {
					for (int c = 0; c < image.channels(); c++) {
						gsf_image.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(0.33 * (image.at<Vec3b>(y, x)[0] + image.at<Vec3b>(y, x)[1] + image.at<Vec3b>(y, x)[2]));
					}
				}
			}
			image = gsf_image;

		}
		else if ((char)q == 'n') //darkroom red nightmode
		{
			image = image = hwnd2mat(hwnd, x, y);
			Mat drf_image = Mat::zeros(image.size(), image.type());
			//drf_image = 255 - image;
			for (int y = 0; y < image.rows; y++) {
				for (int x = 0; x < image.cols; x++) {
					drf_image.at<Vec3b>(y, x)[2] = saturate_cast<uchar>(255 - (0.5 * (image.at<Vec3b>(y, x)[0] + image.at<Vec3b>(y, x)[1])));
				}
			}
			image = drf_image;
		}
		else if ((char)q == 'w') //up
		{
			if (y == 300)
			{
				y = 300;
			}
			else
			{
				y = y + 100;
				q = 'x';
			}
		}
		else if ((char)q == 's') //down
		{
			if (y == 300)
			{
				y = 300;
			}
			else
			{
				y = y - 100;
				q = 'x';
			}
		}
		else if ((char)q == 'a') //left
		{
			if (x == 0)
			{
				x = 0;
			}
			else
			{
				x = x - 100;
				q = 'x';
			}
		}
		else if ((char)q == 'd') //right
		{
			if (x == 500)
			{
				x = 500;
			}
			else
			{
				x = x + 100;
				q = 'x';
			}
		}
		else if ((char)q == '1') //right
		{
			x = 0;
			y = 0;
		}
		else
		{
			//image = image = hwnd2mat(hwnd, x, y);
		}
		/*cvNamedWindow("DisplayShare+", WINDOW_AUTOSIZE);// Create a window for display.
		imshow("DisplayShare+", image);
		cv::waitKey(1);*/

		image = (image.reshape(0, 1)); // to make it continuous
		int  imgSize = image.total() * image.elemSize();
		printf("char: %d", q);
		//if (userInput.size() > 0) // The user has typed in something
		if (/*ctr==0*/ true)
		{

			// Send the text
			//int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			int sendResult = send(sock, (char*)image.data, imgSize, 0);
			if (sendResult != SOCKET_ERROR)
			{
				//ctr = 1;
			}

		}
		q = waitKey(1000);
	} while (/*userInput.size() > 0*/ true);

	// Gracefully close down everything
	closesocket(sock);
	WSACleanup();

	return 0;
}
