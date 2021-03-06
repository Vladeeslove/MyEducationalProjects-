#include "stdafx.h"
#include "Ball.h"
#pragma comment(lib,"OpenGL32.lib")
std::vector<Ball> listBalls;

HGLRC hRC;
HDC   hDC;

BOOL  keys[256];

void drawCircle();
void AdjustMoveCircle();
int random(int min, int max);

float _width = 1100.0f;
float _height = 700.0f;
GLvoid InitGL(GLsizei Width, GLsizei Height)
{


	glClearColor(0.0f, 0.60f, 0.0f, 0.0f);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
}
GLvoid ReSizeGLScene(GLsizei Width, GLsizei Height)
{
	if (Height == 0)
		Height = 1;

	glViewport(0, 0, Width, Height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
}


GLvoid DrawGLScene()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	float aspect = _width / _height;
	glOrtho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
	drawCircle();


}
void drawCircle()
{
	int num_segments = 100;
	GLfloat
		radius,
		_x,
		_y;

	for (int i = 0; i < listBalls.size(); i++)
	{
		radius = listBalls.at(i).radius;
		_x = listBalls.at(i).x;
		_y = listBalls.at(i).y;
		glColor3f(listBalls.at(i).colorR, 0.0f, listBalls.at(i).colorB);
		glBegin(GL_POLYGON);
		for (int ii = 0; ii < num_segments; ii++)
		{

			float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle

			float x = radius * cosf(theta);//calculate the x component
			float y = radius * sinf(theta);//calculate the y component

			glVertex2f(x + _x, y + _y);//output vertex

		}
		glEnd();
	}
}
void AdjustMoveCircle()
{
	int tempWhole,
		tempFraction,
		temp,
		amount_collitions = 0;
	std::thread *arr = new std::thread[10];
	for (int i = 0; i < listBalls.size(); i++)
	{
		for (int j = 0; j < listBalls.size(); j++)
		{
			if (i != j)
			{
				float CD = sqrt(pow((listBalls.at(i).x - listBalls.at(j).x), 2) + pow((listBalls.at(i).y - listBalls.at(j).y), 2)) - listBalls.at(j).radius - listBalls.at(i).radius;
				tempWhole = CD;
				temp = tempWhole * 1000;
				tempFraction = CD * 1000;
				CD = tempWhole;
				CD += tempFraction / 1000.0;

				if (CD < 0.001f)
				{
					arr[amount_collitions] = std::thread(CollitionOfCircle, std::ref(listBalls.at(i)), std::ref(listBalls.at(j)));

					amount_collitions++;
				}
			}
		}
	}


	while (amount_collitions > 0)
	{
		arr[amount_collitions - 1].join();
		arr[amount_collitions - 1].~thread();
		amount_collitions--;
	}

	delete[] arr;
}

LRESULT CALLBACK WndProc(HWND    hWnd,
	UINT    message,
	WPARAM  wParam,
	LPARAM  lParam) {
	RECT    Screen;
	GLuint  PixelFormat;
	static  PIXELFORMATDESCRIPTOR pfd =
	{
			sizeof(PIXELFORMATDESCRIPTOR),
	1,
	PFD_DRAW_TO_WINDOW |
	PFD_SUPPORT_OPENGL |
	PFD_DOUBLEBUFFER,
	PFD_TYPE_RGBA,
	16,
	0, 0, 0, 0, 0, 0,
	0,
	0,
	0,
	0, 0, 0, 0,
	16,
	0,
	0,
	PFD_MAIN_PLANE,
	0,
	0, 0, 0
	};
	switch (message)
	{
	case WM_CREATE:
		hDC = GetDC(hWnd);
		PixelFormat = ChoosePixelFormat(hDC, &pfd);
		if (!PixelFormat)
		{
			MessageBox(0, "Can't Find A Suitable PixelFormat.", "Error", MB_OK | MB_ICONERROR);
			PostQuitMessage(0);
			break;
		}
		if (!SetPixelFormat(hDC, PixelFormat, &pfd))
		{
			MessageBox(0, "Can't Set The PixelFormat.", "Error", MB_OK | MB_ICONERROR);
			PostQuitMessage(0);
			break;
		}
		hRC = wglCreateContext(hDC);
		if (!hRC)
		{
			MessageBox(0, "Can't Create A GL Rendering Context.", "Error", MB_OK | MB_ICONERROR);
			PostQuitMessage(0);
			break;
		}
		if (!wglMakeCurrent(hDC, hRC))
		{
			MessageBox(0, "Can't activate GLRC.", "Error", MB_OK | MB_ICONERROR);
			PostQuitMessage(0);
			break;
		}

		GetClientRect(hWnd, &Screen);
		InitGL(Screen.right, Screen.bottom);
		break;
	case WM_DESTROY:
	case WM_CLOSE:
		ChangeDisplaySettings(NULL, 0);

		wglMakeCurrent(hDC, NULL);
		wglDeleteContext(hRC);
		ReleaseDC(hWnd, hDC);

		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		keys[wParam] = TRUE;
		break;
	case WM_KEYUP:
		keys[wParam] = FALSE;
		break;
	case WM_SIZE:
		ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));
		break;

	default:
		return (DefWindowProc(hWnd, message, wParam, lParam));
	}
	return (0);
}

