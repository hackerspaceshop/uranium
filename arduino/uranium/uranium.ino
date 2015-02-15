/*
* URANIUM
* a necklace consisting of an uranium marble, 6 UV leds (charlieplexed), a button and an attiny85 microcontroller
* this code was witten by florian 'overflo' bittner and is released in the public domain.
* you can get an uranium necklace at the http://hackerspaceshop.com/
*
* this project was inspired by daniel rojas whom i met at 31C3. thank you.
* http://www.danielrojas.net/index.php?page=engineering
*
* and his design was inspired by http://makezine.com/2009/12/28/ring-oscillator-pendant/
* i. love. the. internet.   so much inspiration.
*/

#include <avr/sleep.h>
#include <avr/io.h>

//clear bit macro
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

//set bit macro
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif







#define GATE1 2
#define GATE2 3
#define GATE3 4
#define BUTTON_PIN 0















int good_night=0;









void setup(){

//Set all pins to inputs w/ pull-up resistors (stop them from floating or sourcing power)
for (byte i=0; i<5; i++) {     
    pinMode(i, INPUT);
    digitalWrite(i, HIGH);
	}
	

  
  sbi(GIMSK,PCIE); // Turn on Pin Change interrupts (Tell Attiny85 we want to use pin change interrupts (can be any pin))
  sbi(PCMSK,PCINT0); //Define which pins are doing the interrupting (digital pin 1, DIP 7 in this example)
  
  cbi(ADCSRA,ADEN); //Switch Analog to Digital converter OFF
}





//good night

void goto_sleep()
{
  
  pinMode(GATE1,INPUT);
  pinMode(GATE2,INPUT);
  pinMode(GATE3,INPUT);
  
  sbi(PCMSK,0); // remove interrupt handler (debounce)
  delay(100);   // wait  a little till button can be assumed to be released again
    
    
  // attach the ISR again.  
  sbi(PCMSK,PCINT0); //Define which pins are doing the interrupting 
  

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set sleep mode
  sleep_mode(); //Go to sleep

  good_night=0;

  
}






int dir=0;
int min=1;
int max=17;
int wait=10;






// for button press. 
ISR(PCINT0_vect) {     //All interrupts direct to this ISR function


if(!digitalRead(BUTTON_PIN))
{
  good_night=1;
  wait=0;
} 
  
}







void loop(){

  
  // if button pressed, turn off.
  if(good_night) goto_sleep();  
  
  
  // spin left first
  if(dir)
  {
   show_spin(1); 
  }  
  else // then spin it the other way
  {
   show_spin(0);  
  } 


}




// turn one led on. leds are charlieplexedso there can be ONE on at any time.

void led_on(int led)
{
  
  switch (led)
  {
  case 1:  
  pinMode(GATE1,OUTPUT);
  pinMode(GATE2,OUTPUT);
  pinMode(GATE3,INPUT);  
   
  digitalWrite(GATE1,LOW); 
  digitalWrite(GATE2,HIGH); 
  break;
  
  
  case 2:
  pinMode(GATE1,OUTPUT);
  pinMode(GATE2,OUTPUT);
  pinMode(GATE3,INPUT);  
   
  digitalWrite(GATE1,HIGH); 
  digitalWrite(GATE2,LOW); 
  break;
  
  
  
  
  
  
  case 3:
  
  pinMode(GATE1,INPUT);
  pinMode(GATE2,OUTPUT);
  pinMode(GATE3,OUTPUT);  
  
  
  
  digitalWrite(GATE3,LOW); 
  digitalWrite(GATE2,HIGH); 
  
  
  break;
  
  
  
  
  case 4:
  

  pinMode(GATE1,INPUT);
  pinMode(GATE2,OUTPUT);
  pinMode(GATE3,OUTPUT);  
    

  digitalWrite(GATE3,HIGH); 
  digitalWrite(GATE2,LOW); 
  
  break;
  
  
  
  case 5:
  
    
  pinMode(GATE1,OUTPUT);
  pinMode(GATE2,INPUT);
  pinMode(GATE3,OUTPUT);  
  
  
  
  digitalWrite(GATE3,LOW); 
  digitalWrite(GATE1,HIGH); 
  
  
  
  break;
  
  
  case 6:
  
    
  pinMode(GATE1,OUTPUT);
  pinMode(GATE2,INPUT);
  pinMode(GATE3,OUTPUT);  
  
  

  digitalWrite(GATE3,HIGH); 
  digitalWrite(GATE1,LOW); 
  
break;
  
  
  
  
  
  
  
 
 
  }
  
  
}






void show_spin(int direction_left)
{
  


    
  if(wait>max)
  {
    dir=1;
    wait-=1; 
    delay(20); // dramatic stop of spin
    return;
  }
  
  
  
  if(wait<min)   dir=0; // change velocity  
  
  
  // are we getting faster or slower?
  if(!dir)
   wait+=1;
  else
   wait-=1;  




  
  //turn left
  if(direction_left == 1)
  {
    
    for(int i=1;i<=6;i++)
    {
     led_on(i);
     delay(wait);
    }  


    
  }
  else //turn right
  {
    
      for(int i=6;i>=1;i--)
    {
     led_on(i);
     delay(wait);
    }  
    
    
    
  }
}
