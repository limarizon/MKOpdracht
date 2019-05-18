#include <stdio.h>
#include <iostream>
#include <math.h>


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

const int precision = 1000000;

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
			return p1.y > p2.y;
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
	int status_pos = -1;
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
	//std::unordered_multiset<Point, testComp> testQueue;
	//std::priority_queue<double, std::vector<Point*>, std::greater<double>> QLog2;

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
void calculateIntersectionPoints(struct Circle& c1, struct Circle& c2, std::unordered_set<Point, MyHashFunction>* intersectionPoints, bool optimize = true);

void n_kwadraat_easy(InputInfo& ii);
void n_kwadraat_sweepline(InputInfo& ii);
void n_log_n_sweepline2(InputInfo& ii);
void n_log_n_sweepline3(InputInfo& ii);

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
		n_log_n_sweepline3(input_circles);
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

	n_log_n_sweepline3(test_input_circles_3);
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

double doubleRand() {
	return double(rand()) / (double(RAND_MAX) + 1.0);
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

			x = roundf(x*precision) / precision;
			y = roundf(y*precision) / precision;
			r = roundf(r*precision) / precision;

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

		int extraCircles = 20;
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

			x *= (1.0 + doubleRand());
			y *= (1.0 + doubleRand());
			x = roundf(x * precision) / precision;
			y = roundf(y * precision) / precision;
			r = roundf(r * precision) / precision;

			for (auto circle : test_input_circles_1.circles) {
				if (roundf(circle.radius * precision) / precision == roundf(r * precision) / precision && circle.center.x == x && circle.center.y == y) {
					continue;
				}
			}
			

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
			//test_input_circles_3.QLog2.push(eventPointLeft);
			//test_input_circles_3.QLog2.push(eventPointRight);
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

double distanceCalculate(Point *p1, Point *p2)
{
	double x = p1->x - p2->x;
	double y = p1->y - p2->y;
	double dist;

	dist = pow(x, 2) + pow(y, 2);
	dist = sqrt(dist);

	return dist;
}

void calculateIntersectionPoints(struct Circle& circle_1, struct Circle& circle_2, std::unordered_set<Point, MyHashFunction>* intersectionPoints, bool optimize)
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
		intersectionPoints->insert(intersection_point1);
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
	intersectionPoints->insert(intersection_point1);
	if (!dontAdd2) {
		intersectionPoints->insert(intersection_point2);
	}
	//std::cout << "Intersection X, Y: " << intersection_point1.x << "," << intersection_point1.y << std::endl;
	//std::cout << "Intersection X, Y: " << intersection_point2.x << "," << intersection_point2.y << std::endl;
}

