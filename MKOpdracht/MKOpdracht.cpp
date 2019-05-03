#include <stdio.h>
#include <iostream>

//For reading files
#include <fstream>

//Simple data holder, +/- list
#include <vector>

//for kepeing circles
#include <map>
#include <set>
#include <unordered_set>
#include <queue>

//For measuring time
#include <chrono>

#include <algorithm>

#ifdef _DEBUG
#include <random>
#endif

const int precision = 100000;

struct Point {
	double x;
	double y;

	bool isEdgePointCircle = false;
	struct Circle *parent;
	bool isLeft = true;

	bool isRaakpunt = false;
	bool isSnijpunt = false;
	struct HalfCircle *hc1; //halfcirkels als snijpunt
	struct HalfCircle *hc2;

	void operator-=(const Point& p2)
	{
		x -= p2.x;
		y -= p2.y;
	}

	bool operator==(const Point& p2) const
	{
		return roundf(p2.x*precision) / precision == roundf(this->x*precision) / precision &&
			roundf(p2.y*precision) / precision == roundf(this->y*precision) / precision;
		//return std::fabs(p2.x - x) < 0.000001 && 
		//				std::fabs(p2.y - y) < 0.000001;
	}

	//bool operator<(const Point& p2) const
	//{
	//	return x < p2.x;
	//}

	//bool operator()(const Point& p1, const Point& p2) const {
	//	return std::tie(p1.x, p1.y) > std::tie(p2.x, p2.y);
	//}
};

struct testComp {
	bool operator()(const Point& p1, const Point& p2) {
		if (roundf(p1.x*precision)/precision == roundf(p2.x * precision) / precision) {
			//if (p1.isSnijpunt && p2.isSnijpunt) {
			//	return false;
			//}
			//if (p1.isSnijpunt && !p2.isSnijpunt) {
			//	return false;
			//}
			//if (!p1.isSnijpunt && p2.isSnijpunt) {
			//	//return true;
			//}
			return p1.y < p2.y;
		}
		return p1.x < p2.x;
		//return a.x == b.x ? a.y<b.y : a.x>b.x;
	}
};

// class for hash function 
class MyHashFunction {
public:
	size_t operator()(const Point& p	) const
	{
		//return (std::hash<double>()(p.x)) ^ (std::hash<double>()(p.y));
		//return (std::hash<double>()(p.x*10000000)) ^ (std::hash<double>()(p.y));
		return (std::hash<double>()(roundf(p.x*precision) / precision*100000  + roundf(p.y*precision) / precision));
	}
};

inline bool operator< (const Point& lhs, const Point& rhs) {
	return std::tie(lhs.x, lhs.y) < std::tie(rhs.x, rhs.y);
}


struct HalfCircle {
	bool bIsUpper = true;
	struct Circle *parent;
	std::multimap<double, HalfCircle*>::iterator *mapRef;
	int ID;
};

struct Circle {
	int ID;
	Point center;
	double radius;
	double leftX;
	double rightX;
	bool bIsActive = false;
	std::set<int> intersectsWith;
	struct HalfCircle* upperCircle;
	struct HalfCircle* lowerCircle;
	//std::multimap<double, HalfCircle*>::iterator *mapRefUpper;
	//std::multimap<double, HalfCircle*>::iterator *mapRefLower;

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

	std::multimap<double, Circle*> circleEventPoints;
	std::multimap<double, Circle*> activeCirclesTest;
	std::multimap<double, Point*> circleEventPointsLogN;
	//std::multimap<Point, std::vector<Point>, testComp> testQueue;
	std::multiset<Point, testComp> testQueue;

	std::multimap<double, HalfCircle*> halfCircles;

	std::vector<Circle*> activeCircleVector;
	std::unordered_set<Point, MyHashFunction> intersectionPoints;

	std::chrono::milliseconds milliseconds;
	std::chrono::nanoseconds nanoseconds;
};

//struct cmpPoints {
//	bool operator()(const Point& p1, const Point& p2) const {
//		return std::tie(p1.x, p1.y) < std::tie(p2.x, p2.y);
//	}
//};

struct OutputInfo {
	std::vector<Point> intersection_points = std::vector<Point>();
	double executionTime;
};

struct IntersectingCircles {
	int ID_1;
	int ID_2;
};

InputInfo input_circles;

#ifdef _DEBUG
InputInfo test_input_circles_1;
InputInfo test_input_circles_2;
InputInfo test_input_circles_3;
InputInfo test_input_circles_4;
#endif // _DEBUG




//This is a function prototype, to be able to use it later.
InputInfo openAndReadFile(char*& path);
double distanceCalculate(Point p1, Point p2);
void calculateIntersectionPoints(struct Circle& c1, struct Circle& c2, std::unordered_set<Point, MyHashFunction> &intersectionPoints, bool optimize = true);
void calculateIntersectionPoints(struct HalfCircle& c1, struct HalfCircle& c2, InputInfo &info, double currentX);

void n_kwadraat_easy(InputInfo& ii);
void n_kwadraat_sweepline(InputInfo& ii);
void n_log_n_sweepline(InputInfo& ii);

