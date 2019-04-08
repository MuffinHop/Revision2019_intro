// custom build and feature flags
#ifdef DEBUG
	#define OPENGL_DEBUG        1
	#define FULLSCREEN          0
	#define DESPERATE           0
	#define BREAK_COMPATIBILITY 0
#else
	#define OPENGL_DEBUG        1
	#define FULLSCREEN          0
	#define DESPERATE           0
	#define BREAK_COMPATIBILITY 0
#endif

#define POST_PASS    0
#define USE_MIPMAPS  1
#define USE_AUDIO    1
#define NO_UNIFORMS  0

#include "IntroFramework/gl_loader.h"
#include "definitions.h"

#if OPENGL_DEBUG 
	#include "debug.h"
#include "atltrace.h"
#endif

#include "glext.h"
#include "shaders/fragment.inl"
#include "shaders/sync.inl"
#if POST_PASS
	#include "shaders/post.inl"
#endif

#pragma data_seg(".pids")
// static allocation saves a few bytes
static int pidMain;
static int pidPost;

#ifndef DEBUG

#include "biisi.h"
using namespace WaveSabrePlayerLib;

#endif // DEBUG

// static HDC hDC;

#define _NO_CRT_STDIO_INLINE

#include <stdio.h>


void AllSyncDataHandle(float row);
void *GetAnyGLFuncAddress(const char *name)
{
	void *p = (void *)wglGetProcAddress(name);
	if (p == 0 ||
		(p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
		(p == (void*)-1))
	{
		HMODULE module = LoadLibraryA("opengl32.dll");
		p = (void *)GetProcAddress(module, name);
	}

	return p;
}

const char* gl_function_names[] = {
"glActiveTexture",
"glDebugMessageCallbackAMD",
"glDebugMessageEnableAMD",
"glDebugMessageInsertAMD",
"glGetDebugMessageLogAMD",
"glDebugMessageCallback",
"glDebugMessageControl",
"glGetActiveUniformBlockName",
"glGetActiveUniformBlockiv",
"glGetActiveUniformsiv",
"glGetActiveUniformName",
"glCompileShader",
"glGetShaderiv",
"glGetShaderInfoLog",
"glCreateProgram",
"glProgramBinary",
"glCreateShader",
"glShaderSource",
"glDeleteShader",
"glGetProgramiv",
"glAttachShader",
"glProgramParameteri",
"glLinkProgram",
"glDeleteProgram",
"glUseProgram",
"glGetUniformLocation",
"glGetUniformBlockIndex",
"glGetProgramBinary",
"glGenVertexArrays",
"glBindVertexArray",
"glEnableVertexAttribArray",
"glGenBuffers",
"glBindBuffer",
"glBufferData",
"glVertexAttribPointer",
"glDeleteBuffers",
"glDeleteVertexArrays",
"glUniform1f",
"glUniform3fv",
"glUniform4fv",
"glUniformMatrix4fv",
"glBindBufferBase",
"glDispatchCompute",
"glMemoryBarrier",
"glMultiDrawArrays",
"glDrawBuffers",
"glGenFramebuffers",
"glFramebufferTexture2D",
"glCheckFramebufferStatus",
"glBindFramebuffer",
"glDeleteFramebuffers",
"glGetQueryObjectiv",
"glGetQueryObjectui64v",
"glTexImage2DMultisample",
"glGenerateMipmap",
"glUniform1i",
"glGetAttribLocation",
"glBufferSubData",
"glUniform4iv",
"glUniform1fv",
"glUniform3f"
};

void* gl_function_pointers[sizeof(gl_function_names) / sizeof(const char*)];

#ifdef DEBUG
const char* gl_debug_function_names[] = {
	"glGetTextureParameteriv",
	"glDetachShader",
	"glGenQueries",
	"glEndQuery",
	"glBeginQuery",
	"glGetProgramiv",
	"glMapBuffer",
	"glUnmapBuffer"
};
void* gl_debug_function_pointers[sizeof(gl_function_names) / sizeof(const char*)];
#endif

#define TRACE ATLTRACE
int load_gl_functions() {
	int failed = 0;
	for (int i = 0; i < sizeof(gl_function_names) / sizeof(const char*); i++) {
		const char* name = gl_function_names[i];
		void* ptr = GetAnyGLFuncAddress(name);
		gl_function_pointers[i] = ptr;
		if (ptr == NULL) {
#ifdef DEBUG
			//printf("Failed to load GL func '%s'", name);
#endif
			failed++;
		}
	}

#ifdef DEBUG
	for (int i = 0; i < sizeof(gl_debug_function_names) / sizeof(const char*); i++) {
		const char* name = gl_debug_function_names[i];
		void* ptr = GetAnyGLFuncAddress(name);
		gl_debug_function_pointers[i] = ptr;
		if (ptr == NULL) {
			//printf("Failed to load debug GL func '%s'", name);
			failed++;
		}
	}
#endif

	return failed;
}

DWORD* pBitmapBits;
BITMAPINFO bmi;
HDC fonthDC;
HBITMAP hbmBitmap;
HGDIOBJ hbmOld;
HGDIOBJ hFontOld;
HDC hDC;

HWND hwnd = {};
HFONT headingFont = NULL;
HFONT subtitleFont = NULL;
HFONT smallFont = NULL;
GLuint fontTexture_telegram;
GLuint fontTexture_cards;

_declspec(restrict, noalias) void *my_calloc(size_t nitems, size_t size)
{
	return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nitems * size);
}

