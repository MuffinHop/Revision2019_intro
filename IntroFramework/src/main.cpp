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

#include "smallz4.h"
#include "SimplexNoise.h"

int POST_PASS = 1;
#define USE_MIPMAPS  1
#define USE_AUDIO    1
#define NO_UNIFORMS  0

#include "IntroFramework/gl_loader.h"
#include "definitions.h"

#if OPENGL_DEBUG 
	#include "debug.h"
#endif

#include "glext.h"
#include "shaders/fragment.inl"
#include "shaders/sync.inl"
#include "shaders/post.inl"

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
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>



#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

// error handler
void error(const char* msg)
{
}


// ==================== I/O INTERFACE ====================


// read one byte from input, see getByteFromIn()  for a basic implementation
typedef unsigned char(*GET_BYTE)  ();
// write several bytes,      see sendBytesToOut() for a basic implementation
typedef void(*SEND_BYTES)(const unsigned char*, unsigned int);


#include "packfile.h"
unsigned long packOffset = 0;

/// read a single byte (with simple buffering)
static unsigned char getByteFromIn()
{
	return packdata[packOffset++];
}

/// output stream, usually stdout
static FILE* out = NULL;
/// write a block of bytes

/// LZ4 LZ4 LZ4 LZ4
/// UNPACKED DATA CAN BE READ HERE
/// UNPACKED DATA CAN BE READ HERE
/// UNPACKED DATA CAN BE READ HERE

unsigned char* unpackedData;
unsigned long unpackOffset = 0;

static void sendBytesToOut(const unsigned char* data, unsigned int numBytes)
{
	int ii = 0;
	if (data != NULL && numBytes > 0) {
		for (int i = 0; i < numBytes; i++) {
			for (int j = 7; j >= 0; j--) {
				int bit = (data[i] & (1 << j)) != 0;
				unpackedData[unpackOffset + ii] = bit * 255;
				ii++;
			}
		}
		unpackOffset += ii;
	}
}


// ==================== LZ4 DECOMPRESSOR ====================

