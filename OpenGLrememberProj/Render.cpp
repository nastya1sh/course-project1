#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}





void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  


	//������ ��������� ���������� ��������
	/*double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();*/
	//����� ��������� ���������� ��������

	void half_a_circle(double r, double z, double O[]);
	double* normal(double A[], double B[], double C[]);
	double z, r;
	double A[] = { -9,3,0 }; /*��� ������� � �����������*/
	double B[] = { -4,7,0 };
	double C[] = { 0,0,0 };
	double O[] = { 0,0,0 };
	double N[] = { 0,0,0 };
	double* n;
	z = 5; /*������ ������*/
	
	#pragma region fig
	glBindTexture(GL_TEXTURE_2D, texId);
	glColor3d(0.6, 0.6, 0.6);

	glBegin(GL_TRIANGLES); /*���������*/
/*���*/
	A[0] = 7; A[1] = 4; A[2] = 0;
	B[0] = 5; B[1] = -3; B[2] = 0;
	C[0] = 1; C[1] = -2; C[2] = 0;
	n = normal(A, B, C);
	N[0] = n[0]; N[1] = n[1]; N[2] = n[2];
	glNormal3dv(N);
	glColor3d(0.2, 0.7, 0.7);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);

	A[0] = 7; A[1] = 4; A[2] = 0;
	B[0] = 1; B[1] = -2; B[2] = 0;
	C[0] = 0; C[1] = 2; C[2] = 0;
	n = normal(A, B, C);
	N[0] = n[0]; N[1] = n[1]; N[2] = n[2];
	glNormal3dv(N);
	glColor3d(0.1, 0.8, 0.5);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);

	A[0] = 0; A[1] = 2; A[2] = 0;
	B[0] = 1; B[1] = -2; B[2] = 0;
	C[0] = -2; C[1] = -1; C[2] = 0;
	n = normal(A, B, C);
	N[0] = n[0]; N[1] = n[1]; N[2] = n[2];
	glNormal3dv(N);
	glColor3d(0.3, 0.1, 0.9);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);

	A[0] = -2; A[1] = -1; A[2] = 0;
	B[0] = 1; B[1] = -2; B[2] = 0;
	C[0] = -4; C[1] = -9; C[2] = 0;
	n = normal(A, B, C);
	N[0] = n[0]; N[1] = n[1]; N[2] = n[2];
	glNormal3dv(N);
	glColor3d(0.8, 0.8, 0.1);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);

	A[0] = -2; A[1] = -1; A[2] = 0;
	B[0] = 0; B[1] = 2; B[2] = 0;
	C[0] = -4; C[1] = 7; C[2] = 0;
	n = normal(A, B, C);
	N[0] = -n[0]; N[1] = -n[1]; N[2] = -n[2];
	glNormal3dv(N);
	glColor3d(0.1, 0.1, 0.1);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);

	A[0] = -2; A[1] = -1; A[2] = 0;
	B[0] = -9; B[1] = 3; B[2] = 0;
	C[0] = -4; C[1] = 7; C[2] = 0;
	n = normal(A, B, C);
	N[0] = n[0]; N[1] = n[1]; N[2] = n[2];
	glNormal3dv(N);
	glColor3d(0.9, 0.1, 0.1);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);
