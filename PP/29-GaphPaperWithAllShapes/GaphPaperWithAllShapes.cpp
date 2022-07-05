//Headers
#include<windows.h>
#include<stdio.h>
#include<gl/glew.h> //This line must be before GL.h
#include<gl/GL.h>
#include<vmath.h> //for Matrix calclulation
#include"Image.h"

#define WIN_WIDTH 800
#define WIN_HIGHT 600

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")

using namespace vmath;

enum
{
	ADS_ATTRUBUTE_VERTEX_Position = 0,
	ADS_ATTRUBUT_COLORS,
	ADS_ATTRIBUTE_NORMAL,
	ADS_ATTRIBUTE_TEXTURE0,
};

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

//Variables for Shaders
GLuint gShaderProgramObject;
GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;

GLuint vaoTriangleLineLoop;
GLuint vboTriangleLineLoopPosition;
GLuint vboTriangleLineLoopColor;

GLuint vaoRectangleLineLoop;
GLuint vboRectangleLineLoopPosition;
GLuint vboRectangleLineLoopColor;

GLuint vaoCircleLineLoop;
GLuint vboCircleLineLoopPosition;
GLuint vboCircleLineLoopColor;

GLuint vaoLine;
GLuint vboLinePosition;
GLuint vboLineColor;

GLuint mvpUniform;
mat4 perspectiveProjectionMatrix;

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
	void display(void);
	void unInitilize(void);

	//Variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixeleFormateIndex;
	
	//Code
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

	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		fprintf(GpFile, "Expaction Enableing process fails");
		DestroyWindow(ghwnd);
	}

	//OpenGL related Logs
	fprintf(GpFile, "\n OpenGL Veoncor : %s\n", glGetString(GL_VENDOR));
	fprintf(GpFile, "\n OpenGL rendorer : %s\n", glGetString(GL_RENDERER));
	fprintf(GpFile, "\n OpenGL Version : %s\n", glGetString(GL_VERSION));
	fprintf(GpFile, "\n OpenGL GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION)); //GLSL = Graphic Library Shading Languange

	//AS// //OpenGl Enable Extenations
	//AS// GLint numExt;
	//AS// glGetIntegerv(GL_NUM_EXTENSIONS,&numExt);
	//AS// for (int i = 0; i < numExt; i++)
	//AS// {
	//AS// 	fprintf(GpFile, "\n  : %s : %d\n", glGetString(GL_EXTENSIONS),i);
	//AS// }

	//*** Vertex Shader ***
	//Create Shader
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//Provide Source code to Shader
	const GLchar *vertexShaderSourceCode =
		"#version 460 core"
		"\n"
		"in vec4 vPosition;"
		"in vec4 vColor;"
		"uniform mat4 u_mvp_matrix;"
		"out vec4 out_color;"
		"void main(void)"
		"{"
		"gl_Position = u_mvp_matrix * vPosition;"
		"out_color = vColor;"
		"}";
	glShaderSource(gVertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

	//Compile Shader
	glCompileShader(gVertexShaderObject);
	//Should be complition error checking
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char* szInfoLog = NULL;
	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(GpFile, "Vertex Shader compilatin Log : %s\n", szInfoLog);
				free(szInfoLog);
				unInitilize();
				exit(0);
			}
		}
	}

	//*** Fragment Shader ***
	//Create Shader
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//ProvideSource code to Shader
	const GLchar *fragmentShaderSourceCode =
		"#version 460 core" \
		"\n"
		"in vec4 out_color;"
		"out vec4 fragcolor;"
		"void main(void)"
		"{"
		"fragcolor = out_color;"
		"}";

	glShaderSource(gFragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);

	//Compile Shader
	glCompileShader(gFragmentShaderObject);

	//Should be complition error checking
	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(GpFile, "Vertex Shader compilatin Log : %s\n", szInfoLog);
				free(szInfoLog);
				unInitilize();
				exit(0);
			}
		}
	}

	//*** Shader Program ***
	//Create
	gShaderProgramObject = glCreateProgram();

	//attach Vertex Shader to shader program
	glAttachShader(gShaderProgramObject, gVertexShaderObject);

	//attach Fragment Shader to shader program
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	//Pre-Link biding of shader program object with vertex shader position attrubute
	glBindAttribLocation(gShaderProgramObject, ADS_ATTRUBUTE_VERTEX_Position, "vPosition");
	glBindAttribLocation(gShaderProgramObject, ADS_ATTRUBUT_COLORS, "vColor");

	//Link Shader
	glLinkProgram(gShaderProgramObject);

	//Should be linker error checking
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iShaderProgramLinkStatus > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf(GpFile, "Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				unInitilize();
				exit(0);
			}
		}
	}

	//get MVP uniform location
	mvpUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");

	//*** Vertices, Colors, shader attributers,vbo, vao, initialization ***
	//Feed Vertices through Variable Name
	void createVaoTriangleLineLoop(void);
	createVaoTriangleLineLoop();

	void createVaoCircleLineLoop(void);
	createVaoCircleLineLoop();

	void createVaoRectangleLineLoop(void);
	createVaoRectangleLineLoop();

	void createVaoLine(void);
	createVaoLine();

	//Depth Functions
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE);




	//Set Clear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//set orthographics matrix to identitu matrix
	perspectiveProjectionMatrix = mat4::identity();

	resize(WIN_WIDTH, WIN_HIGHT); //warmup call to resize
}

