#include <Arduino.h>    //ensure core Arduino library is included
#include <util/delay.h> //include the AVR delay library

#define PNUM 3          //define the number problem, simply change value as needed
                        //  1 : check
                        //  2 : check+
                        //  3 : check++

#define potSense false  //pretend like the potentiometer is the sensor for Part 3
                        //helps as a test to make sure the program is working properly when not wired to pump
                        //false means that the SENSOR will be A0, true means the SENSOR will be A1

#if(potSense == true)//handle the macro of SENSOR depending on if sensor should get input from potentiometer
#define SENSOR A1    //potentiometer
#else
#define SENSOR A0    //water level sensor
#endif

#define PUMP 9        //define pump pin
#define BLED (1<<5)  //13  (PORTB)
#define GLED (1<<4)  //12
#define RLED (1<<3)  //11
#define BLEDOFF (0<<5)//define BLEDOFF as LOW on pin 13 
#define GLEDOFF (0<<4)//Low on pin 12
#define RLEDOFF (0<<3)//low on pin 11
#define POT A1        //define the potentiometer's pin as A1


#if(PNUM == 1)      //if answering question 1
bool pumpOn;        //globally declare the pump's on state
bool first;          //globally declare if whether the loop has repeated yet
bool setPump(bool first);  //globally declare prototype of setPump() 
                           //function taking the bool of whether it has run, return the result
#elif(PNUM == 2)    //if question 2
bool runPump;       //globally declare the pump's intended state, will be used to restrict when button is pressed
//void setPump(void);
#elif(PNUM == 3)    //if question 3
uint8_t setVal;     //globally declare setVal, which I suppose never was used
#endif
#if((PNUM == 1)||(PNUM == 2))//if either question 1 or 2
void printWDel(uint8_t cnt); //declare the function prototype of printWDel as taking an unsigned 8 bit count
#elif(PNUM == 3)             //if question 3
void printWDel(uint8_t cnt, const uint8_t valBeingWritten);//declare the prototype of printWDel but add a pass for the value being written 
void blinkDel(const uint8_t cnt);                          //declare the prototype of blinkWDel, takes a CONSTANT unsigned 8 bit integer, to ensure saftey duing blinking
#endif
#if((PNUM == 2)||(PNUM == 3))//if question 2 or 3
void setPump(void);          //set pump should be declared with no parameters
#endif
void printDat();             //declare printDat no matter what question
uint8_t writeVal;            //globally declare writeVal, 
                             //only used in question 3, but it won't interfere


void printDat(){             //define printDat()
  Serial.print(analogRead(SENSOR));  //read and print the value of the sensor
  Serial.print('\t');        //append the tab character
  #if(PNUM == 1)             //if question 1
  Serial.println(pumpOn);    //print the pump's on/off state
  #elif(PNUM == 2)           //if question 2
  Serial.println((((analogRead(POT))>>2))*((uint8_t)(runPump)));  //print out:
                                                                  // the value of the potentiometer divided by 4
                                                                  // use a C-style typecast to convert the boolean runPump to a uint8_t
                                                                  // use this to multiply by the potentiometer result to suppress output when zero
                                                                  // will result in value of zero, if pump should not be running
  //Serial.println((uint8_t)(analogRead(PUMP)));
  #elif(PNUM == 3)            //if question 3
  Serial.println(writeVal);   //print the value being written to the pump
  //Serial.println((analogRead(SENSOR)>>2));
  //Serial.print('\t');Serial.println();
  //Serial.println();
  #endif
  
}

