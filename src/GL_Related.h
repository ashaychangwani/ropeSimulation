#pragma once
/***********************************************
*                                              *
*    Jeff Molofee's Revised OpenGL Basecode    *
*    Thanks To Maxwell Sayles & Peter Puck     *
*                                              *
***********************************************/

#include <windows.h>								
#include <GL\glew.h>
#include <GL\freeglut.h>

typedef struct {
	BOOL keyDown[256];
} Keys;

typedef struct {
	HINSTANCE		hInstance;
	const char* className;
} Application;

typedef struct {
	Application* application;
	char* title;
	int					width;
	int					height;
	int					bitsPerPixel;
	BOOL				isFullScreen;
} GL_WindowInit;

typedef struct {
	Keys* keys;
	HWND				hWnd;
	HDC					hDC;
	HGLRC				hRC;
	GL_WindowInit		init;
	BOOL				isVisible;
	DWORD				lastTickCount;
} GL_Window;

GL_Window* g_window;
Keys* g_keys;

static BOOL g_isProgramLooping;
static BOOL g_createFullScreen;


void Update(DWORD ms);
void Draw(void);

void TerminateApplication(GL_Window* window)
{
	PostMessage(window->hWnd, WM_QUIT, 0, 0);
	g_isProgramLooping = FALSE;
}

