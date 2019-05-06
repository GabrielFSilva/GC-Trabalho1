#pragma once
#include <list>
#include <vector>
#include <random>
#include <math.h>
#include <cmath>
#include "Point.h"
#include "Util.h"

using namespace std;

#define PI 3.14159265

class PolygonR
{
public:
	PolygonR();
	PolygonR(int vertexCount, float centerX, float centerY, float minRange, float maxRange);
	PolygonR(vector<Point> p_vertexList);
	~PolygonR();

	vector<Point> vertices;
	vector<int> nextVertex;
	bool colliding;
	bool insideAABB;

	Point min;
	Point max;

	float colorR;
	float colorG;
	float colorB;
private:
	void CreateRandomVertices(int vertexCount, float centerX, float centerY, float minRange, float maxRange);
	void GetRandomColor();
	void CalculateAABB();
};