/*�����*/
	A[0] = 7; A[1] = 4; A[2] = z;
	B[0] = 5; B[1] = -3; B[2] = z;
	C[0] = 1; C[1] = -2; C[2] = z;
	n = normal(A, B, C);
	N[0] = -n[0]; N[1] = -n[1]; N[2] = -n[2];
	glNormal3dv(N);
	glColor3d(0.2, 0.7, 0.7);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);

	A[0] = 7; A[1] = 4; A[2] = z;
	B[0] = 1; B[1] = -2; B[2] = z;
	C[0] = 0; C[1] = 2; C[2] = z;
	n = normal(A, B, C);
	N[0] = -n[0]; N[1] = -n[1]; N[2] = -n[2];
	glNormal3dv(N);
	glColor3d(0.1, 0.8, 0.5);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);

	A[0] = 0; A[1] = 2; A[2] = z;
	B[0] = 1; B[1] = -2; B[2] = z;
	C[0] = -2; C[1] = -1; C[2] = z;
	n = normal(A, B, C);
	N[0] = -n[0]; N[1] = -n[1]; N[2] = -n[2];
	glNormal3dv(N);
	glColor3d(0.3, 0.1, 0.9);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);

	A[0] = -2; A[1] = -1; A[2] = z;
	B[0] = 1; B[1] = -2; B[2] = z;
	C[0] = -4; C[1] = -9; C[2] = z;
	n = normal(A, B, C);
	N[0] = -n[0]; N[1] = -n[1]; N[2] = -n[2];
	glNormal3dv(N);
	glColor3d(0.8, 0.8, 0.1);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);

	A[0] = -2; A[1] = -1; A[2] = z;
	B[0] = 0; B[1] = 2; B[2] = z;
	C[0] = -4; C[1] = 7; C[2] = z;
	n = normal(A, B, C);
	N[0] = n[0]; N[1] = n[1]; N[2] = n[2];
	glNormal3dv(N);
	glColor3d(0.1, 0.1, 0.1);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);

	A[0] = -2; A[1] = -1; A[2] = z;
	B[0] = -9; B[1] = 3; B[2] = z;
	C[0] = -4; C[1] = 7; C[2] = z;
	n = normal(A, B, C);
	N[0] = -n[0]; N[1] = -n[1]; N[2] = -n[2];
	glNormal3dv(N);
	glColor3d(0.9, 0.1, 0.1);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);

	glEnd();

	glBegin(GL_QUADS); /*������� �������*/

	A[0] = 7; A[1] = 4; A[2] = 0;
	B[0] = 5; B[1] = -3; B[2] = 0;
	C[0] = 5; C[1] = -3; C[2] = z;
	n = normal(A, B, C);
	N[0] = -n[0]; N[1] = -n[1]; N[2] = -n[2];
	glNormal3dv(N);
	glColor3d(0.2, 0.1, 0.7);
	glVertex3d(7, 4, 0);
	glVertex3d(5, -3, 0);
	glVertex3d(5, -3, z);
	glVertex3d(7, 4, z);

	A[0] = 7; A[1] = 4; A[2] = 0;
	B[0] = 0; B[1] = 2; B[2] = 0;
	C[0] = 0; C[1] = 2; C[2] = z;
	n = normal(A, B, C);
	N[0] = n[0]; N[1] = n[1]; N[2] = n[2];
	glNormal3dv(N);
	glColor3d(0.1, 0.4, 0.5);
	glVertex3d(7, 4, 0);
	glVertex3d(0, 2, 0);
	glVertex3d(0, 2, z);
	glVertex3d(7, 4, z);

	A[0] = 5; A[1] = -3; A[2] = 0;
	B[0] = 1; B[1] = -2; B[2] = 0;
	C[0] = 1; C[1] = -2; C[2] = z;
	n = normal(A, B, C);
	N[0] = -n[0]; N[1] = -n[1]; N[2] = -n[2];
	glNormal3dv(N);
	glColor3d(0.7, 0.7, 0.1);
	glVertex3d(5, -3, 0);
	glVertex3d(1, -2, 0);
	glVertex3d(1, -2, z);
	glVertex3d(5, -3, z);

	A[0] = -4; A[1] = -9; A[2] = 0;
	B[0] = 1; B[1] = -2; B[2] = 0;
	C[0] = 1; C[1] = -2; C[2] = z;
	n = normal(A, B, C);
	N[0] = n[0]; N[1] = n[1]; N[2] = n[2];
	glNormal3dv(N);
	glColor3d(0.1, 0.9, 0.3);
	glVertex3d(-4, -9, 0);
	glVertex3d(1, -2, 0);
	glVertex3d(1, -2, z);
	glVertex3d(-4, -9, z);

	A[0] = -4; A[1] = -9; A[2] = 0;
	B[0] = -2; B[1] = -1; B[2] = 0;
	C[0] = -2; C[1] = -1; C[2] = z;
	n = normal(A, B, C);
	N[0] = -n[0]; N[1] = -n[1]; N[2] = -n[2];
	glNormal3dv(N);
	glColor3d(0.1, 0.9, 0.5);
	glVertex3d(-4, -9, 0);
	glVertex3d(-2, -1, 0);
	glVertex3d(-2, -1, z);
	glVertex3d(-4, -9, z);

	A[0] = -2; A[1] = -1; A[2] = 0;
	B[0] = -9; B[1] = 3; B[2] = 0;
	C[0] = -9; C[1] = 3; C[2] = z;
	n = normal(A, B, C);
	N[0] = -n[0]; N[1] = -n[1]; N[2] = -n[2];
	glNormal3dv(N);
	glColor3d(0.5, 0.4, 0.1);
	glVertex3d(-2, -1, 0);
	glVertex3d(-9, 3, 0);
	glVertex3d(-9, 3, z);
	glVertex3d(-2, -1, z);

	A[0] = -4; A[1] = 7; A[2] = 0;
	B[0] = 0; B[1] = 2; B[2] = 0;
	C[0] = 0; C[1] = 2; C[2] = z;
	n = normal(A, B, C);
	N[0] = -n[0]; N[1] = -n[1]; N[2] = -n[2];
	glNormal3dv(N);
	glColor3d(0.7, 0.4, 0.1);
	glVertex3d(-4, 7, 0);
	glVertex3d(0, 2, 0);
	glVertex3d(0, 2, z);
	glVertex3d(-4, 7, z);

	glEnd();