void ReshapeGL(int width, int height)
{
	glViewport(0, 0, (GLsizei)(width), (GLsizei)(height));
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(35.0f, (GLfloat)(width) / (GLfloat)(height), 1.0f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

BOOL ChangeScreenResolution(int width, int height, int bitsPerPixel)
{
	DEVMODE dmScreenSettings;
	ZeroMemory(&dmScreenSettings, sizeof(DEVMODE));
	dmScreenSettings.dmSize = sizeof(DEVMODE);
	dmScreenSettings.dmPelsWidth = width;
	dmScreenSettings.dmPelsHeight = height;
	dmScreenSettings.dmBitsPerPel = bitsPerPixel;
	dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	return TRUE;
}

BOOL CreateWindowGL(GL_Window* window)
{
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;
	DWORD windowExtendedStyle = WS_EX_APPWINDOW;

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		window->init.bitsPerPixel,
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

	RECT windowRect = { 0, 0, window->init.width, window->init.height };

	GLuint PixelFormat;

	if (window->init.isFullScreen == TRUE)
	{
		if (ChangeScreenResolution(window->init.width, window->init.height, window->init.bitsPerPixel) == FALSE)
		{
			MessageBox(HWND_DESKTOP, "Mode Switch Failed.\nRunning In Windowed Mode.", "Error", MB_OK | MB_ICONEXCLAMATION);
			window->init.isFullScreen = FALSE;
		}
		else
		{
			ShowCursor(FALSE);
			windowStyle = WS_POPUP;
			windowExtendedStyle |= WS_EX_TOPMOST;
		}
	}
	else
	{
		AdjustWindowRectEx(&windowRect, windowStyle, 0, windowExtendedStyle);
	}
	window->hWnd = CreateWindowEx(windowExtendedStyle,
		window->init.application->className,
		window->init.title,
		windowStyle,
		0, 0,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		HWND_DESKTOP,
		0,
		window->init.application->hInstance,
		window);

	if (window->hWnd == 0)
	{
		return FALSE;
	}

	window->hDC = GetDC(window->hWnd);
	if (window->hDC == 0)
	{
		// Failed
		DestroyWindow(window->hWnd);
		window->hWnd = 0;
		return FALSE;
	}

	PixelFormat = ChoosePixelFormat(window->hDC, &pfd);
	if (PixelFormat == 0)
	{
		// Failed
		ReleaseDC(window->hWnd, window->hDC);
		window->hDC = 0;
		DestroyWindow(window->hWnd);
		window->hWnd = 0;
		return FALSE;
	}

	if (SetPixelFormat(window->hDC, PixelFormat, &pfd) == FALSE)
	{
		// Failed
		ReleaseDC(window->hWnd, window->hDC);
		window->hDC = 0;
		DestroyWindow(window->hWnd);
		window->hWnd = 0;
		return FALSE;
	}

	window->hRC = wglCreateContext(window->hDC);
	if (window->hRC == 0)
	{
		// Failed
		ReleaseDC(window->hWnd, window->hDC);
		window->hDC = 0;
		DestroyWindow(window->hWnd);
		window->hWnd = 0;
		return FALSE;
	}

	if (wglMakeCurrent(window->hDC, window->hRC) == FALSE)
	{
		// Failed
		wglDeleteContext(window->hRC);
		window->hRC = 0;
		ReleaseDC(window->hWnd, window->hDC);
		window->hDC = 0;
		DestroyWindow(window->hWnd);
		window->hWnd = 0;
		return FALSE;
	}

	ShowWindow(window->hWnd, SW_NORMAL);
	window->isVisible = TRUE;

	ReshapeGL(window->init.width, window->init.height);

	ZeroMemory(window->keys, sizeof(Keys));

	window->lastTickCount = GetTickCount();

	return TRUE;

}

BOOL DestroyWindowGL(GL_Window* window)
{
	if (window->hWnd != 0)
	{
		if (window->hDC != 0)
		{
			wglMakeCurrent(window->hDC, 0);
			if (window->hRC != 0)
			{
				wglDeleteContext(window->hRC);
				window->hRC = 0;

			}
			ReleaseDC(window->hWnd, window->hDC);
			window->hDC = 0;
		}
		DestroyWindow(window->hWnd);
		window->hWnd = 0;
	}

	if (window->init.isFullScreen)
	{
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(TRUE);
	}
	return TRUE;
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	GL_Window* window = (GL_Window*)(GetWindowLong(hWnd, GWL_USERDATA));

	switch (uMsg)
	{
	case WM_SYSCOMMAND:
	{
		switch (wParam)
		{
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
			return 0;
		}
		break;
	}
	return 0;

	case WM_CREATE:
	{
		CREATESTRUCT* creation = (CREATESTRUCT*)(lParam);
		window = (GL_Window*)(creation->lpCreateParams);
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)(window));
	}
	return 0;

	case WM_CLOSE:
		TerminateApplication(window);
		return 0;

	case WM_SIZE:
		switch (wParam)
		{
		case SIZE_MINIMIZED:
			window->isVisible = FALSE;
			return 0;

		case SIZE_MAXIMIZED:
			window->isVisible = TRUE;
			ReshapeGL(LOWORD(lParam), HIWORD(lParam));
			return 0;

		case SIZE_RESTORED:
			window->isVisible = TRUE;
			ReshapeGL(LOWORD(lParam), HIWORD(lParam));
			return 0;
		}
		break;

	case WM_KEYDOWN:
		if ((wParam >= 0) && (wParam <= 255))
		{
			window->keys->keyDown[wParam] = TRUE;
			return 0;
		}
		break;

	case WM_KEYUP:
		if ((wParam >= 0) && (wParam <= 255))
		{
			window->keys->keyDown[wParam] = FALSE;
			return 0;
		}
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

BOOL RegisterWindowClass(Application* application)
{

	WNDCLASSEX windowClass;
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = (WNDPROC)(WindowProc);
	windowClass.hInstance = application->hInstance;
	windowClass.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = application->className;
	if (RegisterClassEx(&windowClass) == 0)
	{
		MessageBox(HWND_DESKTOP, "RegisterClassEx Failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	return TRUE;
}

GLuint	base;

GLYPHMETRICSFLOAT gmf[256];

GLvoid BuildFont(GL_Window* window)
{
	HFONT	font;

	base = glGenLists(256);

	font = CreateFont(-12,
		0,
		0,
		0,
		FW_BOLD,
		FALSE,
		FALSE,
		FALSE,
		ANSI_CHARSET,
		OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		FF_DONTCARE | DEFAULT_PITCH,
		NULL);

	HDC hDC = window->hDC;
	SelectObject(hDC, font);

	wglUseFontOutlines(hDC,
		0,
		255,
		base,
		0.0f,
		0.0f,
		WGL_FONT_POLYGONS,
		gmf);
}

GLvoid KillFont(GLvoid)
{
	glDeleteLists(base, 256);
}

GLvoid glPrint(float x, float y, float z, const char* fmt, ...)
{
	float		length = 0;
	char		text[256];
	va_list		ap;

	if (fmt == NULL)
		return;

	va_start(ap, fmt);
	vsprintf_s(text, fmt, ap);
	va_end(ap);

	for (unsigned int loop = 0; loop < (strlen(text)); loop++)
	{
		length += gmf[text[loop]].gmfCellIncX;
	}

	glTranslatef(x - length, y, z);

	glPushAttrib(GL_LIST_BIT);
	glListBase(base);
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();

	glTranslatef(-x, -y, -z);
}

BOOL Initialize(GL_Window* window, Keys* keys)
{
	g_window = window;
	g_keys = keys;

	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	BuildFont(window);

	return TRUE;
}