void calculateIntersectionPoints(struct Circle* circle_1, struct Circle* circle_2, std::unordered_set<Point, MyHashFunction>* intersectionPoints, bool optimize)
{
	if (circle_1->center.x == circle_2->center.x && circle_1->center.y == circle_2->center.y && circle_1->radius == circle_2->radius) {
		return;
	}

	double distance = distanceCalculate(circle_2->center, circle_1->center);

	if (distance > circle_1->radius + circle_2->radius) {
		return;
	}
	else if (distance < abs(circle_1->radius - circle_2->radius)) {
		return;
	}

	if (optimize) {
		bool alreadyHasIntersection = false;
		for (int ID : circle_1->intersectsWith)
		{
			if (ID == circle_2->ID) {
				alreadyHasIntersection = true;
				break;
			}
		}

		if (alreadyHasIntersection) {
			return;
		}
	}

	//PSSST http://paulbourke.net/geometry/circlesphere/
	double x1 = (pow(distance, 2) - pow(circle_2->radius, 2) + pow(circle_1->radius, 2)) / (2 * distance);
	double a = (1 / distance) * sqrt(
		(-distance + circle_2->radius - circle_1->radius)*
		(-distance - circle_2->radius + circle_1->radius)*
		(-distance + circle_2->radius + circle_1->radius)*
		(distance + circle_2->radius + circle_1->radius)
	);

	if (roundf(distance * precision) / precision == roundf((circle_1->radius + circle_2->radius) * precision) / precision) {
		a = distance;

		Point intersection_middle = {
			circle_1->center.x + x1 * (circle_2->center.x - circle_1->center.x) / distance,	//X coordinate
			circle_1->center.y + x1 * (circle_2->center.y - circle_1->center.y) / distance	//Y coordinate
		};


		Point intersection_point1 = {
			/*intersection_middle.x + (a / 2) * (circle_2->center.y - circle_1->center.y) / distance,
			intersection_middle.y - (a / 2) * (circle_2->center.x - circle_1->center.x) / distance*/
			intersection_middle
		};

		circle_1->intersectsWith.insert(circle_2->ID);
		circle_2->intersectsWith.insert(circle_1->ID);

		intersection_point1.isRaakpunt = true;
		intersection_point1.isSnijpunt = true;
		intersectionPoints->insert(intersection_point1);
		//std::cout << "Intersection X, Y: " << intersection_point1.x << "," << intersection_point1.y << std::endl;
		return;
	}

	Point intersection_middle = {
		circle_1->center.x + x1 * (circle_2->center.x - circle_1->center.x) / distance,	//X coordinate
		circle_1->center.y + x1 * (circle_2->center.y - circle_1->center.y) / distance	//Y coordinate
	};


	Point intersection_point1 = {
		intersection_middle.x + (a / 2) * (circle_2->center.y - circle_1->center.y) / distance,
		intersection_middle.y - (a / 2) * (circle_2->center.x - circle_1->center.x) / distance
	};
	Point intersection_point2 = {
		intersection_middle.x - (a / 2) * (circle_2->center.y - circle_1->center.y) / distance,
		intersection_middle.y + (a / 2) * (circle_2->center.x - circle_1->center.x) / distance
	};
	intersection_point1.isSnijpunt = true;
	intersection_point2.isSnijpunt = true;
	if (circle_1->ID == 944 && circle_2->ID == 506 || circle_1->ID == 506 && circle_2->ID == 944) {
		int YOLO = -1;
	}

	if (intersection_point1.x >= circle_1->rightX) {
		intersection_point1.x = circle_1->rightX - 1.0f / precision;
	}
	if (intersection_point1.x >= circle_2->rightX) {
		intersection_point1.x = circle_2->rightX - 1.0f / precision;
	}
	if (intersection_point2.x >= circle_1->rightX) {
		intersection_point2.x = circle_1->rightX - 1.0f / precision;
	}
	if (intersection_point2.x >= circle_2->rightX) {
		intersection_point2.x = circle_2->rightX - 1.0f / precision;
	}

	bool dontAdd2 = false;
	//if (isnan(intersection_point1.x) || isnan(intersection_point1.x)) {
	if ((isnan(intersection_point1.x) || isnan(intersection_point1.x)) ||
		(roundf(intersection_point1.y * precision) / precision == roundf(circle_1->center.y*precision) / precision &&
		(roundf(intersection_point1.y * precision) / precision == roundf(circle_2->center.y*precision) / precision))) {
		if (roundf(circle_1->leftX * precision) / precision == roundf(circle_2->leftX * precision) / precision) {
			intersection_point1.x = circle_1->leftX - 1.0f / precision;
		}
		if (roundf(circle_1->leftX * precision) / precision == roundf(circle_2->rightX * precision) / precision) {
			intersection_point1.x = circle_1->leftX - 1.0f / precision;
		}
		if (roundf(circle_1->rightX * precision) / precision == roundf(circle_2->leftX * precision) / precision) {
			intersection_point1.x = circle_1->rightX - 1.0f / precision;
		}
		if (roundf(circle_1->rightX * precision) / precision == roundf(circle_2->rightX * precision) / precision) {
			intersection_point1.x = circle_1->rightX - 1.0f / precision;
		}
		if (roundf(circle_1->rightX * precision) / precision == roundf(circle_2->rightX * precision) / precision &&
			roundf(circle_1->leftX * precision) / precision == roundf(circle_2->leftX * precision) / precision) {
			int YOLO = -1;
		}
		if (circle_1->center.y != circle_2->center.y) {
			int YOLO = -1;
		}
		intersection_point1.y = circle_1->center.y - 1.0f / precision;
		intersection_point1.isRaakpunt = true;
		dontAdd2 = true;
	}

	circle_1->intersectsWith.insert(circle_2->ID);
	circle_2->intersectsWith.insert(circle_1->ID);
	intersectionPoints->insert(intersection_point1);
	if (!dontAdd2) {
		intersectionPoints->insert(intersection_point2);
	}
	//std::cout << "Intersection X, Y: " << intersection_point1.x << "," << intersection_point1.y << std::endl;
	//std::cout << "Intersection X, Y: " << intersection_point2.x << "," << intersection_point2.y << std::endl;
}

