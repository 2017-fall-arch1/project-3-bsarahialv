#include <msp430.h>
#include "buzzer.h"


void decisecond() 
{
  static char cnt = 0;		/* # deciseconds/frequecy change */
  if (++cnt > 1) {		
    buzzer_advance_frequency();
    cnt = 0;
  }
}

void
__interrupt_vec(WDT_VECTOR) WDT(){	/* 250 interrupts/sec */
  static char second_count = 0, decisecond_count = 0;
  if (++second_count == 3) {
    //led_toggle();
    second_count = 0;
  }
  if (++decisecond_count == 3) {
    buzzer_advance_frequency();
    decisecond_count = 0;
  }
}