/// decompress everything in input stream (accessed via getByte) and write to output stream (via sendBytes)
void unlz4(GET_BYTE getByte, SEND_BYTES sendBytes, const char* dictionary)
{
	// signature
	unsigned char signature1 = getByte();
	unsigned char signature2 = getByte();
	unsigned char signature3 = getByte();
	unsigned char signature4 = getByte();
	uint32_t signature = (signature4 << 24) | (signature3 << 16) | (signature2 << 8) | signature1;
	int isModern = (signature == 0x184D2204);
	int isLegacy = (signature == 0x184C2102);
	if (!isModern && !isLegacy)
		error("invalid signature");

	unsigned char hasBlockChecksum = FALSE;
	unsigned char hasContentSize = FALSE;
	unsigned char hasContentChecksum = FALSE;
	unsigned char hasDictionaryID = FALSE;
	if (isModern)
	{
		// flags
		unsigned char flags = getByte();
		hasBlockChecksum = flags & 16;
		hasContentSize = flags & 8;
		hasContentChecksum = flags & 4;
		hasDictionaryID = flags & 1;

		// only version 1 file format
		unsigned char version = flags >> 6;
		if (version != 1)
			error("only LZ4 file format version 1 supported");

		// ignore blocksize
		getByte();

		if (hasContentSize)
		{
			// ignore, skip 8 bytes
			getByte(); getByte(); getByte(); getByte();
			getByte(); getByte(); getByte(); getByte();
		}
		if (hasDictionaryID)
		{
			// ignore, skip 4 bytes
			getByte(); getByte(); getByte(); getByte();
		}

		// ignore header checksum (xxhash32 of everything up this point & 0xFF)
		getByte();
	}

	// don't lower this value, backreferences can be 64kb far away
#define HISTORY_SIZE 64*1024
	// contains the latest decoded data
	unsigned char *history;
	// next free position in history[]
	unsigned int  pos = 0;

	history = (unsigned char*)calloc(sizeof(unsigned char) * HISTORY_SIZE, 1);


	// parse all blocks until blockSize == 0
	while (1)
	{
		// block size
		uint32_t blockSize = getByte();
		blockSize |= (uint32_t)getByte() << 8;
		blockSize |= (uint32_t)getByte() << 16;
		blockSize |= (uint32_t)getByte() << 24;

		// highest bit set ?
		unsigned char isCompressed = isLegacy || (blockSize & 0x80000000) == 0;
		if (isModern)
			blockSize &= 0x7FFFFFFF;

		// stop after last block
		if (blockSize == 0)
			break;

		if (isCompressed)
		{
			// decompress block
			uint32_t blockOffset = 0;
			uint32_t numWritten = 0;
			while (blockOffset < blockSize)
			{
				// get a token
				unsigned char token = getByte();

				blockOffset++;

				// determine number of literals
				uint32_t numLiterals = (token >> 4) & 0x0F;
				if (numLiterals == 15)
				{
					// number of literals length encoded in more than 1 byte
					unsigned char current;
					do
					{
						current = getByte();
						numLiterals += current;
						blockOffset++;
					} while (current == 255);
				}

				blockOffset += numLiterals;
				// copy all those literals
				while (numLiterals-- > 0)
				{
					history[pos++] = getByte();

					// flush output buffer
					if (pos == HISTORY_SIZE)
					{
						sendBytes(history, HISTORY_SIZE);
						numWritten += HISTORY_SIZE;
						pos = 0;
					}
				}

				// last token has only literals
				if (blockOffset == blockSize)
					break;

				// match distance is encoded by two bytes (little endian)
				blockOffset += 2;
				uint32_t delta = getByte();
				delta |= (uint32_t)getByte() << 8;
				// zero isn't allowed
				if (delta == 0)
					error("invalid offset");

				// match length (must be >= 4, therefore length is stored minus 4)
				uint32_t matchLength = 4 + (token & 0x0F);
				if (matchLength == 4 + 0x0F)
				{
					unsigned char current;
					do // match length encoded in more than 1 byte
					{
						current = getByte();
						matchLength += current;
						blockOffset++;
					} while (current == 255);
				}

				// copy match
				uint32_t reference = (pos >= delta) ? pos - delta : HISTORY_SIZE + pos - delta;
				if (pos + matchLength < HISTORY_SIZE && reference + matchLength < HISTORY_SIZE)
				{
					// fast copy
					if (pos >= reference + matchLength || reference >= pos + matchLength)
					{
						// non-overlapping
						memcpy(history + pos, history + reference, matchLength);
						pos += matchLength;
					}
					else
					{
						// overlapping
						while (matchLength-- > 0)
							history[pos++] = history[reference++];
					}
				}
				else
				{
					// slower copy, have to take care of buffer limits
					while (matchLength-- > 0)
					{
						// copy single byte
						history[pos++] = history[reference++];

						// cannot write anymore ? => wrap around
						if (pos == HISTORY_SIZE)
						{
							// flush output buffer
							sendBytes(history, HISTORY_SIZE);
							numWritten += HISTORY_SIZE;
							pos = 0;
						}
						// cannot read anymore ? => wrap around
						if (reference == HISTORY_SIZE)
							reference = 0;
					}
				}
			}

			// all legacy blocks must be completely filled - except for the last one
			if (isLegacy && numWritten + pos < 8 * 1024 * 1024)
				break;
		}
		else
		{
			// copy uncompressed data and add to history, too (if next block is compressed and some matches refer to this block)
			while (blockSize-- > 0)
			{
				// copy a byte ...
				history[pos++] = getByte();
				// ... until buffer is full => send to output
				if (pos == HISTORY_SIZE)
				{
					sendBytes(history, HISTORY_SIZE);
					pos = 0;
				}
			}
		}


		if (hasBlockChecksum)
		{
			// ignore checksum, skip 4 bytes
			getByte(); getByte(); getByte(); getByte();
		}
	}

	if (hasContentChecksum)
	{
		// ignore checksum, skip 4 bytes
		getByte(); getByte(); getByte(); getByte();
	}

	// flush output buffer
	sendBytes(history, pos);
}


// ==================== COMMAND-LINE HANDLING ====================



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
GLuint fontTexture_telegram;
GLuint fontTexture_cards;
GLuint fontTexture_greets;
GLuint fontTexture_credits;
GLuint texture_logos;
GLuint texture_perlin;
unsigned char* perlinnoise;


void DrawRectText(const char* sText, COLORREF fg, int left, int top) {
	SetTextColor(fonthDC, fg);
	ExtTextOut(fonthDC, left, top, ETO_OPAQUE, NULL, sText, strlen(sText), NULL);
}

int buftextlefts[15] = { 0 };
int buftexttops[15] = { 0 };
char *buftexts[15] = { "" };
char **buftextptr = buftexts;
int buftextindex = 0;
int buftextcount = 0;

float bufcharscurrent = 0.0;
int bufcharstotal = 0;

int buf2textlefts[32] = { 0 };
int buf2texttops[32] = { 0 };
char *buf2texts[32] = { "" };
char **buf2textptr = buf2texts;
int buf2textindex = 0;
int buf2textcount = 0;

float buf2charscurrent = 0.0;
float buf2charsprev = 0.0;
int buf2charstotal = 0;

