//Headers
#include<windows.h>
#include<stdio.h>
#include<gl/Gl.h>
#include<gl/glu.h>
#include"Icon.h"
#include"Song.h"

#define _USE_MATH_DEFINES
#include<math.h>

#define WIN_WIDTH 800
#define WIN_HIGHT 600
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"winmm.lib")

struct robo
{	
	GLfloat Translate1X;
	GLfloat Translate1Y;
	GLfloat Translate1Z;	

	int rotateVar1;
	GLfloat rotateR1X;
	GLfloat rotateR1Y;
	GLfloat rotateR1Z;

	int rotateVar2;
	GLfloat rotateR2X;
	GLfloat rotateR2Y;
	GLfloat rotateR2Z;

	GLfloat Sphere1Param1;
	GLint Sphere1Param2;
	GLint Sphere1Param3;

	GLfloat Translate2X;
	GLfloat Translate2Y;
	GLfloat Translate2Z;

	GLfloat Sphere2Param1;
	GLint Sphere2Param2;
	GLint Sphere2Param3;

	GLfloat Scale1X;
	GLfloat Scale1Y;
	GLfloat Scale1Z;
	
	bool isUpJoint;
	bool isUpBody;
	bool isheadSphere;

	robo* next;
}*head;

robo* shoulder[2];
robo* elbo[2];
robo* thai[2];
robo* nee[2];
robo* headSphere[1];
GLint ChangeValDirectLeft = 1;
//Font
static GLint nFontList;