void* my_memset(void* s, int c, size_t sz) {
	BYTE* p = (BYTE*)s;
	BYTE x = c & 0xff;
	unsigned int leftover = sz & 0x7;

	/* Catch the pathological case of 0. */
	if (!sz)
		return s;

	/* To understand what's going on here, take a look at the original
	* bytewise_memset and consider unrolling the loop. For this situation
	* we'll unroll the loop 8 times (assuming a 32-bit architecture). Choosing
	* the level to which to unroll the loop can be a fine art...
	*/
	sz = (sz + 7) >> 3;
	switch (leftover) {
	case 0: do {
		*p++ = x;
	case 7:      *p++ = x;
	case 6:      *p++ = x;
	case 5:      *p++ = x;
	case 4:      *p++ = x;
	case 3:      *p++ = x;
	case 2:      *p++ = x;
	case 1:      *p++ = x;
	} while (--sz > 0);
	}
	return s;
}

int fontcolors[3 * 32] = { 0,0,0 };

static const char cap1_s[] = "THE MAN FROM U.N.C.L.E";
static const char sub1_s[] = "featuring the talents of:";
static const char sm1_s[] = "Napoleon Solo";
static const char sm2_s[] = "Ilya Kuriakin";

void FontInRect(const char* sText, RECT &rFont) {

	int     i, nStringLength;
	BOOL    bResult;
	int     *pDx;
	int     nX = rFont.left;
	int     nY = rFont.top;
	int     Width = 0;


	// How long is the string - you need this later in this code.
	nStringLength = strlen(sText);

	// Allocate enough memory for the intercharacter spacing array.
	pDx = (int*)my_calloc(sizeof(int)* nStringLength,1);

	// Initialize the array with the standard values.
	for (i = 0; i < nStringLength; i++) {
		ABC     abc;
		if (!GetCharABCWidths(fonthDC, sText[i], sText[i], &abc)) {
			return;
		}
		pDx[i] = abc.abcA + abc.abcB + abc.abcC;

		// You need the width.
		Width += pDx[i];

		// Also, account for the Black extent of the string.
		if (i == 0) {
			// Adjustment before the first character for underhang
			nX -= abc.abcA;
			Width -= abc.abcA;
		}
		if (i == nStringLength - 1) {
			// Adjustment for overhang
			Width -= abc.abcC;
		}

	}

	int deltaCX = rFont.right - rFont.left - Width;
	int deltaCh = deltaCX / nStringLength;
	int remainder = deltaCX % nStringLength;
	int error = 0;

	// Distribute the adjustment through the intercharacter spacing.
	// For a more typographically correct approach, distribute the 
	// adjustment in the "white space."
	for (i = 0; i < nStringLength; i++) {
		pDx[i] += deltaCh;
		error += remainder;
		if (abs(error) >= nStringLength)    // adjustment?
		{
			int adjustment = abs(error) / error;
			pDx[i] += adjustment;
			error -= nStringLength * adjustment;
		}

	}

	// ExtTextOut() draws our text with our ICS array.
	bResult = ExtTextOut(fonthDC, nX, nY, ETO_OPAQUE, &rFont, sText, nStringLength, pDx);

}