void DrawRectTextBuf(const char* sText, COLORREF fg, int left, int top) {
	buftextptr[buftextindex] = (char*)sText;
	buftextlefts[buftextindex] = left;
	buftexttops[buftextindex] = top;
	bufcharstotal += (int)strlen(sText);
	buftextindex++;
	buftextcount++;
}

void DrawRectTextBuf2(const char* sText, COLORREF fg, int left, int top) {
	buf2textptr[buf2textindex] = (char*)sText;
	buf2textlefts[buf2textindex] = left;
	buf2texttops[buf2textindex] = top;
	buf2charstotal += (int)strlen(sText);
	buf2textindex++;
	buf2textcount++;
}

void DrawRectTextW(LPCWSTR a, int len, COLORREF fg, int left, int top) {
	SetTextColor(fonthDC, fg);
	ExtTextOutW(fonthDC, left, top, ETO_OPAQUE, NULL, a, len, NULL);
}

HFONT latinwide118Font = NULL;
HFONT latinwide56Font = NULL;
HFONT Courier57Font = NULL;
HFONT Courier41Font = NULL;
HFONT Arial24Font = NULL;
HFONT Arial300Font = NULL; 
HFONT Garamond57Font = NULL;

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
extern float Distance;
extern float LensCoeff;
extern float MaxCoC;
extern float RcpMaxCoC;
extern float MarchMinimum;
extern float FarPlane;
extern float Gain_R;
extern float Gain_G;
extern float Gain_B;
extern float Gamma_R;
extern float Gamma_G;
extern float Gamma_B;
extern float Lift_R;
extern float Lift_G;
extern float Lift_B;
extern float Presaturation_R;
extern float Presaturation_G;
extern float Presaturation_B;
extern float ColorTemperatureStrength_R;
extern float ColorTemperatureStrength_G;
extern float ColorTemperatureStrength_B;
extern float ColorTemprature;
extern float TempratureNormalization;
extern float Step;
extern float fogDensity;
extern float TextId;
extern float Environment;
extern float StepIncreaseByDistance;
extern float StepIncreaseMax;
extern float ObjMax;
extern float ObjMin;


void Sync(float second);

void InitFontToTexture() {
	HRESULT hr;
	// Prepare to create a bitmap
	memset(&bmi.bmiHeader, 0, sizeof(BITMAPINFOHEADER));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = (int)1920;
	bmi.bmiHeader.biHeight = (int)1080;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount = 24;

	fonthDC = CreateCompatibleDC(NULL);
	hbmBitmap = CreateDIBSection(fonthDC, &bmi, DIB_RGB_COLORS, (void**)&pBitmapBits, 0, 0);
	SetMapMode(fonthDC, MM_TEXT);

	latinwide118Font = CreateFont(144, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, "Wide Latin");
	latinwide56Font = CreateFont(82, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, "Wide Latin");
	Courier57Font = CreateFont(61, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, "Courier New");
	Courier41Font = CreateFont(45, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, "Courier New");
	Arial24Font = CreateFont(24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, "Arial");
	Arial300Font = CreateFont(300, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, "Arial");
	Garamond57Font = CreateFont(57, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, "Garamond");

	hbmOld = SelectObject(fonthDC, hbmBitmap);
	SetBkMode(fonthDC, TRANSPARENT);
	SetTextAlign(fonthDC, TA_TOP | TA_LEFT);
}


void RenderFont1() {
	HBRUSH brush = CreateSolidBrush(RGB(218, 196, 103)); //create brush
	SelectObject(fonthDC, brush); //select brush into DC
	Rectangle(fonthDC, 0, 0, 1920, 1080); //draw rectangle over whole screen

	/////////////// DRAW
	// canvas: TelegramCanvas
	hFontOld = SelectObject(fonthDC, latinwide118Font);
	DrawRectText("The Secret", RGB(255, 255, 255), 431, 64-32);
	DrawRectText("The Secret", RGB(0, 0, 0), 436, 71 - 32);
	DrawRectText("Service Agency", RGB(255, 255, 255), 192, 178 - 32);
	DrawRectText("Service Agency", RGB(0, 0, 0), 196, 184 - 32);
	hFontOld = SelectObject(fonthDC, Courier57Font);
	DrawRectTextBuf("------------------------------------------------------", RGB(50, 50, 50), 44, 407 - 32);
	DrawRectTextBuf("TO: Agent 'TSConf' SUBJECT: 'Your Assignment'", RGB(50, 50, 50), 43, 372 - 32);
	hFontOld = SelectObject(fonthDC, Courier41Font);
	DrawRectTextBuf("I have selected you for a most important assignment. It’s purpose is to  ", RGB(50, 50, 50), 44, 477 - 32);
	DrawRectTextBuf("give false information to the enemy and destroy key targets.   ", RGB(50, 50, 50), 44, 539 - 32);
	DrawRectTextBuf("If you complete it successfully you will be promoted.   ", RGB(50, 50, 50), 44, 600 - 32);
	DrawRectTextBuf("You’ve been given a briefcase which shall be used to destroy   ", RGB(50, 50, 50), 44, 715 - 32);
	DrawRectTextBuf("the SPECTRUM base and their new ZX decoding machine. For this you shall  ", RGB(50, 50, 50), 44, 776 - 32);
	DrawRectTextBuf("enter their secret facility un-noticed, insert misinformation to their   ", RGB(50, 50, 50), 44, 835 - 32);
	DrawRectTextBuf("systems and finally destroy the aforementioned decoding machine.  ", RGB(50, 50, 50), 45, 894 - 32);
	DrawRectTextBuf("- I wish you good luck, agent.  ",RGB(50,50,50),45,1007 - 32);
	hFontOld = SelectObject(fonthDC, Arial24Font);
	DrawRectText("Doing your dirty work for you since 1969", RGB(50, 50, 50), 1041, 96 - 32);
	// --------------------------------------------- END END END
}


