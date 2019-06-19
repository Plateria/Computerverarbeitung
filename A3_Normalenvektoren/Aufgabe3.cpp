// Ausgangssoftware des 3. Praktikumsversuchs 
// zur Vorlesung Echtzeit-3D-Computergrahpik
// von Prof. Dr. Alfred Nischwitz
// Programm umgesetzt mit der GLTools Library
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

// Definition der Kreiszahl 
#define GL_PI 3.1415f

GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLGeometryTransform transformPipeline;
GLFrustum viewFrustum;
//GLBatch geometryBatch;
GLuint shaders;

/// View space light position
float light_pos[4] = { 0.5f,0.1f,-5.0f,1.0f };
/// Lichtfarben
float light_ambient[4] = { 1.0, 1.0, 1.0, 1.0 };
float light_diffuse[4] = { 1.0f,1.0f,1.0f,1.0f };
float light_specular[4] = { 1.0f,1.0f,1.0f,1.0f };

//Materialeigenschaften
float mat_emissive[4] = { 0.0, 0.0, 0.0, 1.0 };
float mat_ambient[4] = { 0.1, 0.1, 0.0, 1.0 };
float mat_diffuse[4] = { 0.6, 0.0, 0.0, 1.0 };
float mat_specular[4] = { 0.8, 0.8, 0.8, 1.0 };
float specular_power = 10;
// Rotationsgroessen
float rotation[] = { 0, 0, 0, 0 };

int thresholdAngle = 0;
bool showNormalVectors = true;
//GUI
TwBar *bar;

void InitGUI()
{
	bar = TwNewBar("TweakBar");
	TwDefine(" TweakBar size='200 400'");
	TwAddVarRW(bar, "Model Rotation", TW_TYPE_QUAT4F, &rotation, "");
	TwAddVarRW(bar, "Light Position", TW_TYPE_DIR3F, &light_pos, "group=Light axisx=-x axisy=-y axisz=-z");
	TwAddVarRW(bar, "Light Ambient", TW_TYPE_COLOR4F, &light_ambient, "group=Light");
	TwAddVarRW(bar, "Light Diffuse", TW_TYPE_COLOR4F, &light_diffuse, "group=Light");
	TwAddVarRW(bar, "Light Specular", TW_TYPE_COLOR4F, &light_specular, "group=Light");
	TwAddVarRW(bar, "Material Emissive", TW_TYPE_COLOR4F, &mat_emissive, "group=Material");
	TwAddVarRW(bar, "Material Ambient", TW_TYPE_COLOR4F, &mat_ambient, "group=Material");
	TwAddVarRW(bar, "Material Diffuse", TW_TYPE_COLOR4F, &mat_diffuse, "group=Material");
	TwAddVarRW(bar, "Material Specular", TW_TYPE_COLOR4F, &mat_specular, "group=Material");
	//Hier weitere GUI Variablen anlegen. Für Farbe z.B. den Typ TW_TYPE_COLOR4F benutzen
	TwAddVarRW(bar, "Threshold Angle", TW_TYPE_INT32, &thresholdAngle, "");
	TwAddVarRW(bar, "Show Normal Vectors", TW_TYPE_BOOLCPP, &showNormalVectors, "");
}

void DrawNormalVectors(M3DVector3f* cylinderVertices, M3DVector3f* cylinderNormals, int length) {
	M3DVector3f* lineVertices = new M3DVector3f[length * 2];

	for (int i = 0; i < length; i++) {
		m3dLoadVector3(lineVertices[2 * i], cylinderVertices[i][0], cylinderVertices[i][1], cylinderVertices[i][2]);
		m3dLoadVector3(lineVertices[2 * i + 1], cylinderVertices[i][0] + cylinderNormals[i][0],
			cylinderVertices[i][1] + cylinderNormals[i][1], cylinderVertices[i][2] + cylinderNormals[i][2]);
	}

	GLBatch lines;
	lines.Begin(GL_LINES, length * 2);
	lines.CopyVertexData3f(lineVertices);
	lines.End();
	lines.Draw();

	delete[] lineVertices;
}

