// Versuch1b.cpp
// Ausgangssoftware des 1. Praktikumsversuchs 
// zur Vorlesung Echtzeit-3D-Computergrahpik
// von Prof. Dr. Alfred Nischwitz
// Programm umgesetzt mit der GLTools Library und Vertex-Arrays
#include <iostream>
#ifdef WIN32
#include <windows.h>
#endif
#include <GLTools.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>
#include <GLFrustum.h>
#include <math.h>
#include <math3d.h>
#include <GL/freeglut.h>
#include <AntTweakBar.h>

GLShaderManager shaderManager;
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLGeometryTransform transformPipeline;
GLFrustum viewFrustum;

// Definition der Kreiszahl 
#define GL_PI 3.1415f

// Rotationsgroessen
static float rotation[] = { 0, 0, 0, 0 };

// Flags fuer Schalter
bool bCull = false;
bool bOutline = false;
bool bDepth = true;

bool showCylinder = false;
bool showSphere = true;
bool showQuader = false;
bool showScene = false;

float cylinderHeight = 60;
float cylinderRadius = 15;
unsigned int cylinderTesselation = 18;

float sphereRadius = 30;
unsigned int sphereStacks = 10;
unsigned int sphereSectors = 20;

float quaderX = 50;
float quaderY = 50;
float quaderZ = 50;


//GUI
TwBar *bar;
void InitGUI()
{
	bar = TwNewBar("TweakBar");
	TwDefine(" TweakBar size='200 400'");
	TwAddVarRW(bar, "Model Rotation", TW_TYPE_QUAT4F, &rotation, "");
	TwAddVarRW(bar, "Depth Test?", TW_TYPE_BOOLCPP, &bDepth, "");
	TwAddVarRW(bar, "Culling?", TW_TYPE_BOOLCPP, &bCull, "");
	TwAddVarRW(bar, "Backface Wireframe?", TW_TYPE_BOOLCPP, &bOutline, "");
	//Hier weitere GUI Variablen anlegen. Für Farbe z.B. den Typ TW_TYPE_COLOR4F benutzen
	TwAddVarRW(bar, "Show Cylinder?", TW_TYPE_BOOLCPP, &showCylinder, "");
	TwAddVarRW(bar, "Cylinder Height", TW_TYPE_FLOAT, &cylinderHeight, "");
	TwAddVarRW(bar, "Cylinder Radius", TW_TYPE_FLOAT, &cylinderRadius,"");
	TwAddVarRW(bar, "Cylinder Tesselation", TW_TYPE_UINT32, &cylinderTesselation, "");
	TwAddVarRW(bar, "Show Sphere?", TW_TYPE_BOOLCPP, &showSphere, "");
	TwAddVarRW(bar, "Sphere Radius", TW_TYPE_FLOAT, &sphereRadius, "");
	TwAddVarRW(bar, "Sphere Stacks", TW_TYPE_UINT32, &sphereStacks, "");
	TwAddVarRW(bar, "Sphere Sectors", TW_TYPE_UINT32, &sphereSectors, "");
	TwAddVarRW(bar, "Show Quader?", TW_TYPE_BOOLCPP, &showQuader, "");
	TwAddVarRW(bar, "Quader width", TW_TYPE_FLOAT, &quaderX, "");
	TwAddVarRW(bar, "Quader height", TW_TYPE_FLOAT, &quaderY, "");
	TwAddVarRW(bar, "Quader length", TW_TYPE_FLOAT, &quaderZ, "");
	TwAddVarRW(bar, "Show Scene?", TW_TYPE_BOOLCPP, &showScene, "");
}