int random(int min, int max)
{
	static bool flag;
	if (!flag)
	{
		srand(time(NULL));
		flag = true;
	}
	return min + rand() % (max - min);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	MSG             msg;
	WNDCLASS        wc;
	HWND            hWnd;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "ClaSsVlad";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, "Failed To Register The Window Class.", "Error", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	hWnd = CreateWindow(
		"ClaSsVlad",
		"FiveCircles   (ESC for exit)", // Заголовок вверху окна

		CS_HREDRAW | CS_VREDRAW | CS_OWNDC
		,
		0, 0,                   // Позиция окна на экране
		_width, _height,               // Ширина и высота окна

		NULL,
		NULL,
		hInstance,
		NULL);
	if (!hWnd)
	{
		MessageBox(0, "Window Creation Error.", "Error", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	SetFocus(hWnd);
	
	
	float asp = _width / _height;
#pragma region StaticPosition
	listBalls.push_back(Ball(-0.5,				// X by center
		0.5,									// Y by center
		-0.0007f,								// step by X on one interation
		-0.001f,								// step by Y on one interation
		0.25f,									// radius
		0.03f,									// amount red
		0.2f,									// amount green
		0.08f));								// amount blue

	listBalls.push_back(Ball(-0.5,
		-0.5,
		-0.001f,
		0.0007f,
		0.3f,
		0.03f,
		0.2f,
		random(1, 100) / 100.0f));

	listBalls.push_back(Ball(0.5,
		0.5,
		0.004f,
		-0.001,
		0.15f,
		random(1, 100) / 100.0f,
		random(1, 100) / 100.0f,
		random(1, 100) / 100.0f));

	listBalls.push_back(Ball(0.5,
		-0.5,
		0.001f,
		0.001f,
		0.20f,
		random(1, 100) / 100.0f,
		random(1, 100) / 100.0f,
		random(1, 100) / 100.0f));

	listBalls.push_back(Ball(0,
		0,
		-0.0013f,
		-0.0014f,
		0.2f,
		random(1, 100) / 100.0f,
		random(1, 100) / 100.0f,
		random(1, 100) / 100.0f
	));
#pragma endregion
	/*
	dynamic position
	*/

	//float CD = 0, Rotate = 30.0f;
	//float Ox,
	//	Oy,
	//	R,
	//	step_x,
	//	step_y,
	//	colorR,
	//	colorG,
	//	colorB;
	//for (int i = 0; i < 5; i++)
	//{
	//	R = random(10, 50) / 100.0f;
	//	step_x = random(10, 45) / 10000.0f;
	//	step_y = random(10, 45) / 10000.0f;
	//	colorR = random(1, 100) / 100.0f;
	//	colorG = random(1, 100) / 100.0f;
	//	colorB = random(1, 100) / 100.0f;
	//
	//	//srand(time(NULL));
	//	Ox = ((double)(random(0, 20) % (-15)*asp)) / 100.0f;
	//	Oy = 0.0f;
	//	////Consider distance
	//	int temp, tempWhole, tempFraction;
	//	for (int i = 0; i < listBalls.size(); i++)
	//	{
	//		CD = sqrt(pow((listBalls.at(i).x - Ox), 2) + pow((listBalls.at(i).y - Oy), 2)) - R - listBalls.at(i).radius;
	//		tempWhole = CD;
	//		temp = tempWhole * 1000;
	//		tempFraction = CD * 1000;
	//		CD = tempWhole;
	//		CD += tempFraction / 1000.0;
	//		if (CD < 0.001f) {
	//			float x3 = Ox, y3 = Oy;
	//			Rotate += 30.0;
	//			x3 = 0.0f + (Ox -0.0f) * cos(Rotate) - (Oy - 0.0) * sin(Rotate);//Rotate cordinate j-circle in relation to current circle
	//			y3 = 0.0f + (Oy - 0.0) * cos(Rotate) + (Ox - 0.0) * sin(Rotate);
	//			Ox = x3;
	//			Oy = y3;
	//		}
	//			
	//	}
	//	////initObj
	//	listBalls.push_back(Ball(

	//		Ox,			//center X
	//		Oy,			//center Y
	//		step_x,		//step X
	//		step_y,		//step Y
	//		R,			//radius
	//		colorR,		// color R
	//		colorG,		// color G
	//		colorB		// color B
	//	));
	//}
	
	
	
	while (1)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				return TRUE;
			}
		}

		AdjustMoveCircle();
		CollitionOfWall(listBalls, asp*1.0f, 1.0f);
		for (int i = 0; i < listBalls.size(); i++)
		{

			listBalls.at(i).x += listBalls.at(i).speed_x;
			listBalls.at(i).y += listBalls.at(i).speed_y;
		}

		DrawGLScene();
		SwapBuffers(hDC);
		if (keys[VK_ESCAPE]) SendMessage(hWnd, WM_CLOSE, 0, 0);
	}
}