void initializeRoboStruct(void)
{
	/*-------------------------- shoulder [START] ---------------------------------------*/
	//shoulder[0] is right shoulder, shoulder[1] is left shoulder

	shoulder[0] = (robo *)malloc(sizeof robo);
	shoulder[1] = (robo *)malloc(sizeof robo);

	for (int i = 0; i < 2; i++)
	{				
		shoulder[0]->Translate1X = 0.750f;
		shoulder[0]->Translate1Y = 0.75f;
		shoulder[0]->Translate1Z = 0.0f;

		shoulder[1]->Translate1X = -1.5f;
		shoulder[1]->Translate1Y = 0.0f;
		shoulder[1]->Translate1Z = 0.0f;
		
		shoulder[i]->rotateVar1 = -34;
		shoulder[i]->rotateVar2 = 34;
		

		shoulder[i]->rotateR1X = 0.0f;
		shoulder[i]->rotateR1Y = 1.0f;
		shoulder[i]->rotateR1Z = 0.0f;

		shoulder[i]->rotateR2X = 0.0f;
		shoulder[i]->rotateR2Y = 0.0f;
		shoulder[i]->rotateR2Z = 1.0f;

		shoulder[i]->Sphere1Param1 = 0.2f;
		shoulder[i]->Sphere1Param2 = 30;
		shoulder[i]->Sphere1Param3 = 30;

		shoulder[0]->Translate2X = 0.55f;
		shoulder[1]->Translate2X = -0.55f;
		shoulder[i]->Translate2Y = 0.0f;
		shoulder[i]->Translate2Z = 0.0f;

		shoulder[i]->Scale1X = 1.25f;
		shoulder[i]->Scale1Y = 0.5f;
		shoulder[i]->Scale1Z = 1.0f;

		shoulder[i]->Sphere2Param1 = 0.5f;
		shoulder[i]->Sphere2Param2 = 10;
		shoulder[i]->Sphere2Param3 = 10;

		shoulder[i]->isUpBody = true;
		shoulder[i]->isUpJoint = true;
		shoulder[i]->isheadSphere = false;
	}
	
	

	/*-------------------------- shoulder [END] -----------------------------------------*/
	
	elbo[0] = (robo *)malloc(sizeof robo);
	elbo[1] = (robo *)malloc(sizeof robo);

	for (int i = 0; i < 2; i++)
	{
		elbo[0]->Translate1X = 1.25f;
		elbo[1]->Translate1X = -1.3f;
		elbo[i]->Translate1Y = 0.0f;
		elbo[i]->Translate1Z = 0.0f;

		elbo[i]->rotateVar1 = 0;
		elbo[i]->rotateVar2 = 0;
		elbo[i]->rotateR1X = 0.0f;
		elbo[i]->rotateR1Y = 1.0f;
		elbo[i]->rotateR1Z = 0.0f;

		elbo[i]->rotateR2X = 0.0f;
		elbo[i]->rotateR2Y = 0.0f;
		elbo[i]->rotateR2Z = 1.0f;

		elbo[i]->Sphere1Param1 = 0.2f;
		elbo[i]->Sphere1Param2 = 30;
		elbo[i]->Sphere1Param3 = 30;

		elbo[0]->Translate2X = 0.55f;
		elbo[1]->Translate2X = -0.55f;
		elbo[i]->Translate2Y = 0.0f;
		elbo[i]->Translate2Z = 0.0f;

		elbo[i]->Scale1X = 1.5f;
		elbo[i]->Scale1Y = 0.35f;
		elbo[i]->Scale1Z = 1.0f;

		elbo[i]->Sphere2Param1 = 0.5f;
		elbo[i]->Sphere2Param2 = 10;
		elbo[i]->Sphere2Param3 = 10;
		
		elbo[i]->isUpBody = true;
		elbo[i]->isUpJoint = false;
		elbo[i]->isheadSphere = false;
	}	
	
	//thai[0] is Left Thai, thai[1] is Right Thai
	thai[0] = (robo *)malloc(sizeof robo);
	thai[1] = (robo *)malloc(sizeof robo);
	for (int i = 0; i < 2; i++)
	{				
		thai[0]->Translate1X = -0.5f;
		thai[1]->Translate1X = 1.002f;
		thai[0]->Translate1Y = -2.10f;
		thai[1]->Translate1Y = -0.005f;
		thai[i]->Translate1Z = 0.0f;

		thai[i]->rotateVar1 = 0;
		thai[i]->rotateVar2 = 0;
		thai[i]->rotateR1X = 1.0f;
		thai[i]->rotateR1Y = 0.0f;
		thai[i]->rotateR1Z = 0.0f;

		thai[i]->rotateR2X = 0.0f;
		thai[i]->rotateR2Y = 0.0f;
		thai[i]->rotateR2Z = 1.0f;

		thai[i]->Scale1X = 0.5f;
		thai[i]->Scale1Y = 1.5f;
		thai[i]->Scale1Z = 1.0f;

		thai[i]->Sphere1Param1 = 0.5f;
		thai[i]->Sphere1Param2 = 30;
		thai[i]->Sphere1Param3 = 30;	

		thai[i]->isUpBody = false;
		thai[i]->isUpJoint = true;
		thai[i]->isheadSphere = false;
	}	

	nee[0] = (robo *)malloc(sizeof robo);
	nee[1] = (robo *)malloc(sizeof robo);
	for (int i = 0; i < 2; i++)
	{				
		nee[i]->Translate1X = 0.0f;				
		nee[i]->Translate1Y = -0.8f;
		nee[i]->Translate1Z = 0.0f;

		nee[i]->rotateVar1 = 0;
		nee[i]->rotateVar2 = 0;
		nee[i]->rotateR1X = 1.0f;
		nee[i]->rotateR1Y = 0.0f;
		nee[i]->rotateR1Z = 0.0f;

		nee[i]->rotateR2X = 0.0f;
		nee[i]->rotateR2Y = 0.0f;
		nee[i]->rotateR2Z = 1.0f;

		nee[i]->Sphere1Param1 = 0.2f;
		nee[i]->Sphere1Param2 = 30;
		nee[i]->Sphere1Param3 = 30;

		nee[i]->Translate2X = 0.0f;
		nee[i]->Translate2Y = -0.82f;
		nee[i]->Translate2Z = 0.0f;

		nee[i]->Scale1X = 0.5f;
		nee[i]->Scale1Y = 1.85f;
		nee[i]->Scale1Z = 1.0f;

		nee[i]->Sphere2Param1 = 0.5f;
		nee[i]->Sphere2Param2 = 20;
		nee[i]->Sphere2Param3 = 20;

		nee[i]->isUpBody = false;
		nee[i]->isUpJoint = false;
		nee[i]->isheadSphere = false;
	}	

	headSphere[0] = (robo *)malloc(sizeof robo);
	headSphere[0]->Translate1X = 0.75f;
	headSphere[0]->Translate1Y = 0.9f;
	headSphere[0]->Translate1Z = 0.0f;
	

	headSphere[0]->rotateVar1 = 0;
	headSphere[0]->rotateVar2 = 0;
	headSphere[0]->rotateR1X = 1.0f;
	headSphere[0]->rotateR1Y = 0.0f;
	headSphere[0]->rotateR1Z = 0.0f;

	headSphere[0]->rotateR2X = 0.0f;
	headSphere[0]->rotateR2Y = 0.0f;
	headSphere[0]->rotateR2Z = 1.0f;

	headSphere[0]->Scale1X = 1.0f;
	headSphere[0]->Scale1Y = 1.0f;
	headSphere[0]->Scale1Z = 1.0f;

	headSphere[0]->Sphere1Param1 = 0.2f;
	headSphere[0]->Sphere1Param2 = 30;
	headSphere[0]->Sphere1Param3 = 30;

	headSphere[0]->Translate2X = 0.0f;
	headSphere[0]->Translate2Y = 0.55f;
	headSphere[0]->Translate2Z = 0.0f;

	headSphere[0]->Sphere2Param1 = 0.6f;
	headSphere[0]->Sphere2Param2 = 30;
	headSphere[0]->Sphere2Param3 = 30;

	headSphere[0]->isUpBody = true;
	headSphere[0]->isUpJoint = true;
	headSphere[0]->isheadSphere = true;

	head = shoulder[0];
	shoulder[0]->next = elbo[0];
	elbo[0]->next = shoulder[1];
	shoulder[1]->next = elbo[1];
	elbo[1]->next = headSphere[0];
	headSphere[0]->next = thai[0];
	thai[0]->next = nee[0];
	nee[0]->next = thai[1];
	thai[1]->next = nee[1];
	nee[1]->next = NULL;	
}


