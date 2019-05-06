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
#define WINDOW_POS_X 50
#define WINDOW_POS_Y 5
#define WINDOW_SIZE_X 1366
#define WINDOW_SIZE_Y 768
#define ORTHO_SIZE_X 16
#define ORTHO_SIZE_Y 9

#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <ctime>
#include <windows.h>
#include <gl/glut.h>

#include "Polygon.h"

using namespace std;
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

Point referencePoint, pontoRefBorderX;

int collisionMode = 0;
bool mouseOverMode = true;
bool showAABB = true;

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

void DetectCollision();
bool DetectCollisionAABB(PolygonR poly, Point point);
bool DetectCollisionLineCrossing(PolygonR poly, Point point);

void CreatePolygons()
{
	polygons.clear();
	for (int i = 0; i < POLY_COUNT; i++)
	{
		PolygonR *__la = new PolygonR(RandomRange(9, 20), RandomRange(1.0f, 15.0f), RandomRange(1.0f, 8.0f), 0.2f, 0.6f);
		polygons.push_back(*__la);
	}
}

void init(void)
{
	glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
	srand(time(0));
	CreatePolygons();
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

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(0, ORTHO_SIZE_X, 0, ORTHO_SIZE_Y, 0, 1);

	DetectCollision();
	

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	

	glBegin(GL_LINES);
	int next = 0;
	glColor3f(0.7f, 0.1f, 0.7f);
	
	// Draw all Polygons
	for (int i = 0; i < polygons.size(); i++)
	{
		if (polygons[i].colliding)
			glColor3f(0.0f, 1.0f, 0.0f);
		else
			glColor3f(polygons[i].colorR, polygons[i].colorG, polygons[i].colorB);
		for (int j = 0; j < polygons[i].vertices.size(); j++)
		{
			next = j + 1;
			if (j == polygons[i].vertices.size() - 1)
				next = 0;
			glVertex2f(polygons[i].vertices[j].x, polygons[i].vertices[j].y);
			glVertex2f(polygons[i].vertices[next].x, polygons[i].vertices[next].y);
		}
	}
	glEnd();

	if (showAABB)
	{
		glLineWidth(1.0f);
		glBegin(GL_QUADS);
		for (int i = 0; i < polygons.size(); i++)
		{
			if (polygons[i].insideAABB)
				glColor3f(0.0f, 1.0f, 0.0f);
			else
				glColor3f(polygons[i].colorR, polygons[i].colorG, polygons[i].colorB);
			glVertex2f(polygons[i].min.x, polygons[i].min.y);
			glVertex2f(polygons[i].max.x, polygons[i].min.y);
			glVertex2f(polygons[i].max.x, polygons[i].max.y);
			glVertex2f(polygons[i].min.x, polygons[i].max.y);
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

	// HUD Draw
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
	// Mouse Mode
	if (mouseOverMode)
		RenderString(3.0f, 8.7f, "-MOUSE OVER", 0.0f, 0.0f, 0.2f);
	else 
		RenderString(3.0f, 8.7f, "-MOUSE CLICK", 0.0f, 0.0f, 0.2f);
	// Render 
	if (showAABB)
		RenderString(5.0f, 8.7f, "-SHOW AABB", 0.0f, 0.0f, 0.2f);
	else
		RenderString(5.0f, 8.7f, "-HIDE AABB", 0.0f, 0.0f, 0.2f);


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
		for (int i = 0; i < polygons.size(); i++)
		{
			polygons[i].colliding = false;
			polygons[i].insideAABB = false;
		}
		break;
	case '2':
		mouseOverMode = 1 - mouseOverMode;
		break;
	case '3':
		showAABB = !showAABB;
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

void DetectCollision()
{
	if (collisionMode == 0)
		return;
	else if (collisionMode == 1) // LINE CROSSING
	{
		for (int i = 0; i < polygons.size(); i++)
		{
			polygons[i].colliding = DetectCollisionLineCrossing(polygons[i], referencePoint);
		}
	}
	else if (collisionMode == 2) // AABB
	{
		for (int i = 0; i < polygons.size(); i++)
		{
			polygons[i].insideAABB = DetectCollisionAABB(polygons[i], referencePoint);
		}
	}
	else if (collisionMode == 3) // AABB W/ LINE CROSSING
	{
		for (int i = 0; i < polygons.size(); i++)
		{
			polygons[i].insideAABB = DetectCollisionAABB(polygons[i], referencePoint);
			polygons[i].colliding = DetectCollisionLineCrossing(polygons[i], referencePoint);
		}
	}
}

bool DetectCollisionAABB(PolygonR poly, Point point)
{
	return (point.x >= poly.min.x &&
		point.x <= poly.max.x &&
		point.y >= poly.min.y &&
		point.y <= poly.max.y) ? true : false;
}

bool DetectCollisionLineCrossing(PolygonR poly, Point point)
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

