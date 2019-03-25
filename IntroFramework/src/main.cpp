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

#include <WaveSabreCore.h>
#include <WaveSabrePlayerLib.h>
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
using namespace WaveSabreCore;
using namespace WaveSabrePlayerLib;

extern const unsigned char SongBlob[];
// static HDC hDC;

WaveSabreCore::Device *SongFactory(SongRenderer::DeviceId id)
{
	switch (id)
	{
	case SongRenderer::DeviceId::Falcon: return new WaveSabreCore::Falcon();
	case SongRenderer::DeviceId::Slaughter: return new WaveSabreCore::Slaughter();
	case SongRenderer::DeviceId::Thunder: return new WaveSabreCore::Thunder();
	case SongRenderer::DeviceId::Scissor: return new WaveSabreCore::Scissor();
	case SongRenderer::DeviceId::Leveller: return new WaveSabreCore::Leveller();
	case SongRenderer::DeviceId::Crusher: return new WaveSabreCore::Crusher();
	case SongRenderer::DeviceId::Echo: return new WaveSabreCore::Echo();
	case SongRenderer::DeviceId::Smasher: return new WaveSabreCore::Smasher();
	case SongRenderer::DeviceId::Chamber: return new WaveSabreCore::Chamber();
	}
	return nullptr;
}

void progressCallback(double progress, void *data)
{
	const int barLength = 32;
	int filledChars = (int)(progress * (double)(barLength - 1));
	printf("\r[");
	for (int j = 0; j < barLength; j++) putchar(filledChars >= j ? '*' : '-');
	printf("]");
}

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
		const HDC hDC = GetDC(CreateWindow((LPCSTR)0xC018, 0, WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0));
	#else
		#ifdef EDITOR_CONTROLS
			HWND window = CreateWindow("static", 0, WS_POPUP | WS_VISIBLE, 0, 0, XRES, YRES, 0, 0, 0, 0);
			HDC hDC = GetDC(window);
		#else
			// you can create a pseudo fullscreen window by similarly enabling the WS_MAXIMIZE flag as above
			// in which case you can replace the resolution parameters with 0s and save a couple bytes
			// this only works if the resolution is set to the display device's native resolution
			HDC hDC = GetDC(CreateWindow((LPCSTR)0xC018, 0, WS_POPUP | WS_VISIBLE, 0, 0, XRES, YRES, 0, 0, 0, 0));
		#endif
	#endif

	// initalize opengl context
	SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd), &pfd);
	wglMakeCurrent(hDC, wglCreateContext(hDC));
	
	// create and compile shader programs
	pidMain = ((PFNGLCREATESHADERPROGRAMVPROC)wglGetProcAddress("glCreateShaderProgramv"))(GL_FRAGMENT_SHADER, 1, &fragment);
	#if POST_PASS
		pidPost = ((PFNGLCREATESHADERPROGRAMVPROC)wglGetProcAddress("glCreateShaderProgramv"))(GL_FRAGMENT_SHADER, 1, &post);
	#endif

	// initialize sound
	#ifndef EDITOR_CONTROLS
		#if USE_AUDIO
			/*4klang
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)_4klang_render, lpSoundBuffer, 0, 0);
			waveOutOpen(&hWaveOut, WAVE_MAPPER, &WaveFMT, NULL, 0, CALLBACK_NULL);
			waveOutPrepareHeader(hWaveOut, &WaveHDR, sizeof(WaveHDR));
			waveOutWrite(hWaveOut, &WaveHDR, sizeof(WaveHDR));
			*/

			SongRenderer::Song song;
			song.blob = SongBlob;
			song.factory = SongFactory;

			IPlayer *player;

			int numRenderThreads = 3;
			bool preRender = true;
			if (preRender)
			{
				printf("Prerender activated.\n");
				printf("Rendering...\n");

				player = new PreRenderPlayer(&song, numRenderThreads, progressCallback, nullptr);

				printf("\n\n");
}
			else
			{
				player = new RealtimePlayer(&song, numRenderThreads);
			}

			printf("Realtime player activated. Press ESC to quit.\n");
			player->Play();


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
				//4klang
				//waveOutGetPosition(hWaveOut, &MMTime, sizeof(MMTIME));

				auto songPos = player->GetSongPos();
				if (songPos >= player->GetLength()) break;
				int minutes = (int)songPos / 60;
				int seconds = (int)songPos % 60;
				int hundredths = (int)(songPos * 100.0) % 100;

				// it is possible to upload your vars as vertex color attribute (gl_Color) to save one function import
				#if NO_UNIFORMS
					glColor3ui(MMTime.u.sample, 0, 0);
				#else
					// remember to divide your shader time variable with the SAMPLE_RATE (44100 with 4klang)
					//((PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i"))(0, MMTime.u.sample);
				((PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i"))(0, (int)songPos*44100);
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