//global function declarations
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lparam);


//global variable declarations
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
bool gbFullscreen = false;
HWND ghwnd = NULL;
FILE *GpFile = NULL;
bool gbActiveWindow = false;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

int Stmouch1 = 0,Stmouch2 = 0,Stmouch3 = 0;
//delete me// int Head1 = 0, Head2 = 0;
bool Stmouch_Without_Legs = false;
GLuint BackGround_Texture;
GLUquadric *quadric = NULL;

//Winmain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Function declarations
	void initilize(void);
	void display(void);


	//Variable declaratios
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("Abhshek Shah");
	bool bDone = false;



	if (fopen_s(&GpFile, "AS_Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Can not create desired file."), ERROR, MB_OK);
		exit(0);
	}
	else
	{
		fprintf(GpFile, "Logfile successfully created and program started sucessfully\n");
	}

	//code
	//iniatialization of WNDCLASSEX
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ADS_ICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(ADS_ICON));

	//register above class
	RegisterClassEx(&wndclass);
	RECT rect;
	bool return_value = SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	if (!return_value)
	{
		MessageBox(NULL, TEXT("SystemParametersInfo returns false"), TEXT("EROR"), MB_OK);
		exit(0);
	}
	LONG workarea_width = rect.right - rect.left;
	LONG workarea_hight = rect.bottom - rect.top;

	//create window
	hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		szAppName,
		TEXT("Abhishek Shah"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		((workarea_width - WIN_WIDTH) / 2) + rect.left,
		((workarea_hight - WIN_HIGHT) / 2) + rect.top,
		WIN_WIDTH,
		WIN_HIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;
	initilize();
	ShowWindow(hwnd, iCmdShow);

	//UpdateWindow(hwnd);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	
	//Game loop
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == true)
			{
				//Here you should call update function in OpenGL

				//Here you should call display function in OpenGL
				display();

			}
		}
	}



	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lparam)
{
	//local function declarations
	void TogleFullscreen(void);
	void resize(int, int);
	void unInitilize(void);
	void display(void);




	//code
	switch (iMsg)
	{

	case WM_SETFOCUS:
		gbActiveWindow = true;
		break;
	case WM_KILLFOCUS:
		gbActiveWindow = false;
		break;
	case WM_ERASEBKGND:
		return(0);
	case WM_SIZE:
		resize(LOWORD(lparam), HIWORD(lparam));
		break;
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		case(0x46):
		case(0x66):
			TogleFullscreen();
			break;
		default:
			break;
		}
		break;
	}

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		fprintf(GpFile, "\n\nJAY HIND!!!");
		unInitilize();
		PostQuitMessage(0);
		break;
	case WM_CHAR: {
		switch (wParam) {
		//Stmouch Keys
		case'A':
		{
			Stmouch1 = (Stmouch1 + 3) % 360;
			break;
		}
		case'a': {

			Stmouch1 = (Stmouch1 - 3) % 360;
			break;
		}
		case'B': {
			Stmouch2 = (Stmouch2 + 3) % 360;
			break;
		}
		case'b': {
			Stmouch2 = (Stmouch2 - 3) % 360;
			break;
		}
		case'W': {
			Stmouch3 = (Stmouch3 + 3) % 360;
			break;
		}
		case'w': {
			Stmouch3 = (Stmouch3 - 3) % 360;
			break;
		}
		//Head Keys
		case'C':
		{
			headSphere[0]->rotateVar1 = (headSphere[0]->rotateVar1 + 3) % 360;
			break;
		}
		case'c': {

			headSphere[0]->rotateVar1 = (headSphere[0]->rotateVar1 - 3) % 360;
			break;
		}
		case'D': {
			headSphere[0]->rotateVar2 = (headSphere[0]->rotateVar2 + 3) % 360;
			break;
		}
		case'd': {
			headSphere[0]->rotateVar2 = (headSphere[0]->rotateVar2 - 3) % 360;
			break;
		}
				 //Shoulder Right Keys
		case'E':
		{
			shoulder[0]->rotateVar1 = (shoulder[0]->rotateVar1 + 3) % 360;
			break;
		}
		case'e': {

			shoulder[0]->rotateVar1 = (shoulder[0]->rotateVar1 - 3) % 360;
			break;
		}
		//case'G': {
		//	shoulder[0]->rotateVar2 = (shoulder[0]->rotateVar2 + 3) % 360;
		//	fprintf(GpFile, "Press G shoulder[0]->rotateVar2 : %d\n", shoulder[0]->rotateVar2);
		//	break;
		//}
		//case'g': {
		//	shoulder[0]->rotateVar2 = (shoulder[0]->rotateVar2 - 3) % 360;
		//	fprintf(GpFile, "Press g shoulder[0]->rotateVar2 : %d\n", shoulder[0]->rotateVar2);
		//	break;
		//}
				 //Shoulder Left Keys
		//case'H':
		//{
		//	shoulder[1]->rotateVar1 = (shoulder[1]->rotateVar1 + 3) % 360;
		//	fprintf(GpFile, "Press H shoulder[1]->rotateVar1 : %d\n", shoulder[1]->rotateVar1);
		//	break;
		//}
		//case'h': {
		//
		//	shoulder[1]->rotateVar1 = (shoulder[1]->rotateVar1 - 3) % 360;
		//	fprintf(GpFile, "Press h shoulder[1]->rotateVar1 : %d\n", shoulder[1]->rotateVar1);
		//	break;
		//}
		case'I': {
			shoulder[1]->rotateVar2 = (shoulder[1]->rotateVar2 + 3) % 360;
			break;
		}
		case'i': {
			shoulder[1]->rotateVar2 = (shoulder[1]->rotateVar2 - 3) % 360;
			break;
		}
				 //Elbo Right Keys
		case'J':
		{
			elbo[0]->rotateVar1 = (elbo[0]->rotateVar1 + 3) % 360;
			break;
		}
		case'j': {

			elbo[0]->rotateVar1 = (elbo[0]->rotateVar1 - 3) % 360;
			break;
		}
		case'K': {
			elbo[0]->rotateVar2 = (elbo[0]->rotateVar2 + 3) % 360;
			break;
		}
		case'k': {
			elbo[0]->rotateVar2 = (elbo[0]->rotateVar2 - 3) % 360;
			break;
		}
				 //Elbo Left Keys
		case'L':
		{
			elbo[1]->rotateVar1 = (elbo[1]->rotateVar1 + 3) % 360;
			break;
		}
		case'l': {

			elbo[1]->rotateVar1 = (elbo[1]->rotateVar1 - 3) % 360;
			break;
		}
		case'M': {
			elbo[1]->rotateVar2 = (elbo[1]->rotateVar2 + 3) % 360;
			break;
		}
		case'm': {
			elbo[1]->rotateVar2 = (elbo[1]->rotateVar2 - 3) % 360;
			break;
		}
				 //Thai Right Keys
		case'N':
		{
			thai[1]->rotateVar1 = (thai[1]->rotateVar1 + 3) % 360;
			break;
		}
		case'n': {

			thai[1]->rotateVar1 = (thai[1]->rotateVar1 - 3) % 360;
			break;
		}
		case'O': {
			thai[1]->rotateVar2 = (thai[1]->rotateVar2 + 3) % 360;
			break;
		}
		case'o': {
			thai[1]->rotateVar2 = (thai[1]->rotateVar2 - 3) % 360;
			break;
		}
				 //Thai Left Keys
		case'P':
		{
			thai[0]->rotateVar1 = (thai[0]->rotateVar1 + 3) % 360;
			break;
		}
		case'p': {

			thai[0]->rotateVar1 = (thai[0]->rotateVar1 - 3) % 360;
			break;
		}
		case'Q': {
			thai[0]->rotateVar2 = (thai[0]->rotateVar2 + 3) % 360;
			break;
		}
		case'q': {
			thai[0]->rotateVar2 = (thai[0]->rotateVar2 - 3) % 360;
			break;
		}
				 //Nee Right Keys
		case'R':
		{
			nee[1]->rotateVar1 = (nee[1]->rotateVar1 + 3) % 360;
			break;
		}
		case'r': {

			nee[1]->rotateVar1 = (nee[1]->rotateVar1 - 3) % 360;
			break;
		}
		case'T': {
			nee[1]->rotateVar2 = (nee[1]->rotateVar2 + 3) % 360;
			break;
		}
		case't': {
			nee[1]->rotateVar2 = (nee[1]->rotateVar2 - 3) % 360;
			break;
		}
				 //Elbo Left Keys
		case'U':
		{
			nee[0]->rotateVar1 = (nee[0]->rotateVar1 + 3) % 360;
			break;
		}
		case'u': {

			nee[0]->rotateVar1 = (nee[0]->rotateVar1 - 3) % 360;
			break;
		}
		case'V': {
			nee[0]->rotateVar2 = (nee[0]->rotateVar2 + 3) % 360;
			break;
		}
		case'v': {
			nee[0]->rotateVar2 = (nee[0]->rotateVar2 - 3) % 360;
			break;
		}
		//Leg attachment with Stmouch
		case 'Y':
		case 'y':
		{
			if (Stmouch_Without_Legs == true) {
				Stmouch_Without_Legs = false;
				break;
			}
			else {
				Stmouch_Without_Legs = true;
				break;
			}
		}
		default:
			break;
		}
	}
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lparam));
}