int main(int argc, char **argv)
{
	//i=0 would be the executable itself, so we don't want it
	for (auto i = 1; i < argc; i++)
	{
		char* path = argv[i]; //char* c is in this case a /0 terminated string
		input_circles = openAndReadFile(path);
	}

	input_circles.algorithm = 3;

	switch (input_circles.algorithm)
	{
	case 1:
		n_kwadraat_easy(input_circles);
		break;
	case 2:
		n_kwadraat_sweepline(input_circles);
		break;
	case 3:
		n_log_n_sweepline(input_circles);
		break;
	default:
		break;
	}


#ifdef _DEBUG
	for (Point p : input_circles.intersectionPoints)
	{
		std::cout << "Intersection X, Y: " << p.x << "," << p.y << std::endl;
	}
	std::cout << "Input file: " << std::endl;
	std::cout << "ms: " << input_circles.milliseconds.count() << std::endl;
	std::cout << "Found: " << input_circles.intersectionPoints.size() << " intersections" << std::endl << std::endl;

	n_kwadraat_easy(test_input_circles_1);
	std::cout << "Algorithm 1, Test circles: " << test_input_circles_1.num_circles << std::endl;
	std::cout << "ms: " << test_input_circles_1.milliseconds.count() << std::endl;
	std::cout << "Found: " << test_input_circles_1.intersectionPoints.size() << " intersections" << std::endl << std::endl;

	n_kwadraat_sweepline(test_input_circles_2);
	std::cout << "Algorithm 2, Test circles: " << test_input_circles_2.num_circles << std::endl;
	std::cout << "ms: " << test_input_circles_2.milliseconds.count() << std::endl;
	std::cout << "Found: " << test_input_circles_2.intersectionPoints.size() << " intersections" << std::endl << std::endl;

	n_log_n_sweepline(test_input_circles_3);
	std::cout << "Algorithm 3, Test circles: " << test_input_circles_3.num_circles << std::endl;
	std::cout << "ms: " << test_input_circles_3.milliseconds.count() << std::endl;
	std::cout << "Found: " << test_input_circles_3.intersectionPoints.size() << " intersections" << std::endl << std::endl;

	//n_log_n_sweepline(test_input_circles_4);
	//std::cout << "Algorithm 3, Test circles: " << test_input_circles_4.num_circles << std::endl;
	//std::cout << "ms: " << test_input_circles_4.milliseconds.count() << std::endl;
	//std::cout << "Found: " << test_input_circles_4.intersectionPoints.size() << " intersections" << std::endl << std::endl;

	int notFoundCount = 0;
	int raakpunten = 0;
	for (Point p : test_input_circles_1.intersectionPoints) {
		bool bFound = false;
		if (p.isRaakpunt) {
			raakpunten++;
		}

		for(Point p2 : test_input_circles_3.intersectionPoints)
		{
			if (roundf(p.x * precision)/ precision == roundf(p2.x * precision) / precision
				&& roundf(p.y * precision) / precision == roundf( p2.y * precision) / precision) {
				bFound = true;
				break;
			}
		}
		if (!bFound) {
			notFoundCount++;
		}
	}

	double x1 =0,x2=0,x3 = 0;
	double y1 = 0,y2 = 0,y3 = 0;
	for (Point p1 : test_input_circles_1.intersectionPoints)
	{
		x1 += pow(p1.x,2);
		y1 += pow(p1.y,2);
	}

	for (Point p2 : test_input_circles_2.intersectionPoints)
	{
		x2 += pow(p2.x,2);
		y2 += pow(p2.y,2);
	}

	double percentage = (sqrt(x1) / sqrt(x2)) * 100;
	percentage += (sqrt(y1) / sqrt(y2)) * 100;
	percentage /= 2;

	std::cout << percentage << "% the coordinates match" << std::endl;

#else
	for (Point p : input_circles.intersectionPoints)
	{
		std::cout << "Intersection X, Y: " << p.x << "," << p.y << std::endl;
	}

	std::cout << "ms: " << input_circles.milliseconds.count() << std::endl;
	std::cout << "Found: " << input_circles.intersectionPoints.size() << " intersections" << std::endl;
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
			//Circle circleInfo = (Circle{ i, circleCenter, r,x - r,x + r });
			//circleInfo.lowerCircle = new HalfCircle{ false, &circleInfo };
			//circleInfo.upperCircle = new HalfCircle{ true ,&circleInfo };
			inputInfo.circles[i] = (Circle{ i, circleCenter, roundf(r * precision) / precision,roundf((x - r) * precision) / precision,roundf((x + r) * precision) / precision });
			inputInfo.circles[i].lowerCircle = new HalfCircle{ false, &inputInfo.circles[i] };
			inputInfo.circles[i].lowerCircle->ID = inputInfo.circles[i].ID;
			inputInfo.circles[i].upperCircle = new HalfCircle{ true, &inputInfo.circles[i] };
			inputInfo.circles[i].upperCircle->ID = inputInfo.circles[i].ID;
			inputInfo.circleEventPoints.insert(std::pair<double, Circle*>(inputInfo.circles[i].leftX, &inputInfo.circles[i]));
			inputInfo.circleEventPoints.insert(std::pair<double, Circle*>(inputInfo.circles[i].rightX, &inputInfo.circles[i]));

			Point* eventPointLeft = new Point;
			eventPointLeft->isEdgePointCircle = true;
			eventPointLeft->parent = &inputInfo.circles[i];
			eventPointLeft->y = roundf(y * precision) / precision;
			eventPointLeft->x = roundf((x-r) * precision) / precision;

			Point* eventPointRight = new Point;
			eventPointRight->isEdgePointCircle = true;
			eventPointRight->parent = &inputInfo.circles[i];
			eventPointRight->y = roundf(y * precision) / precision;
			eventPointRight->x = roundf((x + r) * precision) / precision;
			eventPointRight->isLeft = false;
			inputInfo.circleEventPointsLogN.insert(std::pair<double, Point*>(eventPointLeft->x, eventPointLeft));
			inputInfo.circleEventPointsLogN.insert(std::pair<double, Point*>(eventPointRight->x, eventPointRight));
			inputInfo.testQueue.emplace(*eventPointLeft);
			inputInfo.testQueue.emplace(*eventPointRight);
		}

