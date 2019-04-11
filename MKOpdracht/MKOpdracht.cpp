#include <stdio.h>
#include <iostream>

//For reading files
#include <fstream>

//Simple data holder, +/- list
#include <vector>

//for kepeing circles
#include <map>
#include <set>

//For measuring time
#include <chrono>

#include <algorithm>

#ifdef _DEBUG
#include <random>
#endif

struct Point {
	double x;
	double y;

	void operator-=(const Point& p2)
	{
		x -= p2.x;
		y -= p2.y;
	}
};

struct Circle {
	int ID;
	Point center;
	double radius;
	float leftX;
	float rightX;
	bool bIsActive = false;
	std::set<int> intersectsWith;

	bool operator==(const Circle& c2)
	{
		return ID == c2.ID;
	}
};

struct coordinatesCircles {
	int leftX;
	int rightY;
};

struct InputInfo {
	int algorithm;
	int num_circles;
	std::vector<Circle> circles;
};

struct OutputInfo {
	std::vector<Point> intersection_points = std::vector<Point>();
	double executionTime;
};

struct IntersectingCircles {
	int ID_1;
	int ID_2;
};

InputInfo input_circles;

std::multimap<float, Circle*> circleEventPoints;
std::multimap<float, Circle*> activeCirclesTest;
std::vector<Circle*> activeCircleVector;
std::vector<Point> intersectionPoints;

std::chrono::milliseconds milliseconds;
std::chrono::nanoseconds nanoseconds;

//This is a function prototype, to be able to use it later.
InputInfo openAndReadFile(char*& path);
double distanceCalculate(Point p1, Point p2);
void calculateIntersectionPoints(Circle& c1, Circle& c2, std::vector<Point> &intersectionPoints);

void n_kwadraat_easy(InputInfo ii);
void n_kwadraat_sweepline(InputInfo ii);
//void n_log_n_sweepline(InputInfo ii);

int main(int argc, char **argv)
{
	//i=0 would be the executable itself, so we don't want it
	for (auto i = 1; i < argc; i++)
	{
		char* path = argv[i]; //char* c is in this case a /0 terminated string
		input_circles = openAndReadFile(path);
	}

	input_circles.algorithm = 2;

	switch (input_circles.algorithm)
	{
	case 1:
		n_kwadraat_easy(input_circles);
		break;
	case 2:
		n_kwadraat_sweepline(input_circles);
		break;
	case 3:
		//n_log_n_sweepline(input_circles);
		break;
	default:
		break;
	}


#ifdef _DEBUG
	//for (Point p : intersectionPoints)
	//{
	//	std::cout << "Intersection X, Y: " << p.x << "," << p.y << std::endl;
	//}

	std::cout << "ms: " << milliseconds.count() << std::endl;
	std::cout << "Found: " << intersectionPoints.size() << " intersections" << std::endl;

#else
	for (Point p : intersectionPoints)
	{
		std::cout << "Intersection X, Y: " << p.x << "," << p.y << std::endl;
	}

	std::cout << "ms: " << milliseconds.count() << std::endl;
	std::cout << "Found: " << intersectionPoints.size() << " intersections" << std::endl;
#endif // _DEBUG



	return 0;
}

InputInfo openAndReadFile(char*& path) {
	std::ifstream file;
	file.open(path);
	InputInfo inputInfo;

	if (file.is_open()) {
		int algorithm;
		file >> algorithm;

		int numCircles;
		file >> numCircles;

		std::cout << "Using algorithm " << algorithm << " on " << numCircles << " circles." << std::endl;

		inputInfo = { algorithm, numCircles, std::vector<Circle>(numCircles) };

		double x, y, r;
		for (auto i = 0; i < numCircles; i++)
		{
			file >> x >> y >> r;
			Point circleCenter = { x,y };
			Circle circleInfo = { i, circleCenter, r,x - r,x + r };
			inputInfo.circles[i] = circleInfo;
			circleEventPoints.insert(std::pair<float, Circle*>(circleInfo.leftX, &inputInfo.circles[i]));
			circleEventPoints.insert(std::pair<float, Circle*>(circleInfo.rightX, &inputInfo.circles[i]));
		}

#ifdef _DEBUG
		int extraCircles = 10000;
		int region = 100;
		int maxRadius = 5;
		inputInfo.circles.resize(extraCircles + inputInfo.num_circles);

		for (auto i = inputInfo.num_circles; i < extraCircles + inputInfo.num_circles; i++)
		{
			x = rand() % region;
			y = rand() % region;
			r = rand() % maxRadius + 1;
			Point circleCenter = { x,y };
			Circle circleInfo = { i, circleCenter, r,x - r,x + r };
			inputInfo.circles[i] = circleInfo;
			circleEventPoints.insert(std::pair<float, Circle*>(circleInfo.leftX, &inputInfo.circles[i]));
			circleEventPoints.insert(std::pair<float, Circle*>(circleInfo.rightX, &inputInfo.circles[i]));
		}
#endif

		std::cout << "Processed input:" << std::endl;
		//for (Circle circleInfo : inputInfo.circles)
		//{
		//	std::cout << "Center X: " << circleInfo.center.x << ", Center Y: " << circleInfo.center.y << ", Radius: " << circleInfo.radius << std::endl;
		//}

	}

	//for (auto it = circleEventPoints.begin();
	//	it != circleEventPoints.end(); ++it)
	//{
	//	std::cout << it->second.leftX << "    " << it->second.rightX << "  ID: " << it->second.ID << std::endl;
	//}
	//


	return inputInfo;
}