void CreateCylinder(float height, float radius, int tesselation)
{
	if (tesselation < 5)
		tesselation = 5;
	if (height < 1)
		height = 1;
	if (radius < 1)
		radius = 1;

	M3DVector3f* cylinderBottomVertices = new M3DVector3f[tesselation];
	M3DVector4f* cylinderBottomColors = new M3DVector4f[tesselation];
	M3DVector3f* cylinderTopVertices = new M3DVector3f[tesselation];
	M3DVector4f* cylinderTopColors = new M3DVector4f[tesselation];
	M3DVector3f* cylinderMantleVertices = new M3DVector3f[(tesselation - 1) * 2];
	M3DVector4f* cylinderMantleColors = new M3DVector4f[(tesselation - 1) * 2];

	m3dLoadVector3(cylinderBottomVertices[0], 0, 0, 0);
	m3dLoadVector4(cylinderBottomColors[0], 1, 0, 0, 1);
	m3dLoadVector3(cylinderTopVertices[0], 0, 0, height);
	m3dLoadVector4(cylinderTopColors[0], 1, 0, 0, 1);

	int iPivot = 1;
	int i = 1;
	int steps = tesselation - 2;
	float increment = (2.0f * GL_PI) / steps;

	for (int step = 0; step <= steps; step++)
	{
		float angle = step * increment;
		float x = radius * sin(-angle);
		float y = radius * cos(-angle);

		if ((iPivot % 2) == 0)
		{
			m3dLoadVector4(cylinderBottomColors[tesselation - i], 1, 0.8, 0.2, 1);
			m3dLoadVector4(cylinderTopColors[i], 1, 0.8, 0.2, 1);
		}
		else {
			m3dLoadVector4(cylinderBottomColors[tesselation - i], 0, 0.8, 0, 1);
			m3dLoadVector4(cylinderTopColors[i], 0, 0.8, 0, 1);
		}

		m3dLoadVector3(cylinderBottomVertices[i], x, y, 0);
		m3dLoadVector3(cylinderTopVertices[tesselation - i], x, y, height);

		m3dLoadVector3(cylinderMantleVertices[(i - 1) * 2], x, y, 0);
		m3dLoadVector3(cylinderMantleVertices[(i - 1) * 2 + 1], x, y, height);
		m3dLoadVector4(cylinderMantleColors[(i - 1) * 2], 1, 0.8, 0.2, 1);
		m3dLoadVector4(cylinderMantleColors[(i - 1) * 2 + 1], 0, 0.8, 0, 1);

		iPivot++;
		i++;
	}

	GLBatch cylinderBottom;
	GLBatch cylinderTop;
	GLBatch cylinderMantle;

	cylinderBottom.Begin(GL_TRIANGLE_FAN, tesselation);
	cylinderBottom.CopyVertexData3f(cylinderBottomVertices);
	cylinderBottom.CopyColorData4f(cylinderBottomColors);
	cylinderBottom.End();

	cylinderTop.Begin(GL_TRIANGLE_FAN, tesselation);
	cylinderTop.CopyVertexData3f(cylinderTopVertices);
	cylinderTop.CopyColorData4f(cylinderTopColors);
	cylinderTop.End();

	cylinderMantle.Begin(GL_TRIANGLE_STRIP, (tesselation - 1) * 2);
	cylinderMantle.CopyVertexData3f(cylinderMantleVertices);
	cylinderMantle.CopyColorData4f(cylinderMantleColors);
	cylinderMantle.End();

	cylinderBottom.Draw();
	cylinderTop.Draw();
	cylinderMantle.Draw();

	delete[] cylinderBottomVertices;
	delete[] cylinderBottomColors;
	delete[] cylinderTopVertices;
	delete[] cylinderTopColors;
	delete[] cylinderMantleVertices;
	delete[] cylinderMantleColors;
}

void CreateCuboidPart(int x, int y, int z, int offset_x, int offset_y, int offset_z) {

	const int val = 8;
	GLBatch corner;
	M3DVector3f bodenVertices[val];
	M3DVector4f bodenColors[val];
	int i = 0;

	m3dLoadVector3(bodenVertices[i], 0 + offset_x, 0 + offset_y, 0 + offset_z);
	m3dLoadVector4(bodenColors[i], 1, 0, 0, 1);
	i++;

	m3dLoadVector3(bodenVertices[i], x + offset_x, 0 + offset_y, 0 + offset_z);
	m3dLoadVector4(bodenColors[i], 1, 0.8, 0, 1);
	i++;

	m3dLoadVector3(bodenVertices[i], x + offset_x, y + offset_y, 0 + offset_z);
	m3dLoadVector4(bodenColors[i], 1, 0.8, 0.2, 1);
	i++;

	m3dLoadVector3(bodenVertices[i], 0 + offset_x, y + offset_y, 0 + offset_z);
	m3dLoadVector4(bodenColors[i], 1, 0.8, 0, 1);
	i++;

	m3dLoadVector3(bodenVertices[i], 0 + offset_x, y + offset_y, z + offset_z);
	m3dLoadVector4(bodenColors[i], 1, 0.8, 0.2, 1);
	i++;

	m3dLoadVector3(bodenVertices[i], 0 + offset_x, 0 + offset_y, z + offset_z);
	m3dLoadVector4(bodenColors[i], 1, 0.8, 0, 1);
	i++;

	m3dLoadVector3(bodenVertices[i], x + offset_x, 0 + offset_y, z + offset_z);
	m3dLoadVector4(bodenColors[i], 1, 0.8, 0.2, 1);
	i++;

	m3dLoadVector3(bodenVertices[i], x + offset_x, 0 + offset_y, 0 + offset_z);
	m3dLoadVector4(bodenColors[i], 1, 0.8, 0, 1);
	i++;

	corner.Begin(GL_TRIANGLE_FAN, val);
	corner.CopyVertexData3f(bodenVertices);
	corner.CopyColorData4f(bodenColors);
	corner.End();
	corner.Draw();

}

