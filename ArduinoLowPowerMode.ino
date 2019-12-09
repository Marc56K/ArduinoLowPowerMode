#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>

volatile int pressed = 1;
volatile int count = 2;

ISR(WDT_vect){}

void update()
{
  count++;
  if (count % 2 != 0)
    return;

  count = 0;
  
  pinMode (LED_BUILTIN, OUTPUT);
  int loops = pressed;
  for (byte i = 0; i < loops; i++)
  {
    digitalWrite (LED_BUILTIN, HIGH);
    delay (500);
    digitalWrite (LED_BUILTIN, LOW);
    delay (500);
  }

  if (loops > 1)
    pressed = 1;
}

void handler0()
{
  pressed = 2;
}

void handler1()
{
  pressed = 3;
}

void setup ()
{
  pinMode (2, INPUT); 
  pinMode (3, INPUT); 
  attachInterrupt(digitalPinToInterrupt(2), handler0, RISING);
  attachInterrupt(digitalPinToInterrupt(3), handler1, RISING);
}

void loop () 
{ 
    wdt_disable();
    update ();

    // disable ADC
    ADCSRA = 0;  

    /* Setup des Watchdog Timers */
    MCUSR &= ~(1<<WDRF);             /* WDT reset flag loeschen */
    WDTCSR |= (1<<WDCE) | (1<<WDE);  /* WDCE setzen, Zugriff auf Presclaler etc. */
    WDTCSR = 1<<WDP0 | 1<<WDP3;      /* Prescaler auf 8.0 s */
    WDTCSR |= 1<<WDIE;               /* WDT Interrupt freigeben */
    wdt_reset();  // pat the dog
    
    set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
    sleep_enable();
    power_adc_disable();    /* Analog-Eingaenge abschalten */
    power_spi_disable();    /* SPI abschalten */
    power_timer0_disable(); /* Timer0 abschalten */
    power_timer2_disable(); /* Timer0 abschalten */
    power_twi_disable();    /* TWI abschalten */
    
    //sleep_cpu ();  
    sleep_mode();
    // cancel sleep as a precaution
    sleep_disable();
    power_all_enable();     /* Komponenten wieder aktivieren */
}