void DrawRectText(const char* sText, COLORREF fg, COLORREF bg, int left, int top, int bottom, int right) {
	SetTextColor(fonthDC, fg);
	ExtTextOut(fonthDC, left, top, ETO_OPAQUE, NULL, sText, strlen(sText), NULL);
}

void DrawRectTextW(LPCWSTR a, int len, COLORREF fg, COLORREF bg, int left, int top, int bottom, int right) {
	SetTextColor(fonthDC, fg);
	ExtTextOutW(fonthDC, left, top, ETO_OPAQUE, NULL, a, len, NULL);
}

HFONT latinwide118Font = NULL;
HFONT Courier57Font = NULL;
HFONT Courier41Font = NULL;
HFONT Arial24Font = NULL;
HFONT Arial300Font = NULL; 

extern float RM_Objects[126];
extern float iMouseX;
extern float iMouseY;
extern float iTime;
extern float DirectionalLightX;
extern float DirectionalLightY;
extern float DirectionalLightZ;
extern float DirectionalLightR;
extern float DirectionalLightG;
extern float DirectionalLightB;
extern float PointLightPositionX;
extern float PointLightPositionY;
extern float PointLightPositionZ;
extern float PointLightR;
extern float PointLightG;
extern float PointLightB;
extern float CameraPositionX;
extern float CameraPositionY;
extern float CameraPositionZ;
extern float LookAtX;
extern float LookAtY;
extern float LookAtZ;
extern float CameraUpX;
extern float CameraUpY;
extern float CameraUpZ;
extern float FOV;
extern float Epsilon;
void Sync(float second);

void InitFontToTexture() {
	HRESULT hr;
	// Prepare to create a bitmap
	my_memset(&bmi.bmiHeader, 0, sizeof(BITMAPINFOHEADER));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = (int)XRES;
	bmi.bmiHeader.biHeight = (int)YRES;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount = 24;

	fonthDC = CreateCompatibleDC(NULL);
	hbmBitmap = CreateDIBSection(fonthDC, &bmi, DIB_RGB_COLORS, (void**)&pBitmapBits, 0, 0);
	SetMapMode(fonthDC, MM_TEXT);

	latinwide118Font = CreateFont(144, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, "Wide Latin");
	Courier57Font = CreateFont(61, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, "Courier New");
	Courier41Font = CreateFont(45, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, "Courier New");
	Arial24Font = CreateFont(24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, "Arial");
	Arial300Font = CreateFont(300, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, "Arial");

	hbmOld = SelectObject(fonthDC, hbmBitmap);
	SetBkMode(fonthDC, TRANSPARENT);
	SetTextAlign(fonthDC, TA_TOP | TA_LEFT);
}

