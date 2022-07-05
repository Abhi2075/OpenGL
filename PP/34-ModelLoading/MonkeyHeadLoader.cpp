#include <windows.h>
#include <stdio.h>

#include <gl/glew.h> //this has to be before GL.h
#include <gl/GL.h>

#include "resource.h"
#include "vmath.h"

#pragma warning (disable : 4996)

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define BUFFER_SIZE 256

using namespace vmath;

enum
{
	ADS_ATTRIBUTE_POSITION = 0,
	ADS_ATTRIBUTE_COLOR,
	ADS_ATTRIBUTE_NORMAL,
	ADS_ATTRIBUTE_TEXCOORD,
};

//global function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//global variable declarations
HWND ghwnd = NULL;
FILE* gpFile = NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
bool gbFullScreen = false;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
bool gbActiveWindow = false;

//handle to a mesh file
FILE* g_fp_meshfile = NULL;

//hold line in a file
char g_line[BUFFER_SIZE];

GLuint vertexShaderObject;
GLuint fragmentShaderObject;
GLuint shaderProgramObject;

GLuint vao;
GLuint vbo;
GLuint elementBuffer;

GLuint mvpMatrixUniform;

mat4 perspectiveProjectionMatrix;

struct vec_int
{
	int* p;
	int size;
};

struct vec_float
{
	float* pf;
	int size;
};

struct vec_float *gp_vertex, *gp_texture, *gp_normal;
struct vec_float *gp_vertex_sorted, *gp_texture_sorted, *gp_normal_sorted;
struct vec_int *gp_vertex_indices, *gp_texture_indices, *gp_normal_indices;

//WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//function declarations
	void Initialize(void);
	void Display(void);

	//variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MoneyHeadApp");
	int iXCoords = 0;
	int iYCoords = 0;
	RECT rc;
	bool bDone = false;

	//code
	//Open file for error logging
	if (fopen_s(&gpFile, "MoneyHeadError.log", "w") != 0)
	{
		MessageBox(NULL, TEXT("Failed to create error log file...!!!"), TEXT("Error"), MB_ICONERROR | MB_OK);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log file created successfully and program started successfully.\n");
	}

	//Initialize WNDCLASSEX
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	//Register above class
	RegisterClassEx(&wndclass);

	//centering the window
	if (SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0) == TRUE)
	{
		iXCoords = ((rc.left + rc.right) / 2) - (WIN_WIDTH / 2);
		iYCoords = ((rc.top + rc.bottom) / 2) - (WIN_HEIGHT / 2);
	}

	//Create Window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("My Application"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		iXCoords,
		iYCoords,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;
	Initialize();

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	//game loop
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
				Display();
			}
		}
	}

	return ((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//function declarations
	void ToggleFullScreen(void);
	void Resize(int, int);
	void Uninitialize(void);

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
		Resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;

		case 0x46: //F
		case 0x66: //f
			ToggleFullScreen();
			break;

		default:
			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		Uninitialize();
		PostQuitMessage(0);
		break;
	}

	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullScreen(void)
{
	//variable declaration
	MONITORINFO mi = { sizeof(MONITORINFO) };

	//code
	if (gbFullScreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, (dwStyle & ~WS_OVERLAPPEDWINDOW));
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, (mi.rcMonitor.right - mi.rcMonitor.left), (mi.rcMonitor.bottom - mi.rcMonitor.top), SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}

		ShowCursor(FALSE);
		gbFullScreen = true;
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, (dwStyle | WS_OVERLAPPEDWINDOW));
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
		gbFullScreen = false;
	}
}

