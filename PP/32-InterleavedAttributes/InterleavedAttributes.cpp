//Headers
#include<windows.h>
#include<stdio.h>
#include<gl/glew.h> //This line must be before GL.h
#include<gl/GL.h>
#include<vmath.h> //for Matrix calclulation
#include"Image.h"

#define WIN_WIDTH 800
#define WIN_HIGHT 600
#define BUFFER_OFFSET(i) ((void*)(i))
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
//ASGLfloat Tangle = 0.0f;
GLfloat Rangle = 0.0f;
//AS//GLuint Stone_Texture;
GLuint Vijay_Kundali_Texture;

GLuint textureSamplerUniform;

//Variables for Shaders
GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

//GLuint gVao_Pyramid;
//GLuint gVbo_Position_Pyramid;
//GLuint gVbo_Texture_Pyramid;

GLuint gVao_Cube ;
GLuint gVbo_Position_Cube;
GLuint gVbo_Texture_Cube;
GLuint gVbo_Color_Cube;
GLuint gVbo_Normal_Cube;
bool bAnimate = false;
bool bTexture = false;
bool bLight = false;
bool bColor = false;
//GLuint gMVPUniform;
GLuint modelMatrixUniform;
GLuint perspectiveProjectionUniform;
GLuint viewMatrixUniform;

GLuint LKeyPressedUniform;
GLuint TKeyPressedUniform;
GLuint CKeyPressedUniform;

//Light Uniform
GLuint LaUniform;
GLuint LdUniform;
GLuint LsUniform;
GLuint lightPositionUniform;

GLuint KaUniform;
GLuint KdUniform;
GLuint KsUniform;
GLuint MatrialShiness;