void RenderFont1() {
	HBRUSH brush = CreateSolidBrush(RGB(218, 196, 103)); //create brush
	SelectObject(fonthDC, brush); //select brush into DC
	Rectangle(fonthDC, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); //draw rectangle over whole screen

	hFontOld = SelectObject(fonthDC, latinwide118Font);
	DrawRectText("The Secret", RGB(255, 255, 255), RGB(218, 196, 103), 431, 64, 333, 1478);
	hFontOld = SelectObject(fonthDC, latinwide118Font);
	DrawRectText("The Secret", RGB(0, 0, 0), RGB(218, 196, 103), 436, 71, 339, 1482);
	hFontOld = SelectObject(fonthDC, latinwide118Font);
	DrawRectText("Service Agency", RGB(255, 255, 255), RGB(218, 196, 103), 192, 178, 447, 1717);
	hFontOld = SelectObject(fonthDC, latinwide118Font);
	DrawRectText("Service Agency", RGB(0, 0, 0), RGB(218, 196, 103), 196, 184, 453, 1722);
	hFontOld = SelectObject(fonthDC, Courier57Font);
	DrawRectText("------------------------------------------------------", RGB(50, 50, 50), RGB(218, 196, 103), 44, 407, 477, 1880);
	hFontOld = SelectObject(fonthDC, Courier57Font);
	DrawRectText("TO: Agent \"TSConf\" SUBJECT: \"Your Assignment\"", RGB(50, 50, 50), RGB(218, 196, 103), 43, 372, 437, 1573);
	hFontOld = SelectObject(fonthDC, Courier41Font);
	DrawRectText("I have selected you for a most important assignment. It’s purpose is to", RGB(50, 50, 50), RGB(218, 196, 103), 44, 477, 525, 1819);
	hFontOld = SelectObject(fonthDC, Courier41Font);
	DrawRectText("give false information to the enemy and destroy key targets.", RGB(50, 50, 50), RGB(218, 196, 103), 44, 539, 592, 1544);
	hFontOld = SelectObject(fonthDC, Courier41Font);
	DrawRectText("If you complete it successfully you will be promoted. ", RGB(50, 50, 50), RGB(218, 196, 103), 44, 600, 651, 1394);
	hFontOld = SelectObject(fonthDC, Courier41Font);
	DrawRectText("You’ve been given a briefcase which shall be used to destroy", RGB(50, 50, 50), RGB(218, 196, 103), 44, 715, 766, 1544);
	hFontOld = SelectObject(fonthDC, Courier41Font);
	DrawRectText("the SPECTRUM base and their new ZX decoding machine. For this you shall", RGB(50, 50, 50), RGB(218, 196, 103), 44, 776, 827, 1819);
	hFontOld = SelectObject(fonthDC, Courier41Font);
	DrawRectText("enter their secret facility un-noticed, insert misinformation to their", RGB(50, 50, 50), RGB(218, 196, 103), 44, 835, 886, 1794);
	hFontOld = SelectObject(fonthDC, Courier41Font);
	DrawRectText("systems and finally destroy the aforementioned decoding machine.", RGB(50, 50, 50), RGB(218, 196, 103), 45, 894, 951, 1645);
	hFontOld = SelectObject(fonthDC, Courier41Font);
	DrawRectText("- I wish you good luck, agent.", RGB(50, 50, 50), RGB(218, 196, 103), 45, 1007, 1055, 795);
	hFontOld = SelectObject(fonthDC, Arial24Font);
	DrawRectText("Doing your dirty work for you since 1969", RGB(50, 50, 50), RGB(218, 196, 103), 1030, 95, 211, 1409);
	// --------------------------------------------- END END END
}

void RenderFont2() {
	hbmOld = SelectObject(fonthDC, hbmBitmap);
	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0)); //create brush
	SelectObject(fonthDC, brush); //select brush into DC
	Rectangle(fonthDC, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); //draw rectangle over whole screen
	hFontOld = SelectObject(fonthDC, Arial300Font);
	LPCWSTR str = L"♠♣♥♦";
	DrawRectTextW(str, 4, RGB(255, 0, 0), RGB(0, 0, 0), 0, 200, 1065, 693);
}

GLubyte *
ConvertRGB(BITMAPINFO *info,        /* I - Original bitmap information */
	void       *bits)        /* I - Original bitmap pixels */
{
	int       i, j,           /* Looping vars */
		bitsize,        /* Total size of bitmap */
		width;          /* Aligned width of bitmap */
	GLubyte   *newbits;       /* New RGB bits */
	GLubyte   *from, *to,     /* RGB looping vars */
		temp;           /* Temporary var for swapping */

						/*
						* Copy the original bitmap to the new array, converting as necessary.
						*/

	switch (info->bmiHeader.biCompression)
	{
	case BI_RGB:
		if (info->bmiHeader.biBitCount == 24)
		{
			width = 3 * info->bmiHeader.biWidth;
			width = (width + 3) & ~3;
			bitsize = width * info->bmiHeader.biHeight;
			if ((newbits = (GLubyte *)my_calloc(bitsize, 1)) == NULL)
				return (NULL);
			/*
			* Swap red & blue in a 24-bit image...
			*/

			for (i = 0; i < info->bmiHeader.biHeight; i++)
				for (j = 0, from = ((GLubyte *)bits) + i * width,
					to = newbits + i * width;
					j < info->bmiHeader.biWidth;
					j++, from += 3, to += 3)
				{
					to[0] = from[2];
					to[1] = from[1];
					to[2] = from[0];
				};
		};
		break;
	case BI_RLE4:
	case BI_RLE8:
	case BI_BITFIELDS:
		break;
	};

	return (newbits);
}

