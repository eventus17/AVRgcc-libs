/*
 * SunRiver (c)2001
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
 * 27.12.2012  --- update -- by jachu
 *
 * v0.4  -- dodano funkcjê GLCD_IntUni
 *
 *
 */

#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include "GLCD_ks108.h"


lcdCoord			glcd_Coord;
uint8_t				glcd_Inverted=0;
glcd_FontCallback	glcd_FontRead;
uint8_t				glcd_FontColor;
const uint8_t*		glcd_Font;


void GLCD_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color) {
	uint8_t length, i, y, yAlt, xTmp, yTmp;
	int16_t m;
	

	// Linia pionowa

	if(x1 == x2) {

		if(y1 > y2) {
			yTmp = y1;
			y1 = y2;
			y2 = yTmp;
		}
		GLCD_DrawVertLine(x1, y1, y2-y1, color);
	

	// Linia pozioma

	} else if(y1 == y2) {	

		if(x1 > x2) {
			xTmp = x1;
			x1 = x2;
			x2 = xTmp;
		}
		GLCD_DrawHoriLine(x1, y1, x2-x1, color);
	
	//
	// Krzywa linia :)
	//
	} else {

		if((y2-y1) >= (x2-x1) || (y1-y2) >= (x2-x1)) {

			if(x1 > x2) {
				xTmp = x1;
				yTmp = y1;
				x1 = x2;
				y1 = y2;
				x2 = xTmp;
				y2 = yTmp;
			}
		
			length = x2-x1;
			m = ((y2-y1)*200)/length;
			yAlt = y1;
			
			for(i=0; i<=length; i++) {
				y = ((m*i)/200)+y1;
				
				if((m*i)%200 >= 100)
					y++;
				else if((m*i)%200 <= -100)
					y--;
				
				GLCD_DrawLine(x1+i, yAlt, x1+i, y, color);
				
				if(length <= (y2-y1) && y1 < y2)
					yAlt = y+1;
				else if(length <= (y1-y2) && y1 > y2)
					yAlt = y-1;
				else
					yAlt = y;
			}
		

		} else {

			if(y1 > y2) {
				xTmp = x1;
				yTmp = y1;
				x1 = x2;
				y1 = y2;
				x2 = xTmp;
				y2 = yTmp;
			}
			
			length = y2-y1;
			m = ((x2-x1)*200)/length;
			yAlt = x1;
			
			for(i=0; i<=length; i++) {
				y = ((m*i)/200)+x1;
				
				if((m*i)%200 >= 100)
					y++;
				else if((m*i)%200 <= -100)
					y--;
				
				GLCD_DrawLine(yAlt, y1+i, y, y1+i, color);
				if(length <= (x2-x1) && x1 < x2)
					yAlt = y+1;
				else if(length <= (x1-x2) && x1 > x2)
					yAlt = y-1;
				else
					yAlt = y;
			}
		}
	}
}

void GLCD_DrawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color) {
	GLCD_DrawHoriLine(x, y, width, color);				// góra
	GLCD_DrawHoriLine(x, y+height, width, color);		// dó³
	GLCD_DrawVertLine(x, y, height, color);				// lewa
	GLCD_DrawVertLine(x+width, y, height, color);		// prawa
}

void GLCD_DrawRoundRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t radius, uint8_t color) {
  	int16_t tSwitch, x1 = 0, y1 = radius;
  	tSwitch = 3 - 2 * radius;
	
	while (x1 <= y1) {
	    GLCD_SetDot(x+radius - x1, y+radius - y1, color);
	    GLCD_SetDot(x+radius - y1, y+radius - x1, color);

	    GLCD_SetDot(x+width-radius + x1, y+radius - y1, color);
	    GLCD_SetDot(x+width-radius + y1, y+radius - x1, color);
	    
	    GLCD_SetDot(x+width-radius + x1, y+height-radius + y1, color);
	    GLCD_SetDot(x+width-radius + y1, y+height-radius + x1, color);

	    GLCD_SetDot(x+radius - x1, y+height-radius + y1, color);
	    GLCD_SetDot(x+radius - y1, y+height-radius + x1, color);

	    if (tSwitch < 0) {
	    	tSwitch += (4 * x1 + 6);
	    } else {
	    	tSwitch += (4 * (x1 - y1) + 10);
	    	y1--;
	    }
	    x1++;
	}
	  	
	GLCD_DrawHoriLine(x+radius, y, width-(2*radius), color);			// góra
	GLCD_DrawHoriLine(x+radius, y+height, width-(2*radius), color);		// dó³
	GLCD_DrawVertLine(x, y+radius, height-(2*radius), color);			// lewa
	GLCD_DrawVertLine(x+width, y+radius, height-(2*radius), color);		// prawa
}

 // Funkcje wspomagane sprzêtowo ----------------------------------------------------------------------

