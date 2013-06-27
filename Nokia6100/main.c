/*
 * main.c
 *
 *  Created on: 03-01-2013
 *      Author: SunRiver
 *         MCU: 
 */

#include <avr/pgmspace.h>
#include <avr/io.h>
#include "N6100.h"
//#include "FONT8x8.h"
//#include "image.h"


int main (void)
{
//unsigned char *pbitmap;
//char image_h, image_w;

PORTB = 0x00;
DDRB  = 0xFF;

LCD_Init();
LCD_Box(0,0,131,131,FILL,WHITE);
LCD_Box(3,3,128,128,NOFILL,BLUE);
LCD_Circle(65,100,10,GREEN);
LCD_Circle(65,110,10,RED);

LCD_String("===============", 22, 6, BLACK, WHITE);
LCD_String("  POZDRAWIAM  ", 42, 6, GREEN, WHITE);
LCD_String("forum.atnel.pl", 52, 6, BLUE, WHITE);
LCD_String("   SunRiver   ", 72, 6, PINK, WHITE);
LCD_String("===============", 82, 6, BLACK, WHITE);
//pbitmap = image;
//image_w = pgm_read_byte(pbitmap+1);
//image_h = pgm_read_byte(pbitmap+2);
//LCD_Bitmap(30, 70, image_w, image_h, pbitmap+5);
}