void RenderFont2() {
	hbmOld = SelectObject(fonthDC, hbmBitmap);
	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0)); //create brush
	SelectObject(fonthDC, brush); //select brush into DC
	Rectangle(fonthDC, 0, 0, 1920, 1080); //draw rectangle over whole screen
	hFontOld = SelectObject(fonthDC, Arial300Font);
	LPCWSTR str = L"♠♣♥♦";
	DrawRectTextW(str, 4, RGB(255, 0, 0), 0, 200);
}


void RenderFont3() {
	hbmOld = SelectObject(fonthDC, hbmBitmap);
	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0)); //create brush
	SelectObject(fonthDC, brush); //select brush into DC
	Rectangle(fonthDC, 0, 0, 1920, 1080); //draw rectangle over whole screen
	hFontOld = SelectObject(fonthDC, latinwide56Font);
	int off = 215;
	int y = 100;
	DrawRectTextBuf2("Logicoma  ", RGB(255, 255, 255), -150+off, 206-y);
	DrawRectTextBuf2("Epoch   ", RGB(255, 255, 255), 525 + off, 355 - y);
	DrawRectTextBuf2("DSS      ", RGB(255, 255, 255), -40 + off, 470 - y);
	DrawRectTextBuf2("Paraguay   ", RGB(255, 255, 255), 442 + off, 470 - y);
	DrawRectTextBuf2("Prismbeings  ", RGB(255, 255, 255), 592 + off, 211 - y);
	DrawRectTextBuf2("Conspiracy    ", RGB(255, 255, 255), 872 + off, 419 - y);
	DrawRectTextBuf2("PWP      ", RGB(255, 255, 255), 1017 + off, 574 - y);
	DrawRectTextBuf2("Dekadence  ", RGB(255, 255, 255), 292 + off, 574 - y);
	DrawRectTextBuf2("Peisik  ", RGB(255, 255, 255), -68 + off, 346 - y);
	DrawRectTextBuf2("lft     ", RGB(255, 255, 255), -93 + off, 569 - y);
	DrawRectTextBuf2("Poo-brain   ", RGB(255, 255, 255), 394 + off, 287 - y);
	DrawRectTextBuf2("Mercury  ", RGB(255, 255, 255), 1068 + off, 287 - y);
	DrawRectTextBuf2("Unique   ", RGB(255, 255, 255), 1102 + off, 500 - y);
	DrawRectTextBuf2("Loonies  ", RGB(255, 255, 255), 1084 + off, 645 - y);
	DrawRectTextBuf2("Calodox  ", RGB(255, 255, 255), 370 + off, 645 - y);
	DrawRectTextBuf2("Titan   ", RGB(255, 255, 255), -154 + off, 645 - y);
	DrawRectTextBuf2("           ", RGB(255, 255, 255), -154 + off, 645 - y);
}

void RenderFont4() {
	hbmOld = SelectObject(fonthDC, hbmBitmap);
	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0)); //create brush
	SelectObject(fonthDC, brush); //select brush into DC
	Rectangle(fonthDC, 0, 0, 1920, 1080); //draw rectangle over whole screen

	hFontOld = SelectObject(fonthDC, Garamond57Font);
	DrawRectText("Code by branch and visy", RGB(255, 255, 255), 53, 714);
	hFontOld = SelectObject(fonthDC, Garamond57Font);
	DrawRectText("Distance field models by Dysposin, branch and visy", RGB(255, 255, 255), 53, 780);
	hFontOld = SelectObject(fonthDC, Garamond57Font);
	DrawRectText("Music by h0ffman (WaveSabre)", RGB(255, 255, 255), 53, 845);
	hFontOld = SelectObject(fonthDC, Garamond57Font);
	DrawRectText("Made with Shadertoy, Unity & Blender", RGB(255, 255, 255), 53, 910);
	hFontOld = SelectObject(fonthDC, Garamond57Font);
	DrawRectText("Special thanks to pumpuli, noby, Ferris & zov", RGB(255, 255, 255), 54, 975);
}

