#include "BilliardsHandle.h"

BilliardHandle::BilliardHandle() {

	A.x = 0; A.y = 0; A.z = 0;
	B_Field.z = 0; B_Field.y = 0; B_Field.x = 0;
	B_Ball.z = 0; B_Ball.y = 0; B_Ball.x = 0;
	angle_beta = 10;

	img = cv::Mat::zeros(cv::Size(480, 260), CV_8UC3);
	imgField = cv::Mat::zeros(cv::Size(480, 260), CV_8UC3);

	color_diff = 70;

	indentTop = 10;
	indentBottom = 20;

}

void BilliardHandle::calibrate() {
	for (int x = -2; x < 2; ++x) {
		for (int y = -2; y < 2; ++y) {
			//std::cout << "x = " << x << " y = " << y << std::endl;
			//std::cout << "Field" << std::endl;
			//std::cout << "x = " << pF.getX() << " y = " << pF.getY() << std::endl;
			B_Field.z += img.at<cv::Vec3b>(pF.getY() + y, pF.getX() + x)[0] / 16.0;
			B_Field.y += img.at<cv::Vec3b>(pF.getY() + y, pF.getX() + x)[1] / 16.0;
			B_Field.x += img.at<cv::Vec3b>(pF.getY() + y, pF.getX() + x)[2] / 16.0;
			//std::cout << "Ball" << std::endl;
			B_Ball.z += img.at<cv::Vec3b>(pB.getY() + y, pB.getX() + x)[0] / 16.0;
			B_Ball.y += img.at<cv::Vec3b>(pB.getY() + y, pB.getX() + x)[1] / 16.0;
			B_Ball.x += img.at<cv::Vec3b>(pB.getY() + y, pB.getX() + x)[2] / 16.0;
		}
	}

	std::cout << "BALLS COLOR: " << (int)B_Ball.x << " " << (int)B_Ball.y << " " << (int)B_Ball.z << std::endl;
	std::cout << "FIELD COLOR: " << (int)B_Field.x << " " << (int)B_Field.y << " " << (int)B_Field.z << std::endl;
}

void BilliardHandle::printColorsOnImg() {
	int count = 0;
	for (int x = 0; x < 260; ++x) {
		for (int y = 0; y < 480; y++) {
			C.z = img.at<cv::Vec3b>(x, y)[0];
			C.y = img.at<cv::Vec3b>(x, y)[1];
			C.x = img.at<cv::Vec3b>(x, y)[2];

			if ((abs(C.x - B_Ball.x) < color_diff) && (abs(C.y - B_Ball.y) < color_diff) && (abs(C.z - B_Ball.z) < color_diff)) {
				count += 1;
				img.at<cv::Vec3b>(x, y)[0] = 255;
				img.at<cv::Vec3b>(x, y)[1] = 255;
				img.at<cv::Vec3b>(x, y)[2] = 255;
			}
			else {
				img.at<cv::Vec3b>(x, y)[0] = 0;
				img.at<cv::Vec3b>(x, y)[1] = 0;
				img.at<cv::Vec3b>(x, y)[2] = 0;
			}
		}
	}
	//std::cout << count << std::endl;
}

