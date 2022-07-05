//Headers
#include<windows.h>
#include<stdio.h>
#include<gl/glew.h> //This line must be before GL.h
#include<gl/GL.h>
#include<vmath.h> //for Matrix calclulation
#include<Sphere.h>
#include"Image.h"

#define WIN_WIDTH 800
#define WIN_HIGHT 600

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"Sphere.lib")

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
GLfloat Rangle = 0.0f;

//Variables for Shaders
GLuint gVertexShaderObject_PV;
GLuint gVertexShaderObject_PF;
GLuint gFragmentShaderObject_PV;
GLuint gFragmentShaderObject_PF;
GLuint gShaderProgramObject;
GLuint gShaderProgramObject_PV;
GLuint gShaderProgramObject_PF;

//GLuint gVao_Cube ;
//GLuint gVbo_Position_Cube;
//GLuint gVbo_Normal_Cube;

//GLuint gMVPUniform;
GLuint modelMatrixUniform;
GLuint perspectiveProjectionUniform;
GLuint viewMatrixUniform;

GLuint LKeyPressedUniform;

//Light Uniform
GLuint LaUniform;
GLuint LdUniform;
GLuint LsUniform;
GLuint lightPositionUniform;
GLfloat Aangle1 = 0.0f;
GLfloat Aangle2 = 0.0f;
GLfloat Aangle3 = 0.0f;

GLuint KaUniform;
GLuint KdUniform;
GLuint KsUniform;
GLuint MatrialShiness;


bool bLightAnimate = false;
bool XKeyPress = false;
bool YKeyPress = false;
bool ZKeyPress = false;
bool bAnimate = false;
bool bLight = false;
bool bVertex_Light = true;
bool bFragment_Light = true;

mat4 modelMatrix;
mat4 viewMatrix;
mat4 gPerspectiveProjectionMatrix;

