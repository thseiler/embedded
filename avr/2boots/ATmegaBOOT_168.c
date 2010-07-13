

/* $Id$ */


/* some includes */
/* some includes */
#include <inttypes.h>
#include <avr/io.h>
#include "stk500v1.h"
#include "mmc_fat.h"

/* function prototypes */
void main (void) __attribute__ ((naked,section (".init9")));

/* some variables */
void (*app_start)(void) = 0x0000;

/* main program starts here */
void main(void)
{	
	asm volatile ( "clr __zero_reg__" );
	SP=RAMEND;

	/* here we learn how we were reset */
	uint8_t reset_reason = MCUSR;
	MCUSR = 0;
	
	WDTCSR |= _BV(WDCE) | _BV(WDE);
	WDTCSR = 0;
	
	/* if we had power up or watchdog, start the app immediately */
	if (! (reset_reason & _BV(EXTRF))) app_start();

	/* warning: this means that the rest of the bootloader only runs,  */
	/*          if an external reset was triggered!                    */

	/* set PWR pin + LED pin + SERIAL pin as output */
	//DDRC |= 0x0F;

    /* ensure we stay on */
	//PWR_PORT |= _BV(PWR);
	
    // quickstart-test, see if up/down buttons are pressed
	//DDRD &= ~_BV(PIND5); 
	//DDRD &= ~_BV(PIND6);
	//PORTD |= _BV(PIND5);
	//PORTD |= _BV(PIND6);
		
	//uint8_t i;
	//for(i = 0;i<255;i++) { i = i; } // quick delay...
		
	// continue only to bootloader, if BEND and UNBEND is pressed together
	//if (bit_is_set(PIND, PIND5) || bit_is_set(PIND,PIND6)) app_start();


	mmc_updater();
	stk500v1();
		
	/* reset via watchdog */
	WDTCSR = _BV(WDE);
	while (1); // 16 ms
}



/* end of file ATmegaBOOT.c */