void BilliardHandle::findField() {
	int topX = 260, bottomX = 0, rightY = 0, leftY = 480;

	for (int x = indentTop; x < 260 - indentBottom; ++x) {
		for (int y = 0; y < 480; ++y) {

			C.z = img.at<cv::Vec3b>(x, y)[0];
			C.y = img.at<cv::Vec3b>(x, y)[1];
			C.x = img.at<cv::Vec3b>(x, y)[2];

			BC = sqrt((B_Field.x - C.x)*(B_Field.x - C.x) + (B_Field.y - C.y)*(B_Field.y - C.y) + (B_Field.z - C.z)*(B_Field.z - C.z));
			AB = sqrt((B_Field.x - A.x)*(B_Field.x - A.x) + (B_Field.y - A.y)*(B_Field.y - A.y) + (B_Field.z - A.z)*(B_Field.z - A.z));
			AC = sqrt((A.x - C.x)*(A.x - C.x) + (A.y - C.y)*(A.y - C.y) + (A.z - C.z)*(A.z - C.z));

			angle_alpha = acos((AB * AB + AC * AC - BC * BC) / (2 * AB * AC));
			angle_alpha = angle_alpha / CV_PI * 180.0;

			if (abs(angle_alpha) < angle_beta) {
				if (x > bottomX)	bottomX = x;
				if (x < topX)		topX = x;
				if (y > rightY)		rightY = y;
				if (y < leftY)		leftY = y;
				imgField.at<cv::Vec3b>(x, y)[0] = 0;
				imgField.at<cv::Vec3b>(x, y)[1] = 255;
				imgField.at<cv::Vec3b>(x, y)[2] = 0;
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

void BilliardHandle::showCircles()
{
	//Коэффициенты
	const int       kMedianBlureApertureSize = 5;

	const double    kCannyFirstThreshold = 50.0f;
	const double    kCannySecondThreshold = 150.0f;

	const double    kRectSideMaxRation = 2.0f;
	const double    kMinRectArea = 20.0f;

	const int       kHoughCirclesParam1 = 100;
	const int       kHoughCirclesParam2 = 5;

	// Константы
	const int kColorWhite1 = 255;
	const int kColorBlack1 = 0;
	const cv::Scalar kColorBlue3 = cv::Scalar(255, 0, 0);
	const cv::Scalar kColorRed3 = cv::Scalar(0, 0, 255);

	cv::Mat originImage = img;
	if (originImage.empty())
	{
		std::cout << "Error: invalid Img " << std::endl;
	}

	// Большинству алгоритмов нужно чернобелое изображение, поэтому сразу делаем его серым.
	cv::Mat grayImage;
	cvtColor(originImage, grayImage, CV_BGR2GRAY);

	// 1. Размываем изображение, чтобы убрать большую часть мелких шумов.
	medianBlur(grayImage, grayImage, kMedianBlureApertureSize);

	// 2. Оставляем только контуры на изображении, чтобы найти на нем нужные фигуры.
	Canny(grayImage, grayImage, kCannyFirstThreshold, kCannySecondThreshold);

	//imshow("сanny", grayImage);

	// 3. Получаем список конутров
	std::vector< std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	findContours(grayImage, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	// 4. Среди контуров оставляем только те, которые по форме могут оказаться кружками (но необязательно)
	std::vector< std::vector<cv::Point> > possibleCircles;
	for (size_t i = 0; i < contours.size(); i++)
	{
		cv::RotatedRect minRect = cv::minAreaRect(cv::Mat(contours[i]));
		float height = minRect.size.height;
		float width = minRect.size.width;
		float area = minRect.size.area();

		// 4.1. Если соотношение сторон описывающего контур прямоугольника сильно отличается от kRectSideMaxRation,
		//      то, скорее всего, контур не является кружочком, в крайнем случае, это сильно вытянутый эллипс.
		// 4.2. Если площадь описывающего контур прямоугольника меньше kMinRectArea, то, скорее всего, это шум.
		if (cv::max(height, width) / cv::min(height, width) > kRectSideMaxRation || area < kMinRectArea)
			continue;

		// Конутр может оказаться кружком
		possibleCircles.push_back(contours[i]);
	}

	// 5. Проверяем оставшиеся контуры на наличие признаков кружков
	size_t circlesCount = 0;
	for (size_t i = 0; i< possibleCircles.size(); ++i)
	{
		cv::Mat drawing = cv::Mat::zeros(grayImage.size(), CV_8UC1);
		drawContours(drawing, possibleCircles, i, kColorWhite1);

		std::vector<cv::Vec3f> circles;
		int minRadius = 0, maxRadius = 0;
		HoughCircles(drawing, circles, CV_HOUGH_GRADIENT, 1, drawing.rows,
			kHoughCirclesParam1, kHoughCirclesParam2, minRadius, maxRadius);
		if (!circles.empty())
		{ // Найдены признаки круга. Будет считать, что контур является кружочком.
			circlesCount += 1;

			drawContours(originImage, possibleCircles, i, kColorBlue3, 2);
			for (size_t i = 0; i < circles.size(); ++i)
			{
				cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
				int radius = cvRound(circles[i][2]);
				//circle(originImage, center, 3, kColorRed3);
				//circle(originImage, center, radius, kColorRed3, 3);
			}
		}
	}

	std::cout << "Found " << circlesCount << " balls" << std::endl;

	cv::namedWindow("Result", CV_WINDOW_AUTOSIZE);
	imshow("Result", originImage);
}