//Sphere Variables
float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
GLuint gNumVertices;
GLuint gNumElements;
GLuint gVao_sphere;
GLuint gVbo_sphere_position;
GLuint gVbo_sphere_normal;
GLuint gVbo_sphere_element;

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
		/*case(0x46):
		case(0x66):
			TogleFullscreen();
			break;*/
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

	case WM_CHAR: 
	{
		switch (wParam) {
		case'q':
		case'Q':
			TogleFullscreen();
			break;
		case'a':
		case'A':
		{
			if (bAnimate == true)
			{
				bAnimate = false;
			}
			else
			{
				bAnimate = true;
			}
			break;
		}
		case'x':
		case'X':
		{
			if (XKeyPress == true)
			{
				XKeyPress = false;
			}
			else
			{
				XKeyPress = true;
			}
			break;
		}

		case'y':
		case'Y':
		{
			if (YKeyPress == true)
			{
				YKeyPress = false;
			}
			else
			{
				YKeyPress = true;
			}
			break;
		}

		case'z':
		case'Z':
		{
			if (ZKeyPress == true)
			{
				ZKeyPress = false;
			}
			else
			{
				ZKeyPress = true;
			}
			break;
		}
		case'l':
		case'L':
		{
			if (bLight == true)
			{
				bLight = false;
			}
			else
			{
				bLight = true;
			}
			break;
		}
		case'v':
		case'V':
		{
			if (bVertex_Light == true)
			{
				bVertex_Light = false;
				bFragment_Light = true;
			}
			else
			{
				bVertex_Light = true;
				bFragment_Light = false;
			}
			break;
		}
		case'f':
		case'F':
		{
			if (bFragment_Light == true)
			{
				bFragment_Light = false;
				bVertex_Light = true;
			}
			else
			{
				bFragment_Light = true;
				bVertex_Light = false;
			}
			break;
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


	//****************************** Per Vertex Lighting Start *********************************************
	//*** Vertex Shader ***
	//Create Shader
	gVertexShaderObject_PV = glCreateShader(GL_VERTEX_SHADER);
	
	//Provide Source code to Shader
	const GLchar *vertexShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_View_Uniform;" \
		"uniform mat4 u_perspective_projection_matrix;" \
		"uniform mat4 u_model_matrix;" \
		"uniform int u_LKeyPress;" \
		"uniform vec3 u_LA;" \
		"uniform vec3 u_LD;" \
		"uniform vec3 u_LS;" \
		"uniform vec3 u_KA;" \
		"uniform vec3 u_KD;" \
		"uniform vec3 u_KS;" \
		"uniform float u_Matrial_Shiness;" \
		"uniform vec4 u_Light_Position;" \
		"out vec3 u_Phong_ADS_Light;" \
		"void main(void)" \
		"{" \
			"if(u_LKeyPress == 1)" \
			"{" \
				"vec4 eye_coordinates = u_View_Uniform * u_model_matrix * vPosition;" \
				"mat3 normal_matrix = mat3(u_View_Uniform * u_model_matrix);" \
				"vec3 transformed_norm = normalize(normal_matrix * vNormal);" \
				"vec3 light_source = normalize(vec3(u_Light_Position - eye_coordinates));" \
				"vec3 reflection_vector = reflect(-light_source,transformed_norm);" \
				"vec3 view_vector = normalize((vec3(-eye_coordinates)));" \
				"vec3 ambint = u_LA * u_KA;" \
				"vec3 diffuse = u_LD * u_KD * max(dot(light_source,transformed_norm),0.0f);" \
				"vec3 specular;" \
				"if(diffuse == 0.0)" \
				"{" \
				"specular = vec3(0.0);" \
				"}" \
				"else" \
				"{" \
				"specular = u_LS * u_KS * pow(max(dot(reflection_vector,view_vector),0.0f),u_Matrial_Shiness);" \
				"}" \
				"u_Phong_ADS_Light = ambint + diffuse + specular;" \
			"}" \
			"else" \
			"{" \
				"u_Phong_ADS_Light = vec3(1.0,1.0,1.0);" \
			"}" \
			"gl_Position = u_perspective_projection_matrix * u_View_Uniform * u_model_matrix * vPosition;" \
		"}";
	glShaderSource(gVertexShaderObject_PV, 1, (const GLchar **)&vertexShaderSourceCode, NULL);
	
	//Compile Shader
	glCompileShader(gVertexShaderObject_PV);
	//Should be complition error checking
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char* szInfoLog = NULL;
	glGetShaderiv(gVertexShaderObject_PV, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_PV, iInfoLogLength, &written, szInfoLog);
				fprintf(GpFile, "Vertex Shader compilatin Log : %s\n", szInfoLog);
				free(szInfoLog);
				unInitilize();
				exit(0);
			}
		}
	}
	
	//*** Fragment Shader ***
	//Create Shader
	gFragmentShaderObject_PV = glCreateShader(GL_FRAGMENT_SHADER);
	
	//ProvideSource code to Shader
	const GLchar *fragmentShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec3 u_Phong_ADS_Light;" \
		"uniform int u_LKeyPress;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		/*"vec4 color;" \
		"if(u_LKeyPress == 1)" \
		"{" \
			"color = vec4(u_Phong_ADS_Light,1.0);" \
		"}" \
		"else" \
		"{" \
			"color = vec4(1.0,1.0,1.0,1.0);" \
		"}" \*/
		"FragColor = vec4(u_Phong_ADS_Light,1.0);;" \
		"}";
	
	glShaderSource(gFragmentShaderObject_PV, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);
	
	//Compile Shader
	glCompileShader(gFragmentShaderObject_PV);
	
	//Should be complition error checking
	glGetShaderiv(gFragmentShaderObject_PV, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject_PV, iInfoLogLength, &written, szInfoLog);
				fprintf(GpFile, "Fragment Shader compilatin Log : %s\n", szInfoLog);
				free(szInfoLog);
				unInitilize();
				exit(0);
			}
		}
	}
	
	//*** Shader Program ***
	//Create
	//if (bVertex_Light == true)
	//{
	//	gShaderProgramObject = gShaderProgramObject_PV;
	//}
	
	gShaderProgramObject_PV = glCreateProgram();
	
	//attach Vertex Shader to shader program
	glAttachShader(gShaderProgramObject_PV, gVertexShaderObject_PV);
	
	//attach Fragment Shader to shader program
	glAttachShader(gShaderProgramObject_PV, gFragmentShaderObject_PV);
	
	//Pre-Link biding of shader program object with vertex shader position attrubute
	glBindAttribLocation(gShaderProgramObject_PV, ADS_ATTRUBUTE_VERTEX_Position, "vPosition");
	glBindAttribLocation(gShaderProgramObject_PV, ADS_ATTRIBUTE_NORMAL, "vNormal");
	
	//Link Shader
	glLinkProgram(gShaderProgramObject_PV);
	
	//Should be linker error checking
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObject_PV, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iShaderProgramLinkStatus > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject_PV, iInfoLogLength, &written, szInfoLog);
				fprintf(GpFile, "Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				unInitilize();
				exit(0);
			}
		}
	}
	
	//get MVP uniform location
	modelMatrixUniform = glGetUniformLocation(gShaderProgramObject_PV, "u_model_matrix");
	perspectiveProjectionUniform = glGetUniformLocation(gShaderProgramObject_PV, "u_perspective_projection_matrix");
	viewMatrixUniform = glGetUniformLocation(gShaderProgramObject_PV, "u_View_Uniform");
	
	
	LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject_PV, "u_LKeyPress");
	
	LaUniform = glGetUniformLocation(gShaderProgramObject_PV, "u_LA");
	LdUniform = glGetUniformLocation(gShaderProgramObject_PV, "u_LD");
	LsUniform = glGetUniformLocation(gShaderProgramObject_PV, "u_LS");
	lightPositionUniform = glGetUniformLocation(gShaderProgramObject_PV, "u_Light_Position");
		
	KaUniform = glGetUniformLocation(gShaderProgramObject_PV, "u_KA");
	KdUniform = glGetUniformLocation(gShaderProgramObject_PV, "u_KD");
	KsUniform = glGetUniformLocation(gShaderProgramObject_PV, "u_KS");
	MatrialShiness = glGetUniformLocation(gShaderProgramObject_PV, "u_Matrial_Shiness");
	
	//*************************************** Per Vertex Lighting End ***************************************************************

	//****************************** Per Fragment Lighting Start *********************************************
	//*** Vertex Shader ***
	//Create Shader
	gVertexShaderObject_PF = glCreateShader(GL_VERTEX_SHADER);

	//Provide Source code to Shader
	const GLchar *vertexShaderSourceCode_PF =
		"#version 460 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_View_Uniform;" \
		"uniform mat4 u_perspective_projection_matrix;" \
		"uniform mat4 u_model_matrix;" \
		"uniform int u_LKeyPress;" \
		"out vec3 transformed_norm;" \
		"out vec3 light_source;" \
		"out vec3 view_vector;" \
		"uniform vec4 u_Light_Position;" \
		"void main(void)" \
		"{" \
		"if(u_LKeyPress == 1)" \
		"{" \
		"vec4 eye_coordinates = u_View_Uniform * u_model_matrix * vPosition;" \
		"mat3 normal_matrix = mat3(u_View_Uniform * u_model_matrix);" \
		"transformed_norm = normalize(normal_matrix * vNormal);" \
		"light_source = normalize(vec3(u_Light_Position - eye_coordinates));" \
		"view_vector = normalize((vec3(-eye_coordinates)));" \
		"}" \
		"gl_Position = u_perspective_projection_matrix * u_View_Uniform * u_model_matrix * vPosition;" \
		"}";
	glShaderSource(gVertexShaderObject_PF, 1, (const GLchar **)&vertexShaderSourceCode_PF, NULL);

	//Compile Shader
	glCompileShader(gVertexShaderObject_PF);
	//Should be complition error checking
	 iInfoLogLength = 0;
	 iShaderCompiledStatus = 0;
	 szInfoLog = NULL;
	glGetShaderiv(gVertexShaderObject_PF, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_PF, iInfoLogLength, &written, szInfoLog);
				fprintf(GpFile, "Vertex Shader compilatin Log : %s\n", szInfoLog);
				free(szInfoLog);
				unInitilize();
				exit(0);
			}
		}
	}

	//*** Fragment Shader ***
	//Create Shader
	gFragmentShaderObject_PF = glCreateShader(GL_FRAGMENT_SHADER);

	//ProvideSource code to Shader
	const GLchar *fragmentShaderSourceCode_PF =
		"#version 460 core" \
		"\n" \
		/*	"in vec3 u_Phong_ADS_Light;" \*/
		"in vec3 transformed_norm;" \
		"in vec3 light_source;" \
		"in vec3 view_vector;" \
		"uniform int u_LKeyPress;" \
		"out vec4 FragColor;" \
		"vec3 u_Phong_ADS_Light;" \
		"uniform vec3 u_LA;" \
		"uniform vec3 u_LD;" \
		"uniform vec3 u_LS;" \
		"uniform vec3 u_KA;" \
		"uniform vec3 u_KD;" \
		"uniform vec3 u_KS;" \
		"uniform float u_Matrial_Shiness;" \
		"void main(void)" \
		"{" \
		"if(u_LKeyPress == 1)" \
		"{" \
		"vec3 reflection_vector = reflect(-light_source,transformed_norm);" \
		"vec3 ambint = u_LA * u_KA;" \
		"vec3 diffuse = u_LD * u_KD * max(dot(light_source,transformed_norm),0.0f);" \
		"vec3 specular;" \
		"if(diffuse == 0.0)" \
		"{" \
		"specular = vec3(0.0);" \
		"}" \
		"else" \
		"{" \
		"specular = u_LS * u_KS * pow(max(dot(reflection_vector,view_vector),0.0f),u_Matrial_Shiness);" \
		"}" \
		"u_Phong_ADS_Light = ambint + diffuse + specular;" \
		"}" \
		"else" \
		"{" \
		"u_Phong_ADS_Light = vec3(1.0,1.0,1.0);" \
		"}" \
		"FragColor = vec4(u_Phong_ADS_Light,1.0);" \
		"}";

	glShaderSource(gFragmentShaderObject_PF, 1, (const GLchar **)&fragmentShaderSourceCode_PF, NULL);

	//Compile Shader
	glCompileShader(gFragmentShaderObject_PF);

	//Should be complition error checking
	glGetShaderiv(gFragmentShaderObject_PF, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject_PF, iInfoLogLength, &written, szInfoLog);
				fprintf(GpFile, "Fragment Shader compilatin Log : %s\n", szInfoLog);
				free(szInfoLog);
				unInitilize();
				exit(0);
			}
		}
	}

	//*** Shader Program ***
	//Create
	//if (bVertex_Light == true)
	//{
	//	gShaderProgramObject = gShaderProgramObject_PV;
	//}

	gShaderProgramObject_PF = glCreateProgram();

	//attach Vertex Shader to shader program
	glAttachShader(gShaderProgramObject_PF, gVertexShaderObject_PF);

	//attach Fragment Shader to shader program
	glAttachShader(gShaderProgramObject_PF, gFragmentShaderObject_PF);

	//Pre-Link biding of shader program object with vertex shader position attrubute
	glBindAttribLocation(gShaderProgramObject_PF, ADS_ATTRUBUTE_VERTEX_Position, "vPosition");
	glBindAttribLocation(gShaderProgramObject_PF, ADS_ATTRIBUTE_NORMAL, "vNormal");

	//Link Shader
	glLinkProgram(gShaderProgramObject_PF);

	//Should be linker error checking
	 iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObject_PF, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iShaderProgramLinkStatus > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject_PF, iInfoLogLength, &written, szInfoLog);
				fprintf(GpFile, "Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				unInitilize();
				exit(0);
			}
		}
	}

	//get MVP uniform location
	modelMatrixUniform = glGetUniformLocation(gShaderProgramObject_PF, "u_model_matrix");
	perspectiveProjectionUniform = glGetUniformLocation(gShaderProgramObject_PF, "u_perspective_projection_matrix");
	viewMatrixUniform = glGetUniformLocation(gShaderProgramObject_PF, "u_View_Uniform");


	LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject_PF, "u_LKeyPress");

	LaUniform = glGetUniformLocation(gShaderProgramObject_PF, "u_LA");
	LdUniform = glGetUniformLocation(gShaderProgramObject_PF, "u_LD");
	LsUniform = glGetUniformLocation(gShaderProgramObject_PF, "u_LS");
	lightPositionUniform = glGetUniformLocation(gShaderProgramObject_PF, "u_Light_Position");

	KaUniform = glGetUniformLocation(gShaderProgramObject_PF, "u_KA");
	KdUniform = glGetUniformLocation(gShaderProgramObject_PF, "u_KD");
	KsUniform = glGetUniformLocation(gShaderProgramObject_PF, "u_KS");
	MatrialShiness = glGetUniformLocation(gShaderProgramObject_PF, "u_Matrial_Shiness");

	//*************************************** Per Fragment Lighting End ***************************************************************


	//Sphere veritces
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

		//Sphere recording starts
	glGenVertexArrays(1, &gVao_sphere);
	glBindVertexArray(gVao_sphere);

	glGenBuffers(1, &gVbo_sphere_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(ADS_ATTRUBUTE_VERTEX_Position, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ADS_ATTRUBUTE_VERTEX_Position);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// normal vbo
	glGenBuffers(1, &gVbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(ADS_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ADS_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// element vbo
	glGenBuffers(1, &gVbo_sphere_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// unbind vao
	glBindVertexArray(0);

	//Sphere recording End

	//Depth Functions
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//Set Clear color
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

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

	//AS// //*** perspective call for Programable Pipeline using Vmath
	//AS// gPerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	if (width <= height)
	{
		gPerspectiveProjectionMatrix = ortho(0.0f,
			15.5f,
			0.0f,//*((GLfloat)hight / (GLfloat)width),
			15.5f*((GLfloat)height / (GLfloat)width),
			-10.0f,
			10.0f);
		//co-ordinates writtenfor glVertex3f() are relative to viewing volume 
	}
	else
	{
		gPerspectiveProjectionMatrix = ortho(0.0f,
			15.5f*((GLfloat)width / (GLfloat)height),
			0.0f,//*((GLfloat)width / (GLfloat)hight),
			15.5f,
			-10.0f,
			10.0f);
	}
}

void display(void)
{
	//Function Declaration
	GLfloat UpdateAngle(GLfloat angle);
	void DrawSphere(void);

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Start using OpenGl Program Object
	if (bVertex_Light == true)
	{
		glUseProgram(gShaderProgramObject_PV);
	}
	else if (bFragment_Light == true)
	{
		glUseProgram(gShaderProgramObject_PF);
	}
		//Here you should draw

		//lighting Enable
		if (bLight == true)
		{
			
			GLfloat radius = 30.0f;
			GLfloat lightambint[] = { 0.0f, 0.0f, 0.0f };
			GLfloat lightdiffuse[] = { 1.0f, 1.0f, 1.0f };
			GLfloat lightspeculer[] = { 1.0f, 1.0f, 1.0f };
			if (XKeyPress == true)
			{
				GLfloat lightposition[] = { 0.0f, radius * (GLfloat)sin(Aangle1), radius * (GLfloat)cos(Aangle1), 1.0f };
				glUniform4fv(lightPositionUniform, 1, (GLfloat*)lightposition);
			}
			if (YKeyPress == true)
			{
				GLfloat lightposition[] = { radius * (GLfloat)sin(Aangle1), 0.0f, radius * (GLfloat)cos(Aangle1), 1.0f };
				glUniform4fv(lightPositionUniform, 1, (GLfloat*)lightposition);
			}
			if (ZKeyPress == true)
			{
				GLfloat lightposition[] = { radius * (GLfloat)sin(Aangle1), radius * (GLfloat)cos(Aangle1), 0.0f, 1.0f };
				glUniform4fv(lightPositionUniform, 1, (GLfloat*)lightposition);
			}
			if (XKeyPress == false && ZKeyPress == false && YKeyPress == false)
			{
				GLfloat lightposition[] = {0.0f ,0.0f, 100.0f,1.0f };
				glUniform4fv(lightPositionUniform, 1, (GLfloat*)lightposition);
			}
			//AS// GLfloat Matrilambint[] = { 0.0f, 0.0f, 0.0f };
			//AS// GLfloat Matrildiffuse[] = { 1.0f, 1.0f, 1.0f };
			//AS// GLfloat Matrilspeculer[] = { 1.0f, 1.0f, 1.0f };
			//AS// //GLfloat matrialshiness[] = { 128.0f };
			//AS// GLfloat matrialshiness = 128.0f;

			glUniform1i(LKeyPressedUniform, 1);

			glUniform3fv(LaUniform, 1, (GLfloat*)lightambint);
			glUniform3fv(LdUniform, 1, (GLfloat*)lightdiffuse);
			glUniform3fv(LsUniform, 1, (GLfloat*)lightspeculer);
			

			//AS// glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
			//AS// glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
			//AS// glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
			//AS// glUniform1f(MatrialShiness, matrialshiness);
			//AS// //glUniform4fv(MatrialShiness, 1, (GLfloat*)matrialshiness);

		}
		else
		{
			glUniform1i(LKeyPressedUniform, 0);
		}

		//Set modelview and modelviewprojection matrices to identity
		modelMatrix = mat4::identity();
		viewMatrix = mat4::identity();

		//AS// //*** Rectangle glTranslated related call using vmath.h ***
		//AS// modelMatrix = vmath::translate(0.0f, 0.0f, -6.0f) * vmath::rotate(Rangle, 1.0f, 0.0f, 0.0f);
		//AS// modelMatrix = modelMatrix * vmath::rotate(Rangle, 0.0f, 1.0f, 0.0f);
		//AS// modelMatrix = modelMatrix * vmath::rotate(Rangle, 0.0f, 0.0f, 1.0f);
		//AS// 
		//AS// //Scaling Cube
		//AS// modelMatrix = modelMatrix * vmath::scale(1.75f, 1.75f, 1.75f);
		//AS// 
		//AS// //Multiply the modelview and gPerspectiveProjectionMatrix to get model veiw projection matrix
		//AS// //modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix;
		//AS// 
		//AS// //Pass above modelviewprojectionmatrix to the vertex shader in 'u_mvp_matrix' shader variable
		//AS// //Whole position value we already clacluted in initWithFrame() by using glGetUniformLocation()
		//AS// glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
		//AS// glUniformMatrix4fv(perspectiveProjectionUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
		//AS// glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
		//AS// 
		//AS// //*** bind vao fro traingle***
		//AS// glBindVertexArray(gVao_sphere);
		//AS// 
		//AS// //*** draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
		//AS// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
		//AS// glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
		//AS// 
		//AS// //*** unBind Vao for triangle ***
		//AS// glBindVertexArray(0);

		//Spher Drawning starts

		//material array
		GLfloat Matrilambint[3];
		GLfloat Matrildiffuse[3];
		GLfloat Matrilspeculer[3];
		GLfloat matrialshiness;
		GLfloat specularExponent = 128.0f;

		//1st sphere on 1st column, emerald
		Matrilambint[0] = 0.0215f;
		Matrilambint[1] = 0.1745f;
		Matrilambint[2] = 0.0215f;

		Matrildiffuse[0] = 0.07568f;
		Matrildiffuse[1] = 0.61424f;
		Matrildiffuse[2] = 0.07568f;

		Matrilspeculer[0] = 0.633f;
		Matrilspeculer[1] = 0.727811f;
		Matrilspeculer[2] = 0.633f;

		matrialshiness = 0.6f * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(1.5f, 14.0f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(perspectiveProjectionUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
		glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);

		DrawSphere();


		//Spher 2 jade
		Matrilambint[0] = 0.135f;
		Matrilambint[1] = 0.2225f;
		Matrilambint[2] = 0.1575f;

		Matrildiffuse[0] = 0.54f;
		Matrildiffuse[1] = 0.89f;
		Matrildiffuse[2] = 0.63f;

		Matrilspeculer[0] = 0.316228f;
		Matrilspeculer[1] = 0.316228f;
		Matrilspeculer[2] = 0.316228f;

		matrialshiness = 0.1f * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(1.5f, 11.5f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

		DrawSphere();

		//Spher 3 obsidian
		Matrilambint[0] = 0.05375f;
		Matrilambint[1] = 0.05f;
		Matrilambint[2] = 0.06625f;

		Matrildiffuse[0] = 0.18275f;
		Matrildiffuse[1] = 0.17f;
		Matrildiffuse[2] = 0.22525f;

		Matrilspeculer[0] = 0.332741f;
		Matrilspeculer[1] = 0.328634f;
		Matrilspeculer[2] = 0.346435f;

		matrialshiness = 0.3f * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(1.5f, 9.0f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

		DrawSphere();

		//Spher 4 Pearl
		Matrilambint[0] = 0.25f;
		Matrilambint[1] = 0.20725f;
		Matrilambint[2] = 0.20725f;

		Matrildiffuse[0] = 1.0f;
		Matrildiffuse[1] = 0.829f;
		Matrildiffuse[2] = 0.829f;

		Matrilspeculer[0] = 0.296648f;
		Matrilspeculer[1] = 0.296648f;
		Matrilspeculer[2] = 0.296648f;

		matrialshiness = 0.088 * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(1.5f, 6.5f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

		DrawSphere();

		//Spher 5 ruby
		Matrilambint[0] = 0.1745f;
		Matrilambint[1] = 0.01175f;
		Matrilambint[2] = 0.01175f;

		Matrildiffuse[0] = 0.61424f;
		Matrildiffuse[1] = 0.04136f;
		Matrildiffuse[2] = 0.04136f;

		Matrilspeculer[0] = 0.72811f;
		Matrilspeculer[1] = 0.626959f;
		Matrilspeculer[2] = 0.626959f;

		matrialshiness = 0.6 * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(1.5f, 4.0f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

		DrawSphere();

		//Spher 6 turquoise
		Matrilambint[0] = 0.1f;
		Matrilambint[1] = 0.18725f;
		Matrilambint[2] = 0.1745f;

		Matrildiffuse[0] = 0.396f;
		Matrildiffuse[1] = 0.74151f;
		Matrildiffuse[2] = 0.69102f;

		Matrilspeculer[0] = 0.297254f;
		Matrilspeculer[1] = 0.223529f;
		Matrilspeculer[2] = 0.027451f;

		matrialshiness = 0.1 * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(1.5f, 1.5f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

		DrawSphere();

		//Spher 7  1st sphere on 2nd column, brass
		Matrilambint[0] = 0.329412f;
		Matrilambint[1] = 0.223529f;
		Matrilambint[2] = 0.027451f;

		Matrildiffuse[0] = 0.780392f;
		Matrildiffuse[1] = 0.568627f;
		Matrildiffuse[2] = 0.113725f;

		Matrilspeculer[0] = 0.992157f;
		Matrilspeculer[1] = 0.941176f;
		Matrilspeculer[2] = 0.807843f;

		matrialshiness = 0.21794872 * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(9.5f, 14.0f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

		DrawSphere();

		//Spher 8 2nd sphere on 2nd column, bronze
		Matrilambint[0] = 0.2125f;
		Matrilambint[1] = 0.1275f;
		Matrilambint[2] = 0.054f;

		Matrildiffuse[0] = 0.714f;
		Matrildiffuse[1] = 0.4284f;
		Matrildiffuse[2] = 0.18144f;

		Matrilspeculer[0] = 0.393548f;
		Matrilspeculer[1] = 0.271906f;
		Matrilspeculer[2] = 0.18144f;

		matrialshiness = 0.2 * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(9.5f, 11.5f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

		DrawSphere();

		//Spher 9 3rd sphere on 2nd column, chrome 
		Matrilambint[0] = 0.25f;
		Matrilambint[1] = 0.25f;
		Matrilambint[2] = 0.25f;

		Matrildiffuse[0] = 0.4f;
		Matrildiffuse[1] = 0.4f;
		Matrildiffuse[2] = 0.4f;

		Matrilspeculer[0] = 0.774597f;
		Matrilspeculer[1] = 0.774597f;
		Matrilspeculer[2] = 0.774597f;

		matrialshiness = 0.6 * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(9.5f, 9.0f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

		DrawSphere();

		//Spher 10 4th sphere on 2nd column, copper
		Matrilambint[0] = 0.19125f;
		Matrilambint[1] = 0.0735f;
		Matrilambint[2] = 0.0225f;

		Matrildiffuse[0] = 0.7038f;
		Matrildiffuse[1] = 0.27048f;
		Matrildiffuse[2] = 0.0828f;

		Matrilspeculer[0] = 0.256777f;
		Matrilspeculer[1] = 0.137622f;
		Matrilspeculer[2] = 0.086014f;

		matrialshiness = 0.1 * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(9.5f, 6.5f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

		DrawSphere();

		//Sphrer 11 5th sphere on 2nd column, gold
		Matrilambint[0] = 0.24725f;
		Matrilambint[1] = 0.1995f;
		Matrilambint[2] = 0.0745f;

		Matrildiffuse[0] = 0.75164f;
		Matrildiffuse[1] = 0.60648f;
		Matrildiffuse[2] = 0.22648f;

		Matrilspeculer[0] = 0.628281f;
		Matrilspeculer[1] = 0.555802f;
		Matrilspeculer[2] = 0.366065f;

		matrialshiness = 0.4 * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(9.5f, 4.0f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

		DrawSphere();

		//Spher 12 6th sphere on 2nd column, silver
		Matrilambint[0] = 0.19225f;
		Matrilambint[1] = 0.19225f;
		Matrilambint[2] = 0.19225f;

		Matrildiffuse[0] = 0.50754f;
		Matrildiffuse[1] = 0.50754f;
		Matrildiffuse[2] = 0.50754f;

		Matrilspeculer[0] = 0.508273f;
		Matrilspeculer[1] = 0.508273f;
		Matrilspeculer[2] = 0.508273f;

		matrialshiness = 0.4 * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(9.5f, 1.5f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

		DrawSphere();

		//Spher 13 1st sphere on 3rd column, black
		Matrilambint[0] = 0.0f;
		Matrilambint[1] = 0.0f;
		Matrilambint[2] = 0.0f;

		Matrildiffuse[0] = 0.01f;
		Matrildiffuse[1] = 0.01f;
		Matrildiffuse[2] = 0.01f;

		Matrilspeculer[0] = 0.50f;
		Matrilspeculer[1] = 0.50f;
		Matrilspeculer[2] = 0.50f;

		matrialshiness = 0.25 * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(17.5f, 14.0f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

		DrawSphere();

		//Spher 14 2nd sphere on 3rd column, cyan
		Matrilambint[0] = 0.0f;
		Matrilambint[1] = 0.1f;
		Matrilambint[2] = 0.06f;

		Matrildiffuse[0] = 0.0f;
		Matrildiffuse[1] = 0.50980392f;
		Matrildiffuse[2] = 0.50980392f;

		Matrilspeculer[0] = 0.50196078f;
		Matrilspeculer[1] = 0.50196078f;
		Matrilspeculer[2] = 0.50196078f;

		matrialshiness = 0.25 * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(17.5f, 11.5f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

		DrawSphere();

		//Spher 15 3rd sphere on 2nd column, green
		Matrilambint[0] = 0.0f;
		Matrilambint[1] = 0.0f;
		Matrilambint[2] = 0.0f;

		Matrildiffuse[0] = 0.1f;
		Matrildiffuse[1] = 0.35f;
		Matrildiffuse[2] = 0.1f;

		Matrilspeculer[0] = 0.45f;
		Matrilspeculer[1] = 0.55f;
		Matrilspeculer[2] = 0.45f;

		matrialshiness = 0.25 * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(17.5f, 9.0f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

		DrawSphere();

		//Spher 16 4th sphere on 3rd column, red
		Matrilambint[0] = 0.0f;
		Matrilambint[1] = 0.0f;
		Matrilambint[2] = 0.0f;

		Matrildiffuse[0] = 0.5f;
		Matrildiffuse[1] = 0.0f;
		Matrildiffuse[2] = 0.0f;

		Matrilspeculer[0] = 0.7f;
		Matrilspeculer[1] = 0.6f;
		Matrilspeculer[2] = 0.6f;

		matrialshiness = 0.25 * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(17.5f, 6.5f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

		DrawSphere();

		//Spher 17 5th sphere on 3rd column, white
		Matrilambint[0] = 0.0f;
		Matrilambint[1] = 0.0f;
		Matrilambint[2] = 0.0f;

		Matrildiffuse[0] = 0.55f;
		Matrildiffuse[1] = 0.55f;
		Matrildiffuse[2] = 0.55f;

		Matrilspeculer[0] = 0.70f;
		Matrilspeculer[1] = 0.70f;
		Matrilspeculer[2] = 0.70f;

		matrialshiness = 0.25 * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(17.5f, 4.0f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

		DrawSphere();

		//Spher 18 6th sphere on 3rd column, yellow plastic
		Matrilambint[0] = 0.0f;
		Matrilambint[1] = 0.0f;
		Matrilambint[2] = 0.0f;

		Matrildiffuse[0] = 0.5f;
		Matrildiffuse[1] = 0.5f;
		Matrildiffuse[2] = 0.5f;

		Matrilspeculer[0] = 0.60f;
		Matrilspeculer[1] = 0.60f;
		Matrilspeculer[2] = 0.50f;

		matrialshiness = 0.25 * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(17.5f, 1.5f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

		DrawSphere();

		//Spher 19 1st sphere on 4th column, black
		Matrilambint[0] = 0.02f;
		Matrilambint[1] = 0.02f;
		Matrilambint[2] = 0.02f;

		Matrildiffuse[0] = 0.01f;
		Matrildiffuse[1] = 0.01f;
		Matrildiffuse[2] = 0.01f;

		Matrilspeculer[0] = 0.4f;
		Matrilspeculer[1] = 0.4f;
		Matrilspeculer[2] = 0.4f;

		matrialshiness = 0.078125 * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(25.5f, 14.0f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

		DrawSphere();

		//Spher 20 2nd sphere on 4th column, cyan
		Matrilambint[0] = 0.0f;
		Matrilambint[1] = 0.05f;
		Matrilambint[2] = 0.05f;

		Matrildiffuse[0] = 0.4f;
		Matrildiffuse[1] = 0.5f;
		Matrildiffuse[2] = 0.5f;

		Matrilspeculer[0] = 0.04f;
		Matrilspeculer[1] = 0.7f;
		Matrilspeculer[2] = 0.7f;

		matrialshiness = 0.078125 * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(25.5f, 11.5f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

		DrawSphere();

		//Spher 21 3rd sphere on 4th column, green
		Matrilambint[0] = 0.0f;
		Matrilambint[1] = 0.05f;
		Matrilambint[2] = 0.0f;

		Matrildiffuse[0] = 0.4f;
		Matrildiffuse[1] = 0.5f;
		Matrildiffuse[2] = 0.4f;

		Matrilspeculer[0] = 0.04f;
		Matrilspeculer[1] = 0.7f;
		Matrilspeculer[2] = 0.04f;

		matrialshiness = 0.078125 * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(25.5f, 9.0f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

		DrawSphere();

		//Spher 22 4th sphere on 4th column, red
		Matrilambint[0] = 0.5f;
		Matrilambint[1] = 0.0f;
		Matrilambint[2] = 0.0f;

		Matrildiffuse[0] = 0.5f;
		Matrildiffuse[1] = 0.4f;
		Matrildiffuse[2] = 0.4f;

		Matrilspeculer[0] = 0.7f;
		Matrilspeculer[1] = 0.04f;
		Matrilspeculer[2] = 0.04f;

		matrialshiness = 0.078125 * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(25.5f, 6.5f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

		DrawSphere();

		//Spher 23 5th sphere on 4th column, white
		Matrilambint[0] = 0.05f;
		Matrilambint[1] = 0.05f;
		Matrilambint[2] = 0.05f;

		Matrildiffuse[0] = 0.5f;
		Matrildiffuse[1] = 0.5f;
		Matrildiffuse[2] = 0.5f;

		Matrilspeculer[0] = 0.7f;
		Matrilspeculer[1] = 0.7f;
		Matrilspeculer[2] = 0.7f;

		matrialshiness = 0.078125 * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(25.5f, 4.0f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

		DrawSphere();

		//Spher 24 6th sphere on 4th column, yellow rubber
		Matrilambint[0] = 0.05f;
		Matrilambint[1] = 0.05f;
		Matrilambint[2] = 0.0f;

		Matrildiffuse[0] = 0.5f;
		Matrildiffuse[1] = 0.5f;
		Matrildiffuse[2] = 0.4f;

		Matrilspeculer[0] = 0.7f;
		Matrilspeculer[1] = 0.7f;
		Matrilspeculer[2] = 0.04f;

		matrialshiness = 0.078125 * specularExponent;

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);

		modelMatrix = vmath::translate(25.5f, 1.5f, 0.0f) *  vmath::scale(2.25f, 2.25f, 2.25f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

		DrawSphere();

		//Stop using OpenGL Program Object
		glUseProgram(0);
	
	//Update Both angles
	if (bAnimate == true)
	{
		Rangle = UpdateAngle(Rangle);
	}
	if (XKeyPress == true || ZKeyPress == true || YKeyPress == true)
	{
		Aangle1 = UpdateAngle(Aangle1);
	}

	SwapBuffers(ghdc);
}

void DrawSphere(void)
{
	//*** bind vao fro traingle***
	glBindVertexArray(gVao_sphere);

	//*** draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//*** unBind Vao for triangle ***
	glBindVertexArray(0);
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
	if (gVao_sphere)
	{
		glDeleteVertexArrays(1, &gVao_sphere);
		gVao_sphere = 0;
	}

	//destroy vbo
	if (gVbo_sphere_position)
	{
		glDeleteBuffers(1, &gVbo_sphere_position);
		gVbo_sphere_position = 0;
	}

	if (gVbo_sphere_normal)
	{
		glDeleteBuffers(1, &gVbo_sphere_normal);
		gVbo_sphere_normal = 0;
	}

	if (gVbo_sphere_element)
	{
		glDeleteBuffers(1, &gVbo_sphere_element);
		gVbo_sphere_element = 0;
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

	if (gShaderProgramObject_PV)
	{
		glUseProgram(gShaderProgramObject_PV);
		GLsizei shaderCount;
		glGetProgramiv(gShaderProgramObject_PV, GL_ATTACHED_SHADERS, &shaderCount);
		GLuint *pShader = NULL;
		pShader = (GLuint*)malloc(shaderCount * sizeof(shaderCount));
		glGetAttachedShaders(gShaderProgramObject_PV, shaderCount, &shaderCount, pShader);
		for (GLsizei i = 0; i < shaderCount; i++)
		{
			glDetachShader(gShaderProgramObject_PV, pShader[i]);
			glDeleteShader(pShader[i]);
			pShader[i] = 0;
		}
		if (pShader)
		{
			free(pShader);
		}
		glDeleteProgram(gShaderProgramObject_PV);
		gShaderProgramObject_PV = 0;
		glUseProgram(0);
	}

	if (gShaderProgramObject_PF)
	{
		glUseProgram(gShaderProgramObject_PF);
		GLsizei shaderCount;
		glGetProgramiv(gShaderProgramObject_PF, GL_ATTACHED_SHADERS, &shaderCount);
		GLuint *pShader = NULL;
		pShader = (GLuint*)malloc(shaderCount * sizeof(shaderCount));
		glGetAttachedShaders(gShaderProgramObject_PF, shaderCount, &shaderCount, pShader);
		for (GLsizei i = 0; i < shaderCount; i++)
		{
			glDetachShader(gShaderProgramObject_PF, pShader[i]);
			glDeleteShader(pShader[i]);
			pShader[i] = 0;
		}
		if (pShader)
		{
			free(pShader);
		}
		glDeleteProgram(gShaderProgramObject_PF);
		gShaderProgramObject_PF = 0;
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