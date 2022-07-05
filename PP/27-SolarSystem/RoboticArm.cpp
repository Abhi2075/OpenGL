#define WIN_WIDTH 1000
#define WIN_HEIGHT 700

#include <Windows.h>
#include <GL/glew.h>
#include <gl/GL.h>
#include <stdio.h>
#include <winuser.h>
#include "MatrixStack.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

enum
{
    ADS_ATTRIBUTE_POSITION = 0,
    ADS_ATTRIBUTE_COLOR,
    ADS_ATTRIBUTE_NORMAL,
    ADS_ATTRIBUTE_TEXCOORD0,
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global variables
bool gbFullscreen = false;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
HWND ghwnd = nullptr;
HDC ghdc = nullptr;
HGLRC ghrc = nullptr;
bool gbActiveWindow = false;
FILE* gpFile = nullptr;
bool gbEscapeKeyIsPressed = false;

GLuint gShaderProgramObject;

unsigned short* sphereElements;
GLfloat* sphereVertices;
GLint sphereNumberOfElements;
GLint sphereMaxNumberOfElements;
GLint sphereNumberOfVertices;
GLuint vboSpherePosition;
GLuint vboSphereIndex;
GLuint vaoSphere;

int shoulder = 0;
int elbow = 0;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
    //function declaration
    int initialize(void);
    void resize(int, int);
    void display(void);
    void update(void);

    WNDCLASSEX wndclass;
    HWND hwnd;
    MSG msg;
    TCHAR szAppName[] = TEXT("BlueWindowWithShaders");
    bool bDone = false;
    int iRet = 0;

    if (fopen_s(&gpFile, "Log_BlueWindowWithShaders.txt", "w") != 0)
    {
        MessageBox(nullptr, TEXT("Log file cannot be created"), TEXT("Error"), MB_OK);
        exit(0);
    }

    fwprintf_s(gpFile, L"File created successfully.\n");

    wndclass.cbSize = sizeof(WNDCLASSEX);
    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.lpfnWndProc = WndProc;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.lpszClassName = szAppName;
    wndclass.lpszMenuName = nullptr;
    wndclass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    RegisterClassEx(&wndclass);

    hwnd = CreateWindowEx(
        WS_EX_APPWINDOW,
        //To have window on task bar after fullscreen when windows is switched from one monitor to another
        szAppName,
        TEXT("Orthographic Triangle"),
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
        0,
        0,
        WIN_WIDTH,
        WIN_HEIGHT,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    ghwnd = hwnd;

    iRet = initialize();
    if (iRet == -1)
    {
        fwprintf_s(gpFile, L"ChoosePixelFormat failed.\n");
        DestroyWindow(hwnd);
    }
    else if (iRet == -2)
    {
        fwprintf_s(gpFile, L"SetPixelFormat failed.\n");
        DestroyWindow(hwnd);
    }
    else if (iRet == -3)
    {
        fwprintf_s(gpFile, L"wglCreateContext failed.\n");
        DestroyWindow(hwnd);
    }
    else if (iRet == -4)
    {
        fwprintf_s(gpFile, L"wglMakeCurrent failed.\n");
        DestroyWindow(hwnd);
    }
    else
    {
        fwprintf_s(gpFile, L"initialize succeeded.\n");
    }

    ShowWindow(hwnd, iCmdShow);
    //UpdateWindow(hwnd);

    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    //Game loop
    while (bDone == false)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
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
                display();
                update();
            }
        }
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    //function declarations
    void ToggleFullscreen(void);
    void resize(int, int);
    //void display(void);
    void uninitialize(void);

    switch (iMsg)
    {
    case WM_CREATE:
        gbActiveWindow = HIWORD(wParam) == 0;
        break;

    case WM_ERASEBKGND: //Don't go to WndProc for WM_PAINT.
        return 0;
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            //gbEscapeKeyIsPressed = true;
            DestroyWindow(hwnd);
            break;

        case 0x46: //'F' key
            ToggleFullscreen();
            break;

        case WM_SETFOCUS:
            gbActiveWindow = true;
            break;

        case WM_KILLFOCUS:
            gbActiveWindow = false;
            break;
        }
        break;

    case WM_CHAR:
        switch (wParam)
        {
        case 0x53: //'S'
            shoulder = (shoulder + 3) % 360;
            break;

        case 0x73: //'s'
            shoulder = (shoulder - 3) % 360;
            break;

        case 0x45: //'E'
            elbow = (elbow + 3) % 360;
            break;

        case 0x65: //'e'
            elbow = (elbow - 3) % 360;
            break;

        default:
            break;
        }

        break;

    case WM_LBUTTONDOWN:
        break;

    case WM_RBUTTONDOWN:
        break;

    case WM_PAINT:
        //display(); //This should be done only for single buffering. For double buffering, comment this line.
        break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_SIZE:
        resize(LOWORD(lParam), HIWORD(lParam));
        break;

    case WM_DESTROY:
        uninitialize();
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

