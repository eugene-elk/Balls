#pragma once
#include <iostream>
#include <string>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

#define Pi 3.14159265

using namespace cv;

struct pointColor
{
	float x, y, z;
};

struct pointPosition
{
	unsigned int x, y;
};

class billiardsHandle
{
private:
	
	pointColor A, B_Field, B_Ball, C;
	pointPosition pB, pF;
	int color_diff;

	double BC, AC, AB;
	double angle_alpha;
	double angle_beta;

	int indentTop, indentBottom;

public:

	Mat in_cam, img, imgField;

	billiardsHandle();

	void calibrate(pointPosition& pField, pointPosition& pBall);
	void printColorsOnImg();
	void findField();

};