void AllSyncDataHandle(float row);

void RenderFontToTexture(GLuint texture) {
	GLvoid * obrazek;
	obrazek = ConvertRGB(&bmi, pBitmapBits);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, XRES, YRES, 0, GL_RGB, GL_UNSIGNED_BYTE, obrazek);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint GenFontTexture() {
	GLuint temp;
	glGenTextures(1, &temp);
	glBindTexture(GL_TEXTURE_2D, temp);

	// elect modulate to mix texture with color for shading
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//when texture area is small, bilinear filter the closest mipmap
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// when texture area is large, bilinear filter the first mipmap
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//the texture wraps over at the edges
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
	return temp;
}

#ifndef EDITOR_CONTROLS
#pragma code_seg(".main")
void entrypoint(void)
#else
#include "editor.h"
int __cdecl main(int argc, char* argv[])
#endif
{
	float time = 0.0f;
	// initialize window
	#if FULLSCREEN
		ChangeDisplaySettings(&screenSettings, CDS_FULLSCREEN);
		ShowCursor(0);
		hwnd = CreateWindow((LPCSTR)0xC018, 0, WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0);
	#else
		#ifdef EDITOR_CONTROLS
			hwnd = CreateWindow("EDIT", NULL, WS_POPUP | WS_VISIBLE, 0, 0, XRES, YRES, 0, 0, 0, 0 );
		#else
			// you can create a pseudo fullscreen window by similarly enabling the WS_MAXIMIZE flag as above
			// in which case you can replace the resolution parameters with 0s and save a couple bytes
			// this only works if the resolution is set to the display device's native resolution
			hwnd = CreateWindow((LPCSTR)0xC018, 0, WS_POPUP | WS_VISIBLE, 0, 0, XRES, YRES, 0, 0, 0, 0);
		#endif
	#endif

	if (hwnd == NULL) {
		MessageBox(NULL, "CreateWindow() failed", "Error", MB_OK);
	}

	InitFontToTexture();

	hDC = GetDC(hwnd);
	// initalize opengl context
	SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd), &pfd);
	wglMakeCurrent(hDC, wglCreateContext(hDC));
	
	// font textures
	RenderFont1();
	fontTexture_telegram = GenFontTexture();
	RenderFontToTexture(fontTexture_telegram);

	RenderFont2();
	fontTexture_cards = GenFontTexture();
	RenderFontToTexture(fontTexture_cards);


	// create and compile shader programs
	pidMain = ((PFNGLCREATESHADERPROGRAMVPROC)wglGetProcAddress("glCreateShaderProgramv"))(GL_FRAGMENT_SHADER, 1, &fragment_frag);
	//pidMain = ((PFNGLCREATESHADERPROGRAMVPROC)wglGetProcAddress("glCreateShaderProgramv"))(GL_VERTEX_SHADER, 1, &sync_vert);
	
	#if POST_PASS
		pidPost = ((PFNGLCREATESHADERPROGRAMVPROC)wglGetProcAddress("glCreateShaderProgramv"))(GL_FRAGMENT_SHADER, 1, &post_frag);
	#endif

#if OPENGL_DEBUG
		shaderDebug(fragment_frag, FAIL_KILL);
#if TWO_PASS
		shaderDebug(post_frag, FAIL_KILL);
