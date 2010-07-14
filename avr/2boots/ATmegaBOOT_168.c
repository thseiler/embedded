/* $Id$ */

/* some includes */
/* some includes */
#include <inttypes.h>
#include <avr/io.h>
#include "stk500v1.h"
#include "mmc_fat.h"

/* function prototypes */
void main (void) __attribute__ ((naked,section (".init9")));
static inline char bootloader_skip_condition();

/* some variables */
void (*app_start)(void) = 0x0000;

/* main program starts here */
void main(void)
{	
	/* this is needed because of the __attribute__ naked, section .init 9 */
	asm volatile ( "clr __zero_reg__" );
	SP=RAMEND;
	
	/* init watchdog */
	WDTCSR |= _BV(WDCE) | _BV(WDE);
	WDTCSR = 0;
	
	/* test if we should skip to the app immediately... */
	if (bootloader_skip_condition()) app_start();

	/* warning: this means that the rest of the bootloader only runs,  */
	/*          if the bootloader_skip_condition returnd false !!!     */

	/* lets first try the mmc */
	mmc_updater();
	
	/* the fall back to serial */
	stk500v1();
		
	/* reset via watchdog */
	WDTCSR = _BV(WDE);
	while (1); // 16 ms
}

/* here is a collection of conditions as to when to enter bootloader */

#ifdef CONDITION_VOLTAGE

static char bootloader_skip_condition() {
    unsigned char high, low;
    unsigned short voltage;

    // enable adc
    PRR &= ~_BV(PRADC);
    ADMUX = _BV(REFS0)  | _BV(REFS1) | 0x06;
    ADCSRA |= _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADSC); // 1/64
    loop_until_bit_is_clear(ADCSRA, ADSC);

    low = ADCL;
    high = ADCH;
    voltage = ((unsigned short)high << 8) | low;

    return (voltage > 292); 
}

#elif defined(CONDITION_FTTH_CONT_BUTTONS)

static char inline bootloader_skip_condition() {
    // quickstart-test, see if up/down buttons are pressed
	DDRD &= ~_BV(PIND5); /* input */
	DDRD &= ~_BV(PIND6);
	PORTD |= _BV(PIND5); /* pullup */
	PORTD |= _BV(PIND6);
		
	uint8_t i;
	for(i = 0;i<255;i++) { i = i; } // quick delay...
		
	// continue only to bootloader, if BEND and UNBEND is pressed together
	return (bit_is_set(PIND, PIND5) || bit_is_set(PIND,PIND6));
}

#else 

/* this is the default condition */
/* we only continue into the bootloader on an external reset */

static char inline bootloader_skip_condition() {
	uint8_t reset_reason = 0;

	/* here we learn how we were reset */
	reset_reason = MCUSR;
	MCUSR = 0;

	return ! (reset_reason & _BV(EXTRF));
}

#endif 



/* end of file ATmegaBOOT.c */