void TogleFullscreen(void)
{
	//loocal variable declarations
	MONITORINFO mi = { sizeof(MONITORINFO) };

	//code
	if (gbFullscreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle&WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, (dwStyle&~WS_OVERLAPPEDWINDOW));
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, (mi.rcMonitor.right - mi.rcMonitor.left), (mi.rcMonitor.bottom - mi.rcMonitor.left), SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(false);
		gbFullscreen = true;
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, (dwStyle | WS_OVERLAPPEDWINDOW));
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED));
		ShowCursor(true);
		gbFullscreen = false;
	}
}

void initilize(void)
{
	//Function declaration
	void resize(int, int);
	bool loadGlTexture(GLuint *texture, TCHAR(resourceID[]));

	//Variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixeleFormateIndex;
	//Code
	PlaySound(MAKEINTRESOURCE(ADS_SONG), NULL, SND_FILENAME | SND_ASYNC | SND_RESOURCE);
	ghdc = GetDC(ghwnd);
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;

	iPixeleFormateIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixeleFormateIndex == 0)
	{
		fprintf(GpFile, "Erroe function iPixeleFormateIndex fail");
		DestroyWindow(ghwnd);
	}
	if (SetPixelFormat(ghdc, iPixeleFormateIndex, &pfd) == FALSE)
	{
		fprintf(GpFile, "Erroe function SetPixelFormat fail");
		DestroyWindow(ghwnd);
	}
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		fprintf(GpFile, "Erroe function wglCreateContext wglMakeCurrent fail");
		DestroyWindow(ghwnd);
	}
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(GpFile, "Erroe function wglMakeCurrent fail");
		DestroyWindow(ghwnd);
	}
	
	//Depth Functions
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	

	//FONT
	HFONT hFont;
	GLYPHMETRICSFLOAT agmf[128];
	LOGFONT logfont;
	logfont.lfHeight = -30;
	logfont.lfWidth = 0;
	logfont.lfEscapement = 0;
	logfont.lfOrientation = 0;
	logfont.lfWeight = FW_BOLD;
	logfont.lfItalic = FALSE;
	logfont.lfUnderline = FALSE;
	logfont.lfStrikeOut = FALSE;
	logfont.lfCharSet = ANSI_CHARSET;
	logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfQuality = DEFAULT_QUALITY;
	logfont.lfPitchAndFamily = DEFAULT_PITCH;
	// Create the font and display list
	hFont = CreateFontIndirect(&logfont);
	SelectObject(ghdc, hFont);
	nFontList = glGenLists(128);
	wglUseFontBitmaps(ghdc, 0, 128, nFontList);
	DeleteObject(hFont);

	//Texture initilizetion code
	loadGlTexture(&BackGround_Texture, MAKEINTRESOURCE(BACKGROUND_BITMAP));

	glEnable(GL_TEXTURE_2D);

	initializeRoboStruct();

	//Set Clear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	resize(WIN_WIDTH, WIN_HIGHT); //warmup call to resize
}

