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
	}
};

struct testComp {
	bool operator()(const Point& p1, const Point& p2) {
		if (roundf(p1.x*precision)/precision == roundf(p2.x * precision) / precision) {
			return p1.y > p2.y;
		}
		return p1.x < p2.x;
	}
};

// class for hash function 
class MyHashFunction {
public:
	size_t operator()(const Point& p	) const
	{
		return (std::hash<double>()(roundf(p.x*precision) / precision*100000  + roundf(p.y*precision) / precision));
	}
};

inline bool operator< (const Point& lhs, const Point& rhs) {
	return std::tie(lhs.x, lhs.y) < std::tie(rhs.x, rhs.y);
}


struct HalfCircle {
	bool bIsUpper = true;
	struct Circle *parent;
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
	std::multiset<Point, testComp> testQueue;


	std::multimap<double, HalfCircle*> halfCircles;

	std::vector<Circle*> activeCircleVector;
	std::unordered_set<Point, MyHashFunction> intersectionPoints;

	std::chrono::milliseconds milliseconds;
	std::chrono::nanoseconds nanoseconds;
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

#ifdef _DEBUG
InputInfo test_input_circles_1;
InputInfo test_input_circles_2;
InputInfo test_input_circles_3;
#endif // _DEBUG

//PROTOTYPES --START
InputInfo openAndReadFile(char*& path);
double distanceCalculate(Point p1, Point p2);
void calculateIntersectionPoints(struct Circle& c1, struct Circle& c2, std::unordered_set<Point, MyHashFunction>* intersectionPoints, bool optimize = true);
void n_kwadraat_easy(InputInfo& ii);
void n_kwadraat_sweepline(InputInfo& ii);
void n_log_n_sweepline3(InputInfo& ii);
//PROTOTYPES --END

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

	//n_kwadraat_sweepline(test_input_circles_2);
	//std::cout << "Algorithm 2, Test circles: " << test_input_circles_2.num_circles << std::endl;
	//std::cout << "ms: " << test_input_circles_2.milliseconds.count() << std::endl;
	//std::cout << "Found: " << test_input_circles_2.intersectionPoints.size() << " intersections" << std::endl << std::endl;

	//n_log_n_sweepline3(test_input_circles_3);
	//std::cout << "Algorithm 3, Test circles: " << test_input_circles_3.num_circles << std::endl;
	//std::cout << "ms: " << test_input_circles_3.milliseconds.count() << std::endl;
	//std::cout << "Found: " << test_input_circles_3.intersectionPoints.size() << " intersections" << std::endl << std::endl;

	//int notFoundCount = 0;
	//int raakpunten = 0;
	//for (Point p : test_input_circles_1.intersectionPoints) {
	//	bool bFound = false;
	//	if (p.isRaakpunt) {
	//		raakpunten++;
	//	}

	//	for(Point p2 : test_input_circles_3.intersectionPoints)
	//	{
	//		if (roundf(p.x * precision)/ precision == roundf(p2.x * precision) / precision
	//			&& roundf(p.y * precision) / precision == roundf( p2.y * precision) / precision) {
	//			bFound = true;
	//			break;
	//		}
	//	}
	//	if (!bFound) {
	//		notFoundCount++;
	//	}
	//}

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
			inputInfo.testQueue.emplace(*eventPointLeft);
			inputInfo.testQueue.emplace(*eventPointRight);
		}

#ifdef _DEBUG
		//int extraCircles = 500;
		//int region = 250;
		//int maxRadius = 20;

		int extraCircles = 4000;
		int region = 100;
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

			//for (auto circle : test_input_circles_1.circles) {
			//	if (roundf(circle.radius * precision) / precision == roundf(r * precision) / precision && circle.center.x == x && circle.center.y == y) {
			//		continue;
			//	}
			//}
			

			Point circleCenter = { x,y };
			test_input_circles_1.circles[i] = { i, circleCenter, r,x - r,x + r };
			test_input_circles_1.circles[i].lowerCircle = new HalfCircle{ false, &test_input_circles_1.circles[i] };
			test_input_circles_1.circles[i].upperCircle = new HalfCircle{ true, &test_input_circles_1.circles[i] };
			test_input_circles_1.circleEventPoints.insert(std::pair<double, Circle*>(test_input_circles_1.circles[i].leftX, &test_input_circles_1.circles[i]));
			test_input_circles_1.circleEventPoints.insert(std::pair<double, Circle*>(test_input_circles_1.circles[i].rightX, &test_input_circles_1.circles[i]));


			test_input_circles_2.circles[i] = { i, circleCenter, r,x - r,x + r };
			test_input_circles_2.circles[i].lowerCircle = new HalfCircle{ false, &test_input_circles_2.circles[i] };
			test_input_circles_2.circles[i].upperCircle = new HalfCircle{ true, &test_input_circles_2.circles[i] };
			test_input_circles_2.circleEventPoints.insert(std::pair<double, Circle*>(test_input_circles_2.circles[i].leftX, &test_input_circles_2.circles[i]));
			test_input_circles_2.circleEventPoints.insert(std::pair<double, Circle*>(test_input_circles_2.circles[i].rightX, &test_input_circles_2.circles[i]));

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
			test_input_circles_3.testQueue.emplace(*eventPointLeft);
			test_input_circles_3.testQueue.emplace(*eventPointRight);
			//test_input_circles_3.QLog2.push(eventPointLeft);
			//test_input_circles_3.QLog2.push(eventPointRight);
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

