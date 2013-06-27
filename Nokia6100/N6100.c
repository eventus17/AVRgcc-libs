/*
 * Biblioteka obs³uguj¹ca LCD z Noki 6100
 * Rozdzielczosc - 132x132 pixele 4096 kolorów
 *
 * Autor : SunRiver
 * Date  : 03.01.2013
 * Wersja: 0.01 beta
 *
 *

=======  POD£¥CZENIE LCD ===========================================================================
=
=        LCD_CS           PORTB4
=        LCD_CLK          PORTB7
=        LCD_DATA         PORTB5
=        LCD_RESET        PORTB1
=        LCD_BACKLIGHT    +5V
=
====================================================================================================

*/

#include <avr/pgmspace.h>
#include <avr/io.h>
#include "N6100.h"
#include "FONT8x8.h"


// DELAYE -------------------------------------------------------------------------------------------

inline void delayloop16(uint16_t count)
{
	asm volatile (  "cp  %A0,__zero_reg__ \n\t"  \
                     "cpc %B0,__zero_reg__ \n\t"  \
                     "breq L_Exit_%=       \n\t"  \
                     "L_LOOP_%=:           \n\t"  \
                     "sbiw %0,1            \n\t"  \
                     "brne L_LOOP_%=       \n\t"  \
                     "L_Exit_%=:           \n\t"  \
                     : "=w" (count)
					 : "0"  (count)
                   );                            
}
void delayloop32(uint32_t loops) 
{
  __asm__ volatile ( "cp  %A0,__zero_reg__ \n\t"  \
                     "cpc %B0,__zero_reg__ \n\t"  \
                     "cpc %C0,__zero_reg__ \n\t"  \
                     "cpc %D0,__zero_reg__ \n\t"  \
                     "breq L_Exit_%=       \n\t"  \
                     "L_LOOP_%=:           \n\t"  \
                     "subi %A0,1           \n\t"  \
                     "sbci %B0,0           \n\t"  \
                     "sbci %C0,0           \n\t"  \
                     "sbci %D0,0           \n\t"  \
                     "brne L_LOOP_%=            \n\t"  \
                     "L_Exit_%=:           \n\t"  \
                     : "=w" (loops)              \
					 : "0"  (loops)              \
                   );                             \
    
	return;
}

// Funkcje steruj¹ce --------------------------------------------------------------------------------

void LCD_Init(void)
{


    LCD_CS(1);
    LCD_CLK(0);
    LCD_DATA(0);

    LCD_RESET(1);
    delay_ms(50);
    LCD_RESET(0);
    delay_ms(50);
    LCD_RESET(1);
    delay_ms(50);

    LCD_CS(1);
    LCD_CLK(1);
    LCD_DATA(1);
    delay_ms(10);

    SendLcd(LCDCommand,SWRESET);
    delay_ms(10);

    SendLcd(LCDCommand,SLEEPOUT);     // Wyjcie z upienia
    SendLcd(LCDCommand,NORON);        // Tryb inversji wywietlania
    SendLcd(LCDCommand,BSTRON);       // BoostON
	SendLcd(LCDCommand,MADCTL);       // MAC (Memory Access Control) - odwrocenie
    SendLcd(LCDData,0xF0);            // Nowy LCD
	SendLcd(LCDCommand,SETCON);       // Ustawienie kontrastu
    SendLcd(LCDData,0x3a);
	delay_ms(10);
    SendLcd(LCDCommand,DISPON);       // W³aczenie wywietlacza
    SendLcd(LCDCommand,COLMOD);       // Ustawienie trybu kolorów

  	SendLcd(LCDData,0x03);			  // 12bitów na pixel
	SendLcd(LCDCommand,NOP);          // Ustawienie trybu kolorów

}
void SendLcd_color(unsigned char color){
  
    LCD_DATA(LCDData);                  // Ustawienie 1 bitu zale¿nie dla data i command

    LCD_CLK(0);							// Pull Clock LOW
    LCD_CLK(1);							// Pull Clock HIGH
    
    LCD_CLK(0);
    SPCR |=0x50;						// W³aczenie sprzêtowego SPI
    SPSR |=0x1;

    SPDR = color; 						// Wys³anie danych
    
    while(!(SPSR & 0x80));				// oczekiwanie na zakoñczenie wysy³ania

	// ==================================  Wy³aczenie CS

    SPCR &=~0x50;						// Wy³aczenie SPI
   
	LCD_CLK(0);						    // Zwalniamy I/O dla innych urzadzeñ
}
void SendLcd(unsigned char type, unsigned char dat) 
{
	char i;

    LCD_DATA(type);                     // Ustawienie 1 bitu dla data lub command
    LCD_CS(0);							// W³aczenie LCD - CS

    LCD_CLK(0);							// Pull Clock LOW
    LCD_CLK(1);							// Pul Clock HIGH
    
    if(dat == 0x0){       				// SPI nie moze przes³ac zera ??
      LCD_DATA(0);
      for(i=0; i<8; i++){
      
        LCD_CLK(0);						// Pull Clock LOW
        LCD_CLK(1);	
      }
    }
    else{
    SPCR |=0x50;						// W³aczenie sprzêtowego SPI
    SPSR |= 0x1;
    SPDR = dat; 						// Wys³anie danych
    
    while(!(SPSR & 0x80));				// czekanie na zakonczenie wysy³ania

    }

    SPCR &=~0x50;						// Wy³aczenie SPI i zwolnienie pinów dla innych urz¹dzeñ

      LCD_CS(1);					    // Wy³aczenie LCD -- CS
}

// Funkcje graficzne --------------------------------------------------------------------------------

void LCD_GotoXY(unsigned char x, unsigned char y)
{
  SendLcd(LCDCommand, PASET);                         // ³adowanie strony do ram
  SendLcd(LCDData,x);                                 // Start wyswietlania strony
  SendLcd(LCDData,x);                                 // Koniec wyswietlania strony
  
  SendLcd(LCDCommand, CASET);                         // Kolumny w RAM start/end
  SendLcd(LCDData,y);                                 // Start Wyswitlania kolumn
  SendLcd(LCDData,y);                                 // koniec wyswietlania kolumn
}
void LCD_Pixel(unsigned char x, unsigned char y, int color)
{
	  LCD_GotoXY(x,y);
  	SendLcd(LCDCommand,RAMWR);                             
        LCD_CS(0);
  	SendLcd_color( (unsigned char)((color>>4)& 0xff) );    
  	SendLcd_color( (unsigned char)((color&0xf)<<4)|0x00 );
        LCD_CS(1);
  	SendLcd(LCDCommand, NOP);
}
void LCD_Line(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, int color) { 
 
  int dy = y1 - y0; 
  int dx = x1 - x0; 
  int stepx, stepy; 
	if (dy < 0) { dy = -dy;  stepy = -1; } else { stepy = 1; } 
  if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; } 
 
  dy <<= 1;        // dy jest teraz 2*dy
  dx <<= 1;        // dx jest teraz 2*dx
 
 
  LCD_Pixel(x0, y0, color); 
 
  if (dx > dy) { 
 		int fraction = dy - (dx >> 1);  // tak samo jak dla 2*dy - dx
    while (x0 != x1) { 
 	    if (fraction >= 0) { 
 				y0 += stepy; 
        fraction -= dx;    // tak samo jak frakcja -= 2*dx
 			} 
 			x0 += stepx; 
 
      fraction += dy;    // tak samo jak frakcja -= 2*dy
      LCD_Pixel(x0, y0, color); 
    } 
 	} 
 	else { 
 		int fraction = dx - (dy >> 1); 
    while (y0 != y1) { 
 			if (fraction >= 0) { 
 				x0 += stepx; 
 				fraction -= dy; 
      } 
      y0 += stepy; 
 			fraction += dx; 
      LCD_Pixel(x0, y0, color); 
    } 
 	} 
 
} 
void LCD_Box(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char fill, int color) { 
  unsigned char   xmin, xmax, ymin, ymax; 
	int   i; 
 
 // Sprawdzenie czy prostok¹t ma byc wype³niony
  if (fill == FILL) { 
 
    // Najlepszym sposobem, aby utworzyæ wype³niony prostok¹t jest zdefiniowanie ramki
	//  i pêtli dwóch pikseli na raz oraz obliczyæ min i max dla kierunku x i y
        xmin = (x0 <= x1) ? x0 : x1;
 		xmax = (x0 > x1) ? x0 : x1; 
		ymin = (y0 <= y1) ? y0 : y1; 
		ymax = (y0 > y1) ? y0 : y1; 
 
		// okreœlamy pole rysowaniaa zgodnie z limitami
		// Ustawienie adresu wierszy  (command 0x2B)
		SendLcd(LCDCommand,PASET); 
		SendLcd(LCDData,xmin); 
		SendLcd(LCDData, xmax); 
		// Ustawienie adresu kolumn  (command 0x2A)
		SendLcd(LCDCommand,CASET); 
		SendLcd(LCDData,ymin); 
		SendLcd(LCDData,ymax); 
 
		// Zapis do pamieci
		SendLcd(LCDCommand,RAMWR); 
                
               
		// pêtla na ca³kowit¹ liczbe pixeli/2
		for (i = 0; i < ((((xmax - xmin + 1) * (ymax - ymin + 1))/2 )+1 ); i++) { 
                  
  		 // u¿yjemy wartoœci koloru do wyprowadzania trzech bajtów danych obejmuj¹cych dwa piksele

          LCD_CS(0);
  		  SendLcd_color((color >> 4) & 0xFF ); 
  		  SendLcd_color(((color & 0xF) << 4) | ((color >> 8) & 0xF) ); 
 		  SendLcd_color(color & 0xFF);
          LCD_CS(1);
		} 
              
                                  
           SendLcd(LCDCommand,NOP); 
	} 
    else { 

  // Najlepszym sposobem, aby wywietlic pusty prostok¹t jest narysowanie czterch linii :P
 	 	LCD_Line(x0, y0, x1, y0, color); 
 	 	LCD_Line(x0, y1, x1, y1, color); 
 	 	LCD_Line(x0, y0, x0, y1, color); 
 	 	LCD_Line(x1, y0, x1, y1, color); 
  }
}
void LCD_Circle(unsigned char x0, unsigned char y0, unsigned char radius, int color) 
{ 
 
  int f = 1 - radius; 
 
  int ddF_x = 0; 
 
  int ddF_y = -2 * radius; 
  unsigned char x = 0; 
 
  unsigned char y = radius; 
 
  
 
  LCD_Pixel(x0, y0 + radius, color); 
  LCD_Pixel(x0, y0 - radius, color); 
 
  LCD_Pixel(x0 + radius, y0, color); 
 
  LCD_Pixel(x0 - radius, y0, color); 
  
 
  while (x < y) { 
 
  if (f >= 0) { 
 
   y--; 
   ddF_y += 2; 
 
   f += ddF_y; 
 
  } 
 
  x++; 
  ddF_x += 2; 
 
    f += ddF_x + 1;     
    LCD_Pixel(x0 + x, y0 + y, color); 
 
    LCD_Pixel(x0 - x, y0 + y, color); 
    LCD_Pixel(x0 + x, y0 - y, color); 
 
    LCD_Pixel(x0 - x, y0 - y, color); 
 
    LCD_Pixel(x0 + y, y0 + x, color); 
 
    LCD_Pixel(x0 - y, y0 + x, color); 
    LCD_Pixel(x0 + y, y0 - x, color); 
 
    LCD_Pixel(x0 - y, y0 - x, color); 

 } 
}

// Funkcje czcionek i obrazów -----------------------------------------------------------------------

void LCD_Char(char c, unsigned char x, unsigned char y, int fColor, int bColor) { 
 
 int    i,j; 
 
 unsigned char   nCols; 
 
 unsigned char  nRows; 
 
 unsigned char  nBytes; 
 unsigned char   PixelRow; 
 
 unsigned char   Mask; 
 
 unsigned int   Word0; 
 
 
 unsigned char *pFont,   *pChar; 
  
  pFont = (unsigned char *)FONT8x8;   
 
  
 
  // pobieranie nColumns, nRows i nBytes
  nCols = pgm_read_byte(pFont); 
 
  nRows = pgm_read_byte(pFont + 1); 
 
  nBytes = pgm_read_byte(pFont + 2); 
   
  // uzyskanie wskaŸnika do ostatniego bajtu odpowiedniego znaku ...
 
  pChar = pFont + (nBytes * (c - 0x1F)); 

  // Pêtla w ka¿dym rzêdzie, praca odty³u czyli od do³u do góry
 
  for (i = 0; i<nRows; i++) { 
 
   
    // pobranie wiersza pikseli z czcionki w tabeli, a nastêpnie dekrementacja wiersza
 
  PixelRow = pgm_read_byte(pChar++); 
  
 
  Mask = 0x80; 
 
    for (j = 0; j < nCols; j += 1) { 

        if ((PixelRow & Mask) == 0)
        Word0 = bColor;
 
        else
 
    Word0 = fColor; 
    
    LCD_Pixel( y+j,x+i, Word0);

   Mask = Mask >> 1; 
  }   
 } 
 
  // Zakoñczenie polecenia zapisu do pamieci
 SendLcd(LCDCommand,NOP);   
} 
void LCD_String(char *pString, unsigned char x, unsigned char  y,  int fColor, int bColor) { 

  while (*pString != 0x00) { 
 
 
    // Wyswietlenie znaku
 
    LCD_Char(*pString++, x, y, fColor, bColor);  
 
 
    // przesuniêcie po³o¿enia Y
 
     y=y+8;
    if (x > 131) break; 
 
 } 
} 
void LCD_Bitmap (unsigned char start_x, unsigned char start_y, unsigned char h_size, unsigned char v_size, unsigned char *bitmap_data) 
{
	int i;
	unsigned char *pBitmap;
	// okrelamy pole rysowania zgodnie z limitami rozdzielczoci
		// Ustawienie Adresu wierszy  (command 0x2B)
		SendLcd(LCDCommand,PASET); 
		SendLcd(LCDData,start_x); 
		SendLcd(LCDData, start_x+h_size-1); 
		// Ustawienie adresu column (command 0x2A)
		SendLcd(LCDCommand,CASET); 
		SendLcd(LCDData,start_y); 
		SendLcd(LCDData,start_y+v_size-1); 
 
		// Zapis do pamieci
		SendLcd(LCDCommand,RAMWR); 

	pBitmap = bitmap_data;                
               
		// pêtla dla ca³kowitej iloci pixeli / 2
		for (i = 0; i< (h_size*v_size)>>1  ; i++) { 
			unsigned char bitmap;                  

        LCD_CS(0);
        
        bitmap = pgm_read_byte(pBitmap++);
  		  SendLcd_color(bitmap);
  		  bitmap =pgm_read_byte(pBitmap++);
  		  SendLcd_color(bitmap); 
  		  bitmap =pgm_read_byte(pBitmap++);
  		  SendLcd_color(bitmap); 
  		  LCD_CS(1);
		} 
    SendLcd(LCDCommand,NOP); 
}