#pragma endregion

	A[0] = -9; /*��� ������� � �����������*/A[1] = 3; A[2] = 0;
	B[0] = -4; B[1] = 7; B[2] = 0;

	O[0] = (A[0] + B[0]) / 2;
	O[1] = (A[1] + B[1]) / 2;

	r = sqrt(pow(A[0] - O[0], 2) + pow(A[1] - O[1], 2));

	half_a_circle(r, z, O); /*���������� ������������*/
   
	//��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}

void half_a_circle(double r, double z, double O[])
{
	double* normal(double [], double [], double []);
	double  x, y = 0;
	double A[] = { 0,0,0 };
	double A1[] = { 0,0,z };
	double B[] = { 0,0,0 };
	double B1[] = { 0,0,z };
	double O1[] = { O[0],O[1],z };
	double N[] = { 0,0,0 };
	double* n;
	glBegin(GL_TRIANGLES);
	for (x = -9; x < -4; x += 0.01)
	{
		y = sqrt(pow(r, 2) - pow((x - O[0]), 2)) + O[1];
		A1[0] = A[0] = x;
		A1[1] = A[1] = y;
		x += 0.01;
		y = sqrt(pow(r, 2) - pow((x - O[0]), 2)) + O[1];
		B1[0] = B[0] = x;
		B1[1] = B[1] = y;
		x -= 0.01;
		n = normal(A, B, O);
		N[0] = n[0]; N[1] = n[1]; N[2] = n[2];
		glNormal3dv(N);
		glColor3d(0.1, 0.2, 0.5);/*���������*/
		glVertex3dv(O);
		glVertex3dv(A);
		glVertex3dv(B);
		n = normal(A1, B1, O1);
		N[0] = -n[0]; N[1] = -n[1]; N[2] = -n[2];
		glNormal3dv(N);
		glVertex3dv(O1);
		glVertex3dv(A1);
		glVertex3dv(B1);

		n = normal(A, A1, B);
		N[0] = n[0]; N[1] = n[1]; N[2] = n[2];
		glColor3d(0.1, 0.8, 0.5); /*������� �������*/
		glVertex3dv(A);
		glVertex3dv(A1);
		glVertex3dv(B);
		n = normal(B1, B, A1);
		N[0] = -n[0]; N[1] = -n[1]; N[2] = -n[2];
		glVertex3dv(B);
		glVertex3dv(B1);
		glVertex3dv(A1);
	}
	glEnd();
	double Ymax = sqrt(pow(r, 2) - pow((-9 - O[0]), 2)) + O[1];
	glBegin(GL_TRIANGLES);
	for (y = 3; y < 7; y += 0.01)
	{
		x = -(sqrt(pow(r, 2) - pow((y - O[1]), 2))) + O[0];
		A1[0] = A[0] = x;
		A1[1] = A[1] = y;
		y += 0.01;
		x = -(sqrt(pow(r, 2) - pow((y - O[1]), 2))) + O[0];
		B1[0] = B[0] = x;
		B1[1] = B[1] = y;
		y -= 0.01;
		n = normal(A, B, O);
		N[0] = n[0]; N[1] = n[1]; N[2] = n[2];
		glNormal3dv(N);
		glColor3d(0.1, 0.2, 0.5);/*���������*/
		glVertex3dv(O);
		glVertex3dv(A);
		glVertex3dv(B);
		n = normal(A1, B1, O1);
		N[0] = -n[0]; N[1] = -n[1]; N[2] = -n[2];
		glNormal3dv(N);
		glVertex3dv(O1);
		glVertex3dv(A1);
		glVertex3dv(B1);

		n = normal(A, A1, B);
		N[0] = n[0]; N[1] = n[1]; N[2] = n[2];
		glColor3d(0.1, 0.8, 0.5); /*������� �������*/
		glVertex3dv(A);
		glVertex3dv(A1);
		glVertex3dv(B);
		n = normal(B1, B, A1);
		N[0] = -n[0]; N[1] = -n[1]; N[2] = -n[2];
		glColor3d(0.1, 0.8, 0.5);
		glVertex3dv(B);
		glVertex3dv(B1);
		glVertex3dv(A1);
	}
	glEnd();
}
double* normal(double A[], double B[], double C[])
{
	double N[3] = { 0 , 0, 0 };
	N[0] = (A[1] - B[1]) * (B[2] - C[2]) - (A[2] - B[2]) * (B[1] - C[1]);
	N[1] = (A[2] - B[2]) * (B[0] - C[0]) - (A[0] - B[0]) * (B[2] - C[2]);
	N[2] = (A[0] - B[0]) * (B[1] - C[1]) - (A[1] - B[1]) * (B[0] - C[0]);
	return N;
}