void GLCD_FillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color) {
	uint8_t mask, pageOffset, h, i, data;
	height++;
	
	pageOffset = y%8;
	y -= pageOffset;
	mask = 0xFF;
	if(height < 8-pageOffset) {
		mask >>= (8-height);
		h = height;
	} else {
		h = 8-pageOffset;
	}
	mask <<= pageOffset;
	
	GLCD_GotoXY(x, y);
	for(i=0; i<=width; i++) {
		data = GLCD_ReadData();
		
		if(color == Cz) {
			data |= mask;
		} else {
			data &= ~mask;
		}

		GLCD_WriteData(data);
	}
	
	while(h+8 <= height) {
		h += 8;
		y += 8;
		GLCD_GotoXY(x, y);
		
		for(i=0; i<=width; i++) {
			GLCD_WriteData(color);
		}
	}
	
	if(h < height) {
		mask = ~(0xFF << (height-h));
		GLCD_GotoXY(x, y+8);
		
		for(i=0; i<=width; i++) {
			data = GLCD_ReadData();
		
			if(color == Cz) {
				data |= mask;
			} else {
				data &= ~mask;
			}
	
			GLCD_WriteData(data);
		}
	}
}

void GLCD_InvertRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
	uint8_t mask, pageOffset, h, i, data, tmpData;
	height++;
	
	pageOffset = y%8;
	y -= pageOffset;
	mask = 0xFF;
	if(height < 8-pageOffset) {
		mask >>= (8-height);
		h = height;
	} else {
		h = 8-pageOffset;
	}
	mask <<= pageOffset;
	
	GLCD_GotoXY(x, y);
	for(i=0; i<=width; i++) {
		data = GLCD_ReadData();
		tmpData = ~data;
		data = (tmpData & mask) | (data & ~mask);
		GLCD_WriteData(data);
	}
	
	while(h+8 <= height) {
		h += 8;
		y += 8;
		GLCD_GotoXY(x, y);
		
		for(i=0; i<=width; i++) {
			data = GLCD_ReadData();
			GLCD_WriteData(~data);
		}
	}
	
	if(h < height) {
		mask = ~(0xFF << (height-h));
		GLCD_GotoXY(x, y+8);
		
		for(i=0; i<=width; i++) {
			data = GLCD_ReadData();
			tmpData = ~data;
			data = (tmpData & mask) | (data & ~mask);
			GLCD_WriteData(data);
		}
	}
}

void GLCD_SetInverted(uint8_t invert) {
	if(glcd_Inverted != invert) {
		GLCD_InvertRect(0,0,127,63);
		glcd_Inverted = invert;
	}
}

/* Poprawka w funkcji
 * oraz dodanie parametru color
 */
void GLCD_SetDot(uint8_t x, uint8_t y, uint8_t color) {
	uint8_t data;
	
	GLCD_GotoXY(x, y-y%8);					// odczyt danych z pamieci LCD
	data = GLCD_ReadData();
	
	if(color == Cz) {
		data |= 0x01 << (y%8);				// ustawienie pixela
	} else {
		data &= ~(0x01 << (y%8));			// wyczyszczenie pixela
	}
	
	GLCD_WriteData(data);					// zapis danych do LCD
}

// Funkcja wyswietlania grafiki -------------------------------------------------------------



// Funkcje obs³ugi czcionek  ----------------------------------------------------------------

uint8_t GLCD_ReadFontData(const uint8_t* ptr) {
	return pgm_read_byte(ptr);
}