void calculateIntersectionPoints(struct Circle* circle_1, struct Circle* circle_2, std::vector<Point>* intersectionPoints, bool optimize)
{
	if (circle_1->center.x == circle_2->center.x && circle_1->center.y == circle_2->center.y && circle_1->radius == circle_2->radius) {
		return;
	}

	double distance = distanceCalculate(circle_2->center, circle_1->center);

	if (distance > circle_1->radius + circle_2->radius) {
		return;
	}
	else if (distance < abs(circle_1->radius - circle_2->radius)) {
		return;
	}

	if (optimize) {
		bool alreadyHasIntersection = false;
		for (int ID : circle_1->intersectsWith)
		{
			if (ID == circle_2->ID) {
				alreadyHasIntersection = true;
				break;
			}
		}

		if (alreadyHasIntersection) {
			return;
		}
	}

	//PSSST http://paulbourke.net/geometry/circlesphere/
	double x1 = (pow(distance, 2) - pow(circle_2->radius, 2) + pow(circle_1->radius, 2)) / (2 * distance);
	double a = (1 / distance) * sqrt(
		(-distance + circle_2->radius - circle_1->radius)*
		(-distance - circle_2->radius + circle_1->radius)*
		(-distance + circle_2->radius + circle_1->radius)*
		(distance + circle_2->radius + circle_1->radius)
	);

	if (roundf(distance * precision) / precision == roundf((circle_1->radius + circle_2->radius) * precision) / precision) {
		a = distance;

		Point intersection_middle = {
			circle_1->center.x + x1 * (circle_2->center.x - circle_1->center.x) / distance,	//X coordinate
			circle_1->center.y + x1 * (circle_2->center.y - circle_1->center.y) / distance	//Y coordinate
		};


		Point intersection_point1 = {
			/*intersection_middle.x + (a / 2) * (circle_2->center.y - circle_1->center.y) / distance,
			intersection_middle.y - (a / 2) * (circle_2->center.x - circle_1->center.x) / distance*/
			intersection_middle
		};

		circle_1->intersectsWith.insert(circle_2->ID);
		circle_2->intersectsWith.insert(circle_1->ID);

		intersection_point1.isRaakpunt = true;
		intersection_point1.isSnijpunt = true;
		intersectionPoints->push_back(intersection_point1);
		//std::cout << "Intersection X, Y: " << intersection_point1.x << "," << intersection_point1.y << std::endl;
		return;
	}

	Point intersection_middle = {
		circle_1->center.x + x1 * (circle_2->center.x - circle_1->center.x) / distance,	//X coordinate
		circle_1->center.y + x1 * (circle_2->center.y - circle_1->center.y) / distance	//Y coordinate
	};


	Point intersection_point1 = {
		intersection_middle.x + (a / 2) * (circle_2->center.y - circle_1->center.y) / distance,
		intersection_middle.y - (a / 2) * (circle_2->center.x - circle_1->center.x) / distance
	};
	Point intersection_point2 = {
		intersection_middle.x - (a / 2) * (circle_2->center.y - circle_1->center.y) / distance,
		intersection_middle.y + (a / 2) * (circle_2->center.x - circle_1->center.x) / distance
	};
	intersection_point1.isSnijpunt = true;
	intersection_point2.isSnijpunt = true;
	if (circle_1->ID == 944 && circle_2->ID == 506 || circle_1->ID == 506 && circle_2->ID == 944) {
		int YOLO = -1;
	}

	if (intersection_point1.x >= circle_1->rightX) {
		intersection_point1.x = circle_1->rightX - 1.0f / precision;
	}
	if (intersection_point1.x >= circle_2->rightX) {
		intersection_point1.x = circle_2->rightX - 1.0f / precision;
	}
	if (intersection_point2.x >= circle_1->rightX) {
		intersection_point2.x = circle_1->rightX - 1.0f / precision;
	}
	if (intersection_point2.x >= circle_2->rightX) {
		intersection_point2.x = circle_2->rightX - 1.0f / precision;
	}

	bool dontAdd2 = false;
	//if (isnan(intersection_point1.x) || isnan(intersection_point1.x)) {
	if ((isnan(intersection_point1.x) || isnan(intersection_point1.x)) ||
		(roundf(intersection_point1.y * precision) / precision == roundf(circle_1->center.y*precision) / precision &&
		(roundf(intersection_point1.y * precision) / precision == roundf(circle_2->center.y*precision) / precision))) {
		if (roundf(circle_1->leftX * precision) / precision == roundf(circle_2->leftX * precision) / precision) {
			intersection_point1.x = circle_1->leftX - 1.0f / precision;
		}
		if (roundf(circle_1->leftX * precision) / precision == roundf(circle_2->rightX * precision) / precision) {
			intersection_point1.x = circle_1->leftX - 1.0f / precision;
		}
		if (roundf(circle_1->rightX * precision) / precision == roundf(circle_2->leftX * precision) / precision) {
			intersection_point1.x = circle_1->rightX - 1.0f / precision;
		}
		if (roundf(circle_1->rightX * precision) / precision == roundf(circle_2->rightX * precision) / precision) {
			intersection_point1.x = circle_1->rightX - 1.0f / precision;
		}
		if (roundf(circle_1->rightX * precision) / precision == roundf(circle_2->rightX * precision) / precision &&
			roundf(circle_1->leftX * precision) / precision == roundf(circle_2->leftX * precision) / precision) {
			int YOLO = -1;
		}
		if (circle_1->center.y != circle_2->center.y) {
			int YOLO = -1;
		}
		intersection_point1.y = circle_1->center.y - 1.0f / precision;
		intersection_point1.isRaakpunt = true;
		dontAdd2 = true;
	}

	circle_1->intersectsWith.insert(circle_2->ID);
	circle_2->intersectsWith.insert(circle_1->ID);
	intersectionPoints->push_back(intersection_point1);
	if (!dontAdd2) {
		intersectionPoints->push_back(intersection_point2);
	}
	//std::cout << "Intersection X, Y: " << intersection_point1.x << "," << intersection_point1.y << std::endl;
	//std::cout << "Intersection X, Y: " << intersection_point2.x << "," << intersection_point2.y << std::endl;
}


std::vector<Point> *temp = new std::vector<Point>;
std::vector<Point> keepPoints;

