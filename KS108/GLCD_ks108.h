/*
 * SunRiver (c) 2001
 * KS108 Library
 *
 * 10.11.2012 --- update
 *
 * v0.3  -- dodano parametr Color
 *       -- poprawi³o to pracê funkcji graficznych
 *       -- usuniêto funkcjê ClearDot
 *
 *       -- parametr color przyjmuje 2 wartoci
 *       -- Cz = dla koloru czarnego -- pixel zaswiecony
 *       -- Bi = dla koloru bia³ego  -- pixel zgaszony
 *
 *
 *
 */

#include <inttypes.h>
#include <avr/pgmspace.h>

#ifndef	GLCD_KS108_H
#define GLCD_KS108_H

// Porty Danych -----------------------------------------------------
// Piny danych D0 - D7  =   PD0 - PD7
// ------------------------------------------------------------------

#define LCD_CMD_PORT		PORTC
#define LCD_CMD_DIR			DDRC

#define LCD_DATA_IN			PIND
#define LCD_DATA_OUT		PORTD
#define LCD_DATA_DIR		DDRD

// Porty steruj¹ce --------------------------------------------------
// ------------------------------------------------------------------
// RESET na VCC
// ------------------------------------------------------------------

#define D_I					0x00		// D/I = PC0
#define R_W					0x01		// R/W = PC1
#define EN					0x02		// EN  = PC2
#define CSEL1				0x03		// CS1 = PC3
#define CSEL2				0x04		// CS2 = PC4


// Czipy -------------------------- podzia³ na 2x(64x64)  --- dla 128x64
#define CHIP1				0x00
#define CHIP2				0x01

// Komendy -------------------------
#define LCD_ON				0x3F
#define LCD_OFF				0x3E
#define LCD_SET_ADD			0x40
#define LCD_SET_PAGE		0xB8
#define LCD_DISP_START		0xC0

// Kolory ------------- dodane ze wzglêdu na koniecznoc gaszenia pixeli
#define Cz					0xFF    // [Cz]arny
#define Bi					0x00    // [Bi]a³y

// Czcionki ------------------------
#define FONT_LENGTH			0
#define FONT_FIXED_WIDTH	2
#define FONT_HEIGHT			3
#define FONT_FIRST_CHAR		4
#define FONT_CHAR_COUNT		5
#define FONT_WIDTH_TABLE	6


typedef struct {
	uint8_t x;
	uint8_t y;
	uint8_t page;
} lcdCoord;

typedef uint8_t (*glcd_FontCallback)(const uint8_t*);


// Funkcje Graficzne ----------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------

void GLCD_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);
void GLCD_DrawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
void GLCD_DrawRoundRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t radius, uint8_t color);
void GLCD_FillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
void GLCD_InvertRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void GLCD_SetInverted(uint8_t invert);
void GLCD_SetDot(uint8_t x, uint8_t y, uint8_t color);

#define GLCD_DrawVertLine(x, y, length, color) {GLCD_FillRect(x, y, 0, length, color);}
#define GLCD_DrawHoriLine(x, y, length, color) {GLCD_FillRect(x, y, length, 0, color);}
#define GLCD_DrawCircle(xCenter, yCenter, radius, color) {GLCD_DrawRoundRect(xCenter-radius, yCenter-radius, 2*radius, 2*radius, radius, color);}
#define GLCD_ClearScreen() {GLCD_FillRect(0, 0, 127, 63, Bi);}


// Funkcje Czcionek ------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------

uint8_t GLCD_ReadFontData(const uint8_t* ptr);		// Odczyt Calbacka
void GLCD_SelectFont(const uint8_t* font, glcd_FontCallback callback, uint8_t color);
int GLCD_PutChar(unsigned char c);
void GLCD_Puts(char* str);
void GLCD_Puts_P(PGM_P str);
uint8_t GLCD_CharWidth(char c);
uint16_t GLCD_StringWidth(char* str);
uint16_t GLCD_StringWidth_P(PGM_P str);

// Funkcje zamieniaj¹ce znaki ---------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------

void GLCD_IntUni(int liczba, uint8_t system);

// Funkcje Kontrolne  ----------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------

void GLCD_GotoXY(uint8_t x, uint8_t y);
void GLCD_Init(uint8_t invert);
uint8_t GLCD_ReadData(void);
void GLCD_WriteCommand(uint8_t cmd, uint8_t chip);
void GLCD_WriteData(uint8_t data);

#endif
