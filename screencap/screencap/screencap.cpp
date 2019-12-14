

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>

#include <iostream>
#include <cstdlib>
#include <Windows.h>

using namespace cv;
using namespace std;
/*
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
*/
// settings
using namespace cv;
using namespace std;

Mat hwnd2mat(HWND hwnd) {
	HDC hwindowDC, hwindowCompatibleDC;

	int height, width, srcheight, srcwidth;
	HBITMAP hbwindow;
	//Mat src;
	BITMAPINFOHEADER  bi;
	//hwindowDC = GetWindowDC(hwnd); No change when using GetWindowDC instead of GetDC
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
	height = 800;  //change this to whatever size you want to resize to
	width = 800;
	//src.create(height, width, CV_8UC3); //8UC3 results in gray, 8SC3 darker grey,  16UC3 black and grey, 16SC3 grey and darkgrey, 32FC3 black, 64FC3 black
	Mat src(height, width, CV_8UC3, Scalar(120, 70, 240));
	//trial: check if this matrix will be changed
	//imshow("Filters", src);
	waitKey(500);

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
	// StretcgBlt 
	// handle destination DC: hwindowCompatibleDC :: handle source DC: hwindowDC :: raster operation: SRCCOPY
	//StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, width, height, SRCCOPY); //changed width and height
	//GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, buffer, (BITMAPINFO*)& bi, DIB_RGB_COLORS);
	src.data = buffer;
	// avoid memory leak
	DeleteObject(hbwindow); DeleteDC(hwindowCompatibleDC); ReleaseDC(hwnd, hwindowDC);

	return src;
}

int main()
{
	Mat image(600, 600, CV_8UC3, Scalar(255, 255, 0));
	//image = imread("desktop.bmp", 1);   // Read the file
	HWND hwnd = NULL;
	image = hwnd2mat(hwnd);
	imshow("DisplayShare+", image);
	/*
	if (!image.data)                              // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	//creating a window
	cvNamedWindow("Filters", WINDOW_AUTOSIZE);// Create a window for display.
	//imshow("Display window", image);                   // Show our image inside it.

	//filter blue_light
	Mat blf_image = Mat::zeros(image.size(), image.type());
	Mat empty = Mat::zeros(image.size(), image.type());
	printf("file type: %d", image.type());
	double flux = 0.5; /*< Simple contrast control 
	//int beta = 1;       /*< Simple brightness control 
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

	//filter grayscale
	Mat gsf_image = Mat::zeros(image.size(), image.type());
	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			for (int c = 0; c < image.channels(); c++) {
				gsf_image.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(0.33 * (image.at<Vec3b>(y, x)[0] + image.at<Vec3b>(y, x)[1] + image.at<Vec3b>(y, x)[2]));
			}
		}
	}

	Mat drf_image = Mat::zeros(image.size(), image.type());
	//drf_image = 255 - image;
	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			drf_image.at<Vec3b>(y, x)[2] = saturate_cast<uchar>(255 - (0.5 * (image.at<Vec3b>(y, x)[0] + image.at<Vec3b>(y, x)[1])));
		}
	}
	Mat tmp, dst;
	tmp = image;
	dst = tmp;
	/*
	while (true)
	{
		int c;
		c = waitKey(10);
		if ((char)c == 27) //esc
		{
			break;
		}
		else if ((char)c == 'z')
		{
			tmp = image;
			dst = tmp;
			printf("zoom\n");
			int zoom = 0;
			while (true)
			{
				c = waitKey(10);

				if ((char)c == 27) //esc
				{
					break;
				}
				else if ((char)c == 93) //]
				{
					pyrUp(tmp, dst, Size(tmp.cols * 2, tmp.rows * 2));
					printf("** Zoom In: Image x 2 \n");
					zoom++;
					if (zoom == 0)
					{
						tmp = image;
						dst = tmp;
					}
				}
				else if ((char)c == 91) //[
				{
					pyrDown(tmp, dst, Size(tmp.cols / 2, tmp.rows / 2));
					printf("** Zoom Out: Image / 2 \n");
					zoom--;
					if (zoom == 0)
					{
						tmp = image;
						dst = tmp;
					}
				}

				imshow("Filters", dst);
				tmp = dst;
			}
		}
		else if ((char)c == 'b')  //blue light filter
		{
			while (true)
			{
				c = waitKey(10);
				imshow("Filters", blf_image);
				if ((char)c == 27)
				{
					break;
				}
			}
		}
		else if ((char)c == 'g') //grayscale
		{
			while (true) {
				c = waitKey(10);
				imshow("Filters", gsf_image);
				if ((char)c == 27)
				{
					break;
				}
			}
		}
		else if ((char)c == 'd') //darkroom
		{
			while (true) {
				c = waitKey(10);
				imshow("Filters", drf_image);
				if ((char)c == 27)
				{
					break;
				}
			}
		}
		else if ((char)c == 'r') //brightness
		{
			int beta = 25;
			int br = 0;
			//int etab = 0.75;
			Mat cf_image = Mat::zeros(image.size(), image.type());
			cf_image = image;
			while (true) {
				c = waitKey(10);
				imshow("Filters", cf_image);
				if ((char)c == 27)
				{
					break;
				}
				else if ((char)c == 93)
				{

					printf("brighten\n");
					for (int y = 0; y < cf_image.rows; y++) {
						for (int x = 0; x < cf_image.cols; x++) {
							for (int c = 0; c < cf_image.channels(); c++) {
								cf_image.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(cf_image.at<Vec3b>(y, x)[c] + beta);
							}
						}
					}
					br++;
					if (br == 0)
					{
						cf_image = image;
						imshow("Filters", image);
					}
				}
				else if ((char)c == 91)
				{
					printf("darken\n");
					br--;
					if (br == 0)
					{
						cf_image = image;
						imshow("Filters", cf_image);
					}
					else
					{
						for (int y = 0; y < image.rows; y++) {
							for (int x = 0; x < image.cols; x++) {
								for (int c = 0; c < image.channels(); c++) {
									cf_image.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(cf_image.at<Vec3b>(y, x)[c]) - 25;
								}
							}
						}
					}

				}
			}
		}
		else
		{
			imshow("Filters", image);
		}
	}
	

	for (int i = 0; i < 3; i++)
	{
		printf("i: %d\n", i);
		imshow("Filters", image);
		waitKey(330);
		imshow("Filters", blf_image);
		waitKey(330);
		imshow("Filters", gsf_image);
		waitKey(330);
		imshow("Filters", drf_image);
		waitKey(330);
	}
	*/
	while (true)
	{
		image = hwnd2mat(hwnd);
		imshow("DisplayShare+", image);
		waitKey(1);
	}
	waitKey(0);
	/*
	Mat tmp, dst;
	tmp = image;
	dst = tmp;
	imshow("Filters", dst);
  /// Loop
	while (true)
	{
		int c;
		c = waitKey(10);

		if ((char)c == 27)
		{
			break;
		}
		if ((char)c == 'u')
		{
			pyrUp(tmp, dst, Size(tmp.cols * 2, tmp.rows * 2));
			printf("** Zoom In: Image x 2 \n");
		}
		else if ((char)c == 'd')
		{
			pyrDown(tmp, dst, Size(tmp.cols / 2, tmp.rows / 2));
			printf("** Zoom Out: Image / 2 \n");
		}

		imshow("Filters", dst);
		tmp = dst;
	}
	*/
	return 0;
}