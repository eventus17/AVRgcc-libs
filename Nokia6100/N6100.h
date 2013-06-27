/*
 * N6100.h
 *
 *  Created on: 03-01-2013
 *      Author: SunRiver
 *
 */

#ifndef N6100_H_
#define N6100_H_

#define DELAY_MS_CONV(ms) ( (uint32_t) (ms*(F_CPU/6000L)) )
#define delay_ms(ms)  delayloop32(DELAY_MS_CONV(ms))
#define DELAY_US_CONV(us) ((uint16_t)(((((us)*1000L)/(1000000000/F_CPU))-1)/4))
#define delay_us(us)	  delayloop16(DELAY_US_CONV(us))

#define WHITE     0xFFF
#define BLACK     0x000
#define RED       0xF00
#define GREEN     0x0F0
#define BLUE      0x00F
#define CYAN      0x1FF
#define MAGENTA   0xF0F
#define YELLOW    0xFF0
#define GRAY      0x222
#define LIGHTBLUE 0xADE
#define PINK      0xF6A

#define NOFILL	0
#define FILL    1

/* Czcionki */
#define FONT_HEIGHT 8     // Font 8x8

#define LCDCommand      0
#define LCDData         1

// Pod³aczenie sygna³ów LCD -------------------------------------------------------

#define LCD_CS(x)           PORTB= (x)? (PORTB|(1<<PB4)) : (PORTB&~(1<<PB4))
#define LCD_CLK(x)          PORTB= (x)? (PORTB|(1<<PB7)) : (PORTB&~(1<<PB7))
#define LCD_DATA(x)         PORTB= (x)? (PORTB|(1<<PB5)) : (PORTB&~(1<<PB5))
#define LCD_RESET(x)        PORTB= (x)? (PORTB|(1<<PB1)) : (PORTB&~(1<<PB1))
#define LCD_BACKLIGHT(x)    PORTD= (x)? (PORTD|(1<<PD0)) : (PORTD&~(1<<PD0))

// Philips PCF8833 --- Kody kontrolne LCD -----------------------------------------

#define NOP        0x00  // nop
#define SWRESET    0x01  // Software RESET
#define BSTROFF    0x02  // booster voltage OFF
#define BSTRON     0x03  // booster voltage ON
#define RDDIDIF    0x04  // Odczyt Identyfikatora LCD
#define RDDST      0x09  // Odczyt Statusu LCD
#define SLEEPIN    0x10  // sleep in
#define SLEEPOUT   0x11  // sleep out
#define PTLON      0x12  // Tryb wywietlania - PARTIAL
#define NORON      0x13  // Tryb wywietlania - NORMAL
#define INVOFF     0x20  // inversja OFF
#define INVON      0x21  // inversja ON
#define DALO       0x22  // wszystkie pixele OFF
#define DAL        0x23  // wszystkie pixele ON
#define SETCON     0x25  // Zapis ustawieñ kontrastu
#define DISPOFF    0x28  // LCD OFF
#define DISPON     0x29  // LCD ON
#define CASET      0x2A  // Ustawienie Adresu Kolumn
#define PASET      0x2B  // Ustawienie Adresu Strony
#define RAMWR      0x2C  // Zapis do pamieci
#define RGBSET     0x2D  // Ustawienie Koloru
#define PTLAR      0x30  // czeciowy obszar wywietlania
#define VSCRDEF    0x33  // Definicja przewijania pionowego
#define TEOFF      0x34  // Tryb Testowy OFF
#define TEON       0x35  // Tryb Testowy ON
#define MADCTL     0x36  // MAC (Memmory Acces Control)
#define SEP        0x37  // Przewijanie w pionie - adres pocz¹tkowy
#define IDMOFF     0x38  // idle mode OFF
#define IDMON      0x39  // idle mode ON
#define COLMOD     0x3A  // Formatowanie interfejsu (piksele)
#define SETVOP     0xB0  // Ustawienie Vop
#define BRS        0xB4  // zamiana wierszy od do³u
#define TRS        0xB6  // zamiana wierszy od góry
#define DISCTR     0xB9  // Display Control
#define DOR2       0xBA  // Dane
#define TCDFE      0xBD  // On/OFF - DF kompensacja temperatury
#define TCVOPE     0xBF  // ON/OFF - Vop kompensatora temperatury
#define EC         0xC0  // kwarc zew/wew
#define SETMUL     0xC2  // Ustawienie mnoznika
#define TCVOPAB    0xC3  // Ustawienie TCVOP dla A i B
#define TCVOPCD    0xC4  // Ustawienie TCVOP dla C i D
#define TCDF       0xC5  // ustawienie dzielnika czestotliwoci
#define DF8COLOR   0xC6  // ustawienie dzielnika trybu 8 kolorowego
#define SETBS      0xC7  // ustawienie biasu
#define RDTEMP     0xC8  // odczyt temperatury
#define NLI        0xC9  // n-line odwrócenie

#define RDID1      0xDA  // odczyt ID1
#define RDID2      0xDB  // odczyt ID2
#define RDID3      0xDC  // odczyt ID3

#define RGB_8BIT    2
#define RGB_12BIT	3

// Moje funkcje -----------------------------------------------------------------------------------------

void delayloop16(uint16_t count);
void delayloop32(uint32_t loops);

void LCD_Init(void);
void SendLcd_color(unsigned char color);
void SendLcd(unsigned char type, unsigned char dat);

void LCD_GotoXY(unsigned char x, unsigned char y);
void LCD_Pixel(unsigned char x, unsigned char y, int color);
void LCD_Line(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, int color);
void LCD_Box(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char fill, int color);
void LCD_Circle(unsigned char x0, unsigned char y0, unsigned char radius, int color);


void LCD_Char(char c, unsigned char x, unsigned char y, int fColor, int bColor);
void LCD_String(char *pString, unsigned char x, unsigned char  y,  int fColor, int bColor);
void LCD_Bitmap (unsigned char start_x, unsigned char start_y, unsigned char h_size, unsigned char v_size, unsigned char *bitmap_data);

#endif /* N6100_H_ */