void processPointForHalfCircles(struct Circle* circle_1, struct Circle* circle_2, struct Point& point, std::multiset<Point, testComp>* eventQueue) {
	HalfCircle* hc1;
	HalfCircle* hc2;
	if (point.y > circle_1->center.y) {
		hc1 = circle_1->upperCircle;
	}
	else {
		hc1 = circle_1->lowerCircle;
	}

	if (point.y > circle_2->center.y) {
		hc2 = circle_2->upperCircle;
	}
	else {
		hc2 = circle_2->lowerCircle;
	}

	point.hc1 = hc1;
	point.hc2 = hc2;
	point.isEdgePointCircle = false;

	eventQueue->emplace(point);
}

void calculateIntersectionPoints(struct Circle* circle_1, struct Circle* circle_2, std::unordered_set<Point, MyHashFunction>* intersectionPoints, std::multiset<Point, testComp>* eventQueue, bool optimize)
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
		processPointForHalfCircles(circle_1, circle_2, intersection_point1, eventQueue);
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
	processPointForHalfCircles(circle_1,circle_2, intersection_point1,eventQueue);
	intersectionPoints->insert(intersection_point1);
	if (!dontAdd2) {
		processPointForHalfCircles(circle_1, circle_2, intersection_point2, eventQueue);
		intersectionPoints->insert(intersection_point2);
	}
	//std::cout << "Intersection X, Y: " << intersection_point1.x << "," << intersection_point1.y << std::endl;
	//std::cout << "Intersection X, Y: " << intersection_point2.x << "," << intersection_point2.y << std::endl;
}

void calculateIntersectionPoints(struct HalfCircle* c1, struct HalfCircle* c2, InputInfo *info, double currentX) {
	calculateIntersectionPoints(c1->parent, c2->parent, &info->intersectionPoints,&info->testQueue, true);
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

int BSearch(std::vector<Point>* vec, Point* value, int start, int end) {
	while (start <= end) {
		int index = (start + end) / 2;
		if (vec->at(index).x < value->x) {
			start = index + 1;
		}
		else if (vec->at(index).x > value->x) {
			end = index - 1;
		}
		else {
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
						//calculateIntersectionPoints(lower->sc, node2->sc, &info, p->x);
						calculateIntersectionPoints(lower->sc->parent, node2->sc->parent, &info.intersectionPoints, &info.testQueue, true);
					}
					if (upper != nullptr) {
						//calculateIntersectionPoints(node1->sc, upper->sc, &info, p->x);
						calculateIntersectionPoints(node1->sc->parent, upper->sc->parent, &info.intersectionPoints, &info.testQueue, true);
					}

					/*for (size_t i = 0; i < T->size(); i++)
					{
						if (T->at(i).sc->status_pos != i) {
							int YOLO = -1;
						}
					}*/
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
					//calculateIntersectionPoints(lower1->sc, upper1->sc, &info, p->x);
					calculateIntersectionPoints(lower1->sc->parent, upper1->sc->parent, &info.intersectionPoints, &info.testQueue, true);
				}

				int pos2 = node2.sc->status_pos;
				auto lower2 = !(pos2 - 1 < 0) ? &T->at(pos2 - 1) : nullptr;
				auto upper2 = !(pos2 + 1 >= T->size()) ? &T->at(pos2 + 1) : nullptr;

				int lowest = pos < pos2 ? pos : pos2;

				if (upper2 != nullptr && lower2 != nullptr) {
					//calculateIntersectionPoints(lower2->sc, upper2->sc, &info, p->x);
					calculateIntersectionPoints(lower2->sc->parent, upper2->sc->parent, &info.intersectionPoints, &info.testQueue, true);
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

				/*for (size_t i = 0; i < T->size(); i++)
				{
					if (T->at(i).sc->status_pos != i) {
						int YOLO = -1;
					}
				}*/
			}
		}
		else if (p->isSnijpunt && !p->isRaakpunt) {
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
				//calculateIntersectionPoints(upper->sc, T->at(pos2).sc, &info, p->x);
				calculateIntersectionPoints(upper->sc->parent, T->at(pos2).sc->parent, &info.intersectionPoints, &info.testQueue, true);
			}
			if (lower != nullptr) {
				//calculateIntersectionPoints(T->at(pos1).sc, lower->sc, &info, p->x);
				calculateIntersectionPoints(T->at(pos1).sc->parent, lower->sc->parent, &info.intersectionPoints, &info.testQueue, true);

			}

			/*for (size_t i = 0; i < T->size(); i++)
			{
				if (T->at(i).sc->status_pos != i) {
					int YOLO = -1;
				}
			}*/
		}
		info.testQueue.erase(p);
	}
	auto end = std::chrono::system_clock::now();
	info.milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	info.nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
}