void ToggleFullscreen(void)
{
    MONITORINFO mi;

    if (gbFullscreen == false)
    {
        dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
        if (dwStyle & WS_OVERLAPPEDWINDOW)
        {
            mi = { sizeof(MONITORINFO) };
            if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(
                MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY),
                &mi))
            {
                SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
                SetWindowPos(ghwnd,
                    HWND_TOP,
                    mi.rcMonitor.left,
                    mi.rcMonitor.top,
                    mi.rcMonitor.right - mi.rcMonitor.left,
                    mi.rcMonitor.bottom - mi.rcMonitor.top,
                    SWP_NOZORDER | SWP_FRAMECHANGED);
            }

            ShowCursor(FALSE);

            gbFullscreen = true;
        }
    }
    else
    {
        SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(ghwnd, &wpPrev);
        SetWindowPos(ghwnd,
            HWND_TOP,
            0,
            0,
            0,
            0,
            SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);

        ShowCursor(TRUE);

        gbFullscreen = false;
    }
}

int initialize(void)
{
    //function declarations
    void resize(int, int);
    void uninitialize(void);

    //variable declarations
    PIXELFORMATDESCRIPTOR pfd;
    int iPixelFormatIndex;

    //----------------- Code -----------------

    //Initialize pfd structure
    memset((void*)& pfd, NULL, sizeof PIXELFORMATDESCRIPTOR);
    //OR you can use below line instead of memset in Windows:
    //ZeroMemory(&pfd, sizeof PIXELFORMATDESCRIPTOR);

    pfd.nSize = sizeof PIXELFORMATDESCRIPTOR;
    pfd.nVersion = 1; //Since Windows freezed OpenGL version on 1.5;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cRedBits = 8;
    pfd.cGreenBits = 8;
    pfd.cBlueBits = 8;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 32;

    ghdc = GetDC(ghwnd);

    //The returned format index is always "one based". i.e. it starts from 1. If it is zero, it is error.
    iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);

    if (iPixelFormatIndex == 0)
    {
        return -1;
    }

    if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
    {
        return -2;
    }

    ghrc = wglCreateContext(ghdc);

    if (ghrc == nullptr)
    {
        return -3;
    }

    if (wglMakeCurrent(ghdc, ghrc) == FALSE)
    {
        return -4;
    }

    const GLenum result = glewInit();
    if (result != GLEW_OK)
    {
        fwprintf_s(gpFile, L"glewInit failed with error:\n%p\n\n", glewGetErrorString(result));
        uninitialize();
        DestroyWindow(ghwnd);
    }

    //------------------------------- SHADERS [START] -----------------------

    //------------------------------- 1) VERTEX SHADER -----------------------

    //Define vertex shader object.
    GLuint gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    //Write vertex shader
    const GLchar* vertexShaderSourceCode =
        "#version 450 core"
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

    //Specify above source code to vertex shader object.
    glShaderSource(gVertexShaderObject, 1, (const GLchar * *)& vertexShaderSourceCode, nullptr);

    //Compile the vertex shader
    glCompileShader(gVertexShaderObject);

    //Error catching [START]
    GLint iShaderCompileStatus = 0;
    GLint iInfoLogLength = 0;
    GLchar* szInfoLog = nullptr;

    glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);

    if (iShaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);

        if (iInfoLogLength > 0)
        {
            szInfoLog = (GLchar*)malloc(iInfoLogLength);

            if (szInfoLog != nullptr)
            {
                GLsizei written;
                glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);

                fwprintf_s(gpFile, L"\nVertex shader failed with error : \n%hs\n", szInfoLog);

                free(szInfoLog);
                uninitialize();
                DestroyWindow(ghwnd);
                exit(0);
            }
        }
    }

    //Error catching [END]

    //------------------------------- 2) FRAGMENT SHADER -----------------------

    //Define fragment shader object.
    GLuint gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    //Write fragment shader
    const GLchar* fragmentShaderSourceCode =
        "#version 450 core"
        "\n"
        "in vec4 out_color;"
        "out vec4 fragcolor;"
        "void main(void)"
        "{"
        "fragcolor = out_color;"
        "}";

    //Specify above source code to fragment shader object.
    glShaderSource(gFragmentShaderObject, 1, (const GLchar * *)& fragmentShaderSourceCode, nullptr);

    //Compile the fragment shader
    glCompileShader(gFragmentShaderObject);

    //Error catching [START]
    glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);

    if (iShaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);

        if (iInfoLogLength > 0)
        {
            szInfoLog = (GLchar*)malloc(iInfoLogLength);

            if (szInfoLog != nullptr)
            {
                GLsizei written;
                glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);

                fwprintf_s(gpFile, L"\nFragment shader failed with error : \n%hs\n", szInfoLog);

                free(szInfoLog);
                uninitialize();
                DestroyWindow(ghwnd);
                exit(0);
            }
        }
    }

    //Error catching [END]

    //------------------------------- SHADER PROGRAM [START] -----------------------

    //Create shader object.
    gShaderProgramObject = glCreateProgram();

    //Attach shaders to shader program.
    glAttachShader(gShaderProgramObject, gVertexShaderObject);
    glAttachShader(gShaderProgramObject, gFragmentShaderObject);

    //Prelinking binding to vertex attributes
    glBindAttribLocation(gShaderProgramObject, ADS_ATTRIBUTE_POSITION, "vPosition");

    //Link shader program
    glLinkProgram(gShaderProgramObject);

    //Postlinking retrieving uniform locations
    glBindAttribLocation(gShaderProgramObject, ADS_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(gShaderProgramObject, ADS_ATTRIBUTE_COLOR, "vColor");

    //Error catching [START]
    GLint iProgramLinkStatus = 0;

    glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);

    if (iProgramLinkStatus == GL_FALSE)
    {
        glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);

        if (iInfoLogLength > 0)
        {
            szInfoLog = (GLchar*)malloc(iInfoLogLength);

            if (szInfoLog != nullptr)
            {
                GLsizei written;
                glGetProgramInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfoLog);

                fwprintf_s(gpFile, L"\nProgram linking failed with error : \n%hs\n", szInfoLog);

                free(szInfoLog);
                uninitialize();
                DestroyWindow(ghwnd);
                exit(0);
            }
        }
    }

    //Error catching [END]

    //------------------------------- SHADER PROGRAM [END] -----------------------

    //------------------------------- SHADERS [END] -----------------------

    void makeSphere(GLfloat, int, int);
    makeSphere(1.0f, 30, 30);
    void prepareMesh();
    prepareMesh();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    perspectiveProjectionMatrix = mat4::identity();

    resize(WIN_WIDTH, WIN_HEIGHT);

    return 0;
}