void CreateCuboid(float width, float height, float length) {
	CreateCuboidPart(width, height, length, 0, 0, 0);
	glFrontFace(GL_CCW);
	CreateCuboidPart(-width, -height, -length, width, height, length);
	glFrontFace(GL_CW);
}

void CreateSphere(float radius, int stacks, int sectors)
{
	if (radius < 1)
		radius = 1;
	if (stacks < 3)
		stacks = 3;
	if (sectors < 3)
		sectors = 3;

	float stackAngleIncrement = GL_PI / stacks;
	float sectorAngleIncrement = (2.0f * GL_PI) / sectors;

	for (int sector = 0; sector < sectors; sector++)
	{
		float sectorAngle = -sector * sectorAngleIncrement;

		M3DVector3f* sphereVertices = new M3DVector3f[2 * stacks];
		M3DVector4f* sphereColors = new M3DVector4f[2 * stacks];
		m3dLoadVector3(sphereVertices[0], 0, 0, radius);
		m3dLoadVector4(sphereColors[0], 1, 0, 0, 1);

		int i = 1;
		for (int stack = 1; stack < stacks; stack++)
		{
			float stackAngle = -stack * stackAngleIncrement;

			float x = (radius * cos((GL_PI / 2.0f) + stackAngle)) * cos(sectorAngle);
			float y = (radius * cos((GL_PI / 2.0f) + stackAngle)) * sin(sectorAngle);
			float z = radius * sin((GL_PI / 2.0f) + stackAngle);

			m3dLoadVector3(sphereVertices[i], x, y, z);
			m3dLoadVector4(sphereColors[i], 1, 0.8, 0.2, 1);

			float xn = (radius * cos((GL_PI / 2.0f) + stackAngle)) * cos(sectorAngle - sectorAngleIncrement);
			float yn = (radius * cos((GL_PI / 2.0f) + stackAngle)) * sin(sectorAngle - sectorAngleIncrement);

			m3dLoadVector3(sphereVertices[i + 1], xn, yn, z);
			m3dLoadVector4(sphereColors[i + 1], 0, 0.8, 0, 1);

			i = i + 2;
		}
		m3dLoadVector3(sphereVertices[i], 0, 0, -radius);
		m3dLoadVector4(sphereColors[i], 1, 0, 0, 1);

		GLBatch sphereSector;
		sphereSector.Begin(GL_TRIANGLE_STRIP, stacks * 2);
		sphereSector.CopyVertexData3f(sphereVertices);
		sphereSector.CopyColorData4f(sphereColors);
		sphereSector.End();
		sphereSector.Draw();

		delete[] sphereVertices;
		delete[] sphereColors;
	}
}

void DrawDumbbell(void) {
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Rotate(90, 0, 1, 0);
	modelViewMatrix.Translate(0, 0, -15);
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES,
		transformPipeline.GetModelViewProjectionMatrix());

	CreateCylinder(30, 3, 20);

	CreateSphere(8, 20, 20);

	modelViewMatrix.Translate(0, 0, 30);
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES,
		transformPipeline.GetModelViewProjectionMatrix());

	CreateSphere(8, 20, 20);

	modelViewMatrix.PopMatrix();
}

void DrawArmAndDumbbell(void) {
	CreateCylinder(30, 4, 20);

	modelViewMatrix.PushMatrix();
	modelViewMatrix.Translate(0, 0, 30);
	modelViewMatrix.Rotate(-40, 1, 0, 0);
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES,
		transformPipeline.GetModelViewProjectionMatrix());

	CreateCylinder(30, 4, 20);

	modelViewMatrix.Translate(0, 0, 30);

	DrawDumbbell();

	modelViewMatrix.PopMatrix();
}

