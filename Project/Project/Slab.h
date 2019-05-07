#pragma once
#include <vector>
#include "PolygonR.h"

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

