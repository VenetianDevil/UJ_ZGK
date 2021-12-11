#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

#define VERTICES vector<Point>
float w = 640, h = 877;

//  g++ -O docs_scanner.cpp `pkg-config opencv --cflags --libs`

Mat getImgPrepared(Mat img) {
  Mat imgPrepared;

	cvtColor(img, imgPrepared, COLOR_BGR2GRAY); // RBG to Gray

	GaussianBlur(imgPrepared, imgPrepared, Size(3, 3), 3, 0); // adding Blur to img

	Canny(imgPrepared, imgPrepared, 25, 150); // finding contours in the img
	
  Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3)); // setting 
	dilate(imgPrepared, imgPrepared, kernel); // "creates shapes" based on contours

	return imgPrepared;
}

bool shapesComparator(const VERTICES & s1, const VERTICES & s2) {
    float peri = arcLength(s2, true);
    VERTICES polygonPoints;
		approxPolyDP(s2, polygonPoints, 0.02 * peri, true);
    
    return polygonPoints.size()==4 && contourArea(s1) < contourArea(s2); // bigger quadrangle
};

VERTICES getSheetVertices(Mat image) {

	vector<VERTICES> contours;
	vector<Vec4i> hierarchy;

	findContours(image, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // find all shapes

  // get biggest contour, assuming thats our paper
  VERTICES biggestContour = *max_element(begin(contours), end(contours), shapesComparator); 

  float peri = arcLength(biggestContour, true); // returns contour perimeter or a curve length
  VERTICES polygonPoints;
	approxPolyDP(biggestContour, polygonPoints, 0.02 * peri, true); // approximates contour with another polygon/curve with less vertices
	VERTICES biggest = { polygonPoints[0], polygonPoints[1], polygonPoints[2], polygonPoints[3] }; // geting those vertices

	return biggest;
}

VERTICES reorderVerticles(VERTICES points) {
	VERTICES reorderedPoints;
	vector<int> sumPoints, subPoints;

	for (int i = 0; i < 4; i++)
	{
		sumPoints.push_back(points[i].x + points[i].y); // min will be 0, max will be 3
		subPoints.push_back(points[i].x - points[i].y); // min will be 2, max will be 1
	}

  // get min or max el. from sumPoints and subPoints and sub the begening to get the index of that value in array 'points'
	reorderedPoints.push_back(points[min_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]); // 0
	reorderedPoints.push_back(points[max_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]); //1
	reorderedPoints.push_back(points[min_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]); //2
	reorderedPoints.push_back(points[max_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]); //3

	return reorderedPoints;
}

bool isOrientationHorizontal(VERTICES points) {
  double vec10 = sqrt(pow(points[0].x - points[1].x, 2) + pow(points[0].y - points[1].y, 2));
  double vec32 = sqrt(pow(points[2].x - points[3].x, 2) + pow(points[2].y - points[3].y, 2));
  double vec20 = sqrt(pow(points[0].x - points[2].x, 2) + pow(points[0].y - points[2].y, 2));
  double vec31 = sqrt(pow(points[1].x - points[3].x, 2) + pow(points[1].y - points[3].y, 2));

  // if sum of horizontal edges is bigger than sum of vertical edges, the img is horizontal
  return (vec10 + vec32 > vec20 + vec31);
}

Mat getWarp(Mat img, VERTICES points, float w, float h )
{
  Mat imgWarp;

	Point2f src[4] = { points[0], points[1], points[2], points[3] }; // the sheet vertices we found
	Point2f dst[4] = { {0.0f,0.0f},{w,0.0f},{0.0f,h},{w,h} }; // the final corners of the img 00. w0, 0h, wh

	Mat matrix = getPerspectiveTransform(src, dst); // calc the perspective
	warpPerspective(img, imgWarp, matrix, Point(w, h)); // "wrap", "fit" the sheet into the full img; set found vertices into img corners

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

	// prepare img to process
	Mat imgPrepared = getImgPrepared(imgOriginal);

	// Get Paper's vertices
	VERTICES sheetVertices = getSheetVertices(imgPrepared);
	sheetVertices = reorderVerticles(sheetVertices);

  // swap width and height if paper is positioned horizontaly
  if(isOrientationHorizontal(sheetVertices)){
    float temp = w;
    w = h;
    h = temp;
  }

	// Warp
	Mat imgWarp = getWarp(imgOriginal, sheetVertices, w, h);

	//Crop
	int cropVal= 5; // margin
	Rect roi(cropVal, cropVal, w - (2 * cropVal), h - (2 * cropVal)); // creates rectangle (our final img) [margin * 2 bcs we take it from both sides]
	Mat imgFinal = imgWarp(roi);

	imwrite("new.png", imgFinal);

}