mat4 modelMatrix;
mat4 viewMatrix;
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
	case WM_CHAR: {
		switch (wParam) {
		case'A': {
			bAnimate = !bAnimate;
			break;
		}
		case'a': {
			bAnimate = !bAnimate;
			break;
		}

		case'C':
		case'c': {
			bColor = !bColor;
			break;
		}
		case'l':
		case'L': {
			bLight = !bLight;
			break;
		}
		
		case'T':
		case't':
		{
			bTexture = !bTexture;
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
	bool loadGlTexture(GLuint *texture, TCHAR(resourceID[]));

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
		"in vec2 vTexCoord;" \
		"in vec4 vColor;" \
		"out vec4 out_Color;" \
		"out vec2 out_TexCoord;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_View_Uniform;" \
		"uniform mat4 u_perspective_projection_matrix;" \
		"uniform mat4 u_model_matrix;" \
		"uniform int u_LKeyPress;" \
		"uniform int u_TKeyPress;" \
		"uniform int u_CKeyPress;" \
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
				"vec3 specular = u_LS * u_KS * pow(max(dot(reflection_vector,view_vector),0.0f),u_Matrial_Shiness);" \
				"u_Phong_ADS_Light = ambint + diffuse + specular;" \
			"}" \
			"else" \
			"{" \
				"u_Phong_ADS_Light = vec3(1.0,1.0,1.0);" \
			"}" \
			"gl_Position = u_perspective_projection_matrix * u_View_Uniform * u_model_matrix * vPosition;" \
			"out_TexCoord = vTexCoord;" \
			"out_Color = vColor;"
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
		"in vec2 out_TexCoord;" \
		"uniform sampler2D u_Texture_Sampler;" \
		"in vec3 u_Phong_ADS_Light;" \
		"uniform int u_LKeyPress;" \
		"uniform int u_TKeyPress;" \
		"uniform int u_CKeyPress;" \
		"in vec4 out_Color;" \
		"out vec4 FragColor;" \
		"vec4 vTextureColor;" \
		"vec4 vColor;" \
		"void main(void)" \
		"{" \
			"if(u_TKeyPress == 1)" \
			"{" \
				"vTextureColor = texture(u_Texture_Sampler,out_TexCoord);" \
			"}" \
			"else" \
			"{" \
				"vTextureColor = vec4(1.0,1.0,1.0,1.0);" \
			"}" \
			"if(u_CKeyPress == 1)" \
			"{" \
				"vColor = out_Color;" \
			"}" \
			"else" \
			"{" \
				"vColor = vec4(1.0,1.0,1.0,1.0);" \
			"}" \
			"FragColor = vColor * vTextureColor * vec4(u_Phong_ADS_Light,1.0);" \
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
	glBindAttribLocation(gShaderProgramObject, ADS_ATTRIBUTE_TEXTURE0, "vTexCoord");
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
	//AS//gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix"); 
	textureSamplerUniform = glGetUniformLocation(gShaderProgramObject, "u_Texture_Sampler");
	modelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
	perspectiveProjectionUniform = glGetUniformLocation(gShaderProgramObject, "u_perspective_projection_matrix");
	viewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_View_Uniform");


	LKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_LKeyPress");
	TKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_TKeyPress");
	CKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_CKeyPress");

	LaUniform = glGetUniformLocation(gShaderProgramObject, "u_LA");
	LdUniform = glGetUniformLocation(gShaderProgramObject, "u_LD");
	LsUniform = glGetUniformLocation(gShaderProgramObject, "u_LS");
	lightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_Light_Position");

	KaUniform = glGetUniformLocation(gShaderProgramObject, "u_KA");
	KdUniform = glGetUniformLocation(gShaderProgramObject, "u_KD");
	KsUniform = glGetUniformLocation(gShaderProgramObject, "u_KS");
	MatrialShiness = glGetUniformLocation(gShaderProgramObject, "u_Matrial_Shiness");

	//*** Vertices, Colors, shader attributers,vbo, vao, initialization ***
	//Feed Vertices through Variable Name
	//AS//GLfloat PraymedVertex = 1.0f;
	GLfloat CubeVertex = 1.0f;
	GLfloat Zero = 0.0f;
	GLfloat cubeColor = 1.0f;
	GLfloat cubeNormal = 1.0f;
	//CubeglTexCoord2f
	GLfloat CubeLeftXBottom = 0.0f;
	GLfloat CubeLeftYBottom = 0.0f;
	GLfloat CubeRightXBottom = 1.0f;
	GLfloat CubeRightYBottom = 0.0f;
	GLfloat CubeLeftXTop = 0.0f;
	GLfloat CubeLeftYTop = 1.0f;
	GLfloat CubeRightXTop = 1.0f;
	GLfloat CubeRightYTop = 1.0f;

	
	const GLfloat rectangleVertices[] =
	{ 
		//Front
		CubeVertex,CubeVertex,CubeVertex,
		cubeColor,Zero,Zero,cubeColor,
		Zero,Zero,cubeNormal,
		CubeRightXTop, CubeRightYTop,

		-CubeVertex,CubeVertex,CubeVertex,
		cubeColor,Zero,Zero,cubeColor,
		Zero,Zero,cubeNormal,
		CubeLeftXTop, CubeLeftYTop,

		-CubeVertex,-CubeVertex,CubeVertex,
		cubeColor,Zero,Zero,cubeColor,
		Zero,Zero,cubeNormal,
		CubeLeftXBottom, CubeLeftYBottom,

		CubeVertex,-CubeVertex,CubeVertex,
		cubeColor,Zero,Zero,cubeColor,
		Zero,Zero,cubeNormal,
		CubeRightXBottom, CubeRightYBottom,

		//Back
		CubeVertex,CubeVertex,-CubeVertex,
		Zero,Zero,cubeColor,cubeColor,
		Zero,Zero,-cubeNormal,
		CubeRightXTop, CubeRightYTop,

		-CubeVertex,CubeVertex,-CubeVertex,
		Zero,Zero,cubeColor,cubeColor,
		Zero,Zero,-cubeNormal,
		CubeLeftXTop, CubeLeftYTop,

		-CubeVertex,-CubeVertex,-CubeVertex,
		Zero,Zero,cubeColor,cubeColor,
		Zero,Zero,-cubeNormal,
		CubeLeftXBottom, CubeLeftYBottom,

		CubeVertex,-CubeVertex,-CubeVertex,
		Zero,Zero,cubeColor,cubeColor,
		Zero,Zero,-cubeNormal,
		CubeRightXBottom, CubeRightYBottom,
		
		//Right
		CubeVertex,CubeVertex,-CubeVertex,
		Zero,cubeColor,Zero,cubeColor,
		cubeNormal,Zero,Zero,
		CubeRightXTop, CubeRightYTop,

		CubeVertex,CubeVertex,CubeVertex,
		Zero,cubeColor,Zero,cubeColor,
		cubeNormal,Zero,Zero,
		CubeLeftXTop, CubeLeftYTop,

		CubeVertex,-CubeVertex,CubeVertex,
		Zero,cubeColor,Zero,cubeColor,
		cubeNormal,Zero,Zero,
		CubeLeftXBottom, CubeLeftYBottom,

		CubeVertex,-CubeVertex,-CubeVertex,
		Zero,cubeColor,Zero,cubeColor,
		cubeNormal,Zero,Zero,
		CubeRightXBottom, CubeRightYBottom,
		
		//Left
		-CubeVertex,CubeVertex,CubeVertex,
		Zero,cubeColor,Zero,cubeColor,
		-cubeNormal,Zero,Zero,
		CubeRightXTop, CubeRightYTop,

		-CubeVertex,CubeVertex,-CubeVertex,
		Zero,cubeColor,Zero,cubeColor,
		-cubeNormal,Zero,Zero,
		CubeLeftXTop, CubeLeftYTop,

		-CubeVertex,-CubeVertex,-CubeVertex,
		Zero,cubeColor,Zero,cubeColor,
		-cubeNormal,Zero,Zero,
		CubeLeftXBottom, CubeLeftYBottom,

		-CubeVertex,-CubeVertex,CubeVertex,
		Zero,cubeColor,Zero,cubeColor,
		-cubeNormal,Zero,Zero,
		CubeRightXBottom, CubeRightYBottom,
		
		//Top
		CubeVertex,CubeVertex,CubeVertex,
		cubeColor,Zero,cubeColor,cubeColor,
		Zero,cubeNormal,Zero,
		CubeRightXTop, CubeRightYTop,

		CubeVertex,CubeVertex,-CubeVertex,
		cubeColor,Zero,cubeColor,cubeColor,
		Zero,cubeNormal,Zero,
		CubeLeftXTop, CubeLeftYTop,

		-CubeVertex,CubeVertex,-CubeVertex,
		cubeColor,Zero,cubeColor,cubeColor,
		Zero,cubeNormal,Zero,
		CubeLeftXBottom, CubeLeftYBottom,

		-CubeVertex,CubeVertex,CubeVertex,
		cubeColor,Zero,cubeColor, cubeColor,
		Zero,cubeNormal,Zero,
		CubeRightXBottom, CubeRightYBottom,
		
		//Bottom
		CubeVertex,-CubeVertex,CubeVertex,
		cubeColor, cubeColor, Zero, cubeColor,
		Zero, -cubeNormal, Zero,
		CubeRightXTop, CubeRightYTop,

		CubeVertex,-CubeVertex,-CubeVertex,
		cubeColor, cubeColor, Zero, cubeColor,
		Zero, -cubeNormal, Zero,
		CubeLeftXTop, CubeLeftYTop,

		-CubeVertex,-CubeVertex,-CubeVertex,
		cubeColor, cubeColor, Zero, cubeColor,
		Zero, -cubeNormal, Zero,
		CubeLeftXBottom, CubeLeftYBottom,

		-CubeVertex,-CubeVertex,CubeVertex,
		cubeColor, cubeColor, Zero, cubeColor,
		Zero, -cubeNormal, Zero,
		CubeRightXBottom, CubeRightYBottom,
	};


	//Rectangle recording starts
	glGenVertexArrays(1, &gVao_Cube);
	glBindVertexArray(gVao_Cube);

	glGenBuffers(1, &gVbo_Position_Cube);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(ADS_ATTRUBUTE_VERTEX_Position, 3, GL_FLOAT, GL_FALSE, 12 * (sizeof(float)), (GLvoid*)(0 * (sizeof(float))));
	glEnableVertexAttribArray(ADS_ATTRUBUTE_VERTEX_Position);
	glVertexAttribPointer(ADS_ATTRUBUT_COLORS, 3, GL_FLOAT, GL_FALSE, 12 *(sizeof(float)), (GLvoid*)(3*(sizeof(float))));
	glEnableVertexAttribArray(ADS_ATTRUBUT_COLORS);
	glVertexAttribPointer(ADS_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 12 * (sizeof(float)), (GLvoid*)(7 * (sizeof(float))));
	glEnableVertexAttribArray(ADS_ATTRIBUTE_NORMAL);
	glVertexAttribPointer(ADS_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 12 * (sizeof(float)), (GLvoid*)(10 * (sizeof(float))));
	glEnableVertexAttribArray(ADS_ATTRIBUTE_TEXTURE0);
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

	//loading texture
	//AS//loadGlTexture(&Stone_Texture, MAKEINTRESOURCE(STONE_BITMAP));
	loadGlTexture(&Vijay_Kundali_Texture, MAKEINTRESOURCE(KUNDALI_BITMAP));

	//glEnable(GL_TEXTURE_2D);


	//Set Clear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//set orthographics matrix to identitu matrix
	gPerspectiveProjectionMatrix = mat4::identity();

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
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //here 1 for batter performance
		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);
		//setting parameter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//floowing call will push data in gmem with use of Gdriver
		//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp.bmBits);
		glGenerateMipmap(GL_TEXTURE_2D);
		DeleteObject(hBitmap);
	}


	return bResult;
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
	//lighting Enable
	//lighting Enable
	if (bLight == true)
	{
		/*GLfloat lightambint[] = { 0.0f, 0.0f, 0.0f };
		GLfloat lightdiffuse[] = { 0.5f, 0.2f, 0.7f };
		GLfloat lightspeculer[] = { 0.7f, 0.7f, 0.7f };
		GLfloat lightposition[] = { 100.0f, 100.0f, 100.0f, 1.0f };*/

		GLfloat lightambint[] = { 0.0f, 0.0f, 0.0f };
		GLfloat lightdiffuse[] = { 1.0f, 1.0f, 1.0f };
		GLfloat lightspeculer[] = { 1.0f, 1.0f, 1.0f };
		GLfloat lightposition[] = { 0.0f, 0.0f, 100.0f, 1.0f };

		GLfloat Matrilambint[] = { 0.0f, 0.0f, 0.0f };
		GLfloat Matrildiffuse[] = { 1.0f, 1.0f, 1.0f };
		GLfloat Matrilspeculer[] = { 1.0f, 1.0f, 1.0f };
		//GLfloat matrialshiness[] = { 128.0f };
		GLfloat matrialshiness = 128.0f;

		glUniform1i(LKeyPressedUniform, 1);

		glUniform3fv(LaUniform, 1, (GLfloat*)lightambint);
		glUniform3fv(LdUniform, 1, (GLfloat*)lightdiffuse);
		glUniform3fv(LsUniform, 1, (GLfloat*)lightspeculer);
		glUniform4fv(lightPositionUniform, 1, (GLfloat*)lightposition);

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

	if (bTexture == true)
	{
		glUniform1i(TKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(TKeyPressedUniform, 0);
	}

	if (bColor == true)
	{
		glUniform1i(CKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(CKeyPressedUniform, 0);
	}

	//Set modelview and modelviewprojection matrices to identity
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	//*** Rectangle glTranslated related call using vmath.h ***
	modelMatrix = vmath::translate(0.0f, 0.0f, -6.0f) * vmath::rotate(Rangle, 1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix * vmath::rotate(Rangle, 0.0f, 1.0f, 0.0f);
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

	//Texture statement
	glBindTexture(GL_TEXTURE_2D, Vijay_Kundali_Texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Vijay_Kundali_Texture);//Bind Texture
	glUniform1i(textureSamplerUniform, 0);
	
	//*** bind vao fro cube***
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
	//AS//Tangle = UpdateAngle(Tangle);
	if (bAnimate)
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

	//Delete Texture
	
	glDeleteTextures(1, &Vijay_Kundali_Texture);

	

	if (gVao_Cube)
	{
		glDeleteVertexArrays(1, &gVao_Cube);
		gVao_Cube = 0;
	}


	

	if (gVbo_Position_Cube)
	{
		glDeleteBuffers(1, &gVbo_Position_Cube);
		gVbo_Position_Cube = 0;
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