void createVaoTriangleLineLoop(void)
{
	glGenVertexArrays(1, &vaoTriangleLineLoop);
	glBindVertexArray(vaoTriangleLineLoop);

	/*------------ Position [START] ------------*/
	glGenBuffers(1, &vboTriangleLineLoopPosition);
	glBindBuffer(GL_ARRAY_BUFFER, vboTriangleLineLoopPosition);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof GLfloat, nullptr, GL_DYNAMIC_DRAW); //6: x, y coordinates of 3 points.
	glVertexAttribPointer(ADS_ATTRUBUTE_VERTEX_Position, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(ADS_ATTRUBUTE_VERTEX_Position);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/*------------ Position [END] ------------*/

	/*------------ Color [START] ------------*/
	GLfloat colors[] =
	{
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
	};

	glGenBuffers(1, &vboTriangleLineLoopColor);
	glBindBuffer(GL_ARRAY_BUFFER, vboTriangleLineLoopColor);
	glBufferData(GL_ARRAY_BUFFER, sizeof colors, colors, GL_STATIC_DRAW);
	glVertexAttribPointer(ADS_ATTRUBUT_COLORS, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(ADS_ATTRUBUT_COLORS);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/*------------ Color [END] ------------*/

	glBindVertexArray(0);
}

void drawTriangleLineLoop(GLfloat vertices[], int verticesSize)
{
	glBindVertexArray(vaoTriangleLineLoop);

	glBindBuffer(GL_ARRAY_BUFFER, vboTriangleLineLoopPosition);
	glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(ADS_ATTRUBUTE_VERTEX_Position, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(ADS_ATTRUBUTE_VERTEX_Position);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_LINE_LOOP, 0, 3);

	glBindVertexArray(0);
}

int numberOfVerticesForInCircleLineLoop = 0;

void createVaoCircleLineLoop(void)
{
	glGenVertexArrays(1, &vaoCircleLineLoop);
	glBindVertexArray(vaoCircleLineLoop);

	for (GLfloat angle = 0; angle < 2 * M_PI; angle += 0.0001f, numberOfVerticesForInCircleLineLoop++);

	/*------------ Position [START] ------------*/
	glGenBuffers(1, &vboCircleLineLoopPosition);
	glBindBuffer(GL_ARRAY_BUFFER, vboCircleLineLoopPosition);
	glBufferData(GL_ARRAY_BUFFER, 2 * numberOfVerticesForInCircleLineLoop * sizeof GLfloat, nullptr, GL_DYNAMIC_DRAW);
	//2: x,y coordinates of each vertex.
	glVertexAttribPointer(ADS_ATTRUBUTE_VERTEX_Position, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(ADS_ATTRUBUTE_VERTEX_Position);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/*------------ Position [END] ------------*/

	/*------------ Color [START] ------------*/
	const int colorsSize = 4 * numberOfVerticesForInCircleLineLoop * sizeof GLfloat;
	GLfloat* colors = (GLfloat*)malloc(colorsSize);
	//4: RGBA components of a vertex.
	for (int i = 0; i < numberOfVerticesForInCircleLineLoop; ++i)
	{
		colors[0 + i * 4] = 1.0f;
		colors[1 + i * 4] = 1.0f;
		colors[2 + i * 4] = 0.0f;
		colors[3 + i * 4] = 1.0f;
	}

	glGenBuffers(1, &vboCircleLineLoopColor);
	glBindBuffer(GL_ARRAY_BUFFER, vboCircleLineLoopColor);
	glBufferData(GL_ARRAY_BUFFER, colorsSize, colors, GL_STATIC_DRAW);
	glVertexAttribPointer(ADS_ATTRUBUT_COLORS, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(ADS_ATTRUBUT_COLORS);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	free(colors);
	/*------------ Color [END] ------------*/

	glBindVertexArray(0);
}

void drawCircleLineLoop(GLfloat* vertices, int verticesSize)
{
	glBindVertexArray(vaoCircleLineLoop);

	glBindBuffer(GL_ARRAY_BUFFER, vboCircleLineLoopPosition);
	glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(ADS_ATTRUBUTE_VERTEX_Position, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(ADS_ATTRUBUTE_VERTEX_Position);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_LINE_LOOP, 0, numberOfVerticesForInCircleLineLoop);

	glBindVertexArray(0);
}

void createVaoRectangleLineLoop(void)
{
	glGenVertexArrays(1, &vaoRectangleLineLoop);
	glBindVertexArray(vaoRectangleLineLoop);

	/*------------ Position [START] ------------*/
	glGenBuffers(1, &vboRectangleLineLoopPosition);
	glBindBuffer(GL_ARRAY_BUFFER, vboRectangleLineLoopPosition);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof GLfloat, nullptr, GL_DYNAMIC_DRAW); //8: x, y coordinates of 4 points.
	glVertexAttribPointer(ADS_ATTRUBUTE_VERTEX_Position, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(ADS_ATTRUBUTE_VERTEX_Position);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/*------------ Position [END] ------------*/

	/*------------ Color [START] ------------*/
	GLfloat colors[16] =
	{
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f
	};

	glGenBuffers(1, &vboRectangleLineLoopColor);
	glBindBuffer(GL_ARRAY_BUFFER, vboRectangleLineLoopColor);
	glBufferData(GL_ARRAY_BUFFER, sizeof colors, colors, GL_STATIC_DRAW);
	glVertexAttribPointer(ADS_ATTRUBUT_COLORS, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(ADS_ATTRUBUT_COLORS);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/*------------ Color [END] ------------*/

	glBindVertexArray(0);
}

void drawRectangleLineLoop(GLfloat vertices[], int verticesSize)
{
	glBindVertexArray(vaoRectangleLineLoop);

	glBindBuffer(GL_ARRAY_BUFFER, vboRectangleLineLoopPosition);
	glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(ADS_ATTRUBUTE_VERTEX_Position, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(ADS_ATTRUBUTE_VERTEX_Position);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_LINE_LOOP, 0, 4);

	glBindVertexArray(0);
}

void createVaoLine(void)
{
	glGenVertexArrays(1, &vaoLine);
	glBindVertexArray(vaoLine);

	/*------------ Position [START] ------------*/
	glGenBuffers(1, &vboLinePosition);
	glBindBuffer(GL_ARRAY_BUFFER, vboLinePosition);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof GLfloat, nullptr, GL_DYNAMIC_DRAW); //4: x, y coordinates of 2 points.
	glVertexAttribPointer(ADS_ATTRUBUTE_VERTEX_Position, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(ADS_ATTRUBUTE_VERTEX_Position);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/*------------ Position [END] ------------*/

	/*------------ Color [START] ------------*/
	glGenBuffers(1, &vboLineColor);
	glBindBuffer(GL_ARRAY_BUFFER, vboLineColor);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof GLfloat, nullptr, GL_DYNAMIC_DRAW); //8: RGBA components of 2 points.
	glVertexAttribPointer(ADS_ATTRUBUT_COLORS, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(ADS_ATTRUBUT_COLORS);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/*------------ Color [END] ------------*/

	glBindVertexArray(0);
}

void drawLine(GLfloat lineVertices[], int lineVerticesSize, GLfloat lineColors[], int lineColorsSize)
{
	glBindVertexArray(vaoLine);

	glBindBuffer(GL_ARRAY_BUFFER, vboLineColor);
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)lineColorsSize, lineColors, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(ADS_ATTRUBUT_COLORS, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(ADS_ATTRUBUT_COLORS);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vboLinePosition);
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)lineVerticesSize, lineVertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(ADS_ATTRUBUTE_VERTEX_Position, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(ADS_ATTRUBUTE_VERTEX_Position);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_LINES, 0, 2);

	glBindVertexArray(0);
}


void resize(int width, int height)
{
	//Code
	if (height == 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	//*** perspective call for Programable Pipeline using Vmath
	perspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}
mat4 modelViewMatrix;
mat4 modelViewProjectionMatrix;
mat4 translationMatrix;
mat4 rotationMatrix;

void loadIdentity(void)
{
    modelViewMatrix = mat4::identity();
    modelViewProjectionMatrix = mat4::identity();
    translationMatrix = mat4::identity();
    rotationMatrix = mat4::identity();

    void setTranslation(void);
    setTranslation();

    void setRotation(void);
    setRotation();
}

void setTranslation(void)
{
    const mat4 transformedMatrix = modelViewMatrix * translationMatrix;
    modelViewMatrix = transformedMatrix;
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
}

void setRotation(void)
{
    const mat4 transformedMatrix = modelViewMatrix * rotationMatrix;
    modelViewMatrix = transformedMatrix;
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
}

void display(void)
{


	void renderGraph(void);
    void renderInCircle(void);
    void renderRectangle(void);
    void renderOuterCircle(void);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(gShaderProgramObject);

    loadIdentity();
    translationMatrix = translate(0.0f, 0.0f, -25.0f);
    setTranslation();

    renderGraph();
    renderInCircle();
    renderRectangle();
    renderOuterCircle();

	SwapBuffers(ghdc);
}

void renderGraph(void)
{
    const int numberOfLinesForBothAxes = 100;
    const GLfloat lap = 1.0f;

    //------------------------ Horizontal lines (bold) [START] ------------------------ 
    //Upper
    GLfloat y = 0.0f;
    for (int i = 0; i < numberOfLinesForBothAxes / 2; i++)
    {
        const GLfloat lineWidth = i == 0 ? 4.0f : 1.0f;
        glLineWidth(lineWidth);

        GLfloat lineVertices[] =
        {
            -100.0f, y,
            100.0f, y
        };

        GLfloat lineColors[] =
        {
            1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
        };

        drawLine(lineVertices, sizeof lineVertices, lineColors, sizeof lineColors);

        y += lap;
    }

    //Lower
    y = 0.0f;
    for (int i = 0; i < numberOfLinesForBothAxes / 2; i++)
    {
        const GLfloat lineWidth = i == 0 ? 4.0f : 1.0f;
        glLineWidth(lineWidth);

        GLfloat lineVertices[] =
        {
            -100.0f, y,
            100.0f, y
        };

        GLfloat lineColors[] =
        {
            1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
        };

        drawLine(lineVertices, sizeof lineVertices, lineColors, sizeof lineColors);

        y -= lap;
    }

    //------------------------ Horizontal lines (bold) [END] ------------------------ 

    //------------------------ Vertical lines (bold) [START] ------------------------ 
    //Right
    GLfloat x = 0.0f;
    for (int i = 0; i < numberOfLinesForBothAxes / 2; i++)
    {
        const GLfloat lineWidth = i == 0 ? 4.0f : 1.0f;
        glLineWidth(lineWidth);

        GLfloat lineVertices[] =
        {
            x, -100.0f,
            x, 100.0f
        };

        GLfloat lineColors[] =
        {
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
        };

        drawLine(lineVertices, sizeof lineVertices, lineColors, sizeof lineColors);

        x += lap;
    }

    //Left
    x = 0.0f;
    for (int i = 0; i < numberOfLinesForBothAxes / 2; i++)
    {
        const GLfloat lineWidth = i == 0 ? 4.0f : 1.0f;
        glLineWidth(lineWidth);

        GLfloat lineVertices[] =
        {
            x, -100.0f,
            x, 100.0f
        };

        GLfloat lineColors[] =
        {
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
        };

        drawLine(lineVertices, sizeof lineVertices, lineColors, sizeof lineColors);

        x -= lap;
    }
}

void renderInCircle(void)
{
    /////////////////////////////////// TRIANGLE ///////////////////////////////////
    const GLfloat triangleWidth = 7.0f;

    glLineWidth(1.5f);
    const GLfloat cx = 0.0f;
    const GLfloat cy = triangleWidth;
    const GLfloat ax = -triangleWidth;
    const GLfloat ay = -triangleWidth;
    const GLfloat bx = triangleWidth;
    const GLfloat by = -triangleWidth;
    GLfloat triangleLineLoopVertices[6] =
    {
        cx, cy, //C
        ax, ay, //A
        bx, by  //B
    };

    drawTriangleLineLoop(triangleLineLoopVertices, sizeof triangleLineLoopVertices);

    /////////////////////////////////// INCIRCLE ///////////////////////////////////
    /*
                                C

                                /\
                               /  \
                              /    \
                             /      \
                        b   /        \   a
                           /          \
                          /            \
                         /              \
                        /                \
                   A    ------------------   B

                                c


    -----------------------------------------------------------------------------------
    COORDINATES OF INCIRCLE :
    -----------------------------------------------------------------------------------

    Given the coordinates of the three vertices of a triangle ABC,
    the coordinates of the incenter O are
    Ox = (aAx + bBx + cCx)/p
    Oy = (aAy + bBy + cCy)/p
    where: Ax and Ay are the x and y coordinates of the point A etc.
    a, b and c are the side lengths opposite vertex A, B and C
    p is perimeter of the triangle (a+b+c)

    -----------------------------------------------------------------------------------
    RADIUS OF INCIRCLE :
    -----------------------------------------------------------------------------------

    radius = area/s;
    where area = sqrt(s*(s-a)*(s-b)*(s-c)),
    s = (a+b+c)/2

    -----------------------------------------------------------------------------------
    DISTANCE BETWEEN TWO POINTS :
    -----------------------------------------------------------------------------------
    The distance between points P(x1, y1) and Q(x2, y2) is given by sqrt(square(x2-x1) + square(y2-y1))
    */

    const GLfloat a = sqrt((cx - bx) * (cx - bx) + (cy - by) * (cy - by)); //Distance between C and B
    const GLfloat b = sqrt((cx - ax) * (cx - ax) + (cy - ay) * (cy - ay)); //Distance between C and A
    const GLfloat c = sqrt((ax - bx) * (ax - bx) + (ay - by) * (ay - by)); //Distance between A and B
    const GLfloat p = (a + b + c);
    const GLfloat s = p / 2.0f;
    const GLfloat area = sqrt(s * (s - a) * (s - b) * (s - c));
    const GLfloat radius = area / s;
    const GLfloat ox = (a * ax + b * bx + c * cx) / p;
    const GLfloat oy = (a * ay + b * by + c * cy) / p;

    const int inCircleLineLoopVerticesSize = 2 * numberOfVerticesForInCircleLineLoop * sizeof GLfloat;
    GLfloat* inCircleLineLoopVertices = (GLfloat*)malloc(inCircleLineLoopVerticesSize);
    int i = 0;

    for (GLfloat angle = 0; angle < 2 * M_PI; angle += 0.0001f, i++)
    {
        inCircleLineLoopVertices[0 + i * 2] = ox + cosf(angle) * radius;
        inCircleLineLoopVertices[1 + i * 2] = oy + sinf(angle) * radius;
    }

    drawCircleLineLoop(inCircleLineLoopVertices, inCircleLineLoopVerticesSize);
    
    free(inCircleLineLoopVertices);
}

void renderOuterCircle(void)
{
    const GLfloat radius = 10.0f;

    const int outerCircleLineLoopVerticesSize = 2 * numberOfVerticesForInCircleLineLoop * sizeof GLfloat;
    GLfloat* outerCircleLineLoopVertices = (GLfloat*)malloc(outerCircleLineLoopVerticesSize);
    int i = 0;

    for (GLfloat angle = 0; angle < 2 * M_PI; angle += 0.0001f, i++)
    {
        outerCircleLineLoopVertices[0 + i * 2] = cosf(angle) * radius;
        outerCircleLineLoopVertices[1 + i * 2] = sinf(angle) * radius;
    }

    drawCircleLineLoop(outerCircleLineLoopVertices, outerCircleLineLoopVerticesSize);

    free(outerCircleLineLoopVertices);
}

void renderRectangle(void)
{
    const GLfloat rectangleWidth = 16.0f;
    const GLfloat rectangleHeight = rectangleWidth * 3 / 4;

    glLineWidth(1.5f);

    GLfloat vertices[8] = 
    {
        -rectangleWidth / 2, rectangleHeight / 2,
        -rectangleWidth / 2, -rectangleHeight / 2,
        rectangleWidth / 2, -rectangleHeight / 2,
        rectangleWidth / 2, rectangleHeight / 2
    };

    drawRectangleLineLoop(vertices, sizeof vertices);
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

	//destroy vao
	if (vboTriangleLineLoopPosition)
	{
		glDeleteBuffers(1, &vboTriangleLineLoopPosition);
		vboTriangleLineLoopPosition = 0;
	}

	if (vaoTriangleLineLoop)
	{
		glDeleteBuffers(1, &vaoTriangleLineLoop);
		vaoTriangleLineLoop = 0;
	}

	if (vboLineColor)
	{
		glDeleteBuffers(1, &vboLineColor);
		vboLineColor = 0;
	}

	if (vboLinePosition)
	{
		glDeleteBuffers(1, &vboLinePosition);
		vboLinePosition = 0;
	}

	if (vboCircleLineLoopColor)
	{
		glDeleteBuffers(1, &vboCircleLineLoopColor);
		vboCircleLineLoopColor = 0;
	}

	if (vboCircleLineLoopPosition)
	{
		glDeleteBuffers(1, &vboCircleLineLoopPosition);
		vboCircleLineLoopPosition = 0;
	}

	//AS// //detech vertex shader from shader program object
	//AS// glDetachShader(gShaderProgramObject, gVertexShaderObject);
	//AS// 
	//AS// //detach fragment shader from shader program object
	//AS// glDetachShader(gShaderProgramObject, gFragmentShaderObject);
	//AS// 
	//AS// //delete vertex shader object
	//AS// glDeleteShader(gVertexShaderObject);
	//AS// gVertexShaderObject = 0;
	//AS// 
	//AS// //delete Fragment shader object
	//AS// glDeleteShader(gFragmentShaderObject);
	//AS// gFragmentShaderObject = 0;
	//AS// 
	//AS// //delete shader program object
	//AS// glDeleteProgram(gShaderProgramObject);
	//AS// gShaderProgramObject = 0;
	//AS// 
	//AS// //unlink shader program
	//AS// glUseProgram(0);

	if (gShaderProgramObject)
	{
		glUseProgram(gShaderProgramObject);
		GLsizei shaderCount;
		glGetProgramiv(gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);
		GLuint *pShader = NULL;
		pShader = (GLuint*)malloc(shaderCount * sizeof(shaderCount));
		glGetAttachedShaders(gShaderProgramObject, shaderCount, &shaderCount, pShader);
		for (GLsizei i = 0; i < shaderCount; i++)
		{
			glDetachShader(gShaderProgramObject, pShader[i]);
			glDeleteShader(pShader[i]);
			pShader[i] = 0;
		}
		if (pShader)
		{
			free(pShader);
		}
		glDeleteProgram(gShaderProgramObject);
		gShaderProgramObject = 0;
		glUseProgram(0);
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
	//TextFile close
	if (GpFile)
	{
		fclose(GpFile);
		GpFile = NULL;
	}
}