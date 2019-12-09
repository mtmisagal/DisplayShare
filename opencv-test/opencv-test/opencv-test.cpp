

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

int main()
{
	Mat image;
	image = imread("desktop.bmp", 1);   // Read the file

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
	double flux = 0.5; /*< Simple contrast control */
	//int beta = 1;       /*< Simple brightness control */
	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			for (int c = 0; c < image.channels(); c++) {
				if(c==2)
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
					drf_image.at<Vec3b>(y, x)[2] = saturate_cast<uchar>(255-(0.5*(image.at<Vec3b>(y, x)[0]+image.at<Vec3b>(y, x)[1])));
		}
	}
	Mat tmp, dst;
	tmp = image;
	dst = tmp;
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
			while(true)
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
			while(true){
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
		else
		{
			imshow("Filters", image);
		}
	}


	/*
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