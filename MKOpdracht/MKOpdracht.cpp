#include <stdio.h>
#include <iostream>

//For reading files
#include <fstream>

//Simple data holder, +/- list
#include <vector>

struct Point {
	double x;
	double y;
};

struct CircleInfo {
	Point center;
	double radius;
};

struct InputInfo {
	int algorithm;
	int num_circles;
	std::vector<CircleInfo> circles;
};

//This is a function prototype, to be able to use it later.
bool openAndReadFile(char*& path);

int main(int argc, char **argv)
{
	//i=0 would be the executable itself, so we don't want it
	for (auto i = 1; i < argc; i++)
	{
		char* path = argv[i]; //char* c is in this case a /0 terminated string
		openAndReadFile(path);
	}
}

bool openAndReadFile(char*& path) {
	std::ifstream file;
	file.open(path);

	if (file.is_open()) {
		int algorithm;
		file >> algorithm;

		int numCircles;
		file >> numCircles;

		std::cout << "Using algorithm " << algorithm << " on " << numCircles << " circles." << std::endl;

		InputInfo inputInfo = { algorithm, numCircles, std::vector<CircleInfo>(numCircles) };

		double x, y, r;
		for (auto i = 0; i < numCircles; i++)
		{
			file >> x >> y >> r;
			Point circleCenter = { x,y };
			CircleInfo circleInfo = { circleCenter, r };
			inputInfo.circles[i] = circleInfo;
		}

		std::cout << "Processed input:" << std::endl;
		for (CircleInfo circleInfo : inputInfo.circles)
		{
			std::cout << "Center X: " << circleInfo.center.x << ", Center Y: " << circleInfo.center.y << ", Radius: " << circleInfo.radius << std::endl;
		}
	}

	return true;
}


