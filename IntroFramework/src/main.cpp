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
#if POST_PASS
	#include "shaders/post.inl"
#endif

#pragma data_seg(".pids")
// static allocation saves a few bytes
static int pidMain;
static int pidPost;


#include "biisi.h"
using namespace WaveSabrePlayerLib;

// static HDC hDC;

#define _NO_CRT_STDIO_INLINE

#include <stdio.h>


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
"glUniform4iv"
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
GLuint fontTexture;

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

	headingFont = CreateFont(YRES*0.2, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, "Impact");
	subtitleFont = CreateFont(YRES*0.05, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, "Times New Roman");
	smallFont = CreateFont(YRES*0.02, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, "Tahoma");

	hbmOld = SelectObject(fonthDC, hbmBitmap);
	hFontOld = SelectObject(fonthDC, headingFont);

	SetTextColor(fonthDC, RGB(255, 0, 0));
	SetBkColor(fonthDC, 0x0000000);
	SetTextAlign(fonthDC, TA_TOP);

	RECT rc1;
	rc1.top = 0;
	rc1.left = 0;
	rc1.bottom = YRES / 8;
	rc1.right = XRES;

	FontInRect(cap1_s, rc1);

	//

	hFontOld = SelectObject(fonthDC, subtitleFont);

	SetTextColor(fonthDC, RGB(255, 255, 255));
	SetBkColor(fonthDC, 0x00000000);
	SetTextAlign(fonthDC, TA_TOP);

	rc1.top = YRES / 8 + YRES / 16;
	rc1.left = XRES / 64;
	rc1.bottom = YRES / 8 + ((YRES / 16)*2);
	rc1.right = XRES/4;

	FontInRect(sub1_s, rc1);

	//

	hFontOld = SelectObject(fonthDC, smallFont);

	SetTextColor(fonthDC, RGB(32, 128, 64));
	SetBkColor(fonthDC, 0x00000000);
	SetTextAlign(fonthDC, TA_TOP);

	rc1.top = YRES / 2 + YRES / 4;
	rc1.left = XRES / 64;
	rc1.bottom = YRES / 2 + YRES / 3;
	rc1.right = XRES / 8;

	FontInRect(sm1_s, rc1);

	//

	hFontOld = SelectObject(fonthDC, smallFont);

	SetTextColor(fonthDC, RGB(32, 128, 64));
	SetBkColor(fonthDC, 0x00000000);
	SetTextAlign(fonthDC, TA_TOP);

	rc1.top = YRES / 2 + YRES / 4;
	rc1.left = XRES / 2 + XRES /  64;
	rc1.bottom = YRES / 2 + YRES / 3;
	rc1.right = XRES / 2 + XRES / 64+ ((XRES / 8) - XRES / 64);

	FontInRect(sm2_s, rc1);

	//ExtTextOut(fonthDC, 0, 0, ETO_OPAQUE, NULL, "THE MAN FROM U.N.C.L.E", 6, NULL);
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

void RenderFontToTexture() {
	// Set text properties

	GLvoid * obrazek;
	obrazek = ConvertRGB(&bmi, pBitmapBits);

	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, XRES, YRES, 0, GL_RGB, GL_UNSIGNED_BYTE, obrazek);
	glBindTexture(GL_TEXTURE_2D, 0);
}

#ifndef EDITOR_CONTROLS
#pragma code_seg(".main")
void entrypoint(void)
#else
#include "editor.h"
int __cdecl main(int argc, char* argv[])
#endif
{
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
	
	// font texture

	glGenTextures(1, &fontTexture);
	glBindTexture(GL_TEXTURE_2D, fontTexture);

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

	RenderFontToTexture();


	// create and compile shader programs
	pidMain = ((PFNGLCREATESHADERPROGRAMVPROC)wglGetProcAddress("glCreateShaderProgramv"))(GL_FRAGMENT_SHADER, 1, &fragment_frag);
	#if POST_PASS
		pidPost = ((PFNGLCREATESHADERPROGRAMVPROC)wglGetProcAddress("glCreateShaderProgramv"))(GL_FRAGMENT_SHADER, 1, &post_frag);
	#endif

		((PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram"))(pidMain);

		double position = 0.0;

		IPlayer *player;

		int numRenderThreads = 3;
		player = new RealtimePlayer(&Song, numRenderThreads);
		player->Play();
	// main loop
	do
	{
		#ifdef EDITOR_CONTROLS
			editor.beginFrame(timeGetTime());
		#endif

		#if !(DESPERATE)
			// do minimal message handling so windows doesn't kill your application
			// not always strictly necessary but increases compatibility and reliability a lot
			// normally you'd pass an msg struct as the first argument but it's just an
			// output parameter and the implementation presumably does a NULL check
			PeekMessage(0, 0, 0, 0, PM_REMOVE);
		#endif

		// render with the primary shader
		auto songPos = player->GetSongPos();
		if (songPos >= player->GetLength()) break;
		int minutes = (int)songPos / 60;
		int seconds = (int)songPos % 60;
		int hundredths = (int)(songPos * 100.0) % 100;

		((PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i"))(0, (static_cast<int>(songPos*44100.0)));
		// font
		glBindTexture(GL_TEXTURE_2D, fontTexture);
		((PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture"))(GL_TEXTURE0);
		((PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i"))(1, 0);

		AllSyncDataHandle(seconds);
		//glUniform4fv(glGetUniformLocation(pidMain, "_Objects"), 2, threshold);

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

		// handle functionality of the editor
		#ifdef EDITOR_CONTROLS
			editor.endFrame(timeGetTime());
			position = editor.handleEvents(&track, position);
			editor.printFrameStatistics();
			editor.updateShaders(&pidMain, &pidPost);
		#endif

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