void GLCD_SelectFont(const uint8_t* font, glcd_FontCallback callback, uint8_t color) {
	glcd_Font = font;
	glcd_FontRead = callback;
	glcd_FontColor = color;
}

int GLCD_PutChar(unsigned char c) {
       uint8_t width = 0;
       uint8_t height = glcd_FontRead(glcd_Font+FONT_HEIGHT);
       uint8_t bytes = (height+7)/8;
       uint8_t tooMuch = height % 8;
       
       uint8_t firstChar = glcd_FontRead(glcd_Font+FONT_FIRST_CHAR);
       uint8_t charCount = glcd_FontRead(glcd_Font+FONT_CHAR_COUNT);
       
       uint16_t index = 0;
       uint8_t x = glcd_Coord.x, y = glcd_Coord.y;
       
       if(c < firstChar || c >= (firstChar+charCount)) {
               return 1;
       }
       c-= firstChar;
       
       // odczytanie wielkoci i indexu
       for(uint8_t i=0; i<c; i++)
               index += glcd_FontRead(glcd_Font+FONT_WIDTH_TABLE+i);
       index = index*bytes+charCount+FONT_WIDTH_TABLE;
       width = glcd_FontRead(glcd_Font+FONT_WIDTH_TABLE+c);
       

       for(uint8_t i=0; i<bytes; i++)
       {
               uint8_t page = i*width;
               for(uint8_t j=0; j<width; j++)
               {
                       uint8_t data = glcd_FontRead(glcd_Font+index+page+j);
                       
                       if (i + 1 == bytes)
                       {
                               uint8_t pageOld = (i - 1) * width;
                               uint8_t dataOld = glcd_FontRead(glcd_Font+index+pageOld+j); //pobranie ostatniego bajtu
                               dataOld >>= tooMuch;
                               data |= dataOld;
                       }
                       
                       if(glcd_FontColor == Cz)
                       {
                               GLCD_WriteData(data);
                       } else
                       {
                               GLCD_WriteData(~data);
                       }
               }
               // 1 pixel odstêpu
               if(glcd_FontColor == Cz)
               {
                       GLCD_WriteData(0x00);
               }
               else
               {
                       GLCD_WriteData(0xFF);
               }
               if (i + 2 < bytes)
                       GLCD_GotoXY(x, glcd_Coord.y+8);
               else
                       GLCD_GotoXY(x, glcd_Coord.y + tooMuch);
       }
       GLCD_GotoXY(x+width+1, y);
       
       return 0;
}

void GLCD_Puts(char* str) {
	int x = glcd_Coord.x;
	while(*str != 0) {
		if(*str == '\n') {
			GLCD_GotoXY(x, glcd_Coord.y+glcd_FontRead(glcd_Font+FONT_HEIGHT));
		} else {
			GLCD_PutChar(*str);
		}
		str++;
	}
}

void GLCD_Puts_P(PGM_P str) {
	int x = glcd_Coord.x;
	while(pgm_read_byte(str) != 0) {
		if(pgm_read_byte(str) == '\n') {
			GLCD_GotoXY(x, glcd_Coord.y+glcd_FontRead(glcd_Font+FONT_HEIGHT));
		} else {
			GLCD_PutChar(pgm_read_byte(str));
		}
		str++;
	}
}

uint8_t GLCD_CharWidth(char c) {
	uint8_t width = 0;
	uint8_t firstChar = glcd_FontRead(glcd_Font+FONT_FIRST_CHAR);
	uint8_t charCount = glcd_FontRead(glcd_Font+FONT_CHAR_COUNT);
	
	// odczyt danych o rozmiarze
	if(c >= firstChar && c < (firstChar+charCount)) {
		c -= firstChar;
		width = glcd_FontRead(glcd_Font+FONT_WIDTH_TABLE+c)+1;
	}
	
	return width;
}

uint16_t GLCD_StringWidth(char* str) {
	uint16_t width = 0;
	
	while(*str != 0) {
		width += GLCD_CharWidth(*str++);
	}
	
	return width;
}

uint16_t GLCD_StringWidth_P(PGM_P str) {
	uint16_t width = 0;
	
	while(pgm_read_byte(str) != 0) {
		width += GLCD_CharWidth(pgm_read_byte(str++));
	}
	
	return width;
}