void DrawBodybuilder(void) {
	CreateCuboid(30, 60, 20);

	modelViewMatrix.PushMatrix();
	modelViewMatrix.Translate(0, 45, 0);
	modelViewMatrix.Rotate(-10, 0, 1, 0);
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES,
		transformPipeline.GetModelViewProjectionMatrix());
	DrawArmAndDumbbell();
	modelViewMatrix.PopMatrix();

	modelViewMatrix.PushMatrix();
	modelViewMatrix.Translate(30, 45, 0);
	modelViewMatrix.Rotate(10, 0, 1, 0);
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES,
		transformPipeline.GetModelViewProjectionMatrix());
	DrawArmAndDumbbell();
	modelViewMatrix.PopMatrix();
}

// Aufruf draw scene
void RenderScene(void)
{
	// Clearbefehle für den color buffer und den depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Schalte culling ein falls das Flag gesetzt ist
	if (bCull)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	// Schalte depth testing ein falls das Flag gesetzt ist
	if (bDepth)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	// Zeichne die Rückseite von Polygonen als Drahtgitter falls das Flag gesetzt ist
	if (bOutline)
		glPolygonMode(GL_BACK, GL_LINE);
	else
		glPolygonMode(GL_BACK, GL_FILL);

	// Speichere den matrix state und führe die Rotation durch
	modelViewMatrix.PushMatrix();
	M3DMatrix44f rot;
	m3dQuatToRotationMatrix(rot, rotation);
	modelViewMatrix.MultMatrix(rot);

	//setze den Shader für das Rendern
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES, transformPipeline.GetModelViewProjectionMatrix());

	
	if (showQuader) {
		CreateCuboid(quaderX, quaderY, quaderZ);
	}
	if (showCylinder) {
		CreateCylinder(cylinderHeight, cylinderRadius, cylinderTesselation);
	}
	if (showSphere) {
		CreateSphere(sphereRadius, sphereStacks, sphereSectors);
	}
	if (showScene) {
		DrawBodybuilder();
	}

	//Auf fehler überprüfen
	gltCheckErrors(0);
	// Hole die im Stack gespeicherten Transformationsmatrizen wieder zurück
	modelViewMatrix.PopMatrix();

	TwDraw();
	// Vertausche Front- und Backbuffer
	glutSwapBuffers();
	glutPostRedisplay();
}

// Initialisierung des Rendering Kontextes
void SetupRC()
{
	// Schwarzer Hintergrund
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// In Uhrzeigerrichtung zeigende Polygone sind die Vorderseiten.
	// Dies ist umgekehrt als bei der Default-Einstellung weil wir Triangle_Fans benützen
	glFrontFace(GL_CW);

	//initialisiert die standard shader
	shaderManager.InitializeStockShaders();
	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
	InitGUI();
}

void SpecialKeys(int key, int x, int y)
{
	TwEventKeyboardGLUT(key, x, y);
	// Zeichne das Window neu
	glutPostRedisplay();
}


void ChangeSize(int w, int h)
{
	GLfloat nRange = 100.0f;

	// Verhindere eine Division durch Null
	if (h == 0)
		h = 1;
	// Setze den Viewport gemaess der Window-Groesse
	glViewport(0, 0, w, h);
	// Ruecksetzung des Projection matrix stack
	projectionMatrix.LoadIdentity();

	// Definiere das viewing volume (left, right, bottom, top, near, far)
	if (w <= h)
		viewFrustum.SetOrthographic(-nRange, nRange, -nRange * float(h) / float(w), nRange * float(h) / float(w), -nRange, nRange);
	else
		viewFrustum.SetOrthographic(-nRange * float(w) / float(h), nRange * float(w) / float(h), -nRange, nRange, -nRange, nRange);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	// Ruecksetzung des Model view matrix stack
	modelViewMatrix.LoadIdentity();

	TwWindowSize(w, h);
}

void ShutDownRC()
{
	//GUI aufräumen
	TwTerminate();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Versuch1");
	glutCloseFunc(ShutDownRC);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		// Veralteter Treiber etc.
		std::cerr << "Error: " << glewGetErrorString(err) << "\n";
		return 1;
	}

	glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
	glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT); // same as MouseMotion
	glutKeyboardFunc((GLUTkeyboardfun)TwEventKeyboardGLUT);

	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutDisplayFunc(RenderScene);

	TwInit(TW_OPENGL_CORE, NULL);
	SetupRC();
	glutMainLoop();

	return 0;
}