void calculateIntersectionPoints(struct HalfCircle* c1, struct HalfCircle* c2, InputInfo *info, double currentX) {
	calculateIntersectionPoints(c1->parent, c2->parent, temp, false);
	if (temp->size() == 0) { return; }

	for (Point p : *temp)
	{
		if (c1->bIsUpper) {
			if (p.y <= c1->parent->center.y) {
				continue;
			}
		}
		else if (!c1->bIsUpper) {
			if (p.y > c1->parent->center.y) {
				continue;
			}
		}

		if (c2->bIsUpper) {
			if (p.y <= c2->parent->center.y) {
				continue;
			}
		}
		else if (!c2->bIsUpper) {
			if (p.y > c2->parent->center.y) {
				continue;
			}
		}
		//p is een correct snijpunt voor de halve cirkels
		keepPoints.push_back((p));
		//info.intersectionPoints.insert(p);
	}

	for (auto i = 0; i < keepPoints.size(); i++)
	{
		if (roundf(keepPoints[i].x * precision) / precision < roundf(currentX * precision) / precision) {
			//info.intersectionPoints.insert(keepPoints[i]);
			continue;
		}

		if (info->intersectionPoints.find(keepPoints[i]) == info->intersectionPoints.end()) {
			keepPoints[i].hc1 = c1;
			keepPoints[i].hc2 = c2;
			keepPoints[i].isEdgePointCircle = false;
			auto poc = new Point(*new auto(keepPoints[i]));
			//info.circleEventPointsLogN.insert(std::pair<double, Point*>(keepPoints[i].x,poc));
			info->testQueue.emplace(*poc);
			info->intersectionPoints.emplace(*poc);
		}


	}
	temp->clear();
	keepPoints.clear();
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
				calculateIntersectionPoints(info.circles[i], info.circles[j], &info.intersectionPoints);
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
				calculateIntersectionPoints(*circle, *activeCircle, &info.intersectionPoints);
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

bool isIn(Point *p, HalfCircle *c) {
	if (c->bIsUpper) {
		if (p->y < c->parent->center.y) {
			return false;
		}
	}else 	if (!c->bIsUpper) {
		if (p->y > c->parent->center.y) {
			return false;
		}
	}
	if (distanceCalculate(p, &c->parent->center) <= c->parent->radius) {
		return true;
	}
	return false;
}

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

template <class T>
struct Node {
	T value;
	Node *left = NULL;
	Node *right = NULL;

	Node(T val) {
		this->value = val;
	}

	Node(T val, Node<T> left, Node<T> right) {
		this->value = val;
		this->left = left;
		this->right = right;
	}

	inline bool operator>(const Node<T> &node) const {
		return value > node.value;
	}

	inline bool operator<(const Node<T> &node) const {
		return value < node.value;
	}

	inline bool operator>=(const Node<T> &node) const {
		return value >= node.value;
	}

	inline bool operator<=(const Node<T> &node) const {
		return value <= node.value;
	}

	inline bool operator==(const Node<T> &node) const {
		return value == node.value;
	}

	//friend bool operator==(Node<T> const& lhs, Node<T> const& rhs) {
	//	return lhs == rhs;
	//}
};

template <class T>
class BST {

private:
	Node<T> *root;

	Node<T>* addHelper(Node<T> *root, T val) {
		if (root->value > val) {
			if (!root->left) {
				return root->left = new Node<T>(val);
			}
			else {
				return addHelper(root->left, val);
			}
		}
		else {
			if (!root->right) {
				return root->right = new Node<T>(val);
			}
			else {
				return addHelper(root->right, val);
			}
		}
	}

	void printHelper(Node<T> *root) {
		if (!root) return;
		printHelper(root->left);
		std::cout << root->value << ' ';
		printHelper(root->right);
	}

	int nodesCountHelper(Node<T> *root) {
		if (!root) return 0;
		else return 1 + nodesCountHelper(root->left) + nodesCountHelper(root->right);
	}

	int heightHelper(Node<T> *root) {
		if (!root) return 0;
		else return 1 + max(heightHelper(root->left), heightHelper(root->right));
	}

	void printMaxPathHelper(Node<T> *root) {
		if (!root) return;
		std::cout << root->value << ' ';
		if (heightHelper(root->left) > heightHelper(root->right)) {
			printMaxPathHelper(root->left);
		}
		else {
			printMaxPathHelper(root->right);
		}
	}

	bool deleteValueHelper(Node<T>* parent, Node<T>* current, T value) {
		if (!current) return false;
		if (current->value == value) {
			if (current->left == NULL || current->right == NULL) {
				Node<T>* temp = current->left;
				if (current->right) temp = current->right;
				if (parent) {
					if (parent->left == current) {
						parent->left = temp;
					}
					else {
						parent->right = temp;
					}
				}
				else {
					this->root = temp;
				}
			}
			else {
				Node<T>* validSubs = current->right;
				while (validSubs->left) {
					validSubs = validSubs->left;
				}
				T temp = current->value;
				current->value = validSubs->value;
				validSubs->value = temp;
				return deleteValueHelper(current, current->right, temp);
			}
			delete current;
			return true;
		}
		return deleteValueHelper(current, current->left, value) ||
			deleteValueHelper(current, current->right, value);
	}

	Node<T>* findValueHelper(Node<T>* parent, Node<T>* current, T value) {
		if (current == NULL) return nullptr;
		if (current->value == value) {
			return current;
		}
		if (value > current->value) {
			return findValueHelper(current, current->right, value);
		}
		else {
			return findValueHelper(current, current->left, value);
		}
	}

	Node<T>* searchHelper(Node<T>* parent, T value) {
		if (parent == nullptr || parent->value == value) {
			return parent;
		}

		if (parent->value < value)
			return searchHelper(parent->right, value);

		return searchHelper(parent->left, value);
	}

	Node<T>* findSuccesorHelper(Node<T>* parent, Node<T>* node) {
		if (parent == nullptr) {
			return nullptr;
		}

		if (node->right != nullptr) {
			return findMin(node->right);
		}

		Node<T>* successor = nullptr;

		while (parent!=nullptr) {
			if (*node < *parent) {
				successor = parent;
				parent = parent->left;
			}
			else if (*node > *parent) {
				parent = parent->right;
			}
			else {
				break;
			}
		}

		return successor;
	}	

	Node<T>* findSuccesorHelper2(Node<T>* parent, Node<T>* node) {
		Node<T>* successor = nullptr;
		while (true) {
			if (*node < parent->value) {
				successor = parent;
				parent = parent->left;
			}else if (*node > parent->value) {
				parent = parent->right;
			}
			else {
				if (parent->right) {
					successor = findMin(parent->right);
				}
				break;
			}

			if (!parent) {
				return nullptr;
			}
		}
		return successor;
	}

	Node<T>* findMinHelper(Node<T>* node) {
		while (node->left) {
			node = node->left;
		}
		return node;
	}

	Node<T>* findMaxHelper(Node<T>* node) {
		while (node->right) {
			node = node->right;
		}
		return node;
	}

	Node<T>* findPredecessorHelper(Node<T>* root, Node<T>* node)
	{
		Node<T>* predecessor = nullptr;

		if (root == nullptr) {
			return predecessor;
		}

		while (root!=nullptr) {
			if (*root == *node) {
				if (root->left) {
					predecessor = root->left;
					while (predecessor->right)
						predecessor = predecessor->right;
				}

				return predecessor;
			}
			else if (*root < *node) {
				predecessor = root;
				root = root->right;
			}
			else {
				root = root->left;
			}
		}
		return predecessor;
	}

	Node<T>* findPredecessorHelper2(Node<T>* parent, Node<T>* node) {
		Node<T>* predecessor = NULL;
		Node<T>* current = parent;

		if (!root)
			return NULL;

		while (current && !(current->value == node->value)) {
			if (current->value > node->value) {
				current = current->left;
			}
			else {
				predecessor = current;
				current = current->right;
			}
		}
		if (current && current->left) {
			predecessor = findMaxHelper(current->left);
		}
		return predecessor;
	}

	void deepSearchHelper(Node<T>* parent,Node<T>*& node ,T value) {
		if (parent == NULL) {
			return;
		}
		if (parent->value == value) {
			node = parent;
			return;
		}
		if (node) {
			return;
		}
		if (parent->left) {
			deepSearchHelper(parent->left, node, value);
		}
		if (node) {
			return;
		}
		if (parent->right) {
			deepSearchHelper(parent->right, node, value);
		}
	}
public:
	Node<T>* add(T val) {
		if (root) {
			return this->addHelper(root, val);
		}
		else {
			return root = new Node<T>(val);
		}
	}

	void print() {
		printHelper(this->root);
	}

	int nodesCount() {
		return nodesCountHelper(root);
	}

	int height() {
		return heightHelper(this->root);
	}

	void printMaxPath() {
		printMaxPathHelper(this->root);
	}

	bool deleteValue(T value) {
		return this->deleteValueHelper(NULL, this->root, value);
	}

	Node<T>* findMin(Node<T>* node) {
		return findMinHelper(node);
	}

	Node<T>* findSuccessor(Node<T>* node) {
		auto temp = findSuccesorHelper(root, node);
		auto temp2 = findSuccesorHelper2(root, node);
		if (temp!=temp2) {
		//	return temp2;
		}
		return findSuccesorHelper(root, node);
	}

	Node<T>* findPredecessor(Node<T>* node) {
		auto temp = findPredecessorHelper(root, node);
		auto temp2 =findPredecessorHelper2(root, node);
		if (temp!=temp2) {
		//	return temp2;
		}
		return findPredecessorHelper(root, node);
	}

	Node<T>* findValue(T value) {
		return this->findValueHelper(NULL, this->root, value);
	}

	Node<T>* search(T value) {
		//auto temp = this->searchHelper(this->root, value);
		if (this->root == NULL) {
			return nullptr;
		}
		Node<T>* temp = nullptr;
		if (temp == nullptr) {
			deepSearchHelper(this->root, temp,value);
			if (temp == nullptr) {
				int YOLO = -1;
			}
		}
		return temp;
	}

	Node<T>* getRoot() {
		return root;
	}
};

static double sweepX = 0.0;

double calcY(HalfCircle* hc) {
	//(x-a)^2 + (y-b)^2 = r^2
	//r^2 - (x-a)^2 = (y-b)^2
	//UPPER:
	//sqrt(R^2 - (x-a)^2) = y-b
	//sqrt(R^2 - (x-a)^2) + b = y
	//LOWER
	//-sqrt(R^2 - (x-a)^2) = y-b
	//-sqrt(R^2 - (x-a)^2)+b = y
	if (hc->bIsUpper) {
		return roundf((sqrt(std::abs(roundf(pow(hc->parent->radius, 2)*precision)/precision - roundf(pow((sweepX - hc->parent->center.x), 2)*precision)/precision)) + hc->parent->center.y)*precision)/precision;
	}
	else {
		return roundf((-sqrt(std::abs(roundf(pow(hc->parent->radius, 2)*precision) / precision - roundf(pow((sweepX - hc->parent->center.x), 2)*precision) / precision)) + hc->parent->center.y)*precision) / precision;
	}
}

double calcY(HalfCircle* hc, double x) {
	//(x-a)^2 + (y-b)^2 = r^2
	//r^2 - (x-a)^2 = (y-b)^2
	//UPPER:
	//sqrt(R^2 - (x-a)^2) = y-b
	//sqrt(R^2 - (x-a)^2) + b = y
	//LOWER
	//-sqrt(R^2 - (x-a)^2) = y-b
	//-sqrt(R^2 - (x-a)^2)+b = y
	if (hc->bIsUpper) {
		return sqrt(pow(hc->parent->radius, 2) - pow((x - hc->parent->center.x), 2)) + hc->parent->center.y;
	}
	else {
		return -sqrt(pow(hc->parent->radius, 2) - pow((x - hc->parent->center.x), 2)) + hc->parent->center.y;
	}
}

const int halfPrecision = 1000;

struct status_node {
	HalfCircle* sc;
	double x;

	inline bool operator==(const status_node &node) const {
		return sc->ID == node.sc->ID && sc->bIsUpper == node.sc->bIsUpper;
	}

	friend bool operator>(status_node const& lhs, status_node const& rhs) {
		auto y1 = roundf(calcY(lhs.sc) * sqrt(precision) ) / sqrt(precision);
		auto y2 = roundf(calcY(rhs.sc) * sqrt(precision)) / sqrt(precision);
		//return calcY(lhs.sc, lhs.x) > calcY(rhs.sc, rhs.x);
		//return calcY(lhs.sc) > calcY(rhs.sc);
		return y1 > y2;
	}

	friend bool operator<(status_node const& lhs, status_node const& rhs) {
		//return calcY(lhs.sc, lhs.x) < calcY(rhs.sc, rhs.x);
		auto y1 = roundf(calcY(lhs.sc) * precision) / precision;
		auto y2 = roundf(calcY(rhs.sc) * precision) / precision;
		//return calcY(lhs.sc) <= calcY(rhs.sc);
		return y1 < y2;
	}

	friend double operator-(status_node const& lhs, status_node const& rhs) {
		return calcY(lhs.sc) - calcY(rhs.sc);
	}

	friend bool operator<=(status_node const& lhs, status_node const& rhs) {
		//return calcY(lhs.sc, lhs.x) < calcY(rhs.sc, rhs.x);
		auto y1 = roundf(calcY(lhs.sc) * precision) / precision;
		auto y2 = roundf(calcY(rhs.sc) * precision) / precision;
		return y1 <= y2;
	}

	friend bool operator>=(status_node const& lhs, status_node const& rhs) {
		//return calcY(lhs.sc, lhs.x) < calcY(rhs.sc, rhs.x);
		auto y1 = roundf(calcY(lhs.sc) * precision) / precision;
		auto y2 = roundf(calcY(rhs.sc) * precision) / precision;
		return y1 >= y2;
	}

	friend std::ostream &operator<<(std::ostream &output, const status_node &D) {
		auto temp = D.sc->bIsUpper ? "UPPER" : "LOWER";
		output << "ID: " << D.sc->ID << ", U: " << temp << std::endl;
		return output;
	}
};

void n_log_n_sweepline2(InputInfo& info) {
	//BST<int> *testTree = new BST<int>();
	//testTree->add(5);
	//auto temp2 = testTree->add(4);
	//auto temp3 = testTree->add(7);
	//testTree->add(8);
	//auto temp = testTree->add(8);
	//testTree->add(9);
	//testTree->add(2);
	//testTree->add(60);
	//testTree->add(39);
	//testTree->add(10);
	//testTree->add(-3);
	//auto what = testTree->findValue(8);
	//auto test2 = testTree->findSuccessor(temp2);
	//auto test3 = testTree->findPredecessor(temp2);
	////testTree->print();
	//Node<int>* test = testTree->findMin(testTree->getRoot());
	BST<status_node> *statusTree = new BST<status_node>();
	auto start = std::chrono::system_clock::now();
	while (!info.testQueue.empty())
	{
		auto p = info.testQueue.begin();
		if (p->parent!=nullptr && p->parent->ID == 10) {
			info.testQueue.erase(p);
			p = info.testQueue.begin();
		}
		sweepX = roundf(p->x * precision) / precision;
		if (p->isEdgePointCircle) {
			if (p->isLeft) {
				if (p->parent->ID == 5) {
					int YOLO = -1;
				}

				sweepX = roundf((p->x - 10.0f / precision) * precision)/precision;
				auto HCU = statusTree->add(status_node{ p->parent->upperCircle,p->x });
				sweepX = roundf(p->x * precision) / precision;

				auto lowerHCU = statusTree->findPredecessor(HCU);
				auto upperHCU = statusTree->findSuccessor(HCU);

				if (upperHCU != nullptr) {
					calculateIntersectionPoints(upperHCU->value.sc, HCU->value.sc, &info, p->x);
				}

				if (lowerHCU != nullptr) {
					calculateIntersectionPoints(lowerHCU->value.sc, HCU->value.sc, &info, p->x);
				}

				sweepX = roundf((p->x - 10.0f / precision) * precision) / precision;
				auto HCL = statusTree->add(status_node{  p->parent->lowerCircle ,p->x  });
				sweepX = roundf(p->x * precision) / precision;

				auto lowerHCL = statusTree->findPredecessor(HCL);
				auto upperHCL = statusTree->findSuccessor(HCL);

				if (lowerHCL != nullptr) {
					calculateIntersectionPoints(HCL->value.sc, lowerHCL->value.sc, &info, p->x);
				}

				if (upperHCL != nullptr) {
					calculateIntersectionPoints(HCL->value.sc, upperHCL->value.sc, &info, p->x);
				}

				if (lowerHCU != nullptr) {
					calculateIntersectionPoints(HCL->value.sc, lowerHCU->value.sc, &info, p->x);
				}

				if (upperHCU != nullptr) {
					calculateIntersectionPoints(HCL->value.sc, upperHCU->value.sc, &info, p->x);
				}

				
				if (lowerHCL != nullptr) {
					calculateIntersectionPoints(HCU->value.sc, lowerHCL->value.sc, &info, p->x);
				}

				if (upperHCL != nullptr) {
					calculateIntersectionPoints(HCU->value.sc, upperHCL->value.sc, &info, p->x);
				}

				if (upperHCU != nullptr) {
					calculateIntersectionPoints(upperHCU->value.sc, HCU->value.sc, &info, p->x);
				}

				if (lowerHCU != nullptr) {
					calculateIntersectionPoints(lowerHCU->value.sc, HCU->value.sc, &info, p->x);
				}
				
			}
			else {
				sweepX = roundf((p->x - 0.0f / precision) * precision) / precision;

				auto node1 = statusTree->search(status_node{p->parent->upperCircle ,sweepX });
				auto node2 = statusTree->search(status_node{p->parent->lowerCircle ,sweepX });

				if (node1 == nullptr || node2 == nullptr) {
					int YOLO = -1;
				}

				sweepX = roundf((p->x + 0.0f/precision) * precision) / precision;
				auto upper = statusTree->findSuccessor(node1);
				auto lower = statusTree->findPredecessor(node2);

				if (upper!=nullptr && lower != nullptr) {
					calculateIntersectionPoints(lower->value.sc, upper->value.sc, &info, p->x);
				}
				
				auto status = statusTree->deleteValue(status_node{ p->parent->upperCircle });
				auto status2 = statusTree->deleteValue(status_node{ p->parent->lowerCircle });
			}
		}
		else if(p->isSnijpunt && !p->isRaakpunt){
			/*std::cout << "****START*****" << std::endl;
			statusTree->print();
			std::cout << "****END*****" << std::endl;*/

			sweepX = roundf((p->x - 10.0f / precision) * precision) / precision;
			//sweepX = p->x - 10.0f/precision;

			auto hc1 = p->hc1;
			auto hc2 = p->hc2;

			auto node1 = statusTree->search({ hc1 , sweepX });
			auto node2 = statusTree->search({ hc2 , sweepX });

			if (calcY(node2->value.sc) > calcY(node1->value.sc)) {
				hc1 = p->hc2;
				hc2 = p->hc1;
			}

			statusTree->deleteValue({ hc2 , sweepX });
			statusTree->deleteValue({ hc1 , sweepX });

			
			sweepX = roundf((p->x + 10.0f / precision) * precision) / precision;
			node1 = statusTree->add(status_node{ hc2,p->x });
			//sweepX = roundf((p->x + 10.0f / precision) * precision) / precision;
			node2 = statusTree->add(status_node{ hc1,p->x });

			sweepX = roundf((p->x - 0.0f/precision) * precision) / precision;
			auto upper = statusTree->findSuccessor(node1);
			sweepX = roundf((p->x + 0.0f/precision) * precision) / precision;
			auto lower = statusTree->findPredecessor(node2);

			if (upper!= nullptr) {
				calculateIntersectionPoints(node2->value.sc, upper->value.sc, &info, p->x);
			}
			if (lower != nullptr) {
				calculateIntersectionPoints(node1->value.sc, lower->value.sc, &info, p->x);
			}
			if (upper != nullptr) {
				calculateIntersectionPoints(node1->value.sc, upper->value.sc, &info, p->x);
			}
			if (lower != nullptr) {
				calculateIntersectionPoints(node2->value.sc, lower->value.sc, &info, p->x);
			}
			//std::cout << "****START*****" << std::endl;
			//statusTree->print();
			//std::cout << "****END*****" << std::endl;
		}

		/*std::cout << "****START*****" << std::endl;
		statusTree->print();
		std::cout << "****END*****" << std::endl;*/
		sweepX = p->x;
		info.testQueue.erase(p);
	}
	//statusTree->print();
	auto end = std::chrono::system_clock::now();
	info.milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	info.nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
}


template <class T>
int BSearch(std::vector<T>* vec, T* value, int start, int end) {
	while (start <= end) {
		int index = (start + end) / 2;
		if (vec->at(index) < *value) {
			start = index + 1;
		}
		else if (vec->at(index) > *value) {
			end = index - 1;
		}
		else{
			return index;
		}
	}
	if (start == 0) {
		return 0;
	}
	else {
		return end + 1;
	}
}

void n_log_n_sweepline3(InputInfo& info) {
	std::vector<status_node> *T = new std::vector<status_node>();
	auto start = std::chrono::system_clock::now();
	while (!info.testQueue.empty())
	{
		auto p = info.testQueue.begin();
		sweepX = p->x;
		if (p->isEdgePointCircle) {
			if (p->isLeft) {
				if (T->size() == 0) {
					T->push_back(status_node{ p->parent->lowerCircle });
					p->parent->lowerCircle->status_pos = 0;
					T->push_back(status_node{ p->parent->upperCircle });
					p->parent->upperCircle->status_pos = 1;
				}
				else {
					auto node1 = new status_node {p->parent->upperCircle};
					auto node2 = new status_node{ p->parent->lowerCircle };

					int pos = BSearch(T, node2, 0, T->size() - 1);
					T->insert(T->begin() + pos, *node2);
					p->parent->lowerCircle->status_pos = pos;

					pos += 1;
					T->insert(T->begin() + pos, *node1);
					p->parent->upperCircle->status_pos = pos;
					for (auto i = pos + 1; i < T->size(); i++)
					{
						T->at(i).sc->status_pos = i;
					}

					int upperIndex = node1->sc->status_pos + 1;
					int lowerIndex = node2->sc->status_pos - 1;
					auto upper = !(upperIndex >= T->size()) ? &T->at(upperIndex) : nullptr;
					auto lower = !(lowerIndex < 0) ? &T->at(lowerIndex) : nullptr;

					if (lower != nullptr) {
						calculateIntersectionPoints(lower->sc, node2->sc, &info, p->x);
					}
					if (upper != nullptr) {
						calculateIntersectionPoints(node1->sc, upper->sc, &info, p->x);
					}

					for (size_t i = 0; i < T->size(); i++)
					{
						if (T->at(i).sc->status_pos != i) {
							int YOLO = -1;
						}
					}
				}
			}
			else {
				if (abs(p->parent->upperCircle->status_pos - p->parent->lowerCircle->status_pos) != 1) {
					int YOLO = -1;
				}
				auto node1 =T->at( p->parent->upperCircle->status_pos );
				auto node2 = T->at( p->parent->lowerCircle->status_pos );


				int pos = node1.sc->status_pos;
				auto upper1 = !(pos + 1 >= T->size()) ? &T->at(pos + 1) : nullptr;
				auto lower1 = !(pos - 2 < 0) ? &T->at(pos - 2) : nullptr;
				//TODO optimize later
				if (upper1 != nullptr && lower1 != nullptr) {
					calculateIntersectionPoints(lower1->sc, upper1->sc, &info, p->x);
				}

				int pos2 = node2.sc->status_pos;
				auto lower2 = !(pos2 - 1 < 0) ? &T->at(pos2 - 1) : nullptr;
				auto upper2 = !(pos2 + 1 >= T->size()) ? &T->at(pos2 + 1) : nullptr;

				int lowest = pos < pos2 ? pos : pos2;

				if (upper2 != nullptr && lower2 != nullptr) {
					calculateIntersectionPoints(lower2->sc, upper2->sc, &info, p->x);
				}

				if (pos > pos2) {
					T->erase(T->begin() + pos);
					T->erase(T->begin() + pos2);
				}
				else {
					T->erase(T->begin() + pos2);
					T->erase(T->begin() + pos);
				}

				for (auto i = lowest; i < T->size(); i++)
				{
					T->at(i).sc->status_pos = i;
				}

				for (size_t i = 0; i < T->size(); i++)
				{
					if (T->at(i).sc->status_pos != i) {
						int YOLO = -1;
					}
				}
			}
		}
		else if (p->isSnijpunt && !p->isRaakpunt) {
			if (abs(p->hc2->status_pos - p->hc1->status_pos) != 1) {
				int YOLO = -1;
			}
			status_node node1;
			status_node node2;
			if (p->hc1->status_pos > p->hc2->status_pos) {
				node1 = T->at(p->hc2->status_pos);
				node2 = T->at(p->hc1->status_pos);
			}
			else {
				node1 = T->at(p->hc1->status_pos);
				node2 = T->at(p->hc2->status_pos);
			}

			auto pos1 = node1.sc->status_pos;
			auto lower = !(pos1 - 1 < 0) ? &T->at(pos1 - 1) : nullptr;
			auto upper = !(pos1 + 2 >= T->size()) ? &T->at(pos1 + 2) : nullptr;
			auto pos2 = pos1 + 1;
			T->at(node1.sc->status_pos) = node2;
			T->at(node2.sc->status_pos) = node1;

			T->at(pos1).sc->status_pos = pos1;
			
			T->at(pos2).sc->status_pos = pos2;

			if (upper!= nullptr) {
				calculateIntersectionPoints(upper->sc, T->at(pos2).sc, &info, p->x);
			}
			if (lower != nullptr) {
				calculateIntersectionPoints(T->at(pos1).sc, lower->sc, &info, p->x);
			}

			for (size_t i = 0; i < T->size(); i++)
			{
				if (T->at(i).sc->status_pos != i) {
					int YOLO = -1;
				}
			}
		}
		info.testQueue.erase(p);
	}
	auto end = std::chrono::system_clock::now();
	info.milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	info.nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
}