// FUNKCJA GLDC_int_uniwers   --- dodane przez jachu...

void GLCD_IntUni(int liczba, uint8_t system)
{
        system = 16; //10 - dziesietny , 2 - binarny , 16 - hex
        char asc_buf[17];
        itoa(liczba,asc_buf,system);
        GLCD_Puts(itoa(liczba,asc_buf,system));
        return;
}
// Funkcje steruj¹ce  -----------------------------------------------------------------

void GLCD_GotoXY(uint8_t x, uint8_t y) {
	uint8_t chip = CHIP1, cmd;
	
	if(x > 127) x = 0;								// sprawdzenie koordynat
	if(y > 63)  y = 0;
	
	glcd_Coord.x = x;								// zapis nowych koordynat
	glcd_Coord.y = y;
	glcd_Coord.page = y/8;
	
	if(x >= 64) {									// wybranie prawej po³owy
		x -= 64;
		chip = CHIP2;
	}
	cmd = LCD_SET_ADD | x;
	GLCD_WriteCommand(cmd, chip);					// ustawienie adresu X dla wybranego uk³adu
	
	cmd = LCD_SET_PAGE | glcd_Coord.page;			// ustawienie adresu Y dla obu uk³adów
	GLCD_WriteCommand(cmd, CHIP1);
	GLCD_WriteCommand(cmd, CHIP2);
}

void GLCD_Init(uint8_t invert) {
	glcd_Coord.x = 0;
	glcd_Coord.y = 0;
	glcd_Coord.page = 0;
	
	glcd_Inverted = invert;
	
	LCD_CMD_DIR = 0xFF;								// Port Komend jako wyjcie
	GLCD_WriteCommand(LCD_ON, CHIP1);				// W³aczenie
	GLCD_WriteCommand(LCD_ON, CHIP2);
	
	GLCD_WriteCommand(LCD_DISP_START, CHIP1);		// startowa linia  = 0
	GLCD_WriteCommand(LCD_DISP_START, CHIP2);
	GLCD_ClearScreen();								// czyszczenie wyswietlacza
	GLCD_GotoXY(0,0);
}

inline void GLCD_Enable(void) {
	LCD_CMD_PORT |= 0x01 << EN;						// EN w stan niski: min. 450ns
	asm volatile("nop\n\t"
				 "nop\n\t"
				 "nop\n\t"
				 ::);
	LCD_CMD_PORT &= ~(0x01 << EN);
	for(volatile uint8_t i=0; i<8; i++);			// ma³y delaj na odczytanie flagi
}

uint8_t GLCD_DoReadData(uint8_t first) {
	uint8_t data;
	volatile uint8_t i;
	
	LCD_DATA_OUT = 0x00;
	LCD_DATA_DIR = 0x00;							// dport danych jako wejcie
	
	if(glcd_Coord.x < 64) {
		LCD_CMD_PORT &= ~(0x01 << CSEL2);			// zrezygnuj z  chip 2
		LCD_CMD_PORT |= 0x01 << CSEL1;				// wybierz chip 1
	} else if(glcd_Coord.x >= 64) {
		LCD_CMD_PORT &= ~(0x01 << CSEL1);			// zrezygnuj z chip 1
		LCD_CMD_PORT |= 0x01 << CSEL2;				// wybierz chip 2
	}
	if(glcd_Coord.x == 64 && first) {				// X-adres = 0 dla chip2
		GLCD_WriteCommand(LCD_SET_ADD, CHIP2); 	    // hau hau ..... hihihi
	}
	
	LCD_CMD_PORT |= 0x01 << D_I;					// D/I = 1
	LCD_CMD_PORT |= 0x01 << R_W;					// R/W = 1
	
	LCD_CMD_PORT |= 0x01 << EN;						// EN stan wysoki: min. 450ns
	asm volatile("nop\n\t"
				 "nop\n\t"
				 "nop\n\t"
				 ::);
	
	data = LCD_DATA_IN;								// odczyt Danych
	
	LCD_CMD_PORT &= ~(0x01 << EN);
	for(i=0; i<8; i++);								// ma³y delaj do odczytania flagi
	
	LCD_DATA_DIR = 0xFF;
	
	GLCD_GotoXY(glcd_Coord.x, glcd_Coord.y);
	
	if(glcd_Inverted)
		data = ~data;
	return data;
}

