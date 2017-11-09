#include "billiardsHandle.h"

billiardsHandle::billiardsHandle()
{
	A.x = 0; A.y = 0; A.z = 0;
	B_Field.z = 0; B_Field.y = 0; B_Field.x = 0;
	B_Ball.z = 0; B_Ball.y = 0; B_Ball.x = 0;
	angle_beta = 10;

	img = Mat::zeros(Size(480, 260), CV_8UC3);
	imgField = Mat::zeros(Size(480, 260), CV_8UC3);

	color_diff = 70;

	indentTop = 10;
	indentBottom = 20;
}

void billiardsHandle::calibrate(pointPosition& pField, pointPosition& pBall)
{
	pF = pField;
	pB = pBall;

	// calibrate
	for (int x = -2; x < 2; x++) 
	{
		for (int y = -2; y < 2; y++) 
		{
			B_Field.z = B_Field.z + (img.at<Vec3b>(pF.y + y, pF.x + x)[0] / 16.0);
			B_Field.y = B_Field.y + (img.at<Vec3b>(pF.y + y, pF.x + x)[1] / 16.0);
			B_Field.x = B_Field.x + (img.at<Vec3b>(pF.y + y, pF.x + x)[2] / 16.0);

			B_Ball.z = B_Ball.z + (img.at<Vec3b>(pB.y + y, pB.x + x)[0] / 16.0);
			B_Ball.y = B_Ball.y + (img.at<Vec3b>(pB.y + y, pB.x + x)[1] / 16.0);
			B_Ball.x = B_Ball.x + (img.at<Vec3b>(pB.y + y, pB.x + x)[2] / 16.0);
		}
	}

	std::cout << "BALLS COLOR: " << (int)B_Ball.x << " " << (int)B_Ball.y << " " << (int)B_Ball.z << std::endl;
	std::cout << "FIELD COLOR: " << (int)B_Field.x << " " << (int)B_Field.y << " " << (int)B_Field.z << std::endl;
}

void billiardsHandle::printColorsOnImg()
{
	int count = 0;
	for (int x = indentTop; x < 260 - indentBottom; x++)
	{
		for (int y = 0; y < 480; y++)
		{
			C.z = img.at<Vec3b>(x, y)[0];
			C.y = img.at<Vec3b>(x, y)[1];
			C.x = img.at<Vec3b>(x, y)[2];

			if ((abs(C.x - B_Ball.x) < color_diff) && (abs(C.y - B_Ball.y) < color_diff) && (abs(C.z - B_Ball.z) < color_diff))
			{
				count += 1;
				img.at<Vec3b>(x, y)[0] = 255;
				img.at<Vec3b>(x, y)[1] = 0;
				img.at<Vec3b>(x, y)[2] = 0;
			}

		}
	}
	//std::cout << count << std::endl;
}

void billiardsHandle::findField()
{
	int topX = 260, bottomX = 0, rightY = 0, leftY = 480;

	for (int x = indentTop; x < 260 - indentBottom; x++)
	{
		for (int y = 0; y < 480; y++)
		{�
			C.z = img.at<Vec3b>(x, y)[0];
			C.y = img.at<Vec3b>(x, y)[1];
			C.x = img.at<Vec3b>(x, y)[2];

			BC = sqrt((B_Field.x - C.x)*(B_Field.x - C.x) + (B_Field.y - C.y)*(B_Field.y - C.y) + (B_Field.z - C.z)*(B_Field.z - C.z));
			AB = sqrt((B_Field.x - A.x)*(B_Field.x - A.x) + (B_Field.y - A.y)*(B_Field.y - A.y) + (B_Field.z - A.z)*(B_Field.z - A.z));
			AC = sqrt((A.x - C.x)*(A.x - C.x) + (A.y - C.y)*(A.y - C.y) + (A.z - C.z)*(A.z - C.z));

			angle_alpha = acos((AB*AB + AC*AC - BC*BC) / (2 * AB*AC));
			angle_alpha = angle_alpha / Pi*180.0;

			if (abs(angle_alpha) < angle_beta)
			{
				if (x > bottomX)	bottomX = x;
				if (x < topX)		topX = x;
				if (y > rightY)		rightY = y;
				if (y < leftY)		leftY = y;
				imgField.at<Vec3b>(x, y)[0] = 0;
				imgField.at<Vec3b>(x, y)[1] = 255;
				imgField.at<Vec3b>(x, y)[2] = 0;
			}
		}
	}
	imshow("field", imgField);
	std::cout << "topX:    " << topX << std::endl;
	std::cout << "bottomX: " << bottomX << std::endl;
	std::cout << "rightY:  " << rightY << std::endl;
	std::cout << "leftY:   " << leftY << std::endl;

	char c = cvWaitKey(100);
}