double distanceCalculate(Point p1, Point p2)
{
	double x = p1.x - p2.x;
	double y = p1.y - p2.y;
	double dist;

	dist = pow(x, 2) + pow(y, 2);
	dist = sqrt(dist);

	return dist;
}

void calculateIntersectionPoints(Circle& circle_1, Circle& circle_2, std::vector<Point>& intersectionPoints)
{
	if (circle_1.center.x == circle_2.center.x && circle_1.center.y == circle_2.center.y && circle_1.radius == circle_2.radius) {
		return;
	}

	double distance = distanceCalculate(circle_2.center, circle_1.center);

	if (distance > circle_1.radius + circle_2.radius) {
		return;
	}
	else if (distance < abs(circle_1.radius - circle_2.radius)) {
		return;
	}

	bool alreadyHasIntersection = false;
	for (int ID : circle_1.intersectsWith)
	{
		if (ID == circle_2.ID) {
			alreadyHasIntersection = true;
			break;
		}
	}

	if (alreadyHasIntersection) {
		return;
	}


	//PSSST http://paulbourke.net/geometry/circlesphere/
	double x1 = (pow(distance, 2) - pow(circle_2.radius, 2) + pow(circle_1.radius, 2)) / (2 * distance);
	double a = (1 / distance) * sqrt(
		(-distance + circle_2.radius - circle_1.radius)*
		(-distance - circle_2.radius + circle_1.radius)*
		(-distance + circle_2.radius + circle_1.radius)*
		(distance + circle_2.radius + circle_1.radius)
	);

	if (roundf(distance * 10000000) / 10000000 == roundf((circle_1.radius + circle_2.radius) * 10000000) / 10000000) {
		a = distance;

		Point intersection_middle = {
			circle_1.center.x + x1 * (circle_2.center.x - circle_1.center.x) / distance,	//X coordinate
			circle_1.center.y + x1 * (circle_2.center.y - circle_1.center.y) / distance	//Y coordinate
		};


		Point intersection_point1 = {
			intersection_middle.x + (a / 2) * (circle_2.center.y - circle_1.center.y) / distance,
			intersection_middle.y - (a / 2) * (circle_2.center.x - circle_1.center.x) / distance
		};

		circle_1.intersectsWith.insert(circle_2.ID);
		circle_2.intersectsWith.insert(circle_1.ID);
		intersectionPoints.push_back(intersection_point1);

		//std::cout << "Intersection X, Y: " << intersection_point1.x << "," << intersection_point1.y << std::endl;
		return;
	}

	Point intersection_middle = {
		circle_1.center.x + x1 * (circle_2.center.x - circle_1.center.x) / distance,	//X coordinate
		circle_1.center.y + x1 * (circle_2.center.y - circle_1.center.y) / distance	//Y coordinate
	};


	Point intersection_point1 = {
		intersection_middle.x + (a / 2) * (circle_2.center.y - circle_1.center.y) / distance,
		intersection_middle.y - (a / 2) * (circle_2.center.x - circle_1.center.x) / distance
	};
	Point intersection_point2 = {
		intersection_middle.x - (a / 2) * (circle_2.center.y - circle_1.center.y) / distance,
		intersection_middle.y + (a / 2) * (circle_2.center.x - circle_1.center.x) / distance
	};

	circle_1.intersectsWith.insert(circle_2.ID);
	circle_2.intersectsWith.insert(circle_1.ID);
	intersectionPoints.push_back(intersection_point1);
	intersectionPoints.push_back(intersection_point2);
	//std::cout << "Intersection X, Y: " << intersection_point1.x << "," << intersection_point1.y << std::endl;
	//std::cout << "Intersection X, Y: " << intersection_point2.x << "," << intersection_point2.y << std::endl;
}

void n_kwadraat_easy(InputInfo info)
{
	//info.circles.push_back({ 6,{0.2, 0.4},0.1 });
	auto start = std::chrono::system_clock::now();

	for (auto i = 0; i < info.circles.size(); i++)
	{
		for (auto j = 0; j < info.circles.size(); j++)
		{

			if (i == j) {
				continue;
			}
			else {
				calculateIntersectionPoints(info.circles[i], info.circles[j], intersectionPoints);
			}
		}
	}

	auto end = std::chrono::system_clock::now();
	milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
}

void n_kwadraat_sweepline(InputInfo info) {
	auto start = std::chrono::system_clock::now();

	for (auto it = circleEventPoints.begin(); it != circleEventPoints.end(); ++it)
	{
		//std::cout << it->second.leftX << "    " << it->second.rightX << "  ID: " << it->second.ID << std::endl;
		Circle* circle = it->second;
		if (!circle->bIsActive) {
			circle->bIsActive = true;


			for (Circle * activeCircle : activeCircleVector)
			{
				calculateIntersectionPoints(*circle, *activeCircle, intersectionPoints);
			}

			//for (std::pair<int, Circle*> activeCircle : activeCirclesTest)
			//{
			//	calculateIntersectionPoints(*circle, *(activeCircle.second), intersectionPoints);
			//}


			activeCircleVector.push_back(it->second);
			//activeCirclesTest.insert(std::pair<float, Circle*>(circle->rightX, circle));
		}
		else {
			//activeCirclesTest.erase(circle->rightX);
			activeCircleVector.erase(std::remove(activeCircleVector.begin(), activeCircleVector.end(), circle), activeCircleVector.end());
		}


		//activeCircleVector
	}

	auto end = std::chrono::system_clock::now();
	milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
}

class Iets {

};

class IetsAnders {

};
