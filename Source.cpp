#include <opencv2/core/core.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
using namespace std;
using namespace cv;

void detectHScolor(const Mat& image, double minHue, double maxHue, 
	double minSat, double maxSat, Mat& mask) {
	// convert into HSV space
	Mat hsv;
	cvtColor(image, hsv, COLOR_BGR2HSV);
	
	// split the 3 channels into 3 images
	vector<Mat> channels;

	split(hsv, channels);
	// channels[0] is the HUE
	// channels[1] is the SATURATION
	// channels[2] is the VALUE
	
	// Hue masking
	Mat mask1; // under maxHue
	threshold(channels[0], mask1, maxHue, 255, THRESH_BINARY_INV);
	Mat mask2; // under maxHue
	threshold(channels[0], mask2, minHue, 255, THRESH_BINARY);

	Mat hueMask; // hue mask
	if (minHue < maxHue)
		hueMask = mask1 & mask2;
	else
		hueMask = mask1 | mask2;

	// Saturation masking
	// under maxSat
	threshold(channels[1], mask1, maxSat, 255, THRESH_BINARY_INV);
	// over minSat
	threshold(channels[1], mask2, minSat, 255, THRESH_BINARY);
	Mat satMask; // saturation mask
	satMask = mask1 & mask2;

	// combined mask
	mask = hueMask & satMask;
}


int main() {
	Mat image = imread("face.jpg");
	Mat mask;
	detectHScolor(image, 160, 10, 25, 166, mask);

	Mat detected(image.size(), CV_8UC3, Scalar(0, 0, 0));
	image.copyTo(detected, mask);

	imshow("Detection result", detected);
	waitKey(0);
	return 0;
}


















//// GRABCUT ALGORITHM
//#include <opencv2/core/core.hpp>
//#include "opencv2/imgcodecs.hpp"
//#include "opencv2/highgui.hpp"
//#include "opencv2/imgproc.hpp"
//#include <iostream>
//using namespace std;
//using namespace cv;
//
//
//int main() {
//	Mat image = imread("temple.jpg"); // image
//	Mat img = imread("temple.jpg");
//	Rect rectangle(50, 50, 600, 400); // rect 
//	Mat mask, bgModel, fgModel;
//	mask.create(image.size(), CV_8UC1);
//
//	grabCut(image, mask, rectangle, bgModel, fgModel, 2, GC_INIT_WITH_RECT);
//
//  // GREY BACKGROUND
//	for (int i = 0; i < image.rows; i++) {
//		for (int j = 0; j < image.cols; j++) {
//			if (mask.ptr(i, j)[0] == 0 || mask.ptr(i, j)[0] == 2) {
//				image.ptr(i, j)[0] = 70;
//				image.ptr(i, j)[1] = 70;
//				image.ptr(i, j)[2] = 70;
//			}
//		}
//	}
//
//	namedWindow("Original Image");
//	namedWindow("Result of GrabCut Algorithm");
//	cv::imshow("Original Image", img);
//	cv::imshow("Result of GrabCut Algorithm", image);
//	cv::waitKey(0);
//
//	return 0;
//}



// a func accessing each pixel
//void salt(Mat image, int n) {
//	int i, j;
//	for (int k = 0; k < n; k++) {
//		// rand() is the random number generator
//		i = rand() % image.cols;
//		j = rand() % image.rows;
//
//		if (image.type() == CV_8UC1) {
//			// gray-level image
//			image.at<uchar>(j, i) = 255;
//		}
//		else if (image.type() == CV_8UC3) {
//			// color image
//			image.at<Vec3b>(j, i)[0] = 255;
//			image.at<Vec3b>(j, i)[1] = 255;
//			image.at<Vec3b>(j, i)[2] = 255;
//		}
//	}
//}


// image onto another image
//Mat image, logo;
	//image = imread("boo.jpg");
	//logo = imread("logo.jpg");
	//Rect myRoI = Rect(image.cols - logo.cols, image.rows - logo.rows, logo.cols, logo.rows);
	//// define image ROI at image bottom-right
	//Mat imageROI(image, myRoI);
	//// insert logo
	//logo.copyTo(imageROI);
	//imshow("Combined image", image);
	//waitKey(0);


//Mat image;
//Mat result;
//Mat img;
//
//void onMouse(int event, int x, int y, int flags, void* param) {
//	Mat* im = reinterpret_cast<Mat*>(param);
//	switch (event)
//	{
//	case EVENT_LBUTTONDOWN:
//		cout << "at (" << x << "," << y << ") value is: " << static_cast<int>(im->at<uchar>(Point(x, y))) << endl;
//		break;
//	}
//}
//int main() {
//	image = imread("boo.jpg", IMREAD_GRAYSCALE);
//	circle(image, Point(200, 110), 100, 0, 3);
//	putText(image, "This is a dog.", Point(40, 200), FONT_HERSHEY_COMPLEX, 1.0, 255, 2);
//
//	namedWindow("Output Image");
//
//	setMouseCallback("Output Image", onMouse, reinterpret_cast<void*>(&image));
//
//	imshow("Output Image", image);
//
//	int c = waitKey(5000);
//
//	destroyAllWindows();
//
//	return 0;
//}


