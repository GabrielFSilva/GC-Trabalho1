// **********************************************************************
// PUCRS/Escola Politécnica - Mestrado
// COMPUTAÇÃO GRÁFICA
//
// Aluno:
// Gabriel Fonseca Silva
//
// Professor:
// Marcio Sarroglia Pinho
// pinho@pucrs.br
// **********************************************************************


#define PI 3.14159265
#define POLY_COUNT 50
#define POINTS_COUNT 100
#define POINT_ROWS 10
#define SLAB_COUNT 16
#define WINDOW_POS_X 50
#define WINDOW_POS_Y 5
#define WINDOW_SIZE_X 1366
#define WINDOW_SIZE_Y 768
#define ORTHO_SIZE_X 16
#define ORTHO_SIZE_Y 9

#include <array>
#include <memory>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <windows.h>
#include <gl/glut.h>

#include "Polygon.h"
#include "Slab.h"
#include "earcut.hpp"


// EarClipping Setup
using namespace std;
using Coord = float;
using N = uint32_t;
using Point = std::array<Coord, 2>;

std::vector<std::vector<Point>> earClippingVector;
std::vector<N> indices;
// ---------

static DWORD last_idle_time;

/*#ifdef WIN64
#include <windows.h>
#include <glut.h>
static DWORD last_idle_time;
#else
#include <sys/timec.h>
static struct timeval last_idle_time;
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <glut.h>
#endif
*/

vector<PolygonR> polygons;
vector<PolygonR> triangles;
vector<Slab> slabs;

PointR referencePoint, pontoRefBorderX;
vector<PointR> collisionPoints;

int collisionMode = 0;
bool useManyCollisionPoints = false;
bool useSlabs = false;
bool useTriangles = false;
bool mouseOverMode = true;
bool drawAABB = false;
bool drawSlabs = false;

void CreateSlabs();
void CreateTrianglesFromPolygons();
void DetectCollision(PointR p_point);
void DetectCollision(vector<int> indexes, PointR p_point);
bool DetectCollisionAABB(PolygonR poly, PointR point);
bool DetectCollisionLineCrossing(PolygonR poly, PointR point);

void animate()
{
	static float dt;
	static float AccumTime = 0;

#ifdef _WIN32
	DWORD time_now;
	time_now = GetTickCount();
	dt = (float)(time_now - last_idle_time) / 1000.0;
#else
	// Figure out time elapsed since last call to idle function
	struct timeval time_now;
	gettimeofday(&time_now, NULL);
	dt = (float)(time_now.tv_sec - last_idle_time.tv_sec) +
		1.0e-6*(time_now.tv_usec - last_idle_time.tv_usec);
#endif
	AccumTime += dt;
	if (AccumTime >= 3) // imprime o FPS a cada 3 segundos
	{
		cout << 1.0 / dt << " FPS" << endl;
		AccumTime = 0;
	}
	// cout << "AccumTime: " << AccumTime << endl;
	// Anima cubos
	//AngY++;
	// Salva o tempo para o pr—ximo ciclo de rendering
	last_idle_time = time_now;

	//if  (GetAsyncKeyState(32) & 0x8000) != 0)
	//  cout << "Espaco Pressionado" << endl;

	// Redesenha
	glutPostRedisplay();
}

void CreatePolygons()
{
	polygons.clear();
	for (int i = 0; i < POLY_COUNT; i++)
	{
		PolygonR *__la = new PolygonR(RandomRange(8, 21), RandomRange(1.0f, 15.0f), RandomRange(1.0f, 8.0f), 0.2f, 0.6f);
		polygons.push_back(*__la);
	}
	CreateTrianglesFromPolygons();
	CreateSlabs();
}

