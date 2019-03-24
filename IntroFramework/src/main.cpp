// custom build and feature flags
#ifdef DEBUG
	#define OPENGL_DEBUG        1
	#define FULLSCREEN          0
	#define DESPERATE           0
	#define BREAK_COMPATIBILITY 0
#else
	#define OPENGL_DEBUG        0
	#define FULLSCREEN          1
	#define DESPERATE           0
	#define BREAK_COMPATIBILITY 0
#endif

#define POST_PASS    1
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
// static HDC hDC;


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


#include <d3d9.h>
#include <d3dx9.h>
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

LPDIRECT3D9 pD3D = NULL;
LPDIRECT3DDEVICE9 pd3dDevice = NULL;
HWND hwnd = {};

LPD3DXFONT pFont = NULL;
LPDIRECT3DTEXTURE9      m_pTexture;   // The d3d texture for this font
LPDIRECT3DVERTEXBUFFER9 m_pVB;        // VertexBuffer for rendering text
DWORD   m_dwTexWidth;                 // Texture dimensions
DWORD   m_dwTexHeight;

void initD3D()
{
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	HRESULT hr = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pd3dDevice);
	if (FAILED(hr))
	{
		MessageBox(NULL, "initD3D() failed", "Error", MB_OK);
		// error handling here.
	}
}

LPD3DXFONT MakeFont()
{
	// 48 Arial
	LPD3DXFONT font = NULL;
	D3DXFONT_DESC desc =
	{ 48, 0, 0, 0, false, DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_PITCH, "" };
	strcpy(desc.FaceName, "Arial");
	D3DXCreateFontIndirect(pd3dDevice, &desc, &font);
	return font;
}

void RenderFontToTexture() {
	// font render to texture using dx9
	HRESULT hr;

	m_pTexture = NULL;
	m_pVB = NULL;

	m_dwTexWidth = m_dwTexHeight = 2048;

	if (pd3dDevice != NULL) {
		// create font texture
		hr = pd3dDevice->CreateTexture(m_dwTexWidth, m_dwTexHeight, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, &m_pTexture, NULL);

		// Prepare to create a bitmap
		DWORD*      pBitmapBits;
		BITMAPINFO bmi;
		ZeroMemory(&bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = (int)m_dwTexWidth;
		bmi.bmiHeader.biHeight = -(int)m_dwTexHeight;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biBitCount = 32;

		HDC fonthDC = CreateCompatibleDC(NULL);
		HBITMAP hbmBitmap = CreateDIBSection(fonthDC, &bmi, DIB_RGB_COLORS, (void**)&pBitmapBits, NULL, 0);
		SetMapMode(fonthDC, MM_TEXT);

		HGDIOBJ hbmOld = SelectObject(fonthDC, hbmBitmap);
		HGDIOBJ hFontOld = SelectObject(fonthDC, pFont);

		// Set text properties
		SetTextColor(fonthDC, RGB(255, 0, 0));
		SetBkColor(fonthDC, 0x00000000);
		SetTextAlign(fonthDC, TA_TOP);

		ExtTextOut(fonthDC, 0, 0, ETO_OPAQUE, NULL, "teksti", 1, NULL);

		// Lock the surface and write the alpha values for the set pixels
		D3DLOCKED_RECT d3dlr;
		m_pTexture->LockRect(0, &d3dlr, 0, 0);
		BYTE* pDstRow = (BYTE*)d3dlr.pBits;
		WORD* pDst16;
		BYTE bAlpha; // 4-bit measure of pixel intensity

		for (int y = 0; y < m_dwTexHeight; y++)
		{
			pDst16 = (WORD*)pDstRow;
			for (int x = 0; x < m_dwTexWidth; x++)
			{
				bAlpha = (BYTE)((pBitmapBits[m_dwTexWidth*y + x] & 0xff) >> 4);
				if (bAlpha > 0)
				{
					*pDst16++ = (WORD)((bAlpha << 12) | 0x0fff);
				}
				else
				{
					*pDst16++ = 0x0000;
				}
			}
			pDstRow += d3dlr.Pitch;
		}

		// Done updating texture, so clean up used objects
		m_pTexture->UnlockRect(0);
		SelectObject(fonthDC, hbmOld);
		SelectObject(fonthDC, hFontOld);
		DeleteObject(hbmBitmap);
		DeleteObject(pFont);
		DeleteDC(fonthDC);
	}
}

#ifndef EDITOR_CONTROLS
#pragma code_seg(".main")
void entrypoint(void)
#else
#include "editor.h"
#include "song.h"
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
		return NULL;
	}

	initD3D();
	HDC hDC = GetDC(hwnd);

	// initalize opengl context
	SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd), &pfd);
	wglMakeCurrent(hDC, wglCreateContext(hDC));
	
	pFont = MakeFont();
	RenderFontToTexture();

	// create and compile shader programs
	pidMain = ((PFNGLCREATESHADERPROGRAMVPROC)wglGetProcAddress("glCreateShaderProgramv"))(GL_FRAGMENT_SHADER, 1, &fragment_frag);
	#if POST_PASS
		pidPost = ((PFNGLCREATESHADERPROGRAMVPROC)wglGetProcAddress("glCreateShaderProgramv"))(GL_FRAGMENT_SHADER, 1, &post_frag);
	#endif

	// initialize sound
	#ifndef EDITOR_CONTROLS
		#if USE_AUDIO
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)_4klang_render, lpSoundBuffer, 0, 0);
			waveOutOpen(&hWaveOut, WAVE_MAPPER, &WaveFMT, NULL, 0, CALLBACK_NULL);
			waveOutPrepareHeader(hWaveOut, &WaveHDR, sizeof(WaveHDR));
			waveOutWrite(hWaveOut, &WaveHDR, sizeof(WaveHDR));
		#endif
	#else
		Leviathan::Editor editor = Leviathan::Editor();
		editor.updateShaders(&pidMain, &pidPost, true);

		// absolute path always works here
		// relative path works only when not ran from visual studio directly
		Leviathan::Song track(L"audio.wav");
		track.play();
		double position = 0.0;
	#endif

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
		((PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram"))(pidMain);
		#ifndef EDITOR_CONTROLS
			// if you don't have an audio system figure some other way to pass time to your shader
			#if USE_AUDIO
				waveOutGetPosition(hWaveOut, &MMTime, sizeof(MMTIME));
				// it is possible to upload your vars as vertex color attribute (gl_Color) to save one function import
				#if NO_UNIFORMS
					glColor3ui(MMTime.u.sample, 0, 0);
				#else
					// remember to divide your shader time variable with the SAMPLE_RATE (44100 with 4klang)
					((PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i"))(0, MMTime.u.sample);
				#endif
			#endif
		#else
			position = track.getTime();
			((PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i"))(0, (static_cast<int>(position*44100.0)));
		#endif
		glRects(-1, -1, 1, 1);

		// render "post process" using the opengl backbuffer
		#if POST_PASS
			glBindTexture(GL_TEXTURE_2D, 1);
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

	ExitProcess(0);
}
