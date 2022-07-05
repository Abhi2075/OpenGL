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
GLfloat Tangle = 0.0f;
GLfloat Rangle = 0.0f;

//Variables for Shaders
GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint gVao_Triangle;
GLuint gVbo_Position_Triangle;
GLuint gVbo_Color_Triangle;

GLuint gVao_Rectangle;
GLuint gVbo_Position_Rectangle;
GLuint gVbo_Color_Rectangle;

GLuint gMVPUniform;

mat4 gPerspectiveProjectionMatrix;

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
		"#version 460 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec4 vColor;" \
		"uniform mat4 u_mvp_matrix;" \
		"out vec4 out_Color;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_mvp_matrix * vPosition;" \
		"out_Color = vColor;" \
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
		"\n" \
		"in vec4 out_Color;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = out_Color;" \
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
	gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");

	//*** Vertices, Colors, shader attributers,vbo, vao, initialization ***
	//Feed Vertices through Variable Name
	GLfloat triangleVertex = 1.0f;
	GLfloat rectangleVertex = 1.0f;
	GLfloat Zero = 0.0f;
	GLfloat triangleColor = 1.0f;
	GLfloat rectangleColor =  1.0f;

	const GLfloat triangleVertices[] =
	{ Zero,triangleVertex,Zero, //apex
	-triangleVertex,-triangleVertex,Zero, //left-bottom
	triangleVertex,-triangleVertex,Zero }; //right vottom

	const GLfloat triangleColors[] =
	{ triangleColor, Zero, Zero, triangleColor, //apex
	Zero, triangleColor, Zero, triangleColor, //left-bottom
	Zero, Zero, triangleColor, triangleColor }; //right vottom

	const GLfloat rectangleVertices[] =
	{ 
		rectangleVertex,rectangleVertex,Zero,
		-rectangleVertex,rectangleVertex,Zero,
		-rectangleVertex,-rectangleVertex,Zero,
		rectangleVertex,-rectangleVertex,Zero,
	};

	const GLfloat rectangleColors[] = 
	{
		rectangleColor,Zero,Zero,rectangleColor,
		rectangleColor,Zero,Zero,rectangleColor,
		rectangleColor,Zero,Zero,rectangleColor,
		rectangleColor,Zero,Zero,rectangleColor,
	};

	//Triangle recording starts
	glGenVertexArrays(1, &gVao_Triangle);
	glBindVertexArray(gVao_Triangle);

	glGenBuffers(1, &gVbo_Position_Triangle);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Triangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(ADS_ATTRUBUTE_VERTEX_Position, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ADS_ATTRUBUTE_VERTEX_Position);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gVbo_Color_Triangle);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Color_Triangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColors), triangleColors, GL_STATIC_DRAW);
	glVertexAttribPointer(ADS_ATTRUBUT_COLORS, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ADS_ATTRUBUT_COLORS);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindVertexArray(0);
	//Trinage recording End

		//Rectangle recording starts
	glGenVertexArrays(1, &gVao_Rectangle);
	glBindVertexArray(gVao_Rectangle);

	glGenBuffers(1, &gVbo_Position_Rectangle);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Rectangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(ADS_ATTRUBUTE_VERTEX_Position, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ADS_ATTRUBUTE_VERTEX_Position);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gVbo_Color_Rectangle);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Color_Rectangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleColors), rectangleColors, GL_STATIC_DRAW);
	glVertexAttribPointer(ADS_ATTRUBUT_COLORS, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ADS_ATTRUBUT_COLORS);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	//Rectangle recording End

	//Depth Functions
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glEnable(GL_CULL_FACE);




	//Set Clear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//set orthographics matrix to identitu matrix
	gPerspectiveProjectionMatrix = mat4::identity();

	resize(WIN_WIDTH, WIN_HIGHT); //warmup call to resize
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
	gPerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
	GLfloat UpdateAngle(GLfloat angle);

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Start using OpenGl Program Object
	glUseProgram(gShaderProgramObject);

	//Here you should draw 
	//Set modelview and modelviewprojection matrices to identity
	mat4 modelViewMatrix = mat4::identity();
	mat4 modelViewProjectionMatrix = mat4::identity();

	//*** Triangle glTranslated related call using vmath.h ***
	modelViewMatrix = vmath::translate(-1.5f, 0.0f, -6.0f) * vmath::rotate(Tangle, 0.0f, 1.0f, 0.0f);
	
	//Multiply the modelview and gPerspectiveProjectionMatrix to get model veiw projection matrix
	modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix;

	//Pass above modelviewprojectionmatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//Whole position value we already clacluted in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//*** bind vao fro traingle***
	glBindVertexArray(gVao_Triangle);

	//*** draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glDrawArrays(GL_TRIANGLES, 0, 3); //3 (each with its x,y,z) vertices in trianglevertices array

	//*** unBind Vao for triangle ***
	glBindVertexArray(0);

	//Rectangle Translatef 
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//*** Rectangle glTranslated related call using vmath.h ***
	modelViewMatrix = vmath::translate(1.5f, 0.0f, -6.0f) * vmath::rotate(Rangle, 1.0f, 0.0f, 0.0f);
	
	//Scaling Rectangle
	modelViewMatrix = modelViewMatrix * vmath::scale(0.75f, 0.75f, 0.75f);

	//Multiply the modelview and gPerspectiveProjectionMatrix to get model veiw projection matrix
	modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix;

	//Pass above modelviewprojectionmatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//Whole position value we already clacluted in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//*** bind vao fro traingle***
	glBindVertexArray(gVao_Rectangle);

	//*** draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4); //3 (each with its x,y,z) vertices in trianglevertices array

	//*** unBind Vao for triangle ***
	glBindVertexArray(0);

	//Stop using OpenGL Program Object
	glUseProgram(0);

	//Update Both angles
	Tangle = UpdateAngle(Tangle);
	Rangle = UpdateAngle(Rangle);

	SwapBuffers(ghdc);
}

GLfloat UpdateAngle(GLfloat angle)
{
	angle = angle + 1.5f;
	if (angle >= 360.0f)
	{
		angle = 0.0f;
	}
	return(angle);
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
	if (gVao_Triangle)
	{
		glDeleteVertexArrays(1, &gVao_Triangle);
		gVao_Triangle = 0;
	}

	if (gVao_Rectangle)
	{
		glDeleteVertexArrays(1, &gVao_Rectangle);
		gVao_Rectangle = 0;
	}


	//destroy vbo
	if (gVbo_Position_Triangle)
	{
		glDeleteBuffers(1, &gVbo_Position_Triangle);
		gVbo_Position_Triangle = 0;
	}

	if (gVbo_Color_Triangle)
	{
		glDeleteBuffers(1, &gVbo_Color_Triangle);
		gVbo_Color_Triangle = 0;
	}

	if (gVbo_Position_Rectangle)
	{
		glDeleteBuffers(1, &gVbo_Position_Rectangle);
		gVbo_Position_Rectangle = 0;
	}

	if (gVbo_Color_Rectangle)
	{
		glDeleteBuffers(1, &gVbo_Color_Rectangle);
		gVbo_Color_Rectangle = 0;
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