uint8_t GLCD_ReadData(void) {
	GLCD_DoReadData(1);							    // dummy read
	return GLCD_DoReadData(0);						// "real" read
}

void GLCD_WriteCommand(uint8_t cmd, uint8_t chip) {
	if(chip == CHIP1) {
		LCD_CMD_PORT &= ~(0x01 << CSEL2);			// zrezygnuj z chip 2
		LCD_CMD_PORT |= 0x01 << CSEL1;				// wybierz chip 1
	} else if(chip == CHIP2) {
		LCD_CMD_PORT &= ~(0x01 << CSEL1);			// zrezygnuj z chip 1
		LCD_CMD_PORT |= 0x01 << CSEL2;				// wybierz chip 2
	}
	
	LCD_CMD_PORT &= ~(0x01 << D_I);					// D/I = 0
	LCD_CMD_PORT &= ~(0x01 << R_W);					// R/W = 0
	LCD_DATA_DIR = 0xFF;							// port danych = wejcie
	LCD_DATA_OUT = cmd;								// zapis komendy
	GLCD_Enable();									// w³aczenie
	LCD_DATA_OUT = 0x00;
}

void GLCD_WriteData(uint8_t data) {
	uint8_t displayData, yOffset, cmdPort;

#ifdef DEBUG
	volatile uint16_t i;
	for(i=0; i<5000; i++);
#endif

	if(glcd_Coord.x >= 128)
		return;

	if(glcd_Coord.x < 64) {
		LCD_CMD_PORT &= ~(0x01 << CSEL2);			// zrezygnuj z  chip 2
		LCD_CMD_PORT |= 0x01 << CSEL1;				// wybierz chip 1
	} else if(glcd_Coord.x >= 64) {
		LCD_CMD_PORT &= ~(0x01 << CSEL1);			// zrezygnuj z chip 1
		LCD_CMD_PORT |= 0x01 << CSEL2;				// wybierz chip 2
	}
	if(glcd_Coord.x == 64)							// X-adres = 0 dla Chip2
		GLCD_WriteCommand(LCD_SET_ADD, CHIP2);
	
	LCD_CMD_PORT |= 0x01 << D_I;					// D/I = 1
	LCD_CMD_PORT &= ~(0x01 << R_W);					// R/W = 0
	LCD_DATA_DIR = 0xFF;							// data port is output
	
	
	yOffset = glcd_Coord.y%8;
	if(yOffset != 0) {
		// pierwsza po³owa LCD -------------------------------------------------
		cmdPort = LCD_CMD_PORT;						// zapisz rozkaz do portu
		displayData = GLCD_ReadData();
		
		LCD_CMD_PORT = cmdPort;						// przywróc stan portu
		LCD_DATA_DIR = 0xFF;						// port danych jako wyjcie
		
		displayData |= data << yOffset;
		if(glcd_Inverted)
			displayData = ~displayData;
		LCD_DATA_OUT = displayData;					// zapis danych
		GLCD_Enable();								// w³¹cz
		
		// druga po³owa LCD ---------------------------------------------------
		GLCD_GotoXY(glcd_Coord.x, glcd_Coord.y+8);
		
		displayData = GLCD_ReadData();
		
		LCD_CMD_PORT = cmdPort;						// przywróc stan portu
		LCD_DATA_DIR = 0xFF;						// port danych jako wyjcie
		
		displayData |= data >> (8-yOffset);
		if(glcd_Inverted)
			displayData = ~displayData;
		LCD_DATA_OUT = displayData;					// zapis danych
		GLCD_Enable();								// w³¹cz
		
		GLCD_GotoXY(glcd_Coord.x+1, glcd_Coord.y-8);
	} else {
		if(glcd_Inverted)
			data = ~data;
		LCD_DATA_OUT = data;						// zapis danych
		GLCD_Enable();								// w³¹cz
		glcd_Coord.x++;
	}
	LCD_DATA_OUT = 0x00;
}