bool loadGlTexture(GLuint *texture, TCHAR(resourceID[]))
{
	//Variable declarartion
	bool bResult = false;
	HBITMAP hBitmap = NULL;
	BITMAP bmp;
	//Code
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL),
		resourceID,
		IMAGE_BITMAP,
		0,
		0,
		LR_CREATEDIBSECTION);
	if (hBitmap)
	{
		bResult = true;
		GetObject(hBitmap, sizeof(BITMAP), &bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);
		//setting parameter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//floowing call will push data in gmem with use of Gdriver
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
		DeleteObject(hBitmap);
	}


	return bResult;
}

void resize(int width, int hight)
{
	//Code
	if (hight == 0)
	{
		hight = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)hight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)hight, 0.1f, 100.0f);
}
void Quote(void)
{
	glColor3f(1.0f, 0.0f, 0.0f);
	glRasterPos2i(0, 0);
	glListBase(nFontList);
	glCallLists(23, GL_UNSIGNED_BYTE, "ASTROMEDICOMP Presents");

}

void Quote1(void)
{
	glColor3f(0.0f, 0.0f, 1.0f);
	glRasterPos2i(0, 0);
	glListBase(nFontList);
	glCallLists(9, GL_UNSIGNED_BYTE, "Thankyou");

}
void drawRobo(robo* temp)
{
	if (temp->isUpJoint == true)
	{
		glTranslatef(temp->Translate1X, temp->Translate1Y, temp->Translate1Z); // Shoulder's sphere's center	
		glPushMatrix(); // Save above Translate location
		glRotatef((GLfloat)temp->rotateVar1, temp->rotateR1X, temp->rotateR1Y, temp->rotateR1Z);
		glRotatef((GLfloat)temp->rotateVar2, temp->rotateR2X, temp->rotateR2Y, temp->rotateR2Z);
		//glTranslatef(1.0f, 0.0f, 0.0f);
		glPushMatrix(); //save above location of translate and rotate
		if (temp->isUpBody == true)
		{
			quadric = gluNewQuadric();
			gluSphere(quadric, temp->Sphere1Param1, temp->Sphere1Param2, temp->Sphere1Param3);// Round spheare
			glTranslatef(temp->Translate2X, temp->Translate2Y, temp->Translate2Z);//
			glScalef(temp->Scale1X, temp->Scale1Y, temp->Scale1Z);
			//glColor3f(0.5f, 0.35f, 0.05f);
			glColor3f(0.4f, 0.9f, 1.0f);
			quadric = gluNewQuadric();
			gluSphere(quadric, temp->Sphere2Param1, temp->Sphere2Param2, temp->Sphere2Param3);
		}
		else
		{
			glScalef(temp->Scale1X, temp->Scale1Y, temp->Scale1Z);
			//glColor3f(0.5f, 0.35f, 0.05f);
			glColor3f(0.4f, 0.9f, 1.0f);
			quadric = gluNewQuadric();
			gluSphere(quadric, temp->Sphere1Param1, temp->Sphere1Param2, temp->Sphere1Param3);
		}
		glPopMatrix(); // Return to glTranslatef(0.25f, 0.75f, 0.0f);
		if (temp->isheadSphere == true)
		{
			glPopMatrix();
		}
	}

	else
	{
		//Right Elbo
		glTranslatef(temp->Translate1X, temp->Translate1Y, temp->Translate1Z); //Translate from glTranslatef(0.25f, 0.75f, 0.0f);
		glRotatef((GLfloat)temp->rotateVar1, temp->rotateR1X, temp->rotateR1Y, temp->rotateR1Z);
		glRotatef((GLfloat)temp->rotateVar2, temp->rotateR2X, temp->rotateR2Y, temp->rotateR2Z);
		//glTranslatef(1.0f, 0.0f, 0.0f);
		glPushMatrix(); //Save above translate and Rotate
		glColor3f(0.4f, 0.9f, 1.0f); //HS://
		quadric = gluNewQuadric();
		gluSphere(quadric, temp->Sphere1Param1, temp->Sphere1Param2, temp->Sphere1Param3);// Round spheare
		glTranslatef(temp->Translate2X, temp->Translate2Y, temp->Translate2Z);//
		glScalef(temp->Scale1X, temp->Scale1Y, temp->Scale1Z);
		//glColor3f(0.5f, 0.35f, 0.05f);
		glColor3f(0.4f, 0.9f, 1.0f);
		quadric = gluNewQuadric();
		gluSphere(quadric, temp->Sphere2Param1, temp->Sphere2Param2, temp->Sphere2Param3);
		glPopMatrix(); //return to glTranslatef(0.25f, 0.0f, 0.0f);
		glPopMatrix(); //return to glTranslatef(0.25f, 0.75f, 0.0f);
	}
}