#if(PNUM == 3)                 //if question 3
void blinkDel(const uint8_t cnt){//define blinkDel: this is one is fun
  const uint8_t ovrTwoHundrd = (((((uint8_t)(analogRead(SENSOR) > 220))))<<3);//Raii a constant uint8_t that will enable the red light 
                                                                              // when the sensor reads greater than 230, this reflects the good pump
                                                                              
  //const uint8_t ovrTwoHundrd = (((((uint8_t)(analogRead(SENSOR) > 350))))<<3);//Raii a constant uint8_t that will enable the red light 
                                                                              // when the sensor reads greater than 350, this reflects the bad pump
  if(cnt == 0){      //check if the light should blink at all or not
    _delay_ms(1000); //if it shouldn't, delay for 1s
  }
  else{              //otherwise, blink the number of times you set
    
    //this will create a bitmask that acts as a stack that will determine the behiavior of the blue LED during flashing
    
    const uint8_t msk = (((cnt)&(0x01))<<5)|                    //the 5th bit should be set to whether the count is an even or odd
                        (((uint8_t)((cnt > 1)&&(cnt != 3)))<<4)|//the 4th bit should be set if the count is either 2, 4, or 5
                        (((cnt)&(0x01))<<3)|                    //the 3rd bit should be set to whether the count is even or odd
                        (((uint8_t)((cnt > 1)&&(cnt != 3)))<<2)|//the 2nd bit should be set if count = 2, 4, or 5
                        (((uint8_t)(cnt > 2))<<1);              //the 1st bit should be set is the count is greater than 2, 
                                                                //the 0th bit is not used as it will check the for-loop's termination
    for(uint8_t i = 0x10; i > 0x00; i=(i>>1)){                  //step through each element of the bitmask
      PORTB = (ovrTwoHundrd)|(((uint8_t)((bool)(msk & i)))<<5); //ensure the red light's state is being pereserved in PORTB and 
                                                                // turn on the blue led if the current bit says so
      _delay_ms(50);                                            //delay for 50 ms
      PORTB = (ovrTwoHundrd)|(BLEDOFF);                         //ensure preservation of the red LED's state and turn off the blue led unconditionally
      _delay_ms(150);                                           //delay for 150 ms
                                                                //since this will parse 5 bits, and exclude the 0th, the total time elapsed should be 1s
    /**
     * In other words, the blink's truth table is as follows:
     *    cnt:    1st flash   2nd flash   3rd flash   4th flash   5th flash
     *    1       on          off         off         off         off
     *    2       off         on          off         on          off
     *    3       on          off         on          off         on
     *    4       off         on          on          on          on
     *    5       on          on          on          on          on
     */
    }
  }
  
}
#endif

