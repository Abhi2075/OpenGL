//Headers
#include<windows.h>
#include<stdio.h>
#include<gl/glew.h> //This line must be before GL.h
#include<gl/GL.h>
#include<vmath.h>
#include"Image.h"

#define WIN_WIDTH 800
#define WIN_HIGHT 600

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")

using namespace vmath;

enum
{
	ADS_ATTRUBUTE_VERTEX = 0,
	ADS_ATTRUBUT_COLIRS,
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
GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;
GLuint gTessellationControlShaderObject;
GLuint gTessellationEvaluationShaderObject;

GLuint gVao;
GLuint gVbo;
GLuint gMVPUniform;

GLuint gNumberOfSegmentsUniform;
GLuint gNumberOfStripsUniform;
GLuint gLineColorUniform;

mat4 gPerspectiveProjectionMatrix;

unsigned int gNumberOfSegments;


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
		case VK_UP:
            gNumberOfSegments++;
            if (gNumberOfSegments >= 50)
            {
                gNumberOfSegments = 50;
            }

            break;

        case VK_DOWN:
            gNumberOfSegments--;
            if (gNumberOfSegments <= 0)
            {
                gNumberOfSegments = 1;
            }

            break;
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
		 "\n"
        "in vec2 vPosition;"
        "void main(void)"
        "{"
        "gl_Position = vec4(vPosition, 0.0, 1.0);"
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
	
	//Create Shader
	gTessellationControlShaderObject = glCreateShader(GL_TESS_CONTROL_SHADER);

    const GLchar* tessellationControlShaderSourceCode =
		"#version 460 core" \
		"\n"
        "layout(vertices = 4) out;"
        "uniform int numberOfSegments;"
        "uniform int numberOfStrips;"
        "void main(void)"
        "{"
        "gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;"
        "gl_TessLevelOuter[0] = float(numberOfStrips);"
        "gl_TessLevelOuter[1] = float(numberOfSegments);"
        "}";

	glShaderSource(gTessellationControlShaderObject, 1, (const GLchar **)&tessellationControlShaderSourceCode, NULL);

	//Compile Shader
	glCompileShader(gTessellationControlShaderObject);

	//Should be complition error checking
	glGetShaderiv(gTessellationControlShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gTessellationControlShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gTessellationControlShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(GpFile, "gTessellationControlShaderObject Shader compilatin Log : %s\n", szInfoLog);
				free(szInfoLog);
				unInitilize();
				exit(0);
			}
		}
	}


	//Create Shader
	gTessellationEvaluationShaderObject = glCreateShader(GL_TESS_EVALUATION_SHADER);

    const GLchar* tessellationEvaluationShaderSourceCode =
		"#version 460 core" \
		"\n"
        "layout(isolines) in;"
        "uniform mat4 u_mvp_matrix;"
        "void main(void)"
        "{"
        "float u = gl_TessCoord.x;"
        "vec3 p0 = gl_in[0].gl_Position.xyz;"
        "vec3 p1 = gl_in[1].gl_Position.xyz;"
        "vec3 p2 = gl_in[2].gl_Position.xyz;"
        "vec3 p3 = gl_in[3].gl_Position.xyz;"
        "float u1 = (1.0 - u);"
        "float u2 = u * u;"
        "float b3 = u2 * u;"
        "float b2 = 3.0 * u2 * u1;"
        "float b1 = 3.0 * u * u1 * u1;"
        "float b0 = u1 * u1 * u1;"
        "vec3 p = p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;"
        "gl_Position = u_mvp_matrix * vec4(p, 1.0);"
        "}";

	glShaderSource(gTessellationEvaluationShaderObject, 1, (const GLchar **)&tessellationEvaluationShaderSourceCode, NULL);

	//Compile Shader
	glCompileShader(gTessellationEvaluationShaderObject);

	//Should be complition error checking
	glGetShaderiv(gTessellationEvaluationShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gTessellationEvaluationShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gTessellationEvaluationShaderObject, iInfoLogLength, &written, szInfoLog);
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
        "uniform vec4 lineColor;"
        "out vec4 fragColor;"
        "void main(void)"
        "{"
        "fragColor = lineColor;"
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
	glAttachShader(gShaderProgramObject, gTessellationControlShaderObject);
    glAttachShader(gShaderProgramObject, gTessellationEvaluationShaderObject);
	//attach Fragment Shader to shader program
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	//Pre-Link biding of shader program object with vertex shader position attrubute
	glBindAttribLocation(gShaderProgramObject, ADS_ATTRUBUTE_VERTEX, "vPosition");

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
	gNumberOfSegmentsUniform = glGetUniformLocation(gShaderProgramObject, "numberOfSegments");
    gNumberOfStripsUniform = glGetUniformLocation(gShaderProgramObject, "numberOfStrips");
    gLineColorUniform = glGetUniformLocation(gShaderProgramObject, "lineColor");

	//*** Vertices, Colors, shader attributers,vbo, vao, initialization ***
	//Feed Vertices through Variable Name
	float vertices[] = {-1.0f, -1.0f, -0.5f, 1.0f, 0.5f, -1.0f, 1.0f, 1.0f};
    glGenVertexArrays(1, &gVao);
    glBindVertexArray(gVao);
    glGenBuffers(1, &gVbo);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(ADS_ATTRUBUTE_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(ADS_ATTRUBUTE_VERTEX);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

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
	gNumberOfSegments = 1;
	//*** perspective call for Programable Pipeline using Vmath
	gPerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{


	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(gShaderProgramObject);

    mat4 modelViewMatrix = mat4::identity();
    mat4 modelViewProjectionMatrix = mat4::identity();

    modelViewMatrix = translate(0.0f, 0.0f, -3.0f);
    modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix;

    glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    glUniform1i(gNumberOfSegmentsUniform, gNumberOfSegments);
    glUniform1i(gNumberOfStripsUniform, 1);
    glUniform4fv(gLineColorUniform, 1, vec4(1.0f, 1.0f, 0.0f, 1.0f));
    glBindVertexArray(gVao);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawArrays(GL_PATCHES, 0, 4);
    glBindVertexArray(0);
    glUseProgram(0);
	SwapBuffers(ghdc);
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
	if (gVao)
	{
		glDeleteVertexArrays(1, &gVao);
		gVao = 0;
	}


	//destroy vbo
	if (gVbo)
	{
		glDeleteBuffers(1, &gVbo);
		gVbo = 0;
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