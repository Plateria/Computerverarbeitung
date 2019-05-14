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

GLBatch konus;
GLBatch boden;


// Definition der Kreiszahl 
#define GL_PI 3.1415f

// Rotationsgroessen
static float rotation[] = { 0, 0, 0, 0 };

// Flags fuer Schalter
bool bCull = false;
bool bOutline = false;
bool bDepth = true;

bool showCylinder = true;

float cylinderHeight = 60;
float cylinderRadius = 15;
unsigned int cylinderTesselation = 18;

void CreateCylinder(void);

//Set Funktion für GUI, wird aufgerufen wenn Variable im GUI geändert wird
void TW_CALL SetCylinderTesselation(const void* value, void* clientData)
{
	//Pointer auf gesetzten Typ casten (der Typ der bei TwAddVarCB angegeben wurde)
	const unsigned int* uintptr = static_cast<const unsigned int*>(value);

	//Setzen der Variable auf neuen Wert
	cylinderTesselation = *uintptr;

	//Hier kann nun der Aufruf gemacht werden um die Geometrie mit neuem Tesselationsfaktor zu erzeugen
	CreateCylinder();
}

//Get Funktion für GUI, damit GUI Variablen Wert zum anzeigen erhält
void TW_CALL GetCylinderTesselation(void* value, void* clientData)
{
	//Pointer auf gesetzten Typ casten (der Typ der bei TwAddVarCB angegeben wurde)
	unsigned int* uintptr = static_cast<unsigned int*>(value);

	//Variablen Wert and GUI weiterreichen
	*uintptr = cylinderTesselation;
}

//Set Funktion für GUI, wird aufgerufen wenn Variable im GUI geändert wird
void TW_CALL SetCylinderHeight(const void* value, void* clientData)
{
	//Pointer auf gesetzten Typ casten (der Typ der bei TwAddVarCB angegeben wurde)
	const float* floatptr = static_cast<const float*>(value);

	//Setzen der Variable auf neuen Wert
	cylinderHeight = *floatptr;

	//Hier kann nun der Aufruf gemacht werden um die Geometrie mit neuem Tesselationsfaktor zu erzeugen
	CreateCylinder();
}

//Get Funktion für GUI, damit GUI Variablen Wert zum anzeigen erhält
void TW_CALL GetCylinderHeight(void* value, void* clientData)
{
	//Pointer auf gesetzten Typ casten (der Typ der bei TwAddVarCB angegeben wurde)
	float* floatptr = static_cast<float*>(value);

	//Variablen Wert and GUI weiterreichen
	*floatptr = cylinderHeight;
}

//Set Funktion für GUI, wird aufgerufen wenn Variable im GUI geändert wird
void TW_CALL SetCylinderRadius(const void* value, void* clientData)
{
	//Pointer auf gesetzten Typ casten (der Typ der bei TwAddVarCB angegeben wurde)
	const float* floatptr = static_cast<const float*>(value);

	//Setzen der Variable auf neuen Wert
	cylinderRadius = *floatptr;

	//Hier kann nun der Aufruf gemacht werden um die Geometrie mit neuem Tesselationsfaktor zu erzeugen
	CreateCylinder();
}

//Get Funktion für GUI, damit GUI Variablen Wert zum anzeigen erhält
void TW_CALL GetCylinderRadius(void* value, void* clientData)
{
	//Pointer auf gesetzten Typ casten (der Typ der bei TwAddVarCB angegeben wurde)
	float* floatptr = static_cast<float*>(value);

	//Variablen Wert and GUI weiterreichen
	*floatptr = cylinderRadius;
}

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
	TwAddVarCB(bar, "Cylinder Height", TW_TYPE_FLOAT, SetCylinderHeight, GetCylinderHeight, NULL, "");
	TwAddVarCB(bar, "Cylinder Radius", TW_TYPE_FLOAT, SetCylinderRadius, GetCylinderRadius, NULL, "");
	TwAddVarCB(bar, "Cylinder Tesselation", TW_TYPE_UINT32, SetCylinderTesselation, GetCylinderTesselation, NULL, "");
}

