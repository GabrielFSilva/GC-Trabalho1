#pragma once
#include <vector>
#include "Polygon.h"

class Slab
{
public:
	Slab();
	~Slab();
	vector<int> polygonIndexes;
	vector<int> triangleIndexes;
	float xMin;
	float xMax;
};