void display(void)
{	
	//Function declaration
	void seen1(void);
	void Seen2(void);
	void seen3(void);
	GLfloat UpdateAngle(GLfloat angle);
	static GLfloat Time = 0.0f;

	if (Time <= 10);
	{
		seen1();
	}
	glColor3f(1.0f, 1.0f, 1.0f);
	if (Time >= 10 && Time <= 60)
	{
		Seen2();
	}

	if (Time >= 60 && Time <= 70)
	{
		seen3();
	}

	if (Time >= 70)
	{
		DestroyWindow(ghwnd);
	}
	
	Time = UpdateAngle(Time);
	SwapBuffers(ghdc);
}

void seen1(void)
{
	//Function declaration
	void Quote(void);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-1.0f, 0.0f, -12.0f);
	Quote();
}

void seen3(void)
{
	//Function declaration
	void Quote1(void);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(-1.0f, 0.0f, -12.0f);
	Quote1();
}

void Seen2(void)
{
	//Function declaration
	int shoulderX1(int value);
	int shoulderX2(int value);
	void Bird2(void);
	
	//Variable declaration
	GLfloat CubeVertex = 13.0f;

	//Texturecoordinate
	//CubeglTexCoord2f
	GLfloat CubeLeftXBottom = 0.0f;
	GLfloat CubeLeftYBottom = 0.0f;
	GLfloat CubeRightXBottom = 1.0f;
	GLfloat CubeRightYBottom = 0.0f;
	GLfloat CubeLeftXTop = 0.0f;
	GLfloat CubeLeftYTop = 1.0f;
	GLfloat CubeRightXTop = 1.0f;
	GLfloat CubeRightYTop = 1.0f;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.0f, 0.0f, -12.0f);

	
	glBindTexture(GL_TEXTURE_2D, BackGround_Texture);
	glBegin(GL_QUADS);
	{
		//Back
		//glColor3f(1.0f, 1.0f, 1.0f);
		glTexCoord2f(CubeRightXTop, CubeRightYTop);
		glVertex3f(CubeVertex * 3, CubeVertex, -CubeVertex);
		glTexCoord2f(CubeLeftXTop, CubeLeftYTop);
		glVertex3f(-CubeVertex * 3, CubeVertex, -CubeVertex);
		glTexCoord2f(CubeLeftXBottom, CubeLeftYBottom);
		glVertex3f(-CubeVertex * 3, -CubeVertex, -CubeVertex);
		glTexCoord2f(CubeRightXBottom, CubeRightYBottom);
		glVertex3f(CubeVertex * 3, -CubeVertex, -CubeVertex);
		glEnd();
	}




	//code

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -12.0f);


	//Main Sphere Stmouch
	glPushMatrix(); //AT ORIGIN
	glRotatef((GLfloat)Stmouch1, 1.0f, 0.0f, 0.0f);
	glRotatef((GLfloat)Stmouch2, 0.0f, 1.0f, 0.0f);
	glRotatef((GLfloat)Stmouch3, 0.0f, 0.0f, 1.0f);
	//glTranslatef(1.0f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(1.0f, 1.85f, 1.0f);
	//glColor3f(0.5f, 0.35f, 0.05f);
	glColor3f(0.4f, 0.9f, 1.0f);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.85f, 30, 30);
	glPopMatrix(); //Return at Origin

	head = shoulder[0];
	for (head; head != headSphere[0]->next; head = head->next)
	{
		drawRobo(head);
	}

	if (Stmouch_Without_Legs == true)
	{
		glPopMatrix(); //for test need to remove
		thai[0]->Translate1Y = -2.10f;
	}
	else
	{
		thai[0]->Translate1Y = -3.80f;
	}

	robo* tempNode = tempNode = thai[0];

	for (tempNode; tempNode != NULL; tempNode = tempNode->next)
	{
		drawRobo(tempNode);
	}

	glPopMatrix(); //return to origin

	//Human translation Starts
	shoulder[0]->rotateVar2 = -60;
	shoulder[1]->rotateVar2 = 60;



	elbo[1]->rotateVar1 = shoulderX1(elbo[1]->rotateVar1);
	elbo[0]->rotateVar1 = shoulderX1(elbo[0]->rotateVar1);
	Stmouch2 = shoulderX1(Stmouch2);
	nee[1]->rotateVar1 = shoulderX1(nee[1]->rotateVar1);
	nee[0]->rotateVar1 = shoulderX1(nee[0]->rotateVar1);
	//Human translation Ends

   ////Tree and Bird
	Bird2();
	////Tree

	glMatrixMode(GL_MODELVIEW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLoadIdentity();
	glTranslatef(-2.5f, 0.0f, -6.0f);
	glBegin(GL_TRIANGLES);
	{
		glColor3f(0.13f, 0.57f, 0.31f);
		glVertex3f(0.0f, 0.5f, 0.0f);
		glVertex3f(-0.5f, -0.25f, 0.0f);
		glVertex3f(0.5f, -0.25f, 0.0f);

		glEnd();
	}

	glBegin(GL_QUADS);
	{
		glVertex3f(0.25f, -0.25f, 0.0f);
		glVertex3f(-0.25f, -0.25f, 0.0f);
		glVertex3f(-0.75f, -0.75f, 0.0f);
		glVertex3f(0.75f, -0.75f, 0.0f);

		glVertex3f(0.5f, -0.75f, 0.0f);
		glVertex3f(-0.5f, -0.75f, 0.0f);
		glVertex3f(-1.0f, -1.25f, 0.0f);
		glVertex3f(1.0f, -1.25f, 0.0f);

		glColor3f(0.647059f, 0.164706f, 0.164706f);
		glVertex3f(0.20f, -1.25f, 0.0f);
		glVertex3f(-0.20f, -1.25f, 0.0f);
		glVertex3f(-0.20f, -2.35f, 0.0f);
		glVertex3f(0.20f, -2.35f, 0.0f);

		glEnd();
	}


	//Bired

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(6.5f, -3.5f, -12.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	glTranslatef(-0.8f, 0.25f, 0.0f);
	glColor3f(0.576471f, 0.858824f, 0.439216f);
	gluSphere(quadric, 0.2f, 30, 30);// Round spheare
	glTranslatef(-0.3f, -0.17f, 0.0f);
	glRotatef(30, 0.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.15f, -0.15f, 0.0f);
		glVertex3f(0.15f, 0.15f, 0.0f);

		glEnd();
		glColor3f(1.0f, 1.0f, 1.0f);
	}

	glLoadIdentity();
	glTranslatef(6.5f, -3.5f, -12.0f);
	glScalef(1.5f, 1.0f, 1.0f);
	quadric = gluNewQuadric();
	glColor3f(0.419608f, 0.137255f, 0.556863f);
	gluSphere(quadric, 0.4f, 30, 30);// Round spheare
	glTranslatef(0.35f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLES);
	{

		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.25f, -0.25f, 0.0f);
		glVertex3f(0.25f, 0.25f, 0.0f);

		glEnd();

	}

	glLoadIdentity();
	glTranslatef(6.5f, -3.5f, -12.0f);
	glLineWidth(8);
	glBegin(GL_LINES);
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-1.0f, 0.27f, 0.0f);
		glVertex3f(-0.4f, 0.1f, 0.0f);

		glEnd();
	}

	glLineWidth(3);
	glBegin(GL_LINES);
	{

		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.2f, -0.8f, 0.0f);

		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.2f, -0.8f, 0.0f);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.2f, -0.8f, 0.0f);
		glVertex3f(0.3f, -1.0f, -0.3f);

		glVertex3f(0.2f, -0.8f, 0.0f);
		glVertex3f(-0.0f, -1.0f, -0.3f);

		glVertex3f(0.2f, -0.8f, 0.0f);
		glVertex3f(0.2f, -1.0f, 0.3f);

		//
		glVertex3f(-0.2f, -0.8f, 0.0f);
		glVertex3f(-0.3f, -1.0f, -0.3f);

		glVertex3f(-0.2f, -0.8f, 0.0f);
		glVertex3f(-0.02f, -1.0f, -0.3f);

		glVertex3f(-0.2f, -0.8f, 0.0f);
		glVertex3f(-0.2f, -1.0f, 0.3f);



		glEnd();
	}
	glPointSize(5.0f);
	glTranslatef(-0.3f, 0.15f, 0.0f);
	glBegin(GL_POINTS);
	{
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.8f, 0.25f, 0.5f);

		glColor3f(1.0f, 1.0f, 1.0f);
		glEnd();
	}

	

}