void makeSphere(GLfloat fRadius, int iSlices, int iStacks)
{
    const GLfloat drho = (GLfloat)M_PI / (GLfloat)iStacks;
    const GLfloat dtheta = 2.0f * (GLfloat)M_PI / (GLfloat)iSlices;
    const GLfloat ds = 1.0f / (GLfloat)iSlices;
    const GLfloat dt = 1.0f / (GLfloat)iStacks;
    GLfloat t = 1.0f;
    GLfloat s = 0.0f;
    int i = 0;
    int j = 0;

    void allocateMesh(int);
    allocateMesh(iSlices * iStacks * 6);

    for (i = 0; i < iStacks; i++)
    {
        GLfloat rho = (GLfloat)(i * drho);
        GLfloat srho = (GLfloat)(sin(rho));
        GLfloat crho = (GLfloat)(cos(rho));
        GLfloat srhodrho = (GLfloat)(sin(rho + drho));
        GLfloat crhodrho = (GLfloat)(cos(rho + drho));

        s = 0.0f;

        // initialization of three 2-D arrays, two are 4 x 3 and one is 4 x 2
        GLfloat** vertex = (GLfloat * *)malloc(sizeof(GLfloat*) * 4); // 4 rows
        for (int a = 0; a < 4; a++)
        {
            vertex[a] = (GLfloat*)malloc(sizeof(GLfloat) * 3); // 3 columns
        }

        for (j = 0; j < iSlices; j++)
        {
            GLfloat thetha = (j == iSlices) ? 0.0f : j * dtheta;
            GLfloat sthetha = (GLfloat)(-sin(thetha));
            GLfloat cthetha = (GLfloat)(cos(thetha));

            GLfloat x = sthetha * srho;
            GLfloat y = cthetha * srho;
            GLfloat z = crho;

            vertex[0][0] = x * fRadius;
            vertex[0][1] = y * fRadius;
            vertex[0][2] = z * fRadius;

            x = sthetha * srhodrho;
            y = cthetha * srhodrho;
            z = crhodrho;

            vertex[1][0] = x * fRadius;
            vertex[1][1] = y * fRadius;
            vertex[1][2] = z * fRadius;

            thetha = ((j + 1) == iSlices) ? 0.0f : (j + 1) * dtheta;
            sthetha = (GLfloat)(-sin(thetha));
            cthetha = (GLfloat)(cos(thetha));


            x = sthetha * srho;
            y = cthetha * srho;
            z = crho;

            s += ds;

            vertex[2][0] = x * fRadius;
            vertex[2][1] = y * fRadius;
            vertex[2][2] = z * fRadius;

            x = sthetha * srhodrho;
            y = cthetha * srhodrho;
            z = crhodrho;

            vertex[3][0] = x * fRadius;
            vertex[3][1] = y * fRadius;
            vertex[3][2] = z * fRadius;

            void addTriangleToMesh(GLfloat * *);
            addTriangleToMesh(vertex);

            // Rearrange for next triangle
            vertex[0][0] = vertex[1][0];
            vertex[0][1] = vertex[1][1];
            vertex[0][2] = vertex[1][2];

            vertex[1][0] = vertex[3][0];
            vertex[1][1] = vertex[3][1];
            vertex[1][2] = vertex[3][2];

            addTriangleToMesh(vertex);
        }

        t -= dt;
    }
}