void CreateCylinder()
{
	if (cylinderTesselation < 5)
		cylinderTesselation = 5;
	if (cylinderHeight < 1)
		cylinderHeight = 1;
	if (cylinderRadius < 1)
		cylinderRadius = 1;

	M3DVector3f* cylinderBottomVertices = new M3DVector3f[cylinderTesselation];
	M3DVector4f* cylinderBottomColors = new M3DVector4f[cylinderTesselation];
	M3DVector3f* cylinderTopVertices = new M3DVector3f[cylinderTesselation];
	M3DVector4f* cylinderTopColors = new M3DVector4f[cylinderTesselation];
	M3DVector3f* cylinderMantleVertices = new M3DVector3f[(cylinderTesselation - 1) * 2];
	M3DVector4f* cylinderMantleColors = new M3DVector4f[(cylinderTesselation - 1) * 2];

	m3dLoadVector3(cylinderBottomVertices[0], 0, 0, 0);
	m3dLoadVector4(cylinderBottomColors[0], 1, 0, 0, 1);
	m3dLoadVector3(cylinderTopVertices[0], 0, 0, cylinderHeight);
	m3dLoadVector4(cylinderTopColors[0], 1, 0, 0, 1);

	int iPivot = 1;
	int i = 1;
	int steps = cylinderTesselation - 2;
	float increment = (2.0f * GL_PI) / steps;

	for (int step = 0; step <= steps; step++)
	{
		float angle = step * increment;
		float x = cylinderRadius * sin(-angle);
		float y = cylinderRadius * cos(-angle);

		if ((iPivot % 2) == 0)
		{
			m3dLoadVector4(cylinderBottomColors[cylinderTesselation - i], 1, 0.8, 0.2, 1);
			m3dLoadVector4(cylinderTopColors[i], 1, 0.8, 0.2, 1);
		}
		else {
			m3dLoadVector4(cylinderBottomColors[cylinderTesselation - i], 0, 0.8, 0, 1);
			m3dLoadVector4(cylinderTopColors[i], 0, 0.8, 0, 1);
		}

		m3dLoadVector3(cylinderBottomVertices[i], x, y, 0);
		m3dLoadVector3(cylinderTopVertices[cylinderTesselation - i], x, y, cylinderHeight);

		m3dLoadVector3(cylinderMantleVertices[(i - 1) * 2], x, y, 0);
		m3dLoadVector3(cylinderMantleVertices[(i - 1) * 2 + 1], x, y, cylinderHeight);
		m3dLoadVector4(cylinderMantleColors[(i - 1) * 2], 1, 0.8, 0.2, 1);
		m3dLoadVector4(cylinderMantleColors[(i - 1) * 2 + 1], 0, 0.8, 0, 1);

		iPivot++;
		i++;
	}

	cylinderBottom = *new GLBatch;
	cylinderTop = *new GLBatch;
	cylinderMantle = *new GLBatch;

	cylinderBottom.Begin(GL_TRIANGLE_FAN, cylinderTesselation);
	cylinderBottom.CopyVertexData3f(cylinderBottomVertices);
	cylinderBottom.CopyColorData4f(cylinderBottomColors);
	cylinderBottom.End();

	cylinderTop.Begin(GL_TRIANGLE_FAN, cylinderTesselation);
	cylinderTop.CopyVertexData3f(cylinderTopVertices);
	cylinderTop.CopyColorData4f(cylinderTopColors);
	cylinderTop.End();

	cylinderMantle.Begin(GL_TRIANGLE_STRIP, (cylinderTesselation - 1) * 2);
	cylinderMantle.CopyVertexData3f(cylinderMantleVertices);
	cylinderMantle.CopyColorData4f(cylinderMantleColors);
	cylinderMantle.End();

	delete[] cylinderBottomVertices;
	delete[] cylinderBottomColors;
	delete[] cylinderTopVertices;
	delete[] cylinderTopColors;
	delete[] cylinderMantleVertices;
	delete[] cylinderMantleColors;
}

void CreateGeometry() {
	//CreateTent();
	//CreateCircle();
	int x = 50, y = 70, z = 50;
	CreateQuadrat(x, y, z, 0, 0, 0);
	CreateQuadrat(-x, -y, -z, x, y, z);
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

	if (showCylinder) {
		cylinderBottom.Draw();
		cylinderTop.Draw();
		cylinderMantle.Draw();
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
	//erzeuge die geometrie
	CreateCylinder();
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
