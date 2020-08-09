#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc.hpp"

using namespace cv;
using namespace std;


class ColorDetector {

private:

	// minimum acceptable distance
	int maxDist;

	// target color
	cv::Vec3b target;

	// image containing color converted image
	cv::Mat converted;
	bool useLab;

	// image containing resulting binary map
	cv::Mat result;

public:

	// empty constructor
	// default parameter initialization here
	ColorDetector() : maxDist(100), target(0, 0, 0), useLab(false) {}

	// extra constructor for Lab color space example
	ColorDetector(bool useLab) : maxDist(100), target(0, 0, 0), useLab(useLab) {}

	// full constructor
	ColorDetector(uchar blue, uchar green, uchar red, int mxDist = 100, bool useLab = false) : maxDist(mxDist), useLab(useLab) {
		// target color
		setTargetColor(blue, green, red);
	}

	// Computes the distance from target color.
	int getDistanceToTargetColor(const cv::Vec3b& color) const {
		return getColorDistance(color, target);
	}

	// Computes the city-block distance between two colors.
	int getColorDistance(const cv::Vec3b& color1, const cv::Vec3b& color2) const {

		return abs(color1[0] - color2[0]) +
			abs(color1[1] - color2[1]) +
			abs(color1[2] - color2[2]);
	}

	// Processes the image. Returns a 1-channel binary image.
	cv::Mat process(const cv::Mat& image);

	cv::Mat operator()(const cv::Mat& image) {

		cv::Mat input;

		if (useLab) { // Lab conversion
			cv::cvtColor(image, input, cv::COLOR_BGR2Lab);
		}
		else {
			input = image;
		}

		cv::Mat output;
		// compute absolute difference with target color
		cv::absdiff(input, cv::Scalar(target), output);
		// split the channels into 3 images
		std::vector<cv::Mat> images;
		cv::split(output, images);
		// add the 3 channels (saturation might occurs here)
		output = images[0] + images[1] + images[2];
		// apply threshold
		cv::threshold(output,  // input image
			output,  // output image
			maxDist, // threshold (must be < 256)
			255,     // max value
			cv::THRESH_BINARY_INV); // thresholding type

		return output;
	}

	// Getters and setters

	// Sets the color distance threshold.
	// Threshold must be positive, otherwise distance threshold
	// is set to 0.
	void setColorDistanceThreshold(int distance) {

		if (distance < 0)
			distance = 0;
		maxDist = distance;
	}

	// Gets the color distance threshold
	int getColorDistanceThreshold() const {

		return maxDist;
	}

	//// Sets the color to be detected
	//// given in BGR color space
	//void setTargetColor(uchar blue, uchar green, uchar red) {
	//	// BGR order
	//	target = cv::Vec3b(blue, green, red);
	//	if (useLab) {
	//		// Temporary 1-pixel image
	//		cv::Mat tmp(1, 1, CV_8UC3);
	//		tmp.at<cv::Vec3b>(0, 0) = cv::Vec3b(blue, green, red);
	//		// Converting the target to Lab color space 
	//		cv::cvtColor(tmp, tmp, cv::COLOR_BGR2Lab);
	//		target = tmp.at<cv::Vec3b>(0, 0);
	//	}
	//}

	// Sets the color to be detected
	void setTargetColor(uchar blue, uchar green, uchar red) {
		// BGR order 
		target = Vec3b(blue, green, red);
	}

	// Sets the color to be detected
	void setTargetColor(cv::Vec3b color) {
		target = color;
	}

	// Gets the color to be detected
	cv::Vec3b getTargetColor() const {

		return target;
	}
};

