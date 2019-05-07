#pragma once
#include <iostream>
using namespace std;

typedef struct PointR
{
	float x, y;
	void imprime(const char *s)
	{
		cout << s << ": " << x << "," << y << endl;
	}
} Ponto;

static PointR Meio(PointR P1, PointR P2)
{
	PointR P;
	P.x = (P1.x + P2.x) / 2.0;
	P.y = (P1.y + P2.y) / 2.0;
	return P;
}