int bitmap_alloc = 0;
GLubyte   *newbits;       /* New RGB bits */

GLubyte *
ConvertRGB(BITMAPINFO *info,        /* I - Original bitmap information */
	void       *bits)        /* I - Original bitmap pixels */
{
	int       i, j,           /* Looping vars */
		bitsize,        /* Total size of bitmap */
		width;          /* Aligned width of bitmap */
	GLubyte   *from, *to,     /* RGB looping vars */
		temp;           /* Temporary var for swapping */

						/*
						* Copy the original bitmap to the new array, converting as necessary.
						*/

	if (info->bmiHeader.biBitCount == 24)
	{
		width = 3 * info->bmiHeader.biWidth;
		width = (width + 3) & ~3;
		bitsize = width * info->bmiHeader.biHeight;
		if (bitmap_alloc == 0) {
			if ((newbits = (GLubyte *)calloc(bitsize, 1)) == NULL)
				return (NULL);

			bitmap_alloc = 1;
		}



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

	return (newbits);
}




void AllSyncDataHandle(float row);

void RenderBitmapToTexture(GLuint texture) {
	GLvoid * obrazek;
	obrazek = ConvertRGB(&bmi, pBitmapBits);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, obrazek);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint GenTexture(GLfloat param = GL_CLAMP) {
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
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param);
	glBindTexture(GL_TEXTURE_2D, 0);
	return temp;
}

int fontinit = 0;

/// parse command-line
int dolz4()
{
	unpackedData = (unsigned char*)calloc(sizeof(unsigned char) * 516*212, 1);

	// and go !
	unlz4(getByteFromIn, sendBytesToOut, NULL);

	texture_logos = GenTexture();

	glBindTexture(GL_TEXTURE_2D, texture_logos);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 516, 212, 0, GL_RED, GL_UNSIGNED_BYTE, unpackedData);
	glBindTexture(GL_TEXTURE_2D, 0);


	/*
	ivory 0, 0, 236, 115
	and 0, 116, 236, 58
	quad 0, 177, 236, 35

	unity 236, 0, 278, 112
	wave  236, 112, 278, 100
	*/
	return 0;
}

//#define STB_IMAGE_WRITE_IMPLEMENTATION

//#include "stb_image_write.h"

#define M_PI 3.1415926535

inline float GetNoise(float x, float y, float octaves, float texSize) {
	double c = 2, a = 1; // torus parameters (controlling size)
	double xt = (c + a * cos(2 * M_PI*(y / texSize)))*cos(2 * M_PI*(x / texSize));
	double yt = (c + a * cos(2 * M_PI*(y / texSize)))*sin(2 * M_PI*(x / texSize));
	double zt = a * sin(2 * M_PI*(y / texSize));
	float noise = octave_noise_3d(octaves, 0.6, 2.00, xt, yt, zt);
	return ((noise+1.0)/2.0)*(255-32);
}


