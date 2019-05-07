#pragma once
#include <list>
#include <array>
#include <vector>
#include <random>
#include <math.h>
#include <cmath>
#include "Pointr.h"
#include "Util.h"

using namespace std;

#define PI 3.14159265

class PolygonR
{
public:
	PolygonR();
	PolygonR(int vertexCount, float centerX, float centerY, float minRange, float maxRange);
	PolygonR(vector<PointR> p_vertexList, float r, float g, float b);
	~PolygonR();

	vector<PointR> vertices;
	vector<int> nextVertex;
	bool colliding;
	bool insideAABB;

	PointR min;
	PointR max;

	float colorR;
	float colorG;
	float colorB;
	vector<std::array<float, 2>> VerticesToPointVector();
private:
	void CreateRandomVertices(int vertexCount, float centerX, float centerY, float minRange, float maxRange);
	void GetRandomColor();
	void CalculateAABB();
};

