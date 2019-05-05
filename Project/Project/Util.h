//#pragma once
#include <stdlib.h>
#include <GL/glut.h>

static float RandomRange(float floor, float ceiling)
{
	return (ceiling - floor) * (((float)rand()) / (float)RAND_MAX) + floor;
};

static int RandomRange(int floor, int ceiling)
{
	return (rand() % (ceiling - floor)) + floor;
};

static int Lado(Point P1, Point P2, Point P)
{
	Point V1, V2;
	V1.x = P2.x - P1.x;
	V1.y = P2.y - P1.y;
	V2.x = P.x - P1.x;
	V2.y = P.y - P1.y;
	//V1.imprime("V1");
	//V2.imprime("V2");
	float k = V1.x * V2.y - V1.y * V2.x;
	if (fabs(k) < 0.0001)
		return 0;
	if (k > 0)
		return -1;
	else return 1;
}
static int existeIntersec(Point P1, Point P2, Point PA, Point PB)
{
	int La, Lb, L1, L2;
	La = Lado(P1, P2, PA);
	//cout << "La = " << La << endl;
	Lb = Lado(P1, P2, PB);
	//cout << "Lb = " << Lb << endl;
	if (La == Lb) return 0;
	L1 = Lado(PA, PB, P1);
	L2 = Lado(PA, PB, P2);
	if (L1 == L2) return 0;
	return 1;
}



static void RenderString(float x, float y, string text, float r = 1.0f, float g = 1.0f, float b = 1.0f)
{
	char *c;
	glColor3f(r, g, b);
	glRasterPos2f(x, y);
	for (int i = 0; i < text.length(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
	}
}