void Initialize(void)
{
	//function declarations
	void Resize(int, int);
	void LoadMeshData(void); //load data from mesh file and populate global vectors

	//variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	//code
	ghdc = GetDC(ghwnd);
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);

	if (iPixelFormatIndex == 0)
	{
		fprintf(gpFile, "ChoosePixelFormat() failed.\n");
		DestroyWindow(ghwnd);
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		fprintf(gpFile, "SetPixelFormat() failed.\n");
		DestroyWindow(ghwnd);
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		fprintf(gpFile, "wglCreateContext() failed.\n");
		DestroyWindow(ghwnd);
	}

	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(gpFile, "wglMakeCurrent() failed.\n");
		DestroyWindow(ghwnd);
	}

	//enable the extensions
	//Important: It must be here after creating OpenGL context but before using any OpenGL function
	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		fprintf(gpFile, "glewInit() failed.\n");
		DestroyWindow(ghwnd);
	}

	//OpenGL related logs
	fprintf(gpFile, "OpenGL Vendor: %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL Version: %s\n", glGetString(GL_VERSION));
	fprintf(gpFile, "GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	//OpenGL enabled extensions
	GLint numExt;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);
	fprintf(gpFile, "OpenGL enabled extensions: %d\n", numExt);

	for (int i = 0; i < numExt; i++)
	{
		fprintf(gpFile, "%s\n", glGetStringi(GL_EXTENSIONS, i));
	}

	//Vertex shader
	//create shader
	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//provide code to shader
	const GLchar* vertexShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 vPosition;" \
		"uniform mat4 u_mvpMatrix;" \
		"void main(void)" \
		"{" \
			"gl_Position = u_mvpMatrix * vPosition;" \
		"}";

	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	//compile shader
	glCompileShader(vertexShaderObject);
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char* szInfoLog = NULL;

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Vertex Shader Compilation log: %s\n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				DestroyWindow(ghwnd);
			}
		}
	}

	//fragment shader
	//create shader
	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//rpovide code to shader
	const GLchar* fragmentShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
			"FragColor = vec4(1.0, 1.0, 1.0, 1.0);" \
		"}";

	//"FragColor = texture(u_texture_sampler, out_texCoord);"

	glShaderSource(fragmentShaderObject, 1, (const char**)&fragmentShaderSourceCode, NULL);

	//compile shader
	glCompileShader(fragmentShaderObject);
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, iInfoLogLength, &written, szInfoLog);

				fprintf(gpFile, "Fragment shader compilation log: %s\n", szInfoLog);

				free(szInfoLog);
				szInfoLog = NULL;
				DestroyWindow(ghwnd);
			}
		}
	}

	//Shader program
	//create
	shaderProgramObject = glCreateProgram();

	//attach vertex shader to shader prograg
	glAttachShader(shaderProgramObject, vertexShaderObject);

	//attach fragment shader to shader program
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	//pre-link binding of shader program object with vertex shader position attribute
	glBindAttribLocation(shaderProgramObject, ADS_ATTRIBUTE_POSITION, "vPosition");

	//link shader
	glLinkProgram(shaderProgramObject);
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Shader program link log: %s\n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;

				DestroyWindow(ghwnd);
			}
		}
	}

	//get MVP uniform location
	mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_mvpMatrix");
	//textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "u_texture_sampler");

	//read mesh file and load global vectors with appropriate data
	LoadMeshData();

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//vertices
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, (gp_vertex_sorted->size) * sizeof(GLfloat), gp_vertex_sorted->pf, GL_STATIC_DRAW);
	glVertexAttribPointer(ADS_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ADS_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, gp_vertex_indices->size * sizeof(GLint), gp_vertex_indices->p, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//set perspective matrix to indentity matrix
	perspectiveProjectionMatrix = mat4::identity();

	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void Resize(int width, int height)
{
	//code
	if (height == 0)
	{
		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	perspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void Display(void)
{
	//local variables
	static GLfloat angle = 0.0f;

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//start using OpenGL program object
	glUseProgram(shaderProgramObject);

	//OpenGL drawing
	//set modelview & modelview projection matrices to identity

	//triangle transformation
	mat4 modelViewMatrix = mat4::identity();
	mat4 modelViewProjectionMatrix = mat4::identity();
	mat4 translationMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();

	translationMatrix = vmath::translate(0.0f, 0.0f, -5.0f);
	rotationMatrix = vmath::rotate(angle, 0.0f, 1.0f, 0.0f);

	modelViewMatrix = translationMatrix * rotationMatrix;

	//multiply the modelview and perspective matrix to get modelviewprojection matrix
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix; //order is important

	//pass above modelviewprojection matrix to the vertex shader in 'u_mvpMatrix' shader variable
	//whose position value we already calcualted in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	glDrawElements(GL_TRIANGLES, gp_vertex_indices->size, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
	glUseProgram(0);

	if (angle >= 360.0f)
	{
		angle = 0.0f;
	}

	angle += 0.1f;

	SwapBuffers(ghdc);
}

void Uninitialize(void)
{
	//function declarations
	int destroy_vec_int(struct vec_int* p_vec_int);
	int destroy_vec_float(struct vec_float* p_vec_float);
	
	//code
	if (gbFullScreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, (dwStyle | WS_OVERLAPPEDWINDOW));
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
		gbFullScreen = false;
	}

	//destroy vao
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	//destroy vbo
	if (vbo)
	{
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}

	//dettach vertex shader from shader program object
	glDetachShader(shaderProgramObject, vertexShaderObject);

	//detach fragment shader from shader program object
	glDetachShader(shaderProgramObject, fragmentShaderObject);

	//delete vertex shader object
	glDeleteShader(vertexShaderObject);
	vertexShaderObject = 0;

	//delete fragment shader object
	glDeleteShader(fragmentShaderObject);
	fragmentShaderObject = 0;

	//delete shader program object
	glDeleteProgram(shaderProgramObject);
	shaderProgramObject = 0;

	//unlink shader program
	glUseProgram(0);

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

	destroy_vec_float(gp_vertex);
	gp_vertex = NULL;
	destroy_vec_float(gp_texture);
	gp_texture = NULL;
	destroy_vec_float(gp_normal);
	gp_normal = NULL;

	destroy_vec_float(gp_vertex_sorted);
	gp_vertex = NULL;
	destroy_vec_float(gp_texture_sorted);
	gp_texture = NULL;
	destroy_vec_float(gp_normal_sorted);
	gp_normal = NULL;

	destroy_vec_int(gp_vertex_indices);
	gp_vertex_indices = NULL;
	destroy_vec_int(gp_texture_indices);
	gp_texture_indices = NULL;
	destroy_vec_int(gp_normal_indices);
	gp_normal_indices = NULL;

	//close file
	if (gpFile)
	{
		fprintf(gpFile, "Log file closed successfully & program is completed succesfully.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}

void LoadMeshData(void)
{
	//function declarations
	struct vec_int* create_vec_int();
	struct vec_float* create_vec_float();
	int push_back_vec_int(struct vec_int* p_vec_int, int data);
	int push_back_vec_float(struct vec_float* p_vec_float, float data);
	int destroy_vec_float(struct vec_float* p_vec_float);

	//variable declarations
	//seperator string
	char* space = (char *)" ";
	char* slash = (char *)"/";

	//token pointers
	char* first_token = NULL;
	char* token = NULL;

	char* f_entries[3] = { NULL, NULL, NULL };
	int nr_pos_cords = 0, nr_tex_cords = 0, nr_normal_cords = 0, nr_faces = 0;
	int i;

	//code
	g_fp_meshfile = fopen("MonkeyHead.OBJ", "r");
	if (!g_fp_meshfile)
	{
		fprintf(gpFile, "Failed to open mesh object file\n");
		DestroyWindow(ghwnd);
	}
	else
	{
		fprintf(gpFile, "Mesh file loaded successfully.\n");
	}

	gp_vertex = create_vec_float();
	gp_texture = create_vec_float();
	gp_normal = create_vec_float();

	gp_vertex_indices = create_vec_int();
	gp_texture_indices = create_vec_int();
	gp_normal_indices = create_vec_int();

	while (fgets(g_line, BUFFER_SIZE, g_fp_meshfile) != NULL)
	{
		first_token = strtok(g_line, space);

		if (strcmp(first_token, "v") == 0)
		{
			nr_pos_cords++;
			while ((token = strtok(NULL, space)) != NULL)
			{
				push_back_vec_float(gp_vertex, atof(token));
			}
		}
		else if (strcmp(first_token, "vt") == 0)
		{
			nr_tex_cords++;
			while ((token = strtok(NULL, space)) != NULL)
			{
				push_back_vec_float(gp_texture, atof(token));
			}
		}
		else if (strcmp(first_token, "vn") == 0)
		{
			nr_normal_cords++;
			while ((token = strtok(NULL, space)) != NULL)
			{
				push_back_vec_float(gp_normal, atof(token));
			}
		}
		else if (strcmp(first_token, "f") == 0)
		{
			nr_faces++;
			for (i = 0; i < 3; i++)
			{
				f_entries[i] = strtok(NULL, space);
			}

			for (i = 0; i < 3; i++)
			{
				token = strtok(f_entries[i], slash);
				push_back_vec_int(gp_vertex_indices, atoi(token) - 1);
				token = strtok(NULL, slash);
				push_back_vec_int(gp_texture_indices, atoi(token) - 1);
				token = strtok(NULL, slash);
				push_back_vec_int(gp_normal_indices, atoi(token) - 1);
			}
		}
	}

	gp_vertex_sorted = create_vec_float();
	for (int i = 0; i < gp_vertex_indices->size; i++)
	{
		push_back_vec_float(gp_vertex_sorted, gp_vertex->pf[i]);
	}

	gp_texture_sorted = create_vec_float();
	for (int i = 0; i < gp_texture_indices->size; i++)
	{
		push_back_vec_float(gp_texture_sorted, gp_texture->pf[i]);
	}

	gp_normal_sorted = create_vec_float();
	for (int i = 0; i < gp_normal_indices->size; i++)
	{
		push_back_vec_float(gp_normal_sorted, gp_normal->pf[i]);
	}

	fclose(g_fp_meshfile);
	g_fp_meshfile = NULL;
}

struct vec_int* create_vec_int()
{
	struct vec_int* p = (struct vec_int*)malloc(sizeof(struct vec_int));
	memset(p, 0, sizeof(struct vec_int));
	return p;
}

struct vec_float* create_vec_float()
{
	struct vec_float* p = (struct vec_float*)malloc(sizeof(struct vec_float));
	memset(p, 0, sizeof(struct vec_float));
	return p;
}

int push_back_vec_int(struct vec_int* p_vec_int, int data)
{
	p_vec_int->p = (int*)realloc(p_vec_int->p, (p_vec_int->size + 1) * sizeof(int));
	p_vec_int->size = p_vec_int->size + 1;
	p_vec_int->p[p_vec_int->size - 1] = data;

	return (0);
}

int push_back_vec_float(struct vec_float* p_vec_float, float data)
{
	p_vec_float->pf = (float*)realloc(p_vec_float->pf, (p_vec_float->size + 1) * sizeof(float));
	p_vec_float->size = p_vec_float->size + 1;
	p_vec_float->pf[p_vec_float->size - 1] = data;

	return (0);
}

int destroy_vec_int(struct vec_int* p_vec_int)
{
	free(p_vec_int->p);
	free(p_vec_int);
	p_vec_int = NULL;

	return (0);
}

int destroy_vec_float(struct vec_float* p_vec_float)
{
	free(p_vec_float->pf);
	free(p_vec_float);
	p_vec_float = NULL;

	return (0);
}
