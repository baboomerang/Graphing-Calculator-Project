ISR( WDT_vect ) {
  EEPROM.write(2, 99);  //set memory error flag
}

void wdt_setup(void) {
  cli(); // disable all interrupts
  wdt_reset(); // reset the WDT timer
  /* WDTCSR configuration:
     WDIE = 1: Interrupt Enable
     WDE = 1 :Reset Enable
     WDP3 = 0 :For 2000ms Time-out
     WDP2 = 1 :For 2000ms Time-out
     WDP1 = 1 :For 2000ms Time-out
     WDP0 = 1 :For 2000ms Time-out */
  // Enter Watchdog Configuration mode:
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  // Set Watchdog settings:
  WDTCSR = (1 << WDIE) | (1 << WDE) | (0 << WDP3) | (0 << WDP2) | (1 << WDP1) | (1 << WDP0);
  sei();
}
