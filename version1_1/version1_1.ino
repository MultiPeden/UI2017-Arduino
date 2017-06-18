#include <ADCTouch.h>
#include <MillisTimer.h>

/*****************
      GLOBALS
******************/

const int buzzer = 4; //buzzer to arduino pin 4

//Initialize timers
MillisTimer timer;
MillisTimer b0_timer;
MillisTimer b1_timer;
MillisTimer b2_timer;
MillisTimer b3_timer;
//Variable to detect wether a button has been pressed
boolean press0, press1, press2, press3;
//reference values to remove offset
int ref0, ref1, ref2,ref3;   
//Which number in the array to save value, is the size of the current sequence.
int seq_size; 
//array for holding button sequences
int seq[4]; 

// THRESHOLD for registering button press
int THRESHOLD ; 

//Debug flag
boolean DEBUG;

/********************
    FUNCTIONS
********************/

void setArray(int *arr) {
//Resets the array to default values (no buttons)
  for (int i=0;i<4;i++){
    arr[i] = -1;
  }
}

//Resets the array to default values (no buttons)
void setArrayS(int t, int v, int *arr) {
  arr[t] = v;
}

//Sets the interrupt value for timeouts
void setInter() {
    if(seq_size > 1){
      printGesture();
    }
    resetGesture();
}

//Interrupt handlers for button haptic feedback, currently a buzzer
void setB0Inter(){
  noTone(buzzer);     // Stop sound...
}
void setB1Inter(){
  noTone(buzzer);     // Stop sound...
}
void setB2Inter(){
  noTone(buzzer);     // Stop sound...
}
void setB3Inter(){
  noTone(buzzer);     // Stop sound...
}


void printGesture(){
  /*
   * Prints the button sequence in format "{b1..bn}" without spaces between the numbers and n is <= 4 
   */
  
  Serial.print("{");
  for (int i = 0; i < seq_size; i++){
    Serial.print(seq[i]);
  }
  Serial.print("}");
  Serial.print("\n");
  
}

void resetGesture() {
  /* 
   * Resets the sequence array and press bools 
  */
  seq_size = 0;
  press0 = false;
  press1 = false;
  press2 = false;
  press3 = false;
  setArray(seq);
}

void setPressed(int i){
  switch(i){
    case 0:
      press0 = true;
      press1 = false;
      press2 = false;
      press3 = false;
      break;
    case 1:
      press0 = false;
      press1 = true;
      press2 = false;
      press3 = false;
      break;
    case 2:
      press0 = false;
      press1 = false;
      press2 = true;
      press3 = false;
      break;
    case 3:
      press0 = false;
      press1 = false;
      press2 = false;
      press3 = true;
      break;
  }
}

void setup() {
  Serial.begin(9600);
  DEBUG = false;
  // Initilize threshold value
  THRESHOLD = -100; 
  // Initialize button pressed sequence array
  setArray(seq);

  // Create references for init values of inpus:
  ref0 = ADCTouch.read(A0, 500);    //create reference values to 
  ref1 = ADCTouch.read(A1, 500);    //create reference values to 
  ref2 = ADCTouch.read(A2, 500);    //create reference values to 
  ref3 = ADCTouch.read(A3, 500);    //create reference values to 

  //Timer settings, time until expire, expire handler. Repeats does not matter  
  timer.setInterval(3000);
  timer.expiredHandler(setInter);
  timer.setRepeats(1);   
  b0_timer.setInterval(200);
  b0_timer.expiredHandler(setB0Inter);
  b0_timer.setRepeats(1);
  
  b1_timer.setInterval(200);
  b1_timer.expiredHandler(setB1Inter);
  b1_timer.setRepeats(1);
  
  b2_timer.setInterval(200);
  b2_timer.expiredHandler(setB2Inter);
  b2_timer.setRepeats(1);
  
  b3_timer.setInterval(200);
  b3_timer.expiredHandler(setB3Inter);
  b3_timer.setRepeats(1);

  pinMode(buzzer, OUTPUT); // Set buzzer - pin 4 as an output
}

void loop() {
  // Recalculate input values according to start offset
  int value0 = ADCTouch.read(A0);   //Read input
  int value1 = ADCTouch.read(A1);   //Read input
  int value2 = ADCTouch.read(A2);   //Read input
  int value3 = ADCTouch.read(A3);   //Read input
  value0 -= ref0;       //remove offset
  value1 -= ref1;       //remove offset
  value2 -= ref2;       //remove offset
  value3 -= ref3;       //remove offset
  // Check timer expiration
  timer.run(); 
  b0_timer.run();
  b1_timer.run();
  b2_timer.run();
  b3_timer.run();
      
  if (seq[3] == -1){ 
  //following if statements will register button press
  //if that button is above the THRESHOLD and not already pressed
  // timer is started if it is the first button press in sequence
    if(value0 < THRESHOLD and not press0){
      seq[seq_size] = 0;
      setPressed(0);
      if(DEBUG){
        tone(buzzer, 100); // Send 1KHz sound signal...
        b0_timer.start();
      }
      seq_size += 1;
      if (seq_size == 1){
        timer.start();
      }
    }
    
    else if(value1 < THRESHOLD and not press1){
      seq[seq_size] = 1;
      setPressed(1);
      if(DEBUG){
        tone(buzzer, 200); // Send 1KHz sound signal...
        b1_timer.start();
      }
      seq_size += 1;
      if (seq_size == 1){
        timer.start();
      }
    }
    
    else if(value2 < THRESHOLD and not press2){
      seq[seq_size] = 2;
      setPressed(2);
      if(DEBUG){
        tone(buzzer, 300); // Send 1KHz sound signal...
        b2_timer.start();
      }
      seq_size += 1;
      if (seq_size == 1){
       timer.start();
      }     
    }
    
    else if(value3 < THRESHOLD and not press3){
      seq[seq_size] = 3;
      setPressed(3);
      if(DEBUG){
        tone(buzzer, 400); // Send 1KHz sound signal...
        b3_timer.start();
      }
      seq_size += 1;
      if (seq_size == 1){
        timer.start();
      }   
    }
  }
  else{
    //If all buttons are pressed within 400 miliseconds we assume it is a "all button" press
    if(timer.getRemainingTime() > 2600){
      Serial.print("{9}\n");
      delay(1000);
    }else{
      printGesture();
    }
    resetGesture();
    timer.reset(); 
  }
}
