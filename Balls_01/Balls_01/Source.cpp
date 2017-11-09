#include <stdlib.h>
#include <stdio.h>

#include "billiardsHandle.h"

using namespace std;

pointPosition pField, pBall;

static void onMouse(int event, int x, int y, int, void*)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		pField.x = x;
		pField.y = y;
		std::cout << pField.x << " " << pField.y << std::endl;
	}
	if (event == EVENT_RBUTTONDOWN)
	{
		pBall.x = x;
		pBall.y = y;
		std::cout << pBall.x << " " << pBall.y << std::endl;
	}
};

int main()
{
	billiardsHandle game;

	VideoCapture capture("video_balls_02.mp4");

	//Начинаем показывать видео для калибровки цвета поля и цвета белых шаров
	//Мышкой выделяем нужные цвета
	//Правый клик - поле
	//Левый клик - шар
	//Для завершения калибровки нажать Enter

	namedWindow("main", 1);
	setMouseCallback("main", onMouse, 0);
	while (1)
	{
		capture >> game.in_cam;
		resize(game.in_cam, game.img, Size(480, 260));
		imshow("main", game.img);
		char c = cvWaitKey(40);
		if (c == 13)
			break;
	}

	game.calibrate(pField, pBall);

	cout << "calibrated" << endl;

	game.findField();

	while (1)
	{
		capture >> game.in_cam;
		resize(game.in_cam, game.img, Size(480, 260));

		game.printColorsOnImg();
		
		imshow("main", game.img);
		char c = cvWaitKey(40);
		if (c == 27)
			break;
	}
	return 0;
}