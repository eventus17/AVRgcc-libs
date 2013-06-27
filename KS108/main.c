// Dla Antystatycznego ---------------------
//
// SunRiver
// Test migania pixelem na ekranie LCD
// LCD 128x64 - sterownik KS108 lub zgodny
//
// Pixel 4x4 --- Blink Pixel
//

#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

// ------ Biblioteki prywatne -----------------
#include "GLCD_KS108.h"
#include "arial_bold_14.h"

#include <util/delay.h>

int main(void) {
	
	// troch� trzeba poszczekac na inicjacje sprzetow�
	for(volatile uint16_t i=0; i<15000; i++);
	
	// Imicjujemy LCD
	GLCD_Init(0);
	
	// wybieramy fonty
	GLCD_SelectFont(Arial_Bold_14, GLCD_ReadFontData, Cz);
	// ustawiamy pozycj�
	GLCD_GotoXY(15,10);
	// Wywietlamy napis
	GLCD_Puts_P(PSTR("Antystatyczny"));
	// rysujemy rameczk�
	GLCD_DrawRoundRect(5, 5, 117, 20, 8, Cz);



	while(1)
	{

        // zapalamy pixel 4x4
		GLCD_SetDot(63, 40, Cz);
		GLCD_SetDot(63, 41, Cz);
		GLCD_SetDot(62, 40, Cz);
		GLCD_SetDot(62, 41, Cz);
		_delay_ms(1000);
		// gasimy pixel 4x4
		GLCD_SetDot(63, 40, Bi);
		GLCD_SetDot(63, 41, Bi);
		GLCD_SetDot(62, 40, Bi);
		GLCD_SetDot(62, 41, Bi);
		_delay_ms(1000);

    }

}