#if((PNUM == 1)||(PNUM == 2)) //if question 1 or 2
void printWDel(uint8_t cnt){  //define print With Delay takes an uinsigned 8 bit integer to correspond with the AVR's 8bit register width
#elif(PNUM == 3)              //if question 3
void printWDel(uint8_t cnt, const uint8_t valBeingWritten){  //define printWDelay to include the valBeingWritten as well
                                                             //I don't think it was used though
#endif
#if((PNUM==1)||(PNUM==2)||(PNUM==3))//no matter what question, just ensure a valid question
  for(uint8_t i = 0; i < cnt; i++){ //starting at zero, loop for the number of times based on the value count (cnt)
    printDat();               //print the data
    #if(PNUM == 1)            //if question 1
    _delay_ms(2000);          //just delay for 2 seconds
    #elif(PNUM == 2)          //if question 2
    for(uint8_t t = 0; t < 200; t++){//count 200 times, resulting in 10ms per iteration and 2 seconds in total
      if((bool)(PIND&(1<<4))){       //check if the button has been pressed
        runPump = !(runPump);        //if it has, logical NOT the runPump and set its state to the next,
                                     //e.g. if its true, set to false, if its false, set to true
      }
        _delay_ms(10);               //delay 10 ms
      } 
    #elif(PNUM == 3)          //if question 3

    blinkDel(((~(writeVal))/54));  //as the value being written to the pump is between 0-255 
                              //  then dividing by 51 should get a value between 0 and 5
                              //this reflects the good pump
    //blinkDel((writeVal/75));  //pass the value of writeVal divided by 51
                              //it should result in a value that is between 1 and 5
    #endif
  }

}
#endif

#if(PNUM == 1)//if question 1
bool setPump(bool first){//define the setPump function using a bool parameter
  while((analogRead(SENSOR) < 230)&&(first)&&(pumpOn)){//while the sensor is below 230, the pump has not repeated yet, and the pump's state is on, then it should be run
    pumpOn = true;                      //set pumpOn
    first = !((analogRead(A0)) >= 240); //check if the pump is sensor reads greater than 220, so as to ensure not repeating prematurely
    analogWrite(PUMP, 0xff);            //write the maximum value of a uint8_t
    printWDel(1);                       //delay for 2 seconds and write data each time
  }
  analogWrite(PUMP, 0x00);              //stop the pump
  pumpOn = false;                       //will be used as output between 0 and 1, (off and on respectively
  printWDel(10);                        //delay for 20 seconds and print data every 2 seconds
  first = true;                         //reset the state of the pump
  return first;                         //in case this needed to be run recursively... it didn't, but it can be used for something else now
}

#elif(PNUM == 2)
void setPump(){                         //if question 2
  while((analogRead(POT) > 40)&&(runPump)){                 //clamp the on value to greater than 40 from the potentiometer, and check if the pump should be run
    analogWrite(PUMP, (uint8_t)((analogRead(POT))>>2));     //the 1024 max input of the potentiometer should be 
                                                            //  divided by 4, truncated to an unsigned 8 bit integer, and then written to the pump

    if(analogRead(POT) < 307){                              //if the potentiometer is less than 307, turn off all LEDs
      PORTB = RLEDOFF|GLEDOFF|BLEDOFF;
    }
    else{
      if(analogRead(POT) < 718){                            //if the potentiometer is less than 718, but not less than 307, then turn on green
        PORTB = RLEDOFF|BLEDOFF|GLED;
      }
      else{
        if(analogRead(POT) < 1000)                          //if the potentiometer is less than 1000 but not less than 718, turn on blue
        PORTB = GLEDOFF|BLED|RLEDOFF;
        else{
          PORTB = GLEDOFF|BLEDOFF|RLED;                     //if max, turn on red
        }
      }
    }
    printWDel(1);                                           //print data, and delay 2 seconds, during which, every 10th of a second, check if the button has been pressed
  }
  analogWrite(PUMP, 0x00);                                  //stop the pump
  PORTB = (RLEDOFF)|(GLEDOFF)|(BLEDOFF);                    //turn off all LEDs
  printWDel(1);                                             //print data, and delay 2 seconds, during which, every 10th of a second, check if the button has been pressed
}

#elif(PNUM == 3)
void setPump(){
  while((analogRead(SENSOR) < 255)){  //this reflects the original max value of the pump, which consistently went between about 0 and 270
                                      //this was rendered useless by the bad pump
                                      //this was reused though, the 3rd time, and reflects the newest version of the data
                                      
  //while((analogRead(SENSOR) < 450)){  //this reflects the new max value of the pump, sort of,
                                      //the pump would oscillate many times between ouputting unacceptable values, for example:
                                      //  being at only 250 to 350 and jumping to 400-500 constantly,
                                      //  ouputting over 300 consistently,
                                      //  outputting a full 1024, multiple times
                                      
    writeVal = (uint8_t)((255-(analogRead(SENSOR)))); //this is a reflection of the original code, 
                                                      //although because it outputs no sign, I could leave it in and hope for the best on the bad pump
                                                      //any further fiddling made it worse than before on the bad pump, so I figured it worked as well as it could
                                                      
    analogWrite(PUMP, writeVal);      //write to the pump
    printWDel(1, writeVal);           //print data and delay for 1 second,
                                      //  during that second, blink a number of times between 1-5

    //analogWrite(PUMP, protectedWriteVal);//scrapped idea
    //printWDel(1, protectedWriteVal);     //scrapped idea
  }
  writeVal = 0x00;                    //stop the pump
  analogWrite(PUMP, writeVal);
  PORTB = (RLED)|(GLEDOFF)|(BLEDOFF); //keep on the red LED
  printWDel(1, 0);                    //print data every second, but don't blink
}

#endif

//now for the actual setup() and loop()

void setup(){                       //define void setup(void)
  Serial.begin(9600);               //start serial at 9600 baud
  #if(PNUM == 1)                    //if question 1
  pumpOn = true;                    //initialize the pump to ON
  first = true;                     //make this the first iteration
  #elif((PNUM == 2)||(PNUM == 3))   //if questions 2 or 3
  DDRD = (0<<4);                    //set pin 4 as input for button
  DDRB = RLED|GLED|BLED;            //set all LEDs as ouput (pins 13, 12, and 11) 
  PORTB = RLEDOFF|GLEDOFF|BLEDOFF;  //turn off all LED's
  #if(PNUM == 2)                    //if question 2
  runPump = true;                   //initialize runPump to true, allowing it to run
  #elif(PNUM == 3)                  //if question 3
  printWDel(1,0);                   //print data and wait for 1 second, during which, don't blink
  writeVal = 0;                     //initialize writeVal to not write anything to the pump 
  //senseStr = 0;
  #endif
  #endif
}

void loop(){                        //define void loop(void)
  #if(PNUM == 1)                    //if question 1
  setPump(first);                   //call setPump and make sure first is passed
  #elif((PNUM == 2)||(PNUM == 3))   //if questions 2 or 3
  setPump();                        //call setPump() with no parameters passed
  #endif  
}