void CreateTrianglesFromPolygons()
{
	triangles.clear();
	vector<PointR> newTriangleVertices;
	PolygonR *__poly;
	for (int i = 0; i < polygons.size(); i ++)
	{
		earClippingVector.clear();
		earClippingVector.push_back(polygons[i].VerticesToPointVector());
		indices = mapbox::earcut<N>(earClippingVector);

		for (int j = 0; j < indices.size(); j += 3)
		{
			newTriangleVertices.clear();
			newTriangleVertices.push_back(polygons[i].vertices[indices[j]]);
			newTriangleVertices.push_back(polygons[i].vertices[indices[j + 1]]);
			newTriangleVertices.push_back(polygons[i].vertices[indices[j + 2]]);
			__poly = new PolygonR(newTriangleVertices, polygons[i].colorR, polygons[i].colorG, polygons[i].colorB);
			triangles.push_back(*__poly);
		}
	}
}

int GetSlabByXPosition(float x)
{
	return max(0, min(floor(x / ((float)ORTHO_SIZE_X / (float)SLAB_COUNT)), SLAB_COUNT - 1));
}

void CreateSlabs()
{
	slabs.clear();
	for (int i = 0; i < SLAB_COUNT; i++)
	{
		Slab *__s = new Slab();
		__s->xMin = (float)i * ((float)ORTHO_SIZE_X / (float)SLAB_COUNT);
		__s->xMax = ((float)(i + 1)) * ((float)ORTHO_SIZE_X / (float)SLAB_COUNT);
		slabs.push_back(*__s);
	}
	PolygonR __poly;
	for (int i = 0; i < POLY_COUNT; i++)
	{
		__poly = polygons[i];
		for (int j = 0; j < SLAB_COUNT; j++)
		{
			if ((__poly.min.x >= slabs[j].xMin && __poly.min.x < slabs[j].xMax) ||
				(__poly.max.x >= slabs[j].xMin && __poly.max.x < slabs[j].xMax) ||
				(__poly.min.x <= slabs[j].xMin && __poly.max.x > slabs[j].xMax))
			{
				slabs[j].polygonIndexes.push_back(i);
			}
		}
	}
	for (int i = 0; i < triangles.size(); i++)
	{
		__poly = triangles[i];
		for (int j = 0; j < SLAB_COUNT; j++)
		{
			if ((__poly.min.x >= slabs[j].xMin && __poly.min.x < slabs[j].xMax) ||
				(__poly.max.x >= slabs[j].xMin && __poly.max.x < slabs[j].xMax) ||
				(__poly.min.x <= slabs[j].xMin && __poly.max.x > slabs[j].xMax))
			{
				slabs[j].triangleIndexes.push_back(i);
			}
		}
	}
}

void ClearCollisions()
{
	for (int i = 0; i < polygons.size(); i++)
	{
		polygons[i].colliding = false;
		polygons[i].insideAABB = false;
	}
	for (int i = 0; i < triangles.size(); i++)
	{
		triangles[i].colliding = false;
		triangles[i].insideAABB = false;
	}
}

void init(void)
{
	glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
	//srand(time(0));
	srand(0); // Seed 0 for testing and benchmarks
	CreatePolygons();

	// Multiple collision points setup
	collisionPoints.clear();
	PointR __p;
	for (int i = 0; i < POINT_ROWS; i++)
	{
		for (int j = 0; j < POINTS_COUNT/POINT_ROWS; j++)
		{
			__p.x = (j * ((float)ORTHO_SIZE_X / (float)POINT_ROWS)) + (0.5f * ((float)ORTHO_SIZE_X / (float)POINT_ROWS));
			__p.y = (i * ((float)ORTHO_SIZE_Y / (float)POINT_ROWS)) + (0.5f * ((float)ORTHO_SIZE_Y / (float)POINT_ROWS));
			collisionPoints.push_back(__p);
		}
	}
}

void reshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(0, ORTHO_SIZE_X, 0, ORTHO_SIZE_Y, 0, 1);
}