#ifdef _DEBUG
		//int extraCircles = 500;
		//int region = 250;
		//int maxRadius = 20;

		int extraCircles = 17;
		int region = 10;
		int maxRadius = 20;

		//extraCircles = 100;
		//region = 1;
		//maxRadius = 1;

		test_input_circles_1 = { 1, extraCircles, std::vector<Circle>(extraCircles) };
		test_input_circles_2 = { 2, extraCircles, std::vector<Circle>(extraCircles) };
		test_input_circles_3 = { 3, extraCircles, std::vector<Circle>(extraCircles) };

		for (auto i = 0; i < extraCircles; i++)
		{
			x = rand() % region;
			y = rand() % region;
			r = rand() % maxRadius / 5.0 + 0.5;
			Point circleCenter = { x,y };
			Circle circleInfo = { i, circleCenter, r,x - r,x + r };
			//test_input_circles_1.circles[i] = { circleInfo };
			//test_input_circles_1.circleEventPoints.insert(std::pair<double, Circle*>(circleInfo.leftX, &test_input_circles_1.circles[i]));
			//test_input_circles_1.circleEventPoints.insert(std::pair<double, Circle*>(circleInfo.rightX, &test_input_circles_1.circles[i]));
			//
			test_input_circles_1.circles[i] = { i, circleCenter, r,x - r,x + r };
			test_input_circles_1.circles[i].lowerCircle = new HalfCircle{ false, &test_input_circles_1.circles[i] };
			test_input_circles_1.circles[i].upperCircle = new HalfCircle{ true, &test_input_circles_1.circles[i] };
			test_input_circles_1.circleEventPoints.insert(std::pair<double, Circle*>(test_input_circles_1.circles[i].leftX, &test_input_circles_1.circles[i]));
			test_input_circles_1.circleEventPoints.insert(std::pair<double, Circle*>(test_input_circles_1.circles[i].rightX, &test_input_circles_1.circles[i]));

			//test_input_circles_2.circles[i] = { circleInfo };
			//test_input_circles_2.circleEventPoints.insert(std::pair<double, Circle*>(circleInfo.leftX, &test_input_circles_2.circles[i]));
			//test_input_circles_2.circleEventPoints.insert(std::pair<double, Circle*>(circleInfo.rightX, &test_input_circles_2.circles[i]));
			//
			test_input_circles_2.circles[i] = { i, circleCenter, r,x - r,x + r };
			test_input_circles_2.circles[i].lowerCircle = new HalfCircle{ false, &test_input_circles_2.circles[i] };
			test_input_circles_2.circles[i].upperCircle = new HalfCircle{ true, &test_input_circles_2.circles[i] };
			test_input_circles_2.circleEventPoints.insert(std::pair<double, Circle*>(test_input_circles_2.circles[i].leftX, &test_input_circles_2.circles[i]));
			test_input_circles_2.circleEventPoints.insert(std::pair<double, Circle*>(test_input_circles_2.circles[i].rightX, &test_input_circles_2.circles[i]));

			//test_input_circles_3.circles[i] = { circleInfo };
			//test_input_circles_3.circleEventPoints.insert(std::pair<double, Circle*>(circleInfo.leftX, &test_input_circles_3.circles[i]));
			//test_input_circles_3.circleEventPoints.insert(std::pair<double, Circle*>(circleInfo.rightX, &test_input_circles_3.circles[i]));
			//
			test_input_circles_3.circles[i] = { i, circleCenter, r,x - r,x + r };
			test_input_circles_3.circles[i].lowerCircle = new HalfCircle{ false, &test_input_circles_3.circles[i] };
			test_input_circles_3.circles[i].lowerCircle->ID = test_input_circles_3.circles[i].ID;
			test_input_circles_3.circles[i].upperCircle = new HalfCircle{ true, &test_input_circles_3.circles[i] };
			test_input_circles_3.circles[i].upperCircle->ID = test_input_circles_3.circles[i].ID;
			test_input_circles_3.circleEventPoints.insert(std::pair<double, Circle*>(test_input_circles_3.circles[i].leftX, &test_input_circles_3.circles[i]));
			test_input_circles_3.circleEventPoints.insert(std::pair<double, Circle*>(test_input_circles_3.circles[i].rightX, &test_input_circles_3.circles[i]));

			Point* eventPointLeft = new Point;
			eventPointLeft->isEdgePointCircle = true;
			eventPointLeft->parent = &test_input_circles_3.circles[i];
			eventPointLeft->y = y;
			eventPointLeft->x = x - r;

			Point* eventPointRight = new Point;
			eventPointRight->isEdgePointCircle = true;
			eventPointRight->parent = &test_input_circles_3.circles[i];
			eventPointRight->y = y;
			eventPointRight->x = x + r;
			eventPointRight->isLeft = false;
			test_input_circles_3.circleEventPointsLogN.insert(std::pair<double, Point*>(eventPointLeft->x, eventPointLeft));
			test_input_circles_3.circleEventPointsLogN.insert(std::pair<double, Point*>(eventPointRight->x, eventPointRight));
			test_input_circles_3.testQueue.emplace(*eventPointLeft);
			test_input_circles_3.testQueue.emplace(*eventPointRight);
		}

		test_input_circles_4 = { 3, 3, std::vector<Circle>(3) };
		//int i = 0;
		//r = 0.5;
		//x = 0;
		//y = 0;
		int i = 0;
		r = 0.5;
		x = 0.0;
		y = 0;
		test_input_circles_4.circles[i] = { i, {x,y}, r,x - r,x + r };
		test_input_circles_4.circles[i].lowerCircle = new HalfCircle{ false, &test_input_circles_4.circles[i] };
		test_input_circles_4.circles[i].upperCircle = new HalfCircle{ true, &test_input_circles_4.circles[i] };
		test_input_circles_4.circleEventPoints.insert(std::pair<double, Circle*>(test_input_circles_4.circles[i].leftX, &test_input_circles_4.circles[i]));
		test_input_circles_4.circleEventPoints.insert(std::pair<double, Circle*>(test_input_circles_4.circles[i].rightX, &test_input_circles_4.circles[i]));

		Point* eventPointLeft = new Point;
		eventPointLeft->isEdgePointCircle = true;
		eventPointLeft->parent = &test_input_circles_4.circles[i];
		eventPointLeft->y = y;
		eventPointLeft->x = x - r;

		Point* eventPointRight = new Point;
		eventPointRight->isEdgePointCircle = true;
		eventPointRight->parent = &test_input_circles_4.circles[i];
		eventPointRight->y = y;
		eventPointRight->x = x + r;
		eventPointRight->isLeft = false;
		test_input_circles_4.circleEventPointsLogN.insert(std::pair<double, Point*>(eventPointLeft->x, eventPointLeft));
		test_input_circles_4.circleEventPointsLogN.insert(std::pair<double, Point*>(eventPointRight->x, eventPointRight));
		test_input_circles_4.testQueue.emplace(*eventPointLeft);
		test_input_circles_4.testQueue.emplace(*eventPointRight);

		//i = 1;
		//r = 0.5;
		//x = 0.8;
		//y = 0;
		i = 1;
		r = 0.5;
		x = 0.8;
		y = 0;
		test_input_circles_4.circles[i] = { i, {x,y}, r,x - r,x + r };
		test_input_circles_4.circles[i].lowerCircle = new HalfCircle{ false, &test_input_circles_4.circles[i] };
		test_input_circles_4.circles[i].upperCircle = new HalfCircle{ true, &test_input_circles_4.circles[i] };
		test_input_circles_4.circleEventPoints.insert(std::pair<double, Circle*>(test_input_circles_4.circles[i].leftX, &test_input_circles_4.circles[i]));
		test_input_circles_4.circleEventPoints.insert(std::pair<double, Circle*>(test_input_circles_4.circles[i].rightX, &test_input_circles_4.circles[i]));

		eventPointLeft = new Point;
		eventPointLeft->isEdgePointCircle = true;
		eventPointLeft->parent = &test_input_circles_4.circles[i];
		eventPointLeft->y = y;
		eventPointLeft->x = x - r;

		eventPointRight = new Point;
		eventPointRight->isEdgePointCircle = true;
		eventPointRight->parent = &test_input_circles_4.circles[i];
		eventPointRight->y = y;
		eventPointRight->x = x + r;
		eventPointRight->isLeft = false;
		test_input_circles_4.circleEventPointsLogN.insert(std::pair<double, Point*>(eventPointLeft->x, eventPointLeft));
		test_input_circles_4.circleEventPointsLogN.insert(std::pair<double, Point*>(eventPointRight->x, eventPointRight));
		test_input_circles_4.testQueue.emplace(*eventPointLeft);
		test_input_circles_4.testQueue.emplace(*eventPointRight);

		//i = 2;
		//r = 0.5;
		//x = 0.4;
		//y = 0;
		i = 2;
		r = 0.5;
		x = 1.6;
		y = 0;
		test_input_circles_4.circles[i] = { i, {x,y}, r,x - r,x + r };
		test_input_circles_4.circles[i].lowerCircle = new HalfCircle{ false, &test_input_circles_4.circles[i] };
		test_input_circles_4.circles[i].upperCircle = new HalfCircle{ true, &test_input_circles_4.circles[i] };
		test_input_circles_4.circleEventPoints.insert(std::pair<double, Circle*>(test_input_circles_4.circles[i].leftX, &test_input_circles_4.circles[i]));
		test_input_circles_4.circleEventPoints.insert(std::pair<double, Circle*>(test_input_circles_4.circles[i].rightX, &test_input_circles_4.circles[i]));

		eventPointLeft = new Point;
		eventPointLeft->isEdgePointCircle = true;
		eventPointLeft->parent = &test_input_circles_4.circles[i];
		eventPointLeft->y = y;
		eventPointLeft->x = x - r;

		eventPointRight = new Point;
		eventPointRight->isEdgePointCircle = true;
		eventPointRight->parent = &test_input_circles_4.circles[i];
		eventPointRight->y = y;
		eventPointRight->x = x + r;
		eventPointRight->isLeft = false;
		test_input_circles_4.circleEventPointsLogN.insert(std::pair<double, Point*>(eventPointLeft->x, eventPointLeft));
		test_input_circles_4.circleEventPointsLogN.insert(std::pair<double, Point*>(eventPointRight->x, eventPointRight));
		test_input_circles_4.testQueue.emplace(*eventPointLeft);
		test_input_circles_4.testQueue.emplace(*eventPointRight);

	/*	i = 3;
		r = 1.0;
		x = 0.8;
		y = 0.5;
		test_input_circles_4.circles[i] = { i, {x,y}, r,x - r,x + r };
		test_input_circles_4.circles[i].lowerCircle = new HalfCircle{ false, &test_input_circles_4.circles[i] };
		test_input_circles_4.circles[i].upperCircle = new HalfCircle{ true, &test_input_circles_4.circles[i] };
		test_input_circles_4.circleEventPoints.insert(std::pair<double, Circle*>(test_input_circles_4.circles[i].leftX, &test_input_circles_4.circles[i]));
		test_input_circles_4.circleEventPoints.insert(std::pair<double, Circle*>(test_input_circles_4.circles[i].rightX, &test_input_circles_4.circles[i]));

		eventPointLeft = new Point;
		eventPointLeft->isEdgePointCircle = true;
		eventPointLeft->parent = &test_input_circles_4.circles[i];
		eventPointLeft->y = y;
		eventPointLeft->x = x - r;

		eventPointRight = new Point;
		eventPointRight->isEdgePointCircle = true;
		eventPointRight->parent = &test_input_circles_4.circles[i];
		eventPointRight->y = y;
		eventPointRight->x = x + r;
		eventPointRight->isLeft = false;
		test_input_circles_4.circleEventPointsLogN.insert(std::pair<double, Point*>(eventPointLeft->x, eventPointLeft));
		test_input_circles_4.circleEventPointsLogN.insert(std::pair<double, Point*>(eventPointRight->x, eventPointRight));*/

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