void Bird2(void)
{
	//Function Declaration
	GLfloat UpdateMainAngle(GLfloat angle);
	GLfloat UpdateXCorridante(GLfloat angle);
	//Variabledeclaration
	static GLfloat MainAngle = 0.0f;
	static GLfloat XCorridanate = 4.0f;
	//code
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 1.0f, -12.0f);
	glPushMatrix();
	glRotatef(MainAngle, 1.0f, 0.0f, 0.0f);
	glTranslatef(XCorridanate, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(1.0f, 0.5f, 1.0f);
	glColor3f(0.576471f, 0.858824f, 0.439216f);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.5f, 10, 10);
	glPopMatrix();

	glTranslatef(1.0f, 0.0f, 0.0f);
	//glRotatef((GLfloat)elbo, 0.0f, 0.0f, 1.0f);
	glTranslatef(-1.7f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 0.5f, 0.5f);
	glColor3f(0.5f, 0.35f, 0.05f);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.5f, 10, 10);
	glPopMatrix();

	glTranslatef(0.5f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 0.5f, 0.5f);
	glColor3f(0.5f, 0.35f, 0.05f);
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(2.0f, 0.0f, 0.0f);

		glEnd();
	}
	glPopMatrix();

	glTranslatef(0.0f, 0.1f, 0.0f);
	glPushMatrix();
	glScalef(0.5f, 0.5f, 0.5f);
	glColor3f(0.5f, 0.35f, 0.05f);
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(2.0f, 0.0f, 0.0f);

		glEnd();
	}
	glPopMatrix();



	glPopMatrix();

	MainAngle = UpdateMainAngle(MainAngle);
	XCorridanate = UpdateXCorridante(XCorridanate);
}