void DrawPolygons(vector<PolygonR> v, float lineWidth = 1.0f)
{
	glLineWidth(lineWidth);
	glBegin(GL_LINES);
	int next = 0;
	for (int i = 0; i < v.size(); i++)
	{
		if (v[i].colliding || (v[i].insideAABB && !drawAABB))
			glColor3f(0.0f, 1.0f, 0.0f);
		else
			glColor3f(v[i].colorR, v[i].colorG, v[i].colorB);
		for (int j = 0; j < v[i].vertices.size(); j++)
		{
			next = j + 1;
			if (j == v[i].vertices.size() - 1)
				next = 0;
			glVertex2f(v[i].vertices[j].x, v[i].vertices[j].y);
			glVertex2f(v[i].vertices[next].x, v[i].vertices[next].y);
		}
	}
	glEnd();
}

void DrawAABBs(vector<PolygonR> v, float lineWidth = 1.0f)
{
	glLineWidth(lineWidth);
	glBegin(GL_QUADS);
	for (int i = 0; i < v.size(); i++)
	{
		if (v[i].insideAABB)
			glColor3f(0.0f, 1.0f, 0.0f);
		else
			glColor3f(v[i].colorR, v[i].colorG, v[i].colorB);
		glVertex2f(v[i].min.x, v[i].min.y);
		glVertex2f(v[i].max.x, v[i].min.y);
		glVertex2f(v[i].max.x, v[i].max.y);
		glVertex2f(v[i].min.x, v[i].max.y);
	}
	glEnd();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(0, ORTHO_SIZE_X, 0, ORTHO_SIZE_Y, 0, 1);

	if (useSlabs)
	{
		if (useTriangles)
		{
			if (useManyCollisionPoints)
			{
				for (int i = 0; i < collisionPoints.size(); i++)
					DetectCollision(slabs[GetSlabByXPosition(collisionPoints[i].x)].triangleIndexes, collisionPoints[i]);
			}
			else
				DetectCollision(slabs[GetSlabByXPosition(referencePoint.x)].triangleIndexes, referencePoint);
		}
		else
		{
			if (useManyCollisionPoints)
			{
				for (int i = 0; i < collisionPoints.size(); i++)
					DetectCollision(slabs[GetSlabByXPosition(collisionPoints[i].x)].polygonIndexes, collisionPoints[i]);
			}
			else
				DetectCollision(slabs[GetSlabByXPosition(referencePoint.x)].polygonIndexes, referencePoint);
		}
	}
	else
	{
		if (useManyCollisionPoints)
		{ 
			for(int i = 0; i < collisionPoints.size(); i++)
				DetectCollision(collisionPoints[i]);
		}
		else
			DetectCollision(referencePoint);
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Draw all Polygons
	if (useTriangles)
		DrawPolygons(triangles, 2.0f);
	else
		DrawPolygons(polygons, 3.0f);
	
	// Draw AABBs
	if (drawAABB)
	{
		if (useTriangles)
			DrawAABBs(triangles, 1.0f);
		else
			DrawAABBs(polygons, 2.0f);
	}

	// Draw Slabs
	if (drawSlabs)
	{
		glLineWidth(1.0f);
		glBegin(GL_LINES);
		glColor3f(1, 1, 1);
		for (int i = 0; i < slabs.size(); i++)
		{
			glVertex2f(slabs[i].xMin, 0.0f);
			glVertex2f(slabs[i].xMin, ORTHO_SIZE_Y);
			glVertex2f(slabs[i].xMax, 0.0f);
			glVertex2f(slabs[i].xMax, ORTHO_SIZE_Y);
		}
		glEnd();
	}

	// Reference Point
	glLineWidth(3.0f);
		glColor3f(1, 1, 1);
	glBegin(GL_LINES);
		glVertex2f(0, referencePoint.y);
		glVertex2f(referencePoint.x, referencePoint.y);
	glEnd();
	glPointSize(10.0f);
	glBegin(GL_POINTS);
		glVertex2f(referencePoint.x, referencePoint.y);
	glEnd();

	// HUD
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(0.8f, 0.9f, 1.0f);
	glBegin(GL_QUADS);
		glVertex2f(0.0f, ORTHO_SIZE_Y); // Top-left
		glVertex2f(ORTHO_SIZE_X, ORTHO_SIZE_Y); // Top-right
		glVertex2f(ORTHO_SIZE_X, ORTHO_SIZE_Y * 0.95f); // Bottom-right
		glVertex2f(0.0f, ORTHO_SIZE_Y * 0.95f); // Bottom-left
	glEnd();

	// Collision Mode
	if (collisionMode == 0)
		RenderString(0.1f, 8.7f, "-NO COLLISION", 0.0f, 0.0f, 0.2f);
	else if (collisionMode == 1)
		RenderString(0.1f, 8.7f, "-ONLY LINE CROSSING", 0.0f, 0.0f, 0.2f);
	else if (collisionMode == 2)
		RenderString(0.1f, 8.7f, "-ONLY AABB", 0.0f, 0.0f, 0.2f);
	else
		RenderString(0.1f, 8.7f, "-AABB W/ LINE CROSSING", 0.0f, 0.0f, 0.2f);
	if (useTriangles)
		RenderString(3.25f, 8.7f, "-TRIANGLES", 0.0f, 0.0f, 0.2f);
	else
		RenderString(3.25f, 8.7f, "-POLYGONS", 0.0f, 0.0f, 0.2f);
	// Using Slabs
	if (useSlabs)
		RenderString(5.0f, 8.7f, "-SLABS ON", 0.0f, 0.0f, 0.2f);
	else
		RenderString(5.0f, 8.7f, "-SLABS OFF", 0.0f, 0.0f, 0.2f);
	// Mouse Mode
	if (mouseOverMode)
		RenderString(7.0f, 8.7f, "-MOUSE OVER", 0.0f, 0.0f, 0.2f);
	else 
		RenderString(7.0f, 8.7f, "-MOUSE CLICK", 0.0f, 0.0f, 0.2f);
	// Render 
	if (drawAABB)
		RenderString(9.0f, 8.7f, "-SHOW AABB", 0.0f, 0.0f, 0.2f);
	else
		RenderString(9.0f, 8.7f, "-HIDE AABB", 0.0f, 0.0f, 0.2f);
	if (drawSlabs)
		RenderString(11.0f, 8.7f, "-SHOW SLABS", 0.0f, 0.0f, 0.2f);
	else
		RenderString(11.0f, 8.7f, "-HIDE SLABS", 0.0f, 0.0f, 0.2f);
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case '1':
		collisionMode++;
		if (collisionMode == 4)
		{
			collisionMode = 0;
		}
		ClearCollisions();
		break;
	case '2':
		useTriangles = !useTriangles;
		ClearCollisions();
		break;
	case '3':
		useSlabs = !useSlabs;
		ClearCollisions();
		break;
	case '4':
		mouseOverMode = 1 - mouseOverMode;
		break;
	case '5':
		drawAABB = !drawAABB;
		break;
	case '6':
		drawSlabs = !drawSlabs;
		break;
	case'w':
		referencePoint.y += 0.5f;
		break;
	case's':
		referencePoint.y -= 0.5f;
		break;
	case'a':
		referencePoint.x -= 0.5f;
		break;
	case'd':
		referencePoint.x += 0.5f;
		break;
	default:
		break;
	}
}

