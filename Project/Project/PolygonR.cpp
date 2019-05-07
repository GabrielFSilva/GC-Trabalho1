#include "PolygonR.h"

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

PolygonR::PolygonR(vector<PointR> p_vertexList, float r, float g, float b)
{
	vertices = p_vertexList;
	colorR = r;
	colorG = g;
	colorB = b;
	CalculateAABB();
}


PolygonR::~PolygonR()
{
	vertices.clear();
}

vector<std::array<float, 2>> PolygonR::VerticesToPointVector()
{
	vector<std::array<float, 2>> __v;
	for (int i = 0; i < vertices.size(); i++)
	{
		__v.push_back({ vertices[i].x, vertices[i].y });
	}

	return __v;
}

void PolygonR::CreateRandomVertices(int vertexCount, float centerX, float centerY, float minRange, float maxRange)
{
	if (vertexCount < 3)
	{
		perror("Not enought vertices");
		exit(1);
	}
	PointR __tempPoint;
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
		if (vertices[i].x < min.x)
			min.x = vertices[i].x;
		if (vertices[i].y > max.y)
			max.y = vertices[i].y;
		if (vertices[i].y < min.y)
			min.y = vertices[i].y;
	}
}
