#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

#define VERTICES vector<Point>

//  g++ -O docs_scanner.cpp `pkg-config opencv --cflags --libs`
///////////////  Project 2 - Document Scanner  //////////////////////

Mat imgOriginal, imgGray, imgBlur, imgCanny, imgThre, imgDil, imgErode, imgWarp, imgCrop;
VERTICES initialPoints,docPoints;
float w = 420, h = 596;

Mat preProcessing(Mat img)
{
	cvtColor(img, imgGray, COLOR_BGR2GRAY);
  // imwrite("gray.png", imgGray);
	GaussianBlur(imgGray, imgBlur, Size(3, 3), 3, 0);
  // imwrite("blur.png", imgBlur);

	Canny(imgBlur, imgCanny, 25, 150);
  // imwrite("canny.png", imgCanny);
	
  Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(imgCanny, imgDil, kernel);
  imwrite("dil.png", imgDil);

	//erode(imgDil, imgErode, kernel);
	return imgDil;
}

bool shapesComparator(const VERTICES & s1, const VERTICES & s2) {
    float peri = arcLength(s2, true);
    VERTICES conPoly;
		approxPolyDP(s2, conPoly, 0.02 * peri, true);
    
    return conPoly.size()==4 && contourArea(s1) < contourArea(s2);
};

VERTICES getContours(Mat image) {

	vector<VERTICES> contours;
	vector<Vec4i> hierarchy;

	findContours(image, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//drawContours(img, contours, -1, Scalar(255, 0, 255), 2);
	// vector<VERTICES> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());

	VERTICES biggest;
	int maxArea=0;

  VERTICES biggestContour = *max_element(begin(contours), end(contours), shapesComparator);

  float peri = arcLength(biggestContour, true);
  VERTICES conPoly;
	approxPolyDP(biggestContour, conPoly, 0.02 * peri, true);
	biggest = { conPoly[0], conPoly[1], conPoly[2], conPoly[3] };

	return biggest;
}

void drawPoints(VERTICES points, Scalar color)
{
	for (int i = 0; i < points.size(); i++)
	{
		circle(imgOriginal, points[i], 10, color, FILLED);
		putText(imgOriginal, to_string(i), points[i], FONT_HERSHEY_PLAIN, 4, color, 4);
	}
}

VERTICES reorder(VERTICES points)
{
	VERTICES newPoints;
	vector<int> sumPoints, subPoints;

	for (int i = 0; i < 4; i++)
	{
		sumPoints.push_back(points[i].x + points[i].y);
		subPoints.push_back(points[i].x - points[i].y);
	}

	newPoints.push_back(points[min_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]); // 0
	newPoints.push_back(points[max_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]); //1
	newPoints.push_back(points[min_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]); //2
	newPoints.push_back(points[max_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]); //3

	return newPoints;
}

bool isOrientationHorizontal(VERTICES points){
  double vec10 = sqrt(pow(points[0].x - points[1].x, 2) + pow(points[0].y - points[1].y, 2));
  double vec32 = sqrt(pow(points[2].x - points[3].x, 2) + pow(points[2].y - points[3].y, 2));
  double vec20 = sqrt(pow(points[0].x - points[2].x, 2) + pow(points[0].y - points[2].y, 2));
  double vec31 = sqrt(pow(points[1].x - points[3].x, 2) + pow(points[1].y - points[3].y, 2));

  return (vec10 + vec32 > vec20 + vec31);
}

Mat getWarp(Mat img, VERTICES points, float w, float h )
{
	Point2f src[4] = { points[0], points[1], points[2], points[3] };
	Point2f dst[4] = { {0.0f,0.0f},{w,0.0f},{0.0f,h},{w,h} };

	Mat matrix = getPerspectiveTransform(src, dst);
	warpPerspective(img, imgWarp, matrix, Point(w, h));

	return imgWarp;
}

int main() {

	// string path = "Resources/paper.jpg";
	// string path = "Resources/zolta_kartka.jpg";
	// string path = "Resources/kara.jpg";
	string path = "Resources/trees.jpg";
	imgOriginal = imread(path);
	//resize(imgOriginal, imgOriginal, Size(), 0.5, 0.5);

	// Preprpcessing - Step 1 
	imgThre = preProcessing(imgOriginal);

	// Get Contours - Biggest  - Step 2
	initialPoints = getContours(imgThre);
	//drawPoints(initialPoints, Scalar(0, 0, 255));
	docPoints = reorder(initialPoints);
	//drawPoints(docPoints, Scalar(0, 255, 0));

  // swap width and height if paper is positioned horizontaly
  if(isOrientationHorizontal(docPoints)){
    float temp = w;
    w = h;
    h = temp;
  }

	// Warp - Step 3 
	imgWarp = getWarp(imgOriginal, docPoints, w, h);

	//Crop - Step 4
	int cropVal= 5;
	Rect roi(cropVal, cropVal, w - (2 * cropVal), h - (2 * cropVal));
	imgCrop = imgWarp(roi);

	imwrite("org.png", imgOriginal);
	//imshow("Image Dilation", imgThre);
	//imshow("Image Warp", imgWarp);
	imwrite("new.png", imgCrop);
	waitKey(0);

}