void SpecialKeysControl(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		glutFullScreen();
		break;
	case GLUT_KEY_DOWN:
		glutPositionWindow(WINDOW_POS_X, WINDOW_POS_Y);
		glutReshapeWindow(WINDOW_SIZE_X, WINDOW_SIZE_Y);
		break;
	case GLUT_KEY_LEFT:
		CreatePolygons();
		break;
	default:
		break;
	}
}

void mouseClick(int button, int state, int x, int y)
{
	if (mouseOverMode == 0 && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		referencePoint.x = (float)ORTHO_SIZE_X * (float)x / (float)WINDOW_SIZE_X;
		referencePoint.y = ORTHO_SIZE_Y - ((float)ORTHO_SIZE_Y * (float)y / (float)WINDOW_SIZE_Y);
		pontoRefBorderX.y = referencePoint.y;
	}
}

void mouseMove(int x, int y)
{
	if (mouseOverMode == 1) {
		referencePoint.x = (float)ORTHO_SIZE_X * (float)x / (float)WINDOW_SIZE_X;
		referencePoint.y = ORTHO_SIZE_Y - ((float)ORTHO_SIZE_Y * (float)y / (float)WINDOW_SIZE_Y);
		pontoRefBorderX.y = referencePoint.y;
	}
}

int  main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutInitWindowPosition(WINDOW_POS_X, WINDOW_POS_Y);

	// Set window size
	glutInitWindowSize((WINDOW_SIZE_X), WINDOW_SIZE_Y);

	// Create window
	glutCreateWindow("Trabalho 1 - Gabriel Fonseca Silva");

	glOrtho(0, ORTHO_SIZE_X, 0, ORTHO_SIZE_Y, 0, 1);
	// executa algumas inicializações
	init();

	glutDisplayFunc(display);
	glutIdleFunc(animate);

	// Window reshape function
	glutReshapeFunc(reshape);

	// Keyboard callback function
	glutKeyboardFunc(keyboard);

	// Special keys keyboard callback function
	glutSpecialFunc(SpecialKeysControl);

	// Mouse callback function
	glutMouseFunc(mouseClick);
	glutPassiveMotionFunc(mouseMove);
	// Main Code
	glutMainLoop();

	return 0;
}