void CreateGeometry()
{
	float radius = 0.5f;
	float height = 2.0f;

	int steps = 8;
	float increment = (2.0f * GL_PI) / steps;

	M3DVector3f* cylinderVertices = new M3DVector3f[steps * 4 * 3];
	M3DVector3f* cylinderNormals = new M3DVector3f[steps * 4 * 3];

	for (int step = 0; step < steps; step++) {
		float angle = step * increment;
		float x = radius * sin(angle);
		float y = radius * cos(angle);
		float xn = radius * sin(angle + increment);
		float yn = radius * cos(angle + increment);

		float topX = 0;
		float topY = 0;
		float topZ = 1;
		float bottomX = 0;
		float bottomY = 0;
		float bottomZ = -1;
		float length = sqrt(pow(x, 2) + pow(y, 2));
		float lengthNext = sqrt(pow(xn, 2) + pow(yn, 2));
		float mantleX = x / length;
		float mantleY = y / length;
		float mantleZ = 0;
		float mantleNextX = xn / lengthNext;
		float mantleNextY = yn / lengthNext;
		float mantleNextZ = 0;

		if (thresholdAngle <= 45) {
			float tmpX = (mantleX + mantleNextX) / 2;
			float tmpY = (mantleY + mantleNextY) / 2;
			mantleX = tmpX;
			mantleY = tmpY;
			mantleNextX = tmpX;
			mantleNextY = tmpY;
		}
		else if (thresholdAngle > 90) {
			float tmpLength = sqrt(pow(mantleX, 2) + pow(mantleY, 2) + pow(0.5, 2));
			topX = mantleX / tmpLength;
			topY = mantleY / tmpLength;
			topZ = 0.5 / tmpLength;
			bottomX = mantleX / tmpLength;
			bottomY = mantleY / tmpLength;
			bottomZ = -0.5 / tmpLength;
			mantleX = mantleX / tmpLength;
			mantleY = mantleY / tmpLength;
			mantleZ = 0.5 / tmpLength;
			float tmpNextLength = sqrt(pow(mantleNextX, 2) + pow(mantleNextY, 2) + pow(0.5, 2));
			mantleNextX = mantleNextX / tmpNextLength;
			mantleNextY = mantleNextY / tmpNextLength;
			mantleNextZ = 0.5 / tmpNextLength;
		}

		m3dLoadVector3(cylinderVertices[step * 12], 0, 0, height / 2);
		m3dLoadVector3(cylinderNormals[step * 12], 0, 0, 1);

		m3dLoadVector3(cylinderVertices[step * 12 + 1], x, y, height / 2);
		m3dLoadVector3(cylinderNormals[step * 12 + 1], topX, topY, topZ);
		m3dLoadVector3(cylinderVertices[step * 12 + 2], xn, yn, height / 2);
		if (thresholdAngle > 90) {
			m3dLoadVector3(cylinderNormals[step * 12 + 2], mantleNextX, mantleNextY, mantleNextZ);
		}
		else {
			m3dLoadVector3(cylinderNormals[step * 12 + 2], topX, topY, topZ);
		}

		m3dLoadVector3(cylinderVertices[step * 12 + 3], x, y, height / 2);
		m3dLoadVector3(cylinderNormals[step * 12 + 3], mantleX, mantleY, mantleZ);
		m3dLoadVector3(cylinderVertices[step * 12 + 4], x, y, -height / 2);
		m3dLoadVector3(cylinderNormals[step * 12 + 4], mantleX, mantleY, -mantleZ);
		m3dLoadVector3(cylinderVertices[step * 12 + 5], xn, yn, -height / 2);
		m3dLoadVector3(cylinderNormals[step * 12 + 5], mantleNextX, mantleNextY, -mantleNextZ);

		m3dLoadVector3(cylinderVertices[step * 12 + 6], x, y, height / 2);
		m3dLoadVector3(cylinderNormals[step * 12 + 6], mantleX, mantleY, mantleZ);
		m3dLoadVector3(cylinderVertices[step * 12 + 7], xn, yn, -height / 2);
		m3dLoadVector3(cylinderNormals[step * 12 + 7], mantleNextX, mantleNextY, -mantleNextZ);
		m3dLoadVector3(cylinderVertices[step * 12 + 8], xn, yn, height / 2);
		m3dLoadVector3(cylinderNormals[step * 12 + 8], mantleNextX, mantleNextY, mantleNextZ);

		m3dLoadVector3(cylinderVertices[step * 12 + 9], 0, 0, -height / 2);
		m3dLoadVector3(cylinderNormals[step * 12 + 9], 0, 0, -1);
		m3dLoadVector3(cylinderVertices[step * 12 + 10], x, y, -height / 2);
		m3dLoadVector3(cylinderNormals[step * 12 + 10], bottomX, bottomY, bottomZ);
		m3dLoadVector3(cylinderVertices[step * 12 + 11], xn, yn, -height / 2);
		if (thresholdAngle > 90) {
			m3dLoadVector3(cylinderNormals[step * 12 + 11], mantleNextX, mantleNextY, -mantleNextZ);
		}
		else {
			m3dLoadVector3(cylinderNormals[step * 12 + 11], bottomX, bottomY, bottomZ);
		}
	}

	GLBatch geometryBatch;
	geometryBatch.Begin(GL_TRIANGLES, steps * 4 * 3);
	geometryBatch.CopyVertexData3f(cylinderVertices);
	geometryBatch.CopyNormalDataf(cylinderNormals);
	geometryBatch.End();
	geometryBatch.Draw();

	if (showNormalVectors) {
		DrawNormalVectors(cylinderVertices, cylinderNormals, steps * 4 * 3);
	}

	delete[] cylinderVertices;
	delete[] cylinderNormals;
}

