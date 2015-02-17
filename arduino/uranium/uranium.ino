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






// hw rev 2
#define GATE1 PIN_B0
#define GATE2 PIN_B1
#define GATE3 PIN_B4
#define BUTTON_PIN PIN_B2



// hw rev 1
#define GATE1 PIN_B2
#define GATE2 PIN_B3
#define GATE3 PIN_B4
#define BUTTON_PIN PIN_B0



// all the programs
// the numbering is not important as long as it is unique
// RUNTIME_STATE is looked up in loop() and is changed in button handler ISR on short press



const uint8_t STATE_SPIN_TORNADO =0;
const uint8_t STATE_ALL_ON =1;
const uint8_t STATE_CONSTANT_SPIN =2;
const uint8_t STATE_FADE_RAYS = 3;


// disabled
//const uint8_t STATE_FADE_SPIN =3;


// must be 1 higher than mall states above
const uint8_t STATE_MAXIMUM=4;



// current program runnig start_up() changes this to the last known state from eeprom
uint8_t RUNTIME_STATE = STATE_FADE_RAYS;




int matrix_table[6][3]=
{
  {        GATE3, GATE1, GATE2    },
  {        GATE3, GATE2, GATE1    },
  {        GATE3, GATE2, GATE1    },
  {        GATE1, GATE2, GATE3    },
  {        GATE1, GATE2, GATE3    },
  {        GATE3, GATE1, GATE2    }
};




int good_night=0;


void setup(){

  //Set all pins to inputs w/ pull-up resistors (stop them from floating or sourcing power)
  for (byte i=0; i<5; i++) {     
    pinMode(i, INPUT_PULLUP);
  }



// no need for this shit. PWM wont work in a meaningful way with charlieplexed leds :(

  // the darkest hack. oh my that cost me some time.
  // we set the PWM frequency here by modivying the clock divider for PWM generation.
  //TCCR0B = TCCR0B & 0b11111000 | 0x02;
  
  







   sbi(GIMSK,PCIE); // Turn on Pin Change interrupts (Tell Attiny85 we want to use pin change interrupts (can be any pin))
   sbi(PCMSK,PCINT0); //Define which pins are doing the interrupting (digital pin 1, DIP 7 in this example)

  cbi(ADCSRA,ADEN); //Switch Analog to Digital converter OFF
}



int die=0;

//good night

void goto_sleep()
{


  
  
  
  pinMode(GATE1,INPUT);
  pinMode(GATE2,INPUT);
  pinMode(GATE3,INPUT);

  sbi(PCMSK,0); // remove interrupt handler (debounce)
  delay(1000);   // wait  a little till button can be assumed to be released again


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





uint32_t button_pressed=0;
uint32_t button_released=0;



 // for button press. 
 ISR(PCINT0_vect) {     //All interrupts direct to this ISR function
 
 
  wait=0;
 
 // button released
 if(digitalRead(BUTTON_PIN))
 {
  //longpress? 
  if((millis()-button_pressed) >100)
  {
   good_night=1;
   button_pressed=0;
  } 
  else
  {
   
   RUNTIME_STATE=RUNTIME_STATE+1;
   if(RUNTIME_STATE == STATE_MAXIMUM)
     RUNTIME_STATE =0;
    
  }
  
  
  
 } 
 else
 {
   
   button_pressed=millis();
    
    
 }
 
 }
 





void loop(){




  // if button pressed, turn off.
  if(good_night) goto_sleep();  


  switch(RUNTIME_STATE)
  {
  case STATE_SPIN_TORNADO:
    // spin left first
    if(dir)
      show_spin_tornado(1);
    else // then spin it the other way
    show_spin_tornado(0); 
  break;

  case STATE_ALL_ON:
    all_on();
    break;

  case STATE_CONSTANT_SPIN:
    constant_spin();
    break;

    case STATE_FADE_RAYS:
     fade_rays();
    break;



// disabled for now.
//    case STATE_FADE_SPIN:
//     fade_spin();
//    break;



  }
}




// turn one led on. leds are charlieplexed so there can be ONE on at any time.

void led_on(int led)
{

  pinMode(matrix_table[led][0],OUTPUT);
  pinMode(matrix_table[led][1],OUTPUT);
  pinMode(matrix_table[led][2],INPUT);  


  switch(led)
  {
    
   case 0:
   case 2:
   case 3:   
    digitalWrite(matrix_table[led][0],HIGH); 
    digitalWrite(matrix_table[led][1],LOW);   
   break;   
    
    
    
   case 1:
   case 4:
   case 5:   
    digitalWrite(matrix_table[led][0],LOW); 
    digitalWrite(matrix_table[led][1],HIGH);   
   break;
   

   
   
  }

}





void softPWM(int pin, int freq, int sp) { // software PWM function that fakes analog output
digitalWrite(pin,HIGH); //on
delayMicroseconds(sp*freq);
digitalWrite(pin,LOW); //off
delayMicroseconds(sp*(255-freq));
}





void led_strobe(int led, int strobe)
{
  pinMode(matrix_table[led][0],OUTPUT);
  pinMode(matrix_table[led][1],OUTPUT);
  pinMode(matrix_table[led][2],INPUT);  



  switch(led)
  {
    
   case 0:
   case 2:
   case 3:   

     digitalWrite(matrix_table[led][0],HIGH);  
     softPWM(matrix_table[led][1],strobe,1);  
   break;   
    
    
    
   case 1:
   case 4:
   case 5:   
     digitalWrite(matrix_table[led][0],LOW);    
      softPWM(matrix_table[led][1],250-strobe,1); 
   break;
   

   
   
  }








}










void show_spin_tornado(int direction_left)
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

    for(int i=0;i<6;i++)
    {
      led_on(i);
      delay(wait);
    }  



  }
  else //turn right
  {

    for(int i=5;i>=0;i--)
    {
      led_on(i);
      delay(wait);
    }  



  }
}



void all_on()
{
  for(int i=0;i<6;i++)
  {
    led_on(i);
    delayMicroseconds(100);  
  }  

}



// works only on new hardware revision.
// but this is not worth the effort at all.
// decent PWM effects are not possible with charlieplexed setups because we always get some wrong drain  over the leds :(
// weird flicker on leds that are not explicitely used is the result.
// this is the only PWM faded effect that still looks nice..


void fade_spin()
{
  
  int del=520;
  int del2=3000;  
  int del3=10;
 
  for (int i=0;i<=5;i++)
  { 
    for (int j=1;j<=250;j++)
    {
     led_strobe(i,j);
      delayMicroseconds(del);   
    }
    
     delay(del3);
      
    for (int j=250;j>0;j--)
    {
     led_strobe(i,j);
      delayMicroseconds(del);      
    }
    
    
        delay(del3);
    
  }
}









void constant_spin()
{
  
  int del=7;

 
  for (int i=0;i<=5;i++)
  { 
     led_on(i);
     delay(del);
   }
  


}

















void fade_rays()
{
  

    int i=random(0,6);

  
    for (int j=1; j<=250; j++)
    {
       led_strobe(i,j);
       if(RUNTIME_STATE != STATE_FADE_RAYS) return;
    }
    

}





