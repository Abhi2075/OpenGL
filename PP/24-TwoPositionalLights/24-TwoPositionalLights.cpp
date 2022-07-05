//Headers
#include<windows.h>
#include<stdio.h>
#include<gl/glew.h> //This line must be before GL.h
#include<gl/GL.h>
#include<vmath.h> //for Matrix calclulation
//#include<Sphere.h>
#include"Image.h"

#define WIN_WIDTH 800
#define WIN_HIGHT 600

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")
//#pragma comment(lib,"Sphere.lib")

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
GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint gVao_Cube ;
GLuint gVbo_Position_Cube;
GLuint gVbo_Normal_Cube;

//GLuint gMVPUniform;
GLuint modelMatrixUniform;
GLuint perspectiveProjectionUniform;
GLuint viewMatrixUniform;

GLuint LKeyPressedUniform;

//Light Uniform
GLuint LaUniform[2];
GLuint LdUniform[2];
GLuint LsUniform[2];
GLuint lightPositionUniform[2];

GLuint KaUniform;
GLuint KdUniform;
GLuint KsUniform;
GLuint MatrialShiness;




bool bAnimate = false;
bool bLight = false;

mat4 modelMatrix;
mat4 viewMatrix;
mat4 gPerspectiveProjectionMatrix;

//AS// //Sphere Variables
//AS// float sphere_vertices[1146];
//AS// float sphere_normals[1146];
//AS// float sphere_textures[764];
//AS// unsigned short sphere_elements[2280];
//AS// GLuint gNumVertices;
//AS// GLuint gNumElements;
//AS// GLuint gVao_sphere;
//AS// GLuint gVbo_sphere_position;
//AS// GLuint gVbo_sphere_normal;
//AS// GLuint gVbo_sphere_element;

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

	case WM_CHAR: 
	{
		switch (wParam) {
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

	//*** Vertex Shader ***
	//Create Shader
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

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
		"uniform vec3 u_LA[2];" \
		"uniform vec3 u_LD[2];" \
		"uniform vec3 u_LS[2];" \
		"uniform vec3 u_KA;" \
		"uniform vec3 u_KD;" \
		"uniform vec3 u_KS;" \
		"uniform float u_Matrial_Shiness;" \
		"uniform vec4 u_Light_Position[2];" \
		"out vec3 u_Phong_ADS_Light_temp[2];" \
		"out vec3 u_Phong_ADS_Light;" \
		"vec3 ambint[2];" \
		"vec3 diffuse[2];" \
		"vec3 specular[2];" \
		"vec3 light_source[2];" \
		"vec3 reflection_vector[2];" \
		"void main(void)" \
		"{" \
			"if(u_LKeyPress == 1)" \
			"{" \
				"vec4 eye_coordinates = u_View_Uniform * u_model_matrix * vPosition;" \
				"mat3 normal_matrix = mat3(u_View_Uniform * u_model_matrix);" \
				"vec3 transformed_norm = normalize(normal_matrix * vNormal);" \
				"vec3 view_vector = normalize((vec3(-eye_coordinates)));" \
				"for(int i = 0; i < 2; i++)" \
				"{" \
					"light_source[i] = normalize(vec3(u_Light_Position[i] - eye_coordinates)); " \
					"reflection_vector[i] = reflect(-light_source[i],transformed_norm);" \
					"ambint[i] = u_LA[i] * u_KA;" \
					"diffuse[i] = u_LD[i] * u_KD * max(dot(light_source[i],transformed_norm),0.0f);" \
					"specular[i] = u_LS[i] * u_KS * pow(max(dot(reflection_vector[i],view_vector),0.0f),u_Matrial_Shiness);" \
					"u_Phong_ADS_Light_temp[i] = ambint[i] + diffuse[i] + specular[i];" \
				"}" \
				"u_Phong_ADS_Light = u_Phong_ADS_Light_temp[0] + u_Phong_ADS_Light_temp[1];" \
			"}" \
			"else" \
			"{" \
			"u_Phong_ADS_Light = vec3(1.0,1.0,1.0);" \
			"}" \
			"gl_Position = u_perspective_projection_matrix * u_View_Uniform * u_model_matrix * vPosition;" \
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
				fprintf(GpFile, "Fragment Shader compilatin Log : %s\n", szInfoLog);
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
	glBindAttribLocation(gShaderProgramObject, ADS_ATTRIBUTE_NORMAL, "vNormal");

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
	modelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
	perspectiveProjectionUniform = glGetUniformLocation(gShaderProgramObject, "u_perspective_projection_matrix");
	viewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_View_Uniform");
	

	LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_LKeyPress");

	LaUniform[0] = glGetUniformLocation(gShaderProgramObject, "u_LA[0]");
	LdUniform[0] = glGetUniformLocation(gShaderProgramObject, "u_LD[0]");
	LsUniform[0] = glGetUniformLocation(gShaderProgramObject, "u_LS[0]");
	lightPositionUniform[0] = glGetUniformLocation(gShaderProgramObject, "u_Light_Position[0]");

	LaUniform[1] = glGetUniformLocation(gShaderProgramObject, "u_LA[1]");
	LdUniform[1] = glGetUniformLocation(gShaderProgramObject, "u_LD[1]");
	LsUniform[1] = glGetUniformLocation(gShaderProgramObject, "u_LS[1]");
	lightPositionUniform[1] = glGetUniformLocation(gShaderProgramObject, "u_Light_Position[1]");

	/*for (int i = 0; i < 3; i++)
	{
		LaUniform[i] = glGetUniformLocation(gShaderProgramObject, "u_LA[i]");
		LdUniform[i] = glGetUniformLocation(gShaderProgramObject, "u_LD[i]");
		LsUniform[i] = glGetUniformLocation(gShaderProgramObject, "u_LS[i]");
		lightPositionUniform[i] = glGetUniformLocation(gShaderProgramObject, "u_Light_Position[i]");
	}*/
	KaUniform = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform = glGetUniformLocation(gShaderProgramObject, "u_KS");
	MatrialShiness = glGetUniformLocation(gShaderProgramObject, "u_Matrial_Shiness");
	

	//*** Vertices, Colors, shader attributers,vbo, vao, initialization ***
	//Feed Vertices through Variable Name
	GLfloat cubeVertex = 1.0f;
	GLfloat Zero = 0.0f;
	GLfloat cubeNormal = 1.0f;
	const GLfloat rectangleNormals[] =
	{
		//Front
		Zero,Zero,cubeNormal,
		Zero,Zero,cubeNormal,
		Zero,Zero,cubeNormal,
		Zero,Zero,cubeNormal,

		//Back
		Zero,Zero,-cubeNormal,
		Zero,Zero,-cubeNormal,
		Zero,Zero,-cubeNormal,
		Zero,Zero,-cubeNormal,

		//Right
		cubeNormal,Zero,Zero,
		cubeNormal,Zero,Zero,
		cubeNormal,Zero,Zero,
		cubeNormal,Zero,Zero,

		//Left
		-cubeNormal,Zero,Zero,
		-cubeNormal,Zero,Zero,
		-cubeNormal,Zero,Zero,
		-cubeNormal,Zero,Zero,

		//Top
		Zero,cubeNormal,Zero,
		Zero,cubeNormal,Zero,
		Zero,cubeNormal,Zero,
		Zero,cubeNormal,Zero,

		//Bottom
		Zero,-cubeNormal,Zero,
		Zero,-cubeNormal,Zero,
		Zero,-cubeNormal,Zero,
		Zero,-cubeNormal,Zero,
	};

	const GLfloat rectangleVertices[] =
	{
		//Front
		cubeVertex,cubeVertex,cubeVertex,
		-cubeVertex,cubeVertex,cubeVertex,
		-cubeVertex,-cubeVertex,cubeVertex,
		cubeVertex,-cubeVertex,cubeVertex,

		//Back
		cubeVertex,cubeVertex,-cubeVertex,
		-cubeVertex,cubeVertex,-cubeVertex,
		-cubeVertex,-cubeVertex,-cubeVertex,
		cubeVertex,-cubeVertex,-cubeVertex,

		//Right
		cubeVertex,cubeVertex,-cubeVertex,
		cubeVertex,cubeVertex,cubeVertex,
		cubeVertex,-cubeVertex,cubeVertex,
		cubeVertex,-cubeVertex,-cubeVertex,

		//Left
		-cubeVertex,cubeVertex,cubeVertex,
		-cubeVertex,cubeVertex,-cubeVertex,
		-cubeVertex,-cubeVertex,-cubeVertex,
		-cubeVertex,-cubeVertex,cubeVertex,

		//Top
		cubeVertex,cubeVertex,cubeVertex,
		cubeVertex,cubeVertex,-cubeVertex,
		-cubeVertex,cubeVertex,-cubeVertex,
		-cubeVertex,cubeVertex,cubeVertex,

		//Bottom
		cubeVertex,-cubeVertex,cubeVertex,
		cubeVertex,-cubeVertex,-cubeVertex,
		-cubeVertex,-cubeVertex,-cubeVertex,
		-cubeVertex,-cubeVertex,cubeVertex,
	};

	//Rectangle recording starts
	glGenVertexArrays(1, &gVao_Cube);
	glBindVertexArray(gVao_Cube);

	glGenBuffers(1, &gVbo_Position_Cube);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(ADS_ATTRUBUTE_VERTEX_Position, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ADS_ATTRUBUTE_VERTEX_Position);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gVbo_Normal_Cube);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Normal_Cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleNormals), rectangleNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(ADS_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ADS_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	//Rectangle recording End

	//Depth Functions
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

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

	//lighting Enable
	if (bLight == true)
	{
		struct Light {
			vec3 LightAmbintColor;
			vec3 LightDiffuseColor;
			vec3 LightSpecularColor;
			vec4 LightPosition;
		};

		struct Light light[2];

		light[0].LightAmbintColor = { 0.0f, 0.0f, 0.0f };
		light[0].LightDiffuseColor = { 1.0f, 0.0f, 0.0f };
		light[0].LightSpecularColor = { 1.0f, 0.0f, 0.0f };
		light[0].LightPosition = { -2.0f, 0.0f, 0.0f, 1.0f };

		light[1].LightAmbintColor = { 0.0f, 0.0f, 0.0f };
		light[1].LightDiffuseColor = { 0.0f, 0.0f, 1.0f };
		light[1].LightSpecularColor = { 0.0f, 0.0f, 1.0f };
		light[1].LightPosition = { 2.0f, 0.0f, 0.0f, 1.0f };

		GLfloat Matrilambint[] = { 0.0f, 0.0f, 0.0f };
		GLfloat Matrildiffuse[] = { 1.0f, 1.0f, 1.0f };
		GLfloat Matrilspeculer[] = { 1.0f, 1.0f, 1.0f };
		//GLfloat matrialshiness[] = { 128.0f };
		GLfloat matrialshiness = 128.0f;

		glUniform1i(LKeyPressedUniform, 1);

		for (int i = 0; i < 2; i++)
		{
			glUniform3fv(LaUniform[i], 1, light[i].LightAmbintColor);
			glUniform3fv(LdUniform[i], 1, light[i].LightDiffuseColor);
			glUniform3fv(LsUniform[i], 1, light[i].LightSpecularColor);
			glUniform4fv(lightPositionUniform[i], 1, light[i].LightPosition);
		}
		//AS// glUniform3fv(LaUniform[1], 1, light[1].LightAmbintColor);
		//AS// glUniform3fv(LdUniform[1], 1, light[1].LightDiffuseColor);
		//AS// glUniform3fv(LsUniform[1], 1, light[1].LightSpecularColor);
		//AS// glUniform4fv(lightPositionUniform[1], 1, light[1].LightPosition);

		glUniform3fv(KaUniform, 1, (GLfloat*)Matrilambint);
		glUniform3fv(KdUniform, 1, (GLfloat*)Matrildiffuse);
		glUniform3fv(KsUniform, 1, (GLfloat*)Matrilspeculer);
		glUniform1f(MatrialShiness, matrialshiness);
		//glUniform4fv(MatrialShiness, 1, (GLfloat*)matrialshiness);
		
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	//Set modelview and modelviewprojection matrices to identity
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	//*** Rectangle glTranslated related call using vmath.h ***
	modelMatrix = vmath::translate(0.0f, 0.0f, -6.0f) * vmath::rotate(Rangle, 0.0f, 1.0f, 0.0f);
	modelMatrix = modelMatrix * vmath::rotate(Rangle, 1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix * vmath::rotate(Rangle, 0.0f, 0.0f, 1.0f);

	//Scaling Cube
	modelMatrix = modelMatrix * vmath::scale(0.75f, 0.75f, 0.75f);

	//Multiply the modelview and gPerspectiveProjectionMatrix to get model veiw projection matrix
	//modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix;

	//Pass above modelviewprojectionmatrix to the vertex shader in 'u_mvp_matrix' shader variable
	//Whole position value we already clacluted in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(perspectiveProjectionUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);

	//*** bind vao fro traingle***
	glBindVertexArray(gVao_Cube);

	//*** draw either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4); //0-4 (each with its x,y,z,w) vertices in cubevertices array
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4); //4-4 (each with its x,y,z,w) vertices in cubevertices array
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4); //8-4 (each with its x,y,z,w) vertices in cubevertices array
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);//12-4 (each with its x,y,z,w) vertices in cubevertices array
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);//16-4 (each with its x,y,z,w) vertices in cubevertices array
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);//20-4 (each with its x,y,z,w) vertices in cubevertices array

	//*** unBind Vao for triangle ***
	glBindVertexArray(0);

	//Stop using OpenGL Program Object
	glUseProgram(0);

	//Update Both angles
	if (bAnimate == true)
	{
		Rangle = UpdateAngle(Rangle);
	}

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
	if (gVao_Cube)
	{
		glDeleteVertexArrays(1, &gVao_Cube);
		gVao_Cube = 0;
	}

	//destroy vbo
	if (gVbo_Position_Cube)
	{
		glDeleteBuffers(1, &gVbo_Position_Cube);
		gVbo_Position_Cube = 0;
	}

	if (gVbo_Normal_Cube)
	{
		glDeleteBuffers(1, &gVbo_Normal_Cube);
		gVbo_Normal_Cube = 0;
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