void calculateIntersectionPoints(struct Circle& circle_1, struct Circle& circle_2, std::unordered_set<Point, MyHashFunction>& intersectionPoints, bool optimize)
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

	if (optimize) {
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
	}

	//PSSST http://paulbourke.net/geometry/circlesphere/
	double x1 = (pow(distance, 2) - pow(circle_2.radius, 2) + pow(circle_1.radius, 2)) / (2 * distance);
	double a = (1 / distance) * sqrt(
		(-distance + circle_2.radius - circle_1.radius)*
		(-distance - circle_2.radius + circle_1.radius)*
		(-distance + circle_2.radius + circle_1.radius)*
		(distance + circle_2.radius + circle_1.radius)
	);

	if (roundf(distance * precision) / precision == roundf((circle_1.radius + circle_2.radius) * precision) / precision) {
		a = distance;

		Point intersection_middle = {
			circle_1.center.x + x1 * (circle_2.center.x - circle_1.center.x) / distance,	//X coordinate
			circle_1.center.y + x1 * (circle_2.center.y - circle_1.center.y) / distance	//Y coordinate
		};


		Point intersection_point1 = {
			/*intersection_middle.x + (a / 2) * (circle_2.center.y - circle_1.center.y) / distance,
			intersection_middle.y - (a / 2) * (circle_2.center.x - circle_1.center.x) / distance*/
			intersection_middle
		};

		circle_1.intersectsWith.insert(circle_2.ID);
		circle_2.intersectsWith.insert(circle_1.ID);

		intersection_point1.isRaakpunt = true;
		intersection_point1.isSnijpunt = true;
		intersectionPoints.insert(intersection_point1);
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
	intersection_point1.isSnijpunt = true;
	intersection_point2.isSnijpunt = true;
	if (circle_1.ID == 944 && circle_2.ID == 506 || circle_1.ID == 506 && circle_2.ID == 944) {
		int YOLO = -1;
	}

	if (intersection_point1.x >= circle_1.rightX) {
		intersection_point1.x = circle_1.rightX - 1.0f/precision;
	}
	if (intersection_point1.x >= circle_2.rightX) {
		intersection_point1.x = circle_2.rightX - 1.0f / precision;
	}
	if (intersection_point2.x >= circle_1.rightX) {
		intersection_point2.x = circle_1.rightX - 1.0f / precision;
	}
	if (intersection_point2.x >= circle_2.rightX) {
		intersection_point2.x = circle_2.rightX - 1.0f / precision;
	}

	bool dontAdd2 = false;
	//if (isnan(intersection_point1.x) || isnan(intersection_point1.x)) {
	if ((isnan(intersection_point1.x) || isnan(intersection_point1.x)) || 
		(roundf(intersection_point1.y * precision)/precision == roundf(circle_1.center.y*precision)/precision &&
		(roundf(intersection_point1.y * precision) / precision == roundf(circle_2.center.y*precision) / precision))) {
		if (roundf(circle_1.leftX * precision) / precision == roundf(circle_2.leftX * precision) / precision) {
			intersection_point1.x = circle_1.leftX - 1.0f / precision;
		}
		if (roundf(circle_1.leftX * precision) / precision == roundf(circle_2.rightX * precision) / precision) {
			intersection_point1.x = circle_1.leftX - 1.0f / precision;
		}
		if (roundf(circle_1.rightX * precision) / precision == roundf(circle_2.leftX * precision) / precision) {
			intersection_point1.x = circle_1.rightX - 1.0f / precision;
		}
		if (roundf(circle_1.rightX * precision) / precision == roundf(circle_2.rightX * precision) / precision) {
			intersection_point1.x = circle_1.rightX - 1.0f / precision;
		}
		if (roundf(circle_1.rightX * precision) / precision == roundf(circle_2.rightX * precision) / precision &&
			roundf(circle_1.leftX * precision) / precision == roundf(circle_2.leftX * precision) / precision) {
			int YOLO = -1;
		}
		if (circle_1.center.y != circle_2.center.y) {
			int YOLO = -1;
		}
		intersection_point1.y = circle_1.center.y - 1.0f / precision;
		intersection_point1.isRaakpunt = true;
		dontAdd2 = true;
	}

	circle_1.intersectsWith.insert(circle_2.ID);
	circle_2.intersectsWith.insert(circle_1.ID);
	intersectionPoints.insert(intersection_point1);
	if (!dontAdd2) {
		intersectionPoints.insert(intersection_point2);
	}
	//std::cout << "Intersection X, Y: " << intersection_point1.x << "," << intersection_point1.y << std::endl;
	//std::cout << "Intersection X, Y: " << intersection_point2.x << "," << intersection_point2.y << std::endl;
}

void calculateIntersectionPoints(struct HalfCircle& c1, struct HalfCircle& c2, InputInfo &info, double currentX) {
	std::unordered_set<Point, MyHashFunction> temp;
	calculateIntersectionPoints(*c1.parent, *c2.parent, temp,false);
	std::vector<Point> keepPoints;
	if (temp.size() == 0) { return; }
	if (c1.ID == 19 && c2.ID == 5) {
		int YOLO = -1;
	}
	for(Point p : temp)
	{
		if (c1.bIsUpper) {
			if (p.y < c1.parent->center.y) {
				continue;
			}
		}else if (!c1.bIsUpper) {
			if (p.y > c1.parent->center.y) {
				continue;
			}
		}

		if (c2.bIsUpper) {
			if (p.y < c2.parent->center.y) {
				continue;
			}
		}
		else if (!c2.bIsUpper) {
			if (p.y > c2.parent->center.y) {
				continue;
			}
		}
		//p is een correct snijpunt voor de halve cirkels
		keepPoints.push_back((p));
		//info.intersectionPoints.insert(p);
	}

	for (auto i = 0; i < keepPoints.size(); i++)
	{
		if (roundf(keepPoints[i].x * precision)/precision < roundf(currentX * precision) / precision) {
			//info.intersectionPoints.insert(keepPoints[i]);
			continue;
		}

		if (info.intersectionPoints.find(keepPoints[i]) == info.intersectionPoints.end()) {
			keepPoints[i].hc1 = &c1;
			keepPoints[i].hc2 = &c2;
			keepPoints[i].isEdgePointCircle = false;
			auto poc = new Point(keepPoints[i]);
			//info.circleEventPointsLogN.insert(std::pair<double, Point*>(keepPoints[i].x,poc));
			info.testQueue.emplace(*poc);
			info.intersectionPoints.insert(*poc);
		}


	}
}

void n_kwadraat_easy(InputInfo& info)
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
				calculateIntersectionPoints(info.circles[i], info.circles[j], info.intersectionPoints);
			}
		}
	}

	auto end = std::chrono::system_clock::now();
	info.milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	info.nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
}

void n_kwadraat_sweepline(InputInfo& info) {
	auto start = std::chrono::system_clock::now();

	for (auto it = info.circleEventPoints.begin(); it != info.circleEventPoints.end(); ++it)
	{
		//std::cout << it->second.leftX << "    " << it->second.rightX << "  ID: " << it->second.ID << std::endl;
		Circle* circle = it->second;
		if (!circle->bIsActive) {
			circle->bIsActive = true;


			for (Circle * activeCircle : info.activeCircleVector)
			{
				calculateIntersectionPoints(*circle, *activeCircle, info.intersectionPoints);
			}

			//for (std::pair<int, Circle*> activeCircle : activeCirclesTest)
			//{
			//	calculateIntersectionPoints(*circle, *(activeCircle.second), intersectionPoints);
			//}


			info.activeCircleVector.push_back(it->second);
			//activeCirclesTest.insert(std::pair<double, Circle*>(circle->rightX, circle));
		}
		else {
			//activeCirclesTest.erase(circle->rightX);
			info.activeCircleVector.erase(std::remove(info.activeCircleVector.begin(), info.activeCircleVector.end(), circle), info.activeCircleVector.end());
		}


		//activeCircleVector
	}

	auto end = std::chrono::system_clock::now();
	info.milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	info.nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
}

bool isIn(Point &p, Circle &c) {
	if (distanceCalculate(p,c.center) <= c.radius) {
		return true;
	}
	return false;
}

bool isIn(Point &p, HalfCircle &c) {
	if (c.bIsUpper) {
		if (p.y < c.parent->center.y) {
			return false;
		}
	}else 	if (!c.bIsUpper) {
		if (p.y >= c.parent->center.y) {
			return false;
		}
	}
	if (distanceCalculate(p, c.parent->center) <= c.parent->radius) {
		return true;
	}
	return false;
}

void n_log_n_sweepline(InputInfo& info) {
	std::multimap<double, HalfCircle*, std::greater<double>> T;
	auto start = std::chrono::system_clock::now();
	while (!info.testQueue.empty())
	{
		auto p = info.testQueue.begin();
		if (p->parent != nullptr && p->parent->ID == 17) {
			int YOLO = -1;
		}
		while (!info.testQueue.empty() && p->parent != nullptr && p->parent->ID == 10) {
			info.testQueue.erase(info.testQueue.begin());
			p = info.testQueue.begin();
		}
		if (p == info.testQueue.end()) {
			break;
		}
		if (p->isSnijpunt) {
			if (!p->isRaakpunt) {
				if (p->hc1->ID == 7 && p->hc2->ID == 14) {
					int YOLO = -1;
				}
				auto p1 = p->hc1->mapRef;
				auto p2 = p->hc2->mapRef;
				auto nextp1 = std::prev(*p1, 1);
				auto prevp2 = std::next(*p2, 1);
				if (p->hc1->ID == 17 && p->hc2->ID == 3) {
					int YOLO = -1;
				}
				
				//if (nextp1 != *p2) {
				//	std::swap(p1,p2);
				//}
				if (nextp1!=T.end() && prevp2!=T.end() && nextp1->second->parent->ID == (*p2)->second->ID && prevp2->second->parent->ID == (*p1)->second->ID) {
					std::swap(p1, p2);
					nextp1 = std::prev(*p1, 1);
					prevp2 = std::next(*p2, 1);
				}

				if (nextp1!=T.end()) {
					auto testKey = nextp1;
					bool bFound = false;
					while (testKey != T.end() && testKey->first <= p->y) {
						bFound = true;
						testKey = std::prev(testKey);
					}
					if (bFound && testKey!=*p2) {
						if (testKey!=T.end()) {
							testKey = std::next(testKey);
						}
						else {
							testKey = T.begin();
						}
						while (testKey!=*p1 && testKey!=*p2) {
							auto temp = T.insert(std::pair<double, HalfCircle*>(roundf(p->y*precision) / precision, (testKey)->second));
							temp->second->mapRef = new auto (temp);
							auto deleteKey = testKey;
							testKey = std::next(testKey);
							T.erase(deleteKey);
						}
					}
				}
				auto testKey = prevp2;
				bool bFound = false;
				if (prevp2 != T.end()) {
					while (testKey != T.end() && testKey->first >= p->y) {
						bFound = true;
						testKey = std::next(testKey);
					}
				}
				//if (bFound) {
				//	testKey = std::prev(testKey);
				//	while (testKey != *p2) {
				//		auto temp = T.insert(std::pair<double, HalfCircle*>(roundf(p->y*precision) / precision, (testKey)->second));
				//		temp->second->mapRef = new auto (temp);
				//		auto deleteKey = testKey;
				//		testKey = std::prev(testKey);
				//		T.erase(deleteKey);
				//	}
				//}

				//auto poc2 = T.insert(std::pair<double, HalfCircle*>(p->y, p->hc2));
				//auto poc1 = T.insert(std::pair<double, HalfCircle*>(p->y, p->hc1));
				auto poc1 = T.insert(std::pair<double, HalfCircle*>(roundf(p->y*precision) / precision, (*p2)->second));
				auto poc2 = T.insert(std::pair<double, HalfCircle*>(roundf(p->y*precision)/precision, (*p1)->second));

				poc2->second->mapRef = new std::multimap<double, HalfCircle*>::iterator(poc2);
				poc1->second->mapRef = new std::multimap<double, HalfCircle*>::iterator(poc1);
				
				if (bFound) {
					testKey = std::prev(testKey);
					while (testKey != *p2 && testKey != *p1) {
						if (testKey == poc1) {
							testKey = std::prev(testKey);
							continue;
						}
						if (testKey == poc2) {
							testKey = std::prev(testKey);
							continue;
						}
						auto tempFix = std::prev(testKey);
						if (tempFix != *p2) {
							while (tempFix != *p2) {
								tempFix = std::prev(tempFix);
							}
							tempFix = std::next(tempFix);
							auto temp = T.insert(std::pair<double, HalfCircle*>(roundf(p->y*precision) / precision, (tempFix)->second));
							temp->second->mapRef = new auto (temp);
							auto deleteKey = tempFix;
							//testKey = std::prev(tempFix);
							T.erase(deleteKey);
							continue;
						}

						auto temp = T.insert(std::pair<double, HalfCircle*>(roundf(p->y*precision) / precision, (testKey)->second));
						temp->second->mapRef = new auto (temp);
						auto deleteKey = testKey;
						testKey = std::prev(testKey);
						T.erase(deleteKey);
					}
				}

				T.erase(*p1);
				T.erase(*p2);

				auto test = std::prev(poc1);
				if (test != T.end() && test->second->parent->ID == poc1->second->parent->ID) {
					if (!test->second->bIsUpper) {
						auto bu = poc1->second;
						poc1->second = test->second;
						poc1->second->mapRef = new auto(poc1);
						test->second = bu;
						test->second->mapRef = new auto(test);
						poc1 = test;
						test = std::prev(test);
					}
				}
				test = std::prev(poc2);
				while (test != T.end() && test != poc1) {
					auto bu = poc2->second;
					poc2->second = test->second;
					poc2->second->mapRef = new auto(poc2);
					test->second = bu;
					test->second->mapRef = new auto(test);
					poc2 = test;
					test = std::prev(test);
				}



				auto p1Upper = poc1 != T.end() ? std::prev(poc1, 1) : T.end();
				auto p2Lower = poc2 != T.end() ? std::next(poc2, 1) : T.end();

				if (p1Upper != T.end()) {
					calculateIntersectionPoints(*p1Upper->second, *poc1->second, info, p->x);
				}
				if (p2Lower != T.end()) {
					calculateIntersectionPoints(*poc2->second, *p2Lower->second, info, p->x);
				}
			}
		}
		if (p->isEdgePointCircle && p->isLeft) { //Is left?
			if (T.size() == 0) {
				auto p1 = T.insert(std::pair<double, HalfCircle*>(roundf((p->parent->center.y) * precision) / precision, p->parent->upperCircle));
				p1->second->mapRef = new std::multimap<double, HalfCircle*>::iterator(p1);

				auto p2 = T.insert(std::pair<double, HalfCircle*>(roundf( (p->parent->center.y ) * precision) / precision, p->parent->lowerCircle));
				p2->second->mapRef = new std::multimap<double, HalfCircle*>::iterator(p2);
			}
			else {
				if (p->parent->ID == 6 || p->parent->ID == 10) {
					int YOLO = -1;
				}
				double yKeyHigh = p->parent->center.y;
				double yKeyLow = p->parent->center.y;
				Point testPoint = { p->x,p->y };
				auto testFrom = T.upper_bound(roundf(yKeyHigh * precision) / precision); //Maybe -1
				if (testFrom == T.end()) {
					testFrom = std::prev(testFrom);
				}
				auto testTo = T.upper_bound(roundf(yKeyLow * precision)/precision);
				if (testFrom->second->bIsUpper) {
					while (testFrom->second->bIsUpper) {
						if (isIn(testPoint, *testFrom->second->parent)) {
							testFrom = std::next(testFrom);
						}
						else {
							break;
						}
					}
					if ( testFrom != T.end()) {
						testFrom = std::prev(testFrom);
					}
				}
				else if (isIn(testPoint,*testFrom->second->parent)) {
					//testPoint.y = roundf((testFrom->second->parent->center.y + 1.0f/precision) * precision)/precision;
					//testFrom = T.end();
					testFrom = std::prev(testFrom);
				}
				if (testFrom != T.end() && !isIn(testPoint, *testFrom->second)) {
					auto temp = std::next(testFrom);
					while (temp != T.end() && roundf(temp->first * precision) / precision == roundf(testFrom->first * precision) / precision) {
						testFrom = temp;
						if (isIn(testPoint, *testFrom->second)) {
							break;
						}
						temp = std::next(testFrom);
					}
					testFrom = std::prev(testFrom);
				}
				//while (testFrom!= T.end() && testFrom->first > p->y) {
				//	auto temp = std::next(testFrom);
				//	if () {

				//	}
				//}

				//auto p1 = T.insert(std::pair<double, HalfCircle*>(testFrom != T.end() ? (isIn(testPoint,*testFrom->second->parent) ? roundf(testFrom->first * precision) / precision : roundf(testPoint.y * precision) / precision) : roundf(testPoint.y*precision)/precision, p->parent->upperCircle));
				//auto p1 = T.insert(std::pair<double, HalfCircle*>(testFrom != T.end() ? (isIn(testPoint, *testFrom->second->parent) ? roundf((testPoint.y + p->parent->radius)*precision)/precision : roundf((testPoint.y + p->parent->radius) * precision) / precision) : roundf((testPoint.y+p->parent->radius)*precision) / precision, p->parent->upperCircle));

				//05-02
				//if (p->parent->ID == 4) {
				//	int YOLO = -1;
				//}
				//bool bFoundIn = false;
				//auto start = T.begin();
				//while (start != T.end()) {
				//	if (isIn(testPoint, *start->second)) {
				//		bFoundIn = true;
				//		break;
				//	}
				//	start = std::next(start);
				//}
				//if (bFoundIn) {
				//	while(start != T.end() && isIn(testPoint, *start->second)) {
				//		start = std::next(start);
				//	}
				//	start = std::prev(start, 2);
				//}
				//if (!bFoundIn && start == T.end()) {
				//	start = std::prev(start);
				//}

				auto start = T.begin();
				bool bFoundFirst = false;
				while (start!=T.end() && (!isIn(testPoint,*start->second) || start->second->bIsUpper)) {
					if (isIn(testPoint, *start->second)) {
						if (p->parent->ID == 7) {
							int YOLO = -1;
						}
						bFoundFirst = true;
					}
					else if (bFoundFirst) {
						auto goDeeper = start;
						auto deepest = start;
						while (goDeeper != T.end()) {
							if (goDeeper->second->bIsUpper && isIn(testPoint,*goDeeper->second)) {
								deepest = goDeeper;
							}
							goDeeper = std::next(goDeeper);
						}
						if (deepest!=start) {
							int YOLO = -1;
							//start = deepest;
						}
						auto test2 = start;
						while (test2 != T.end() && test2->first > p->y) {
							test2 = std::next(test2);
						}
						test2 = std::prev(test2);
						//start = std::next(start);
						start = test2;
						start = std::next(start);
						break;
					}
					start = std::next(start);
				}
				start = std::prev(start);

				auto goDeeper = T.begin();
				auto deepest = T.end();
				while (goDeeper != T.end()) {
					if (isIn(testPoint, *goDeeper->second)) {
						deepest = goDeeper;
					}
					goDeeper = std::next(goDeeper);
				}
				auto temp = std::prev(start);
				testFrom = start;
				if (deepest != T.end() && deepest->second->bIsUpper && !testFrom->second->bIsUpper && deepest->first < testFrom->first) {
					testFrom = deepest;
					int YOLO = -1;
				}
				auto mustBeLower = std::next(testFrom);
				if (p->parent->ID == 8) {
					int YOLO = -1;
				}
				if (testFrom->first < p->y && !isIn(testPoint, *testFrom->second)) {
					if (std::prev(testFrom)!=T.end()) {
						int YOLO = -1;
						testFrom = std::prev(testFrom);
						mustBeLower = std::next(testFrom);
					}
				}
				if (isIn(testPoint, *testFrom->second) && p->y > testFrom->first) {
					auto temp = T.insert(std::pair<double, HalfCircle*>(p->y,testFrom->second));
					T.erase(testFrom);
					testFrom = temp;
					testFrom->second->mapRef = new auto(testFrom);
				}
				else if ( mustBeLower != T.end() && !mustBeLower->second->bIsUpper && isIn(testPoint, *mustBeLower->second)) {
					auto tempLower = std::next(mustBeLower);
					auto temp = T.insert(std::pair<double, HalfCircle*>(p->y, mustBeLower->second));
					T.erase(mustBeLower);
					mustBeLower = temp;
					mustBeLower->second->mapRef = new auto(mustBeLower);
					if (tempLower!= T.end()) {
						while (tempLower != T.end() && isIn(testPoint, *tempLower->second) && tempLower!=mustBeLower) {
							tempLower = std::prev(tempLower);
						}
						tempLower = std::next(tempLower);
						while (tempLower != T.end() && tempLower != mustBeLower) {
							tempLower = std::next(tempLower);
							auto move = std::prev(tempLower);
							auto temp = T.insert(std::pair<double, HalfCircle*>(p->y, move->second));
							T.erase(move);
							temp->second->mapRef = new auto(temp);
						}
					}
				}

				auto p1 = T.insert(std::pair<double, HalfCircle*>(testFrom != T.end() ? 
					(isIn(testPoint, *testFrom->second->parent) 
					?	//roundf((testFrom->first)*precision) / precision 
						roundf((testPoint.y)*precision) / precision
				//	: roundf((testFrom->first) * precision) / precision)
					: roundf((testPoint.y ) * precision) / precision) 
					: roundf((testPoint.y )*precision) / precision, p->parent->upperCircle));
		
				//if (p1->first > testFrom->first) {
				//	auto stopCondition = testFrom->second;
				//	auto testLower = std::next(p1, 1);
				//	while (std::prev(p1, 1) == T.end()||stopCondition !=std::prev(p1,1)->second) {
				//		auto bu = testLower->second;
				//		testLower->second = p1->second;
				//		p1->second = bu;
				//		p1->second->mapRef = new auto(p1);
				//		p1 = testLower;
				//	}
				//	T.erase(p1);
				//	p1 = T.insert(std::pair<double, HalfCircle*>(roundf((testPoint.y)*precision) / precision, p->parent->upperCircle));
				//}
				//else {
				//	auto testUpper = std::prev(p1, 1);
				//	if (testUpper != T.end()) {
				//		while (testUpper != testFrom) {

				//			//if (!testUpper->second->bIsUpper) {
				//			auto bu = testUpper->second;
				//			testUpper->second = p1->second;
				//			p1->second = bu;
				//			//std::swap(p1, testUpper);
				//			p1->second->mapRef = new auto(p1);
				//			p1 = testUpper;
				//			//}
				//			//else {
				//			//	break;
				//			//}

				//			testUpper = std::prev(testUpper, 1);
				//		}
				//	}
				//	T.erase(p1);
				//	p1 = T.insert(std::pair<double, HalfCircle*>(roundf((testPoint.y)*precision) / precision, p->parent->upperCircle));
				//}

				p1->second->mapRef = new std::multimap<double, HalfCircle*>::iterator(p1);

				auto aboveP1 = std::prev(p1);
				while (aboveP1 != T.end() && aboveP1!=testFrom && aboveP1->first < testFrom->first) {
					break;
				}

				//auto p2 = T.insert(std::pair<double, HalfCircle*>(roundf(p1->first * precision) / precision, p->parent->lowerCircle));
				auto p2 = T.insert(std::pair<double, HalfCircle*>(roundf(p->y * precision) / precision, p->parent->lowerCircle));
				auto testLower = std::next(p2, 1);
			//	if (testLower != T.end() && testLower->second->parent->ID != p2->second->parent->ID) {
			//		while (testLower != p1) {
			//			//if (roundf(testLower->first * precision) / precision != roundf(p2->first * precision) / precision) {
			//			//	break;
			//			//}
			//			//else {
			//			//	if (testLower != p1) {
			//			auto bu = testLower->second;
			//			testLower->second = p2->second;
			//			p2->second = bu;
			//			p2->second->mapRef = new auto(p2);
			//			p2 = testLower;
			///*				}
			//				else {
			//					break;
			//				}
			//			}*/
			//			testLower = std::prev(testLower, 1);
			//		}
			//	}
				//

				while (mustBeLower != testLower && std::prev(mustBeLower) != testLower) {
					auto temp = T.insert(*mustBeLower);
					while (std::next(mustBeLower)->first == mustBeLower->first && std::next(mustBeLower)->second != p1->second) {
						auto tempLower = T.insert(*std::next(mustBeLower));
						T.erase(std::next(mustBeLower));
						tempLower->second->mapRef = new auto(tempLower);
					}
					T.erase(mustBeLower);
					mustBeLower = temp;
					mustBeLower->second->mapRef = new auto(temp);
					testLower = std::next(p2, 1);
				}
				p2->second->mapRef = new std::multimap<double, HalfCircle*>::iterator(p2);

				if (p1->second->parent->ID != p2->second->parent->ID) {
					int YOLO = -1;
				}

				auto p1Upper = std::prev(p1, 1);
				auto p2Lower = std::next(p2, 1);

				if (p1Upper != T.end()) {
					calculateIntersectionPoints(*p1Upper->second, *p1->second, info, p->x);
				}

				if (p2Lower != T.end()) {
					calculateIntersectionPoints(*p2->second, *p2Lower->second, info, p->x);
				}
			}

		}
		if (p->isEdgePointCircle && !p->isLeft) { //Is Right?
			if (p->parent->ID == 1) {
				int YOLO = -1;
			}
			auto upperUp = std::prev(*p->parent->upperCircle->mapRef, 1);
			auto upperLow = std::next(*p->parent->upperCircle->mapRef, 1);

			auto lowerUp = std::prev(*p->parent->lowerCircle->mapRef, 1);
			auto lowerLow = std::next(*p->parent->lowerCircle->mapRef, 1);

			if (upperUp != T.end() && upperLow != T.end()) {
				calculateIntersectionPoints(*upperUp->second, *upperLow->second, info, p->x);
			}

			if (lowerUp != T.end() && lowerLow != T.end()) {
				calculateIntersectionPoints(*lowerUp->second, *lowerLow->second, info, p->x);
			}

			if (upperUp != T.end() && lowerLow != T.end()) {
				calculateIntersectionPoints(*upperUp->second, *lowerLow->second, info, p->x);
			}

			if (upperLow != T.end() && lowerUp != T.end()) {
				calculateIntersectionPoints(*upperLow->second, *lowerUp->second, info, p->x);
			}
			//T.erase(*p->parent->lowerCircle->mapRef);
			T.erase(*p->parent->lowerCircle->mapRef);
			T.erase(*p->parent->upperCircle->mapRef);
		}
		info.testQueue.erase(info.testQueue.begin());
	}

	auto end = std::chrono::system_clock::now();
	info.milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	info.nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
}
