
#include <16F877A.h> 
#fuses HS,NOPROTECT,NOLVP,NOWDT 
#use delay(clock=20000000) 
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7) 

#define CAN_DO_DEBUG TRUE 

#define EXT_CAN_CS   PIN_D1 
#define EXT_CAN_SI   PIN_C4 
#define EXT_CAN_SO   PIN_C5 
#define EXT_CAN_SCK  PIN_C3 

#include <can-mcp251x.H> 
#include <can-mcp251x_sam.c> 

#define BUTTON    PIN_A4 

#define BUTTON_PRESSED  !input(BUTTON) 

#define PIN_LED1  PIN_A1 
#define PIN_LED2  PIN_A2 
#define PIN_LED3  PIN_A3 

#define LED1_HIGH output_low(PIN_LED1) 
#define LED1_LOW  output_high(PIN_LED1) 
#define LED2_HIGH output_low(PIN_LED2) 
#define LED2_LOW  output_high(PIN_LED2) 
#define LED3_HIGH output_low(PIN_LED3) 
#define LED3_LOW  output_high(PIN_LED3) 

int16 ms; 

#int_timer2 
void isr_timer2(void) { 
   ms++; //keep a running timer that increments every milli-second 
} 

#define ASK_FOR_ID_AD_B      0x201  //ask for AD info from CAN port B 
#define SET_LED_ID_B         0x202  //set LEDs for CAN port B 

void main() 
{ 
   int b_leds=0; 
  
   struct rx_stat rxstat; 
    
   int32 rx_id; 
   int buffer[8]; 
   int rx_len; 

   int i; 

   setup_port_a(RA0_ANALOG); 
   setup_adc(ADC_CLOCK_INTERNAL); 
   set_adc_channel(0); 

   for(i=0;i<8;i++) { 
      buffer[i]=0; 
   } 

   LED1_HIGH; 
   LED2_HIGH; 
   LED3_HIGH; 
   printf("\r\n\r\nCCS CAN EXAMPLE\r\n"); 
   delay_ms(1000); 
   LED1_LOW; 
   LED2_LOW; 
   LED3_LOW; 

   setup_timer_2(T2_DIV_BY_4,53,3); 

   can_init(); 

   enable_interrupts(INT_TIMER2);   //enable timer2 interrupt 
   enable_interrupts(GLOBAL);       //enable all interrupts (else timer2 wont happen) 

   printf("\r\nRunning..."); 

   while(TRUE) 
   { 
      if ( can_kbhit() ) 
      { 
         printf("\r\n"); 
         if(can_getd(rx_id, &buffer[0], rx_len, rxstat)) { 
            if (rx_id == ASK_FOR_ID_AD_B) { 
               printf("Channel B AD: %X\r\n",buffer[0]); 
            } 
          
         } 
      } 

      if ( can_tbe() && (ms > 2000))       //every two seconds, send new data if transmit buffer is empty 
      { 
         ms=0; 

         //change leds on port b 
         printf("\r\n\r\nSet LEDs on Port B to %U",b_leds); 
         can_putd(SET_LED_ID_B, &b_leds, 1, 1, 1, 0); 
         b_leds++; 
         if (b_leds > 7) {b_leds=0;} 
      } 

      
   } 
}