void prepareMesh()
{
    glGenVertexArrays(1, &vaoSphere);
    glBindVertexArray(vaoSphere);

    //------------------------------ Position [START] ------------------------------
    glGenBuffers(1, &vboSpherePosition);
    glBindBuffer(GL_ARRAY_BUFFER, vboSpherePosition);
    glBufferData(GL_ARRAY_BUFFER, (sphereMaxNumberOfElements * 3 * sizeof(GLfloat) / 3), nullptr,
        GL_DYNAMIC_DRAW);
    glVertexAttribPointer(ADS_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(ADS_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //------------------------------ Position [END] ------------------------------

    //------------------------------ Index [START] ------------------------------
    glGenBuffers(1, &vboSphereIndex);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboSphereIndex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (sphereMaxNumberOfElements * 3 * sizeof(unsigned short) / 3), nullptr,
        GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    //------------------------------ Index [END] ------------------------------

    glBindVertexArray(0);

    void cleanupSphereMeshData();
    cleanupSphereMeshData();
}

void drawMesh(GLenum mode)
{
    glBindVertexArray(vaoSphere);

    //------------------------------ Position [START] ------------------------------
    glBindBuffer(GL_ARRAY_BUFFER, vboSpherePosition);
    glBufferData(GL_ARRAY_BUFFER, (sphereMaxNumberOfElements * 3 * sizeof(GLfloat) / 3), sphereVertices,
        GL_DYNAMIC_DRAW);
    glVertexAttribPointer(ADS_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(ADS_ATTRIBUTE_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //------------------------------ Position [END] ------------------------------

    //------------------------------ Index [START] ------------------------------
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboSphereIndex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (sphereMaxNumberOfElements * 3 * sizeof(unsigned short) / 3), sphereElements,
        GL_DYNAMIC_DRAW);

    glDrawElements(mode, sphereNumberOfElements, GL_UNSIGNED_SHORT, nullptr);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    //------------------------------ Index [END] ------------------------------

    glBindVertexArray(0);
}

bool isFoundIdentical(GLfloat val1, GLfloat val2, GLfloat diff)
{
    if (fabs(val1 - val2) < diff)
    {
        return true;
    }

    return false;
}

void addTriangleToMesh(GLfloat** singleVertex)
{
    const GLfloat diff = 0.00001f;
    int i, j;

    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < sphereNumberOfVertices; j++)
        {
            if (isFoundIdentical(sphereVertices[j * 3], singleVertex[i][0], diff) &&
                isFoundIdentical(sphereVertices[(j * 3) + 1], singleVertex[i][1], diff) &&
                isFoundIdentical(sphereVertices[(j * 3) + 2], singleVertex[i][2], diff))
            {
                sphereElements[sphereNumberOfElements] = (short)j;
                sphereNumberOfElements++;

                break;
            }
        }

        if ((j == sphereNumberOfVertices) && (sphereNumberOfVertices < sphereMaxNumberOfElements) && (
            sphereNumberOfElements <
            sphereMaxNumberOfElements))
        {
            sphereVertices[sphereNumberOfVertices * 3] = singleVertex[i][0];
            sphereVertices[(sphereNumberOfVertices * 3) + 1] = singleVertex[i][1];
            sphereVertices[(sphereNumberOfVertices * 3) + 2] = singleVertex[i][2];
            sphereElements[sphereNumberOfElements] = (short)sphereNumberOfVertices;
            sphereNumberOfElements++;
            sphereNumberOfVertices++;
        }
    }
}

void allocateMesh(int numIndices)
{
    void cleanupSphereMeshData();
    cleanupSphereMeshData();

    sphereMaxNumberOfElements = numIndices;
    sphereNumberOfElements = 0;
    sphereNumberOfVertices = 0;

    const int iNumIndices = numIndices / 3;

    sphereElements = (unsigned short*)malloc(iNumIndices * 3 * sizeof(unsigned short));
    // 3 is x,y,z and 2 is sizeof short
    sphereVertices = (GLfloat*)malloc(iNumIndices * 3 * sizeof(GLfloat)); // 3 is x,y,z and 4 is sizeof float
}

void cleanupSphereMeshData()
{
    if (sphereElements != nullptr)
    {
        free(sphereElements);
        sphereElements = nullptr;
    }

    if (sphereVertices != nullptr)
    {
        free(sphereVertices);
        sphereVertices = nullptr;
    }
}

void resize(int width, int height)
{
    if (height == 0)
    {
        height = 1;
    }

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    perspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

#define SKIN_RGB 1.0f, 227.0f / 255.0f, 159.0f / 255.0f

void drawSphere()
{
    makeSphere(0.5f, 10, 10);
    glVertexAttrib3f(ADS_ATTRIBUTE_COLOR, SKIN_RGB);
    drawMesh(GL_TRIANGLES);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(gShaderProgramObject);

    loadIdentity();
    translationMatrix = translate(0.0f, 0.0f, -12.0f);
    setTranslation();

    pushMatrix();
        rotationMatrix = rotate((GLfloat)shoulder, 0.0f, 0.0f, 1.0f);
        setRotation();

        pushMatrix();
            //scaleMatrix = scale(2.0f, 0.5f, 1.0f);
            //setScale();
            drawSphere();
        popMatrix();

        translationMatrix = translate(1.0f, 0.0f, 0.0f);
        setTranslation();

        rotationMatrix = rotate((GLfloat)elbow, 0.0f, 1.0f, 0.0f);
        setRotation();

        translationMatrix = translate(2.0f, 0.0f, 0.0f);
        setTranslation();

        pushMatrix();
            //scaleMatrix = scale(2.0f, 0.5f, 1.0f);
            //setScale();
            drawSphere();
        popMatrix();
    
    popMatrix();

    glUseProgram(0);

    //glFlush(); //This is for single buffer.
    SwapBuffers(ghdc); //This is for double buffer.
}

void update(void)
{
}

void uninitialize(void)
{
    if (gbFullscreen == true)
    {
        SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
    }

    //Break the current context
    if (wglGetCurrentContext() == ghrc)
    {
        wglMakeCurrent(nullptr, nullptr);
    }

    if (ghrc)
    {
        wglDeleteContext(ghrc);
        ghrc = nullptr;
    }

    if (ghdc)
    {
        ReleaseDC(ghwnd, ghdc);
        ghdc = nullptr;
    }

    //------------------------------- SHADERS [START] -----------------------

    if (gShaderProgramObject)
    {
        GLsizei shaderCount;

        glUseProgram(gShaderProgramObject);

        //Get no. of shaders attached.
        glGetProgramiv(gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

        GLuint* pShaders = (GLuint*)malloc(sizeof(GLuint) * shaderCount);

        if (pShaders)
        {
            glGetAttachedShaders(gShaderProgramObject, shaderCount, &shaderCount, pShaders);

            for (GLsizei shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++)
            {
                glDetachShader(gShaderProgramObject, pShaders[shaderNumber]);
                glDeleteShader(pShaders[shaderNumber]);
                pShaders[shaderNumber] = 0;
            }

            free(pShaders);
        }

        glDeleteProgram(gShaderProgramObject);
        gShaderProgramObject = 0;

        glUseProgram(0);
    }

    //------------------------------- SHADERS [END] -----------------------

    if (gpFile)
    {
        fwprintf_s(gpFile, L"\nFile closed successfully.");
        fclose(gpFile);

        gpFile = nullptr;
    }
}