void GeneratePerlin() {
	float texSize = 4096;
	size_t octaves = 16;

	perlinnoise = (unsigned char*) malloc(((int)texSize*(int)texSize)*3);
	texture_perlin = GenTexture(GL_REPEAT);
	(void)texture_perlin;

	long i = 0;
	for (float y = 0.; y < texSize; y += 1.) {
		for (float x = 0.; x < texSize; x += 1.) {
			float noiseval = GetNoise(x, y, octaves, texSize);
			perlinnoise[i++] = noiseval;
			perlinnoise[i++] = noiseval;
			perlinnoise[i++] = noiseval;
		}
	}

//	stbi_write_bmp("noise.bmp", (int)texSize, (int)texSize, 3, perlinnoise);



	glBindTexture(GL_TEXTURE_2D, texture_perlin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)texSize, (int)texSize, 0, GL_RGB, GL_UNSIGNED_BYTE, perlinnoise);
	((PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap"))(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float startTime = 0.0;
float endTime = 0.0;
float dt = 0.0;


#ifndef EDITOR_CONTROLS
#pragma code_seg(".main")
void entrypoint(void)
#else
#include "editor.h"
int __cdecl main(int argc, char* argv[])
#endif
{

	float time = 20.0f;
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

	GLuint TextIds[5] = { 0, 0, 0, 0, 0};


	// create and compile shader programs
	pidMain = ((PFNGLCREATESHADERPROGRAMVPROC)wglGetProcAddress("glCreateShaderProgramv"))(GL_FRAGMENT_SHADER, 1, &fragment_frag);
	//pidMain = ((PFNGLCREATESHADERPROGRAMVPROC)wglGetProcAddress("glCreateShaderProgramv"))(GL_VERTEX_SHADER, 1, &sync_vert);
	
	pidPost = ((PFNGLCREATESHADERPROGRAMVPROC)wglGetProcAddress("glCreateShaderProgramv"))(GL_FRAGMENT_SHADER, 1, &post_frag);

#if OPENGL_DEBUG
		shaderDebug(fragment_frag, FAIL_KILL);
#if TWO_PASS
		shaderDebug(post_frag, FAIL_KILL);
#endif
#endif


		((PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram"))(pidMain);

		double position = 0.0;

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


		GLuint DistanceID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_Distance");
		GLuint LensCoeffID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_LensCoeff");
		GLuint MaxCoCID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_MaxCoC");
		GLuint RcpMaxCoCID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_RcpMaxCoC");
		GLuint MarchMinimumID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_MarchMinimum");
		GLuint FarPlaneID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_FarPlane");
		GLuint StepID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_Step");
		GLuint fogDensityID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "fogDensity");
		GLuint TextID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_TextId");
		GLuint iMouseID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_iMouse");
		GLuint environmentID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_Environment");
		GLuint StepIncreaseByDistanceID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_StepIncreaseByDistance");
		GLuint StepIncreaseMaxID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_StepIncreaseMax");
		GLuint IChannel0ID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_iChannel0");

		GLuint OBJMAXID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "OBJMAX");
		GLuint OBJMINID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "OBJMIN");


		GLuint MainTexID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidPost, "_MainTex");
		GLuint iResolutionID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidPost, "iResolution");
		GLuint GainID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidPost, "_Gain");
		GLuint GammaID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidPost, "_Gamma");
		GLuint LiftID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidPost, "_Lift");
		GLuint PresaturationID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidPost, "_Presaturation");
		GLuint ColorTemperatureStrengthID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidPost, "_ColorTemperatureStrength");
		GLuint ColorTempratureID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidPost, "_ColorTemprature");
		GLuint TempratureNormalizationID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidPost, "_TempratureNormalization");


#ifndef DEBUG
		IPlayer *player;
