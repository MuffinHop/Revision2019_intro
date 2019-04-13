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

int POST_PASS = 1;
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

#include <stdio.h>


#define _CRT_SECURE_NO_WARNINGS

#include <stdint.h> // uint32_t
#include <stdlib.h> // exit()
#include <string.h> // memcpy

#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

_declspec(restrict, noalias) void *my_calloc(size_t nitems, size_t size)
{
	return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nitems * size);
}


// error handler
void error(const char* msg)
{
	exit(1);
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

char* unpackedData;
unsigned long unpackOffset = 0;

static void sendBytesToOut(const unsigned char* data, unsigned int numBytes)
{
	if (data != NULL && numBytes > 0) {
		for (int i = 0; i < numBytes; i++) {
			unpackedData[unpackOffset + i] = data[i];
		}
		unpackOffset += numBytes;
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

	history = (unsigned char*)my_calloc(sizeof(unsigned char) * HISTORY_SIZE, 1);


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


/// parse command-line
int dolz4()
{
	unpackedData = (char*)my_calloc(sizeof(char)*1024, 64);

	// and go !
	unlz4(getByteFromIn, sendBytesToOut, NULL);
	return 0;
}


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

void Sync(float second);

void InitFontToTexture() {
	HRESULT hr;
	// Prepare to create a bitmap
	my_memset(&bmi.bmiHeader, 0, sizeof(BITMAPINFOHEADER));
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
	Rectangle(fonthDC, 0, 0, 1920, 1080); //draw rectangle over whole screen

	/////////////// DRAW
	// canvas: TelegramCanvas
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
	DrawRectText("TO: Agent 'TSConf' SUBJECT: 'Your Assignment'", RGB(50, 50, 50), RGB(218, 196, 103), 43, 372, 437, 1573);
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
	DrawRectText("- I wish you good luck, agent.",RGB(50,50,50),RGB(218,196,103),45,1007,1055,795);
	hFontOld = SelectObject(fonthDC, Arial24Font);
	DrawRectText("Doing your dirty work for you since 1969", RGB(50, 50, 50), RGB(218, 196, 103), 1041, 96, 237, 1465);
	// --------------------------------------------- END END END
}

void RenderFont2() {
	hbmOld = SelectObject(fonthDC, hbmBitmap);
	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0)); //create brush
	SelectObject(fonthDC, brush); //select brush into DC
	Rectangle(fonthDC, 0, 0, 1920, 1080); //draw rectangle over whole screen
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, obrazek);
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

int fontinit = 0;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




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

	dolz4();

	InitFontToTexture();

	hDC = GetDC(hwnd);
	// initalize opengl context
	SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd), &pfd);
	wglMakeCurrent(hDC, wglCreateContext(hDC));

	GLuint TextIds[3] = { 0, 0, 0};



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


		GLuint DistanceID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_Distance");
		GLuint LensCoeffID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_LensCoeff");
		GLuint MaxCoCID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_MaxCoC");
		GLuint RcpMaxCoCID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_RcpMaxCoC");
		GLuint MarchMinimumID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_MarchMinimum");
		GLuint FarPlaneID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_FarPlane");
		GLuint StepID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_Step");
		GLuint fogDensityID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "fogDensity");
		GLuint TextID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidMain, "_TextId");



		GLuint MainTexID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidPost, "_MainTex");
		GLuint iResolutionID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidPost, "iResolution");
		GLuint GainID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidPost, "_Gain");
		GLuint GammaID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidPost, "_Gamma");
		GLuint LiftID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidPost, "_Lift");
		GLuint PresaturationID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidPost, "_Presaturation");
		GLuint ColorTemperatureStrengthID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidPost, "_ColorTemperatureStrength");
		GLuint ColorTempratureID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidPost, "_ColorTemprature");
		GLuint TempratureNormalizationID = ((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(pidPost, "_TempratureNormalization");


		

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
		Sync(time);


		// font
		int index = (int)TextId;
		GLuint tid = TextIds[index];


		((PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram"))(pidMain);
		POST_PASS = 1;

		if (tid > 0) {
			glBindTexture(GL_TEXTURE_2D, tid);
			((PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture"))(GL_TEXTURE0);
			((PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i"))(1, 0);
			POST_PASS = 0;
		}
		
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

		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(StepID, Step);

		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(fogDensityID, fogDensity);
		
		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(TextID, TextId);

		



		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(DistanceID, Distance);
		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(LensCoeffID, LensCoeff);
		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(MaxCoCID, MaxCoC);
		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(RcpMaxCoCID, RcpMaxCoC);
		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(MarchMinimumID, MarchMinimum);
		((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(FarPlaneID, FarPlane);


#ifdef DEBUG
		time += 30.0f / 60.0f; 
#endif
		glRects(-1, -1, 1, 1);

		// render "post process" using the opengl backbuffer
		if (POST_PASS == 1) {
			glBindTexture(GL_TEXTURE_2D, 1);
#if USE_MIPMAPS
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 0, 0, XRES, YRES, 0);
			((PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap"))(GL_TEXTURE_2D);
#else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0, XRES, YRES, 0);
#endif
			((PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture"))(GL_TEXTURE1);
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

				// font textures
				RenderFont1();
				fontTexture_telegram = GenFontTexture();
				RenderFontToTexture(fontTexture_telegram);
				TextIds[1] = fontTexture_telegram;

				RenderFont2();
				fontTexture_cards = GenFontTexture();
				RenderFontToTexture(fontTexture_cards);
				TextIds[2] = fontTexture_cards;

				fontinit = 1;
			}
		}

		SwapBuffers(hDC);



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