// Aufruf draw scene
void RenderScene(void)
{
	// Clearbefehle für den color buffer und den depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	modelViewMatrix.LoadIdentity();
	modelViewMatrix.Translate(0, 0, -5);
	// Speichere den matrix state und führe die Rotation durch
	modelViewMatrix.PushMatrix();

	M3DMatrix44f rot;
	m3dQuatToRotationMatrix(rot, rotation);
	modelViewMatrix.MultMatrix(rot);

	//setze den Shader für das Rendern
	glUseProgram(shaders);
	// Matrizen an den Shader übergeben
	glUniformMatrix4fv(glGetUniformLocation(shaders, "mvpMatrix"), 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
	glUniformMatrix4fv(glGetUniformLocation(shaders, "mvMatrix"), 1, GL_FALSE, transformPipeline.GetModelViewMatrix());
	glUniformMatrix3fv(glGetUniformLocation(shaders, "normalMatrix"), 1, GL_FALSE, transformPipeline.GetNormalMatrix(true));
	// Lichteigenschaften übergeben
	glUniform4fv(glGetUniformLocation(shaders, "light_pos_vs"), 1, light_pos);
	glUniform4fv(glGetUniformLocation(shaders, "light_ambient"), 1, light_ambient);
	glUniform4fv(glGetUniformLocation(shaders, "light_diffuse"), 1, light_diffuse);
	glUniform4fv(glGetUniformLocation(shaders, "light_specular"), 1, light_specular);
	glUniform1f(glGetUniformLocation(shaders, "spec_power"), specular_power);
	//Materialeigenschaften übergeben
	glUniform4fv(glGetUniformLocation(shaders, "mat_emissive"), 1, mat_emissive);
	glUniform4fv(glGetUniformLocation(shaders, "mat_ambient"), 1, mat_ambient);
	glUniform4fv(glGetUniformLocation(shaders, "mat_diffuse"), 1, mat_diffuse);
	glUniform4fv(glGetUniformLocation(shaders, "mat_specular"), 1, mat_specular);
	//Zeichne Model
	CreateGeometry();

	//geometryBatch.Draw();

	// Hole die im Stack gespeicherten Transformationsmatrizen wieder zurück
	modelViewMatrix.PopMatrix();
	// Draw tweak bars
	TwDraw();
	gltCheckErrors(shaders);
	// Vertausche Front- und Backbuffer
	glutSwapBuffers();
	glutPostRedisplay();
}

// Initialisierung des Rendering Kontextes
void SetupRC()
{
	// Schwarzer Hintergrund
	glClearColor(0.12f, 0.35f, 0.674f, 0.0f);

	// In Uhrzeigerrichtung zeigende Polygone sind die Vorderseiten.
	// Dies ist umgekehrt als bei der Default-Einstellung weil wir Triangle_Fans benützen
	glFrontFace(GL_CCW);

	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
	//erzeuge die geometrie
	//CreateGeometry();
	shaders = gltLoadShaderPairWithAttributes("VertexShader.glsl", "FragmentShader.glsl", 2,
		GLT_ATTRIBUTE_VERTEX, "vVertex",
		GLT_ATTRIBUTE_NORMAL, "vNormal");

	gltCheckErrors(shaders);
	InitGUI();
}

void ShutDownRC()
{
	//Aufräumen
	glDeleteProgram(shaders);

	//GUI aufräumen
	TwTerminate();
}



void ChangeSize(int w, int h)
{
	// Verhindere eine Division durch Null
	if (h == 0)
		h = 1;
	// Setze den Viewport gemaess der Window-Groesse
	glViewport(0, 0, w, h);
	// Ruecksetzung des Projection matrix stack
	projectionMatrix.LoadIdentity();
	viewFrustum.SetPerspective(45, float(w) / float(h), 1, 100);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	// Ruecksetzung des Model view matrix stack
	modelViewMatrix.LoadIdentity();

	// Send the new window size to AntTweakBar
	TwWindowSize(w, h);
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("A3 Normalenvektoren");
	glutCloseFunc(ShutDownRC);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		//Veralteter Treiber etc.
		std::cerr << "Error: " << glewGetErrorString(err) << "\n";
		return 1;
	}
	glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
	glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT); // same as MouseMotion
	glutKeyboardFunc((GLUTkeyboardfun)TwEventKeyboardGLUT);
	glutSpecialFunc((GLUTspecialfun)TwEventKeyboardGLUT);

	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);
	TwInit(TW_OPENGL_CORE, NULL);
	SetupRC();
	glutMainLoop();

	return 0;
}