#endif
#endif


		((PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram"))(pidMain);

		double position = 0.0;

#ifndef DEBUG
		IPlayer *player;

		int numRenderThreads = 3;
		player = new RealtimePlayer(&Song, numRenderThreads);
		player->Play();
#endif

		GLint length = sizeof(RM_Objects) / sizeof(RM_Objects[0]);
		GLuint ObjectsID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_Objects");
		GLuint resolutionID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_iResolution");
		GLuint iTimeID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_iTime");
		GLuint DirectionalLightID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_DirectionalLight");
		GLuint DirectionalLightColorID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_DirectionalLightColor");
		GLuint PointLightPositionID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_PointLightPosition");
		GLuint PointLightColorID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_PointLightColor");
		GLuint CameraPositionID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_CameraPosition");
		GLuint CameraLookAtID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_CameraLookAt");
		GLuint CameraUpID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_CameraUp");
		GLuint fovID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_FOV");

	// main loop
	do
	{
		#if !(DESPERATE)
			// do minimal message handling so windows doesn't kill your application
			// not always strictly necessary but increases compatibility and reliability a lot
			// normally you'd pass an msg struct as the first argument but it's just an
			// output parameter and the implementation presumably does a NULL check
			PeekMessage(0, 0, 0, 0, PM_REMOVE);
		#endif

#ifndef DEBUG
		// render with the primary shader
		auto songPos = player->GetSongPos();
		if (songPos >= player->GetLength()) break;
		int minutes = (int)songPos / 60;
		int seconds = (int)songPos % 60;
		int hundredths = (int)(songPos * 100.0) % 100;
		time = songPos;
		//((PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i"))(0, (static_cast<int>(songPos*44100.0)));
#endif


		// font
		glBindTexture(GL_TEXTURE_2D, fontTexture_telegram);
		((PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture"))(GL_TEXTURE0);
		((PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i"))(1, 0);

		((PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform1fv"))(ObjectsID, length, RM_Objects);

		((PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"))( resolutionID, XRES, YRES, XRES, YRES);

		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(iTimeID, iTime);

		((PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"))(DirectionalLightID, DirectionalLightX, DirectionalLightY, DirectionalLightZ, 0.0f);

		((PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"))(DirectionalLightColorID, DirectionalLightR, DirectionalLightG, DirectionalLightB, 0.0f);

		((PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"))(PointLightPositionID, PointLightPositionX, PointLightPositionY, PointLightPositionZ, 0.0f);

		((PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"))(PointLightColorID, PointLightR, PointLightG, PointLightB, 0.0f);

		((PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"))(CameraPositionID, CameraPositionX, CameraPositionY, CameraPositionZ, 0.0f);

		((PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"))(CameraLookAtID, LookAtX, LookAtY, LookAtZ, 0.0f);

		((PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"))(CameraUpID, CameraUpX, CameraUpY, CameraUpZ, 0.0f);

		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(fovID, FOV * 2.0f);


#ifdef DEBUG
		time += 30.0f / 60.0f;
#endif
		glRects(-1, -1, 1, 1);


		// render "post process" using the opengl backbuffer
		#if POST_PASS
			glBindTexture(GL_TEXTURE_2D, 2);
			#if USE_MIPMAPS
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0, XRES, YRES, 0);
				((PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap"))(GL_TEXTURE_2D);
			#else
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0, XRES, YRES, 0);
			#endif
			((PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture"))(GL_TEXTURE0);
			((PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram"))(pidPost);
			((PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i"))(0, 0);
			glRects(-1, -1, 1, 1);
		#endif

		SwapBuffers(hDC);

		Sync(time);


	} while(!GetAsyncKeyState(VK_ESCAPE)
		#if USE_AUDIO
			&& MMTime.u.sample < MAX_SAMPLES
		#endif
	);
	SelectObject(fonthDC, hbmOld);
	SelectObject(fonthDC, hFontOld);
	DeleteObject(hbmBitmap);
	DeleteObject(headingFont);
	DeleteObject(subtitleFont);
	DeleteObject(smallFont);
	DeleteDC(fonthDC);

	ExitProcess(0);
}