void DetectCollision(PointR p_point)
{
	if (collisionMode == 0)
		return;
	vector<PolygonR> *__v;
	if (useTriangles)
		__v = &triangles;
	else
		__v = &polygons;
	for (int i = 0; i < __v->size(); i++)
	{
		if (collisionMode == 1 || collisionMode == 3)
		{
			__v->at(i).colliding = DetectCollisionLineCrossing(__v->at(i), p_point);
		}
		if (collisionMode == 2 || collisionMode == 3)
		{
			__v->at(i).insideAABB = DetectCollisionAABB(__v->at(i), p_point);
		}
	}
}

void DetectCollision(vector<int> p_indexes, PointR p_point)
{
	if (collisionMode == 0)
		return;
	vector<PolygonR> *__v;
	if (useTriangles)
		__v = &triangles;
	else
		__v = &polygons;
	for (int i = 0; i < p_indexes.size(); i++)
	{
		if (collisionMode == 1 || collisionMode == 3)
		{
			__v->at(p_indexes[i]).colliding = DetectCollisionLineCrossing(__v->at(p_indexes[i]), p_point);
		}
		if (collisionMode == 2 || collisionMode == 3)
		{
			__v->at(p_indexes[i]).insideAABB = DetectCollisionAABB(__v->at(p_indexes[i]), p_point);
		}
	}
}

bool DetectCollisionAABB(PolygonR poly, PointR point)
{
	return (point.x >= poly.min.x &&
		point.x <= poly.max.x &&
		point.y >= poly.min.y &&
		point.y <= poly.max.y) ? true : false;
}

bool DetectCollisionLineCrossing(PolygonR poly, PointR point)
{
	int __tempCount = 0;
	int __nextVertex = 0;
	
	for (int j = 0; j < poly.vertices.size(); j++)
	{
		__nextVertex = j + 1;
		if (j == poly.vertices.size() - 1)
			__nextVertex = 0;
		__tempCount += existeIntersec(pontoRefBorderX, referencePoint, poly.vertices[j], poly.vertices[__nextVertex]) == 1;
	}
	return __tempCount % 2 == 1 ? true : false;
}