#endif

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

		if (fontinit > 0) {

#ifndef DEBUG
			// render with the primary shader
			auto songPos = player->GetSongPos();
			if (songPos >= player->GetLength()) break;
			time = songPos;
#endif
			startTime = time;
			Sync(time);
		}


		// font
		int index = (int)TextId;
		GLuint tid = TextIds[index];

		((PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram"))(pidMain);
		POST_PASS = 1;

		if (tid > 0) {
			glBindTexture(GL_TEXTURE_2D, tid);
			((PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture"))(GL_TEXTURE0);
			((PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i"))(1, 0);
		}
		((PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture"))(GL_TEXTURE1);

		glBindTexture(GL_TEXTURE_2D, texture_perlin);
		((PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i"))(IChannel0ID,1);

		((PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture"))(GL_TEXTURE0);


		/*
		GLuint mainTexture = 0;
		glGenTextures(0, &mainTexture);
		glBindTexture(GL_TEXTURE_2D, mainTexture);
		//params
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, XRES, YRES, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//framebuffer
		GLuint framebuffer = 0;
		((PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffers"))(0, &framebuffer);
		((PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer"))(GL_FRAMEBUFFER, framebuffer);
		((PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer"))(GL_DRAW_FRAMEBUFFER, framebuffer);
		((PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2D"))(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mainTexture, 0);

		const GLenum attachements[] = { GL_COLOR_ATTACHMENT0 };
		((PFNGLDRAWBUFFERSPROC)wglGetProcAddress("glDrawBuffers"))(1, attachements);
		*/
		


		((PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform1fv"))(ObjectsID, length, RM_Objects);

		((PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"))(resolutionID, XRES*0.75, YRES*0.75, XRES*0.75, YRES*0.75);

		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(iTimeID, iTime);

		((PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"))(DirectionalLightID, DirectionalLightX, DirectionalLightY, DirectionalLightZ, 0.0f);

		((PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"))(DirectionalLightColorID, DirectionalLightR, DirectionalLightG, DirectionalLightB, 0.0f);

		((PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"))(PointLightPositionID, PointLightPositionX, PointLightPositionY, PointLightPositionZ, 0.0f);

		((PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"))(PointLightColorID, PointLightR, PointLightG, PointLightB, 0.0f);

		((PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"))(CameraPositionID, CameraPositionX, CameraPositionY, CameraPositionZ, 0.0f);

		((PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"))(CameraLookAtID, LookAtX, LookAtY, LookAtZ, 0.0f);

		((PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"))(CameraUpID, CameraUpX, CameraUpY, CameraUpZ, 0.0f);

		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(fovID, FOV * 2.0f);

		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(StepID, Step);

		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(fogDensityID, fogDensity);
		
		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(TextID, TextId);

		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(OBJMAXID, ObjMax);

		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(OBJMINID, ObjMin);


		


		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(DistanceID, Distance);
		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(LensCoeffID, LensCoeff);
		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(MaxCoCID, MaxCoC);
		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(RcpMaxCoCID, RcpMaxCoC);
		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(MarchMinimumID, MarchMinimum);
		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(FarPlaneID, FarPlane);

		((PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"))(iMouseID, iMouseX, iMouseY, 0.0f, 0.0f);
		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(environmentID, Environment);

		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(StepIncreaseByDistanceID, StepIncreaseByDistance);
		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(StepIncreaseMaxID, StepIncreaseMax);



#ifdef DEBUG
		time += 60.0f / 60.0f / 16.0f; 
#endif
		glRects(-1, -1, 1, 1);

		// render "post process" using the opengl backbuffer
		if (POST_PASS == 1) {
			glBindTexture(GL_TEXTURE_2D, 1);
#if USE_MIPMAPS
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0, XRES*0.75, YRES*0.75, 0);
			((PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap"))(GL_TEXTURE_2D);
#else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0, XRES, YRES, 0);
#endif
			((PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture"))(GL_TEXTURE0);
			((PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram"))(pidPost);
			((PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i"))(MainTexID, 0);



			((PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"))(iResolutionID, XRES, YRES, XRES, YRES);
			((PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"))(GainID, Gain_R, Gain_G, Gain_B, 0);
			((PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"))(GammaID, Gamma_R, Gamma_G, Gamma_B, 0);
			((PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"))(LiftID, Lift_R, Lift_G, Lift_B, 0);
			((PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"))(PresaturationID, Presaturation_R, Presaturation_G, Presaturation_B, 0);
			((PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f"))(ColorTemperatureStrengthID, ColorTemperatureStrength_R, ColorTemperatureStrength_G, ColorTemperatureStrength_B, 0);
			((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(ColorTempratureID, ColorTemprature);
			((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(TempratureNormalizationID, TempratureNormalization);

			glRects(-1, -1, 1, 1);

			if (fontinit == 0) {
				fontinit = 1;

				// font textures

				RenderFont2();
				fontTexture_cards = GenTexture();
				RenderBitmapToTexture(fontTexture_cards);
				TextIds[2] = fontTexture_cards;

				RenderFont3();
				fontTexture_greets = GenTexture();
				RenderBitmapToTexture(fontTexture_greets);
				TextIds[3] = fontTexture_greets;

				RenderFont4();
				fontTexture_credits = GenTexture();
				RenderBitmapToTexture(fontTexture_credits);
				TextIds[4] = fontTexture_credits;


				dolz4();
				TextIds[5] = texture_logos;

				GeneratePerlin();


				RenderFont1();
				fontTexture_telegram = GenTexture();
				RenderBitmapToTexture(fontTexture_telegram);
				TextIds[1] = fontTexture_telegram;

				buftextindex = 0;
				bufcharscurrent = 0.0;
				buf2textindex = 0;
				buf2charscurrent = 0.0;

#ifndef DEBUG

				int numRenderThreads = 3;
				player = new RealtimePlayer(&Song, numRenderThreads);
				player->Play();
#endif

			}

		}

		SwapBuffers(hDC);


		if (fontinit >= 1) {
			if (TextId == 1.0) {
				if (buftextindex < buftextcount) {
					if (buftextindex < 2) hFontOld = SelectObject(fonthDC, Courier57Font);
					else hFontOld = SelectObject(fonthDC, Courier41Font);

					char subbuff[128];
					memcpy(subbuff, buftextptr[buftextindex], (int)bufcharscurrent);
					subbuff[(int)bufcharscurrent] = '\0';

					DrawRectText(subbuff, RGB(0, 0, 0), buftextlefts[buftextindex], buftexttops[buftextindex]);

					RenderBitmapToTexture(fontTexture_telegram);
					TextIds[1] = fontTexture_telegram;
					
					float speed = 30.;
					if (buftextindex > 1) speed = 70.;
					bufcharscurrent += dt * speed;

					int len = strlen(buftextptr[buftextindex]);
					if (bufcharscurrent > len + 1) {
						buftextindex++;
						bufcharscurrent = 0;
						if (buftextindex == 3) DrawRectText("                                         assignment                      ", RGB(230, 50, 50), 44, 477 - 32);
						if (buftextindex == 3) DrawRectText("                                         assignment                      ", RGB(230, 50, 50), 44-1, 477-1 - 32);

						if (buftextindex == 4) DrawRectText("                              enemy                            ", RGB(230, 50, 50), 44, 539 - 32);
						if (buftextindex == 6) DrawRectText("                    briefcase                                  ", RGB(230, 50, 50), 44, 715 - 32);
						if (buftextindex == 7) DrawRectText("    SPECTRUM                    ZX          machine                      ", RGB(230, 50, 50), 44, 776 - 32);
						if (buftextindex == 4) DrawRectText("                              enemy                            ", RGB(230, 50, 50), 44-1, 539-1 - 32);
						if (buftextindex == 6) DrawRectText("                    briefcase                                  ", RGB(230, 50, 50), 44-1, 715-1 - 32);
						if (buftextindex == 7) DrawRectText("    SPECTRUM                    ZX          machine                      ", RGB(230, 50, 50), 44-1, 776-1 - 32);
					}
				}
				fontinit = 2;
			}

			if (TextId == 3.0) {
				if (fontinit == 2) {
					fontinit++;
					HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0)); //create brush
					SelectObject(fonthDC, brush); //select brush into DC
					Rectangle(fonthDC, 0, 0, 1920, 1080); //draw rectangle over whole screen

				}
				if (buf2textindex < buf2textcount) {
					hFontOld = SelectObject(fonthDC, latinwide56Font);

					char subbuff[128];
					memcpy(subbuff, buf2textptr[buf2textindex], (int)buf2charscurrent);
					subbuff[(int)buf2charscurrent] = '\0';

					float len = (float)strlen(buf2textptr[buf2textindex]);

					float wordtime = (buf2charscurrent - 1) / (len + 1);
					int f = (int)(255 * wordtime);

					DrawRectText(subbuff, RGB(f, f, f), buf2textlefts[buf2textindex], buf2texttops[buf2textindex]);

					float prevlen = 0.0;
					if (buf2textindex > 0) {
						char subbuff2[128];
						memcpy(subbuff2, buf2textptr[buf2textindex - 1], (int)buf2charsprev);
						subbuff2[(int)buf2charsprev] = '\0';

						prevlen = (float)strlen(buf2textptr[buf2textindex - 1]);
						float wordtime2 = (buf2charsprev - 1) / (prevlen + 1);
						int f2 = 255 - (int)(255 * wordtime2)*3.;
						if (f2 < 0) f2 = 0;

						DrawRectText(subbuff2, RGB(f2, f2, f2), buf2textlefts[buf2textindex - 1], buf2texttops[buf2textindex - 1]);
					}

					if (buf2charscurrent > len + 1 && buf2charsprev > prevlen+1) {
						buf2textindex++;
						buf2charscurrent = 0;
						buf2charsprev = 0;
						if (buf2textindex >= 2) {

							DrawRectText(buf2textptr[buf2textindex - 2], RGB(0, 0, 0), buf2textlefts[buf2textindex - 2] - 1, buf2texttops[buf2textindex - 2] - 1);
							DrawRectText(buf2textptr[buf2textindex - 2], RGB(0, 0, 0), buf2textlefts[buf2textindex - 2] + 1, buf2texttops[buf2textindex - 2] - 1);
							DrawRectText(buf2textptr[buf2textindex - 2], RGB(0, 0, 0), buf2textlefts[buf2textindex - 2] - 2, buf2texttops[buf2textindex - 2] + 1);
							DrawRectText(buf2textptr[buf2textindex - 2], RGB(0, 0, 0), buf2textlefts[buf2textindex - 2] + 2, buf2texttops[buf2textindex - 2] + 1);
						}
					}

					RenderBitmapToTexture(fontTexture_greets);
					TextIds[3] = fontTexture_greets;
					buf2charscurrent += dt * 12.;
					buf2charsprev += dt * 30.;
				}

			}
		}



#ifndef DEBUG
		auto songPos = player->GetSongPos();
		endTime = songPos;
		dt = endTime - startTime;
#else
		dt = 0.001;
#endif


	} while(!GetAsyncKeyState(VK_ESCAPE)
		#if USE_AUDIO
			&& MMTime.u.sample < MAX_SAMPLES
		#endif
	);
	SelectObject(fonthDC, hbmOld);
	SelectObject(fonthDC, hFontOld);
	DeleteObject(hbmBitmap);
	DeleteDC(fonthDC);

	ExitProcess(0);
}