cv::Mat ColorDetector::process(const cv::Mat& image) {

	// re-allocate binary map if necessary
	// same size as input image, but 1-channel
	result.create(image.size(), CV_8U);

	// Converting to Lab color space 
	if (useLab)
		cv::cvtColor(image, converted, cv::COLOR_BGR2Lab);

	// get the iterators
	cv::Mat_<cv::Vec3b>::const_iterator it = image.begin<cv::Vec3b>();
	cv::Mat_<cv::Vec3b>::const_iterator itend = image.end<cv::Vec3b>();
	cv::Mat_<uchar>::iterator itout = result.begin<uchar>();

	// get the iterators of the converted image 
	if (useLab) {
		it = converted.begin<cv::Vec3b>();
		itend = converted.end<cv::Vec3b>();
	}

	// for each pixel
	for (; it != itend; ++it, ++itout) {

		// process each pixel ---------------------

		  // compute distance from target color
		if (getDistanceToTargetColor(*it) < maxDist) {

			*itout = 255;

		}
		else {

			*itout = 0;
		}

		// end of pixel processing ----------------
	}

	return result;
}

int main() {
	// 1. Create image processor object
	ColorDetector cdetect;

	// 2. Read input image
	cv::Mat image = cv::imread("temple.jpg");
	if (image.empty())
		return 0;
	cv::namedWindow("Original Image");
	cv::imshow("Original Image", image);

	// 3. Set input parameters
	cdetect.setTargetColor(230, 190, 130); // here blue sky

	// 4. Process the image and display the result
	cv::namedWindow("result");
	cv::Mat result = cdetect.process(image);
	cv::imshow("result", result);

	// or using functor
	// here distance is measured with the Lab color space
	ColorDetector colordetector(230, 190, 130,  // color
		45, true); // Lab threshold
	cv::namedWindow("result (functor)");
	result = colordetector(image);
	cv::imshow("result (functor)", result);

	// testing floodfill
	cv::floodFill(image,            // input/ouput image
		cv::Point(100, 50),         // seed point
		cv::Scalar(255, 255, 255),  // repainted color
		(cv::Rect*)0,  // bounding rectangle of the repainted pixel set
		cv::Scalar(35, 35, 35),     // low and high difference threshold
		cv::Scalar(35, 35, 35),     // most of the time will be identical
		cv::FLOODFILL_FIXED_RANGE); // pixels are compared to seed color

	cv::namedWindow("Flood Fill result");
	result = colordetector(image);
	cv::imshow("Flood Fill result", image);

	// Creating artificial images to demonstrate color space properties
	cv::Mat colors(100, 300, CV_8UC3, cv::Scalar(100, 200, 150));
	cv::Mat range = colors.colRange(0, 100);
	range = range + cv::Scalar(10, 10, 10);
	range = colors.colRange(200, 300);
	range = range + cv::Scalar(-10, -10, 10);

	cv::namedWindow("3 colors");
	cv::imshow("3 colors", colors);

	cv::Mat labImage(100, 300, CV_8UC3, cv::Scalar(100, 200, 150));
	cv::cvtColor(labImage, labImage, cv::COLOR_BGR2Lab);
	range = colors.colRange(0, 100);
	range = range + cv::Scalar(10, 10, 10);
	range = colors.colRange(200, 300);
	range = range + cv::Scalar(-10, -10, 10);
	cv::cvtColor(labImage, labImage, cv::COLOR_Lab2BGR);

	cv::namedWindow("3 colors (Lab)");
	cv::imshow("3 colors (Lab)", colors);

	// brightness versus luminance
	cv::Mat grayLevels(100, 256, CV_8UC3);
	for (int i = 0; i < 256; i++) {
		grayLevels.col(i) = cv::Scalar(i, i, i);
	}

	range = grayLevels.rowRange(50, 100);
	cv::Mat channels[3];
	cv::split(range, channels);
	channels[1] = 128;
	channels[2] = 128;
	cv::merge(channels, 3, range);
	cv::cvtColor(range, range, cv::COLOR_Lab2BGR);


	cv::namedWindow("Luminance vs Brightness");
	cv::imshow("Luminance vs Brightness", grayLevels);

	cv::waitKey();

	return 0;
}














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


