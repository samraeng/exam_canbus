
#include <16F877A.h> 
#fuses HS,NOPROTECT,NOLVP,NOWDT 
#use delay(clock=20000000) 
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7) 

#define CAN_DO_DEBUG TRUE 

#define EXT_CAN_CS   PIN_D1 
#define EXT_CAN_SI   PIN_C4 
#define EXT_CAN_SO   PIN_C5 
#define EXT_CAN_SCK  PIN_C3 

#include <can-mcp251x.h> 
#include <can-mcp251x_sam.c> 

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

#define RESPOND_TO_ID_AD   0x201 
#define RESPOND_TO_ID_LED  0x202 

void main() 
{ 
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

      if ( can_kbhit() )   //if data is waiting in buffer... 
      { 
         if(can_getd(rx_id, &buffer[0], rx_len, rxstat))   //...then get data from buffer 
       {                    
            if (rx_id == RESPOND_TO_ID_LED) 
         { 
               printf("Chaning LEDs\r\n\r\n"); 
               if (bit_test(buffer[0],0)) {LED1_HIGH;} else {LED1_LOW;} 
               if (bit_test(buffer[0],1)) {LED2_HIGH;} else {LED2_LOW;} 
               if (bit_test(buffer[0],2)) {LED3_HIGH;} else {LED3_LOW;} 
            } 
            if (rx_id == RESPOND_TO_ID_AD) 
         { 
               i=read_adc(); 
               printf("Sending AD reading: %X\r\n\r\n",i); 
               can_putd(RESPOND_TO_ID_AD, &i, 1,1,1,0); //put data on transmit buffer 
            } 
         } 
      } 
   } 
}
