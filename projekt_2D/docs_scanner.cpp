#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

#define VERTICES vector<Point>

//  g++ -O docs_scanner.cpp `pkg-config opencv --cflags --libs`
///////////////  Project 2 - Document Scanner  //////////////////////

VERTICES initialPoints, docPoints;
float w = 420, h = 596;

Mat preProcessing(Mat img) {
  Mat imgPrecessed;

	cvtColor(img, imgPrecessed, COLOR_BGR2GRAY);
	GaussianBlur(imgPrecessed, imgPrecessed, Size(3, 3), 3, 0);

	Canny(imgPrecessed, imgPrecessed, 25, 150);
	
  Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(imgPrecessed, imgPrecessed, kernel);

	return imgPrecessed;
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
	vector<Rect> boundRect(contours.size());

  VERTICES biggestContour = *max_element(begin(contours), end(contours), shapesComparator);

  float peri = arcLength(biggestContour, true);
  VERTICES conPoly;
	approxPolyDP(biggestContour, conPoly, 0.02 * peri, true);
	VERTICES biggest = { conPoly[0], conPoly[1], conPoly[2], conPoly[3] };

	return biggest;
}

VERTICES reorder(VERTICES points) {
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

bool isOrientationHorizontal(VERTICES points) {
  double vec10 = sqrt(pow(points[0].x - points[1].x, 2) + pow(points[0].y - points[1].y, 2));
  double vec32 = sqrt(pow(points[2].x - points[3].x, 2) + pow(points[2].y - points[3].y, 2));
  double vec20 = sqrt(pow(points[0].x - points[2].x, 2) + pow(points[0].y - points[2].y, 2));
  double vec31 = sqrt(pow(points[1].x - points[3].x, 2) + pow(points[1].y - points[3].y, 2));

  return (vec10 + vec32 > vec20 + vec31);
}

Mat getWarp(Mat img, VERTICES points, float w, float h )
{
  Mat imgWarp;

	Point2f src[4] = { points[0], points[1], points[2], points[3] }; // znalezione rogi kartki
	Point2f dst[4] = { {0.0f,0.0f},{w,0.0f},{0.0f,h},{w,h} }; // docelowe rogi zdjęcia 00. w0, 0h, wh

	Mat matrix = getPerspectiveTransform(src, dst); //kalkuluje perspektywe
	warpPerspective(img, imgWarp, matrix, Point(w, h)); //"zwija" prostuje znalezione punkty kartki do rogów pliku

	return imgWarp;
}

int main(int argc, char *argv[]) {

  Mat imgOriginal;

  if(argc < 2){
    cout << "Nie podano ścieżki do pliku" << endl;
    return -1;
  }

  if(argc == 2){
    try{
      imgOriginal = imread(argv[1]);
    } catch( Exception& e ){
      cout << "Problem z odczytem pliku" << endl;
      cout << e.what() << endl;
      return -1;
    }
  }

  if (imgOriginal.empty()) {
    cout << "Nie ma takiego pliku" << endl;
    return -1;
  }
	// string path = "Resources/paper.jpg";
	// string path = "Resources/reklama.jpg";
	// string path = "Resources/kara.jpg";
	// string path = "Resources/trees.jpg";

	// imgOriginal = imread(path);
	//resize(imgOriginal, imgOriginal, Size(), 0.5, 0.5);

	// Preprpcessing - Step 1 
	Mat imgPrecessed = preProcessing(imgOriginal);

	// Get Contours - Biggest  - Step 2
	initialPoints = getContours(imgPrecessed);
	docPoints = reorder(initialPoints);

  // swap width and height if paper is positioned horizontaly
  if(isOrientationHorizontal(docPoints)){
    float temp = w;
    w = h;
    h = temp;
  }

	// Warp - Step 3 
	Mat imgWarp = getWarp(imgOriginal, docPoints, w, h);

	//Crop - Step 4
	int cropVal= 5;
	Rect roi(cropVal, cropVal, w - (2 * cropVal), h - (2 * cropVal));
	Mat imgFinal = imgWarp(roi);

	// imwrite("org.png", imgOriginal);
	imwrite("new.png", imgFinal);
	waitKey(0);

}