GLfloat UpdateXCorridante(GLfloat angle)
{
	angle = angle - 0.035f;

	return(angle);
}

GLfloat UpdateMainAngle(GLfloat angle)
{
	angle = angle + 0.5f;
	if (angle >= 360.0f)
	{
		angle = 0.0f;
	}
	return(angle);
}

GLfloat UpdateAngle(GLfloat angle)
{
	angle = angle + 0.05f;
	if (angle >= 360.0f)
	{
		angle = 0.0f;
	}
	return(angle);
}

int shoulderX1(int value)
{
	
	
	if (value <= 10.0)
	{
		value = value - 1;
		return(value);
	}
	if (value >= -10)
	{
		value = value - 1;
		return(value);
	}
	
	
}
int UpdateSto(int value)
{
	if (value <= 10.0)
	{
		value = value - 1;
		return(value);
	}
	if (value >= -10)
	{
		value = value - 1;
		return(value);
	}
}

void unInitilize(void)
{

	//code
	if (gbFullscreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, (dwStyle | WS_OVERLAPPEDWINDOW));
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED));
		ShowCursor(true);
	}
	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}
	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}
	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	glDeleteTextures(1, &BackGround_Texture);
	gluDeleteQuadric(quadric);
	//TextFile close
	if (GpFile)
	{
		fclose(GpFile);
		GpFile = NULL;
	}
}