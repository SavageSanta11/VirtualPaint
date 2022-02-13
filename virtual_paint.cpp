#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;


/////////////////  Project 1 - Virtual Painter //////////////////////

Mat img;
VideoCapture cap(0); // 1 is camera id for secondary webcam
	//0 is the id for laptop webcam

vector<vector<int>> newPoints;  // to store all points
//if 0 it will draw purple
//if 1 it will draw green
//the third parameter is to describe the color the point should be drawn in


						   // hmin, smin, vmin hmax, smax, vmax
vector<vector<int>> myColors{ {106,135,0,179,255,255}, // jiffy blue
								{38,114,24,179,255,255},
									{0,114,151,179,255,255} //scissors red
};// mechanical pencil Green
//now describe the colors we're going to show
vector<Scalar> myColorValues{ {255,0,0},		// Purple
								{0,255,0},	
									{0,0,255} };//red	


Point getContours(Mat image) {


	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(image, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//drawContours(img, contours, -1, Scalar(255, 0, 255), 2);
	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());

	Point myPoint(0, 0);

	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);
		cout << area << endl;

		string objectType;

		if (area > 1000)
		{
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

			cout << conPoly[i].size() << endl;
			boundRect[i] = boundingRect(conPoly[i]);
			// to find the bounding rectangle around the polygon
			//we use the bounding box we made above to find the tip of the marker
			myPoint.x = boundRect[i].x + boundRect[i].width / 2;// divide by 2 is so that be draw from the center of the line and not the corner
			myPoint.y = boundRect[i].y;

			//drawContours(img, conPoly, i, Scalar(255, 0, 255), 2);
			//rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5); //draws a green color bounding box
		}
	}
	return myPoint;
}

// smin, vmin, hmin, hmax, smax, vmax
vector<vector<int>> findColor(Mat img)
{
	Mat imgHSV;
	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	for (int i = 0; i < myColors.size(); i++)
	{
		Scalar lower(myColors[i][0], myColors[i][1], myColors[i][2]);
		Scalar upper(myColors[i][3], myColors[i][4], myColors[i][5]);
		Mat mask;
		inRange(imgHSV, lower, upper, mask);
		//imshow(to_string(i), mask);
		Point myPoint = getContours(mask);
		if (myPoint.x != 0) {
			newPoints.push_back({ myPoint.x,myPoint.y,i });
		}
	}
	return newPoints;
}

void drawOnCanvas(vector<vector<int>> newPoints, vector<Scalar> myColorValues)
{

	for (int i = 0; i < newPoints.size(); i++)
	{
		circle(img, Point(newPoints[i][0], newPoints[i][1]), 10, myColorValues[newPoints[i][2]], FILLED);
	}
}


void main() {

	while (true) {

		cap.read(img);
		newPoints = findColor(img);
		drawOnCanvas(newPoints, myColorValues);

		imshow("Image", img);
		waitKey(1);
	}
}