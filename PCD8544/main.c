/*
 * main.c
 *
 *  Created on: 13-01-2013
 *      Author: SunRiver
 *   Toolchain: Atmel GNU 3.4.0_663
 *    Compiler: avr-gcc 4.6.2
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>


#include "pcd8544.h"  // --- biblioteka wyswietlacza
#include "bmp.h"      // --- zawiera bitmape


int main()
{
    PCD_Ini();  // inicjacja LCD


    //--- wywietlamy obrazek

    PCD_Contr(0x7F);  // kontrast
    PCD_Clr();
    PCD_Img(waitImage);  // bitmapa
    PCD_Upd();


    _delay_ms(2500);
    PCD_Contr(0x00); // kontrast
    PCD_Upd();       // update
    _delay_ms(2500);

    // --- wywietlanie stringów
    // Maksymalnie uzyskamy 6 wierszy po 14 znaków

    PCD_Clr();
    PCD_GotoXYFont(1,1);
    PCD_FStr(FONT_1X,(unsigned char*)PSTR("    WITAM !   "));
    PCD_GotoXYFont(1,2);
    PCD_FStr(FONT_1X,(unsigned char*)PSTR("forum.atnel.pl"));
    PCD_GotoXYFont(1,3);
    PCD_FStr(FONT_1X,(unsigned char*)PSTR("--------------"));
    //PCD_SBar ( 60, 40, 5, 12, 1);
    //PCD_SBar ( 20, 40, 5, 12, 1);
    PCD_Upd();

    _delay_ms(5000);


    return 0;
}
