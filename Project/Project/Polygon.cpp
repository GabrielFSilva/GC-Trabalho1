#include "Polygon.h"

PolygonR::PolygonR()
{
	GetRandomColor();
}

PolygonR::PolygonR(int vertexCount, float centerX, float centerY, float minRange, float maxRange)
{
	CreateRandomVertices(vertexCount, centerX, centerY, minRange, maxRange);
	GetRandomColor();
	CalculateAABB();
}

PolygonR::PolygonR(vector<Point> p_vertexList)
{
	vertices = p_vertexList;
	GetRandomColor();
	CalculateAABB();
}


PolygonR::~PolygonR()
{
	vertices.clear();
}

void PolygonR::CreateRandomVertices(int vertexCount, float centerX, float centerY, float minRange, float maxRange)
{
	if (vertexCount < 3)
	{
		perror("Not enought vertices");
		exit(1);
	}
	Point __tempPoint;
	vertices.clear();
	float __interval = 360.0f / vertexCount;
	float __toRadians = PI / 180.0f;
	float __range = maxRange - minRange;
	float __offset;
	for (int i = 0; i < vertexCount; i++)
	{
		__offset = RandomRange(minRange, maxRange);
		__tempPoint.x = (cos(i * __interval * __toRadians) * (__offset)) + centerX;
		__tempPoint.y = (sin(i * __interval * __toRadians) * (__offset)) + centerY;

		vertices.push_back(__tempPoint);
	}
}

void PolygonR::GetRandomColor()
{
	colorR = RandomRange(0.6f, 1.0f);
	colorG = 0.4f;
	colorB = RandomRange(0.6f, 1.0f);
}

void PolygonR::CalculateAABB()
{
	min.x = INFINITY;
	min.y = INFINITY;
	max.x = -INFINITY;
	max.y = -INFINITY;

	for (int i = 0; i < vertices.size(); i++)
	{
		if (vertices[i].x > max.x)
			max.x = vertices[i].x;
		else if (vertices[i].x < min.x)
			min.x = vertices[i].x;
		if (vertices[i].y > max.y)
			max.y = vertices[i].y;
		else if (vertices[i].y < min.y)
			min.y = vertices[i].y;
	}
}
