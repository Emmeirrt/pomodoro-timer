//NECESSARY LIBRARIES
#include <Adafruit_NeoPixel.h>//include needed library for making ring-led's control easier

//DEFINING VALUES
const byte TILT = 2;
const byte KNOB_CLK = 3;
const byte KNOB_DT = 4;
const byte KNOB_SW = 5;
#ifdef __AVR__
#include <avr/power.h>         //Required for 16 MHz Adafruit Trinket
#endif
#define LED_COUNT 24           //number of LEDs on the ring
#define LED_PIN 6              //id of the pin that Data-in of the LED ring is connected
#define chunk  150               //how many seconds corresdponds to a chunk(2-led)

//UNCATEGORISED DEBUG VARIABLES
int DEBUG = 2;                 //to enable serial monitor debugging 1;State-Machines 2;Pomodoro Values
int a = 0;
int b = 0;
int c = 0;
int d = 0;
int e = 0;
int f = 0;


// VARIABLES ARE GROUP BY WHERE THEY APPEAR FIRST IN THE CODE

//MAIN VARIABLES
int focus_timer = (25*60);       //default focus time in seconds
int break_timer = (5*60);       //default break time in seconds
int sessions_left = 2;        //default number of sessions
int initial_focus = (50*60);  //starting value of the focus timer
int initial_break = (10*60);  //starting value of the break timer

//DISPLAY
int mode = 0;        //changes ui 0(focus), 1(break), 2(number of sessions)
int prev_st_mode = 0;//to debug display state-machine
int led_counter = 1; // number of LEDs that will light up

//MODECHANGE
int st_modechange = 0;           //initial state of the modechange state-machine
int prev_st_modechange = 0;      //to debug modechange state-machine
unsigned long t_0_modechange = 0;//initial time for modechange event
unsigned long t_modechange = 0;  //current time for modechange event
unsigned long bounce_delay = 5;  //delay in ms to prevent bounce

//SESSION SETTINGS
int st_sessionsettings = 0;           //initial state of the sessionsettings state-machine
int prev_st_sessionsettings = 0;      //to debug sessionsettings state-machine
unsigned long t_0_sessionsettings = 0;//initial time for sessionsettings event
unsigned long t_sessionsettings = 0;  //current time for sessionsettings event
int delta = 0;                        // decides if the value will increase or decrease
int prev_KNOB_CLK = 0;

//COUNTDOWN
int st_countdown = 0;           //initial state of the countdown state-machine
int prev_st_countdown = 0;      //to debug countdown state_machine
unsigned long t_0_countdown = 0;//initial time for countdown event
unsigned long t_countdown = 0;  //current time for countdown event
unsigned long second = 1000;    //seconds in ms
int started_before = 1;         //is device in the middle of a countdown? (default:no, it is a new countdown)






Adafruit_NeoPixel pixels(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


//SETUP CODE AND MAIN LOOP
void setup() {
  Serial.begin(9600);
  pinMode(TILT, INPUT);
  pinMode(KNOB_CLK, INPUT);
  pinMode(KNOB_DT, INPUT);
  pinMode(KNOB_SW, INPUT_PULLUP);
  pixels.begin();          // INITIALIZE NeoPixel pixels object (REQUIRED)
  pixels.show();           // Turn OFF all ASAP
  pixels.setBrightness(50);// Set BRIGHTNESS to about 1/5 (max = 255)
}

void loop() {//cycles trough every state-machine, and outputs the data if the DEBUG is activated
  display();
  modechange();
  sessionsettings();
  countdown();
  if (1 == DEBUG) {
    if (prev_st_mode != mode) {
      Serial.print("Display State: ");
      Serial.println(mode);
    }

    if (prev_st_modechange != st_modechange) {
      Serial.print("Modechange State: ");
      Serial.println(st_modechange);
    }

    if (prev_st_sessionsettings != st_sessionsettings) {
      Serial.print("Sessionsettings State: ");
      Serial.println(st_sessionsettings);
    }

    if (prev_st_countdown != st_countdown) {
      Serial.print("Countdown State: ");
      Serial.println(st_countdown);
    }
  }
  else if(2 == DEBUG) {
    if (a != focus_timer || b != break_timer || c != sessions_left || d != initial_focus || e != initial_break) {
      Serial.println((String)focus_timer + " " + initial_focus + " / " + break_timer + " " + initial_break + " / " + sessions_left);
      a = focus_timer;
      b = break_timer;
      c = sessions_left;
      d = initial_focus;
      e = initial_break;
    }
  }
}

//STATE-MACHINE 1 : THE DISPLAY
void display() {
  prev_st_mode = mode;//display state is saved for debugging
  pixels.clear();
  switch (mode) {
    case 0:
      { 
        if (HIGH == digitalRead(TILT)) {
          led_counter = ((focus_timer + chunk - 1) / chunk);
        } else {
          led_counter = ((initial_focus + chunk - 1) / chunk);
        }
        for (int l = 0; l < led_counter; l++) {//displays the focus time by 5 minutes chunks on the LED-ring
          pixels.setPixelColor(l, pixels.Color(20, 0, 0));
        }
        break;
      }

    case 1:
      {
        if (HIGH == digitalRead(TILT)) {
          led_counter = ((break_timer + chunk - 1) / chunk);
        } else {
          led_counter = ((initial_break + chunk - 1) / chunk);
        }
        for (int l = 0; l < led_counter; l++) {//displays the break time by 5 minutes chunks on the LED-ring
          pixels.setPixelColor(l, pixels.Color(0, 0, 20));
        }
        break;
      }

    case 2:
      {
        if (HIGH == digitalRead(TILT)) {
          led_counter = sessions_left-1;
        } else {
          led_counter = sessions_left;
        }
        for (int l = 0; l < led_counter; l++) {//displays the number of sessions on the LED-ring
          pixels.setPixelColor(l, pixels.Color(0, 20, 0));
        }
        break;
      }
  }
  pixels.show();
}

//STATE-MACHİNE 2 : MODE-CHANGE
void modechange() {
  prev_st_modechange = st_modechange;//modechange state is saved for debugging
  switch (st_modechange) {

    case 0:
      {//wait for the knob to go LOW
        if (LOW == digitalRead(KNOB_SW)) { st_modechange = 1; }
        break;
      }

    case 1:{//record the time and proceed
        t_0_modechange = millis();
        st_modechange = 2;
        break;
      }

    case 2:{//check for proper bounce_delay. if bounce occurse; reset
        t_modechange = millis();
        if (t_modechange - t_0_modechange > bounce_delay) { st_modechange = 3; }
        if (HIGH == digitalRead(KNOB_SW)) { st_modechange = 0; }
        break;
      }

    case 3:{//trigger the modechange event
        mode++;
        mode = mode % 3;
        st_modechange = 4;
        break;
      }

    case 4:{//reset when knob goes HIGH
        if (HIGH == digitalRead(KNOB_SW)) { st_modechange = 0; }
        break;
      }
  }
  if(0 == sessions_left&& HIGH == digitalRead(TILT)) {
    started_before = 0;
  }
}

//STATE-MACHİNE 3 : SESSİON LENGTH AND QUANTITY ADJUSTMENTS
void sessionsettings() {
  prev_st_sessionsettings = st_sessionsettings;//sessionsettings state is saved for debugging
  switch (st_sessionsettings) {
    case 0:{//save initial knob value
        prev_KNOB_CLK = digitalRead(KNOB_CLK);
        st_sessionsettings = 1;
        break;
      }

    case 1:{//wait for the knob to be rotates
        if (prev_KNOB_CLK != digitalRead(KNOB_CLK)) {
          if (digitalRead(KNOB_CLK) != digitalRead(KNOB_DT)) { delta++; }//clokwise rotation
          if (digitalRead(KNOB_CLK) == digitalRead(KNOB_DT)) { delta--; }//counter-clockwise rotation
          st_sessionsettings = 2;
        }
        break;
      }

    case 2:{//record the time and proceed
        t_0_sessionsettings = millis();
        st_sessionsettings = 3;
        break;
      }

    case 3:{//check for proper bounce_delay. if bounce occurse; reset
        t_sessionsettings = millis();
        if (t_sessionsettings - t_0_sessionsettings > bounce_delay) { st_sessionsettings = 4; }
        if (prev_KNOB_CLK == KNOB_CLK) { st_sessionsettings = 0; }
        break;
      }

    case 4:{//direct towards the right setting
        if (0 == mode) { st_sessionsettings = 5; }//focus settings
        if (1 == mode) { st_sessionsettings = 6; }//break settings
        if (2 == mode) { st_sessionsettings = 7; }//session quantity settings
        break;
      }

    case 5:{//change focus timer
        if (HIGH == digitalRead(TILT)) {
          focus_timer = constrain(focus_timer + (delta * chunk), 0, chunk*LED_COUNT);
        } else {
          initial_focus = constrain(initial_focus + (delta * chunk), 0, chunk*LED_COUNT);
          started_before = 0;
        }
        st_sessionsettings = 8;
        delta = 0;
        break;
      }

    case 6:{//change break timer
        if (HIGH == digitalRead(TILT)) {
          break_timer = constrain(break_timer + (delta * chunk), 0, chunk*LED_COUNT);
        } else {
          initial_break = constrain(initial_break + (delta * chunk), 0, chunk*LED_COUNT);
          started_before = 0;
        }
        st_sessionsettings = 8;
        delta = 0;
        break;
      }

    case 7:{//change the number of sessions
        sessions_left = constrain(sessions_left + delta, 1, chunk*LED_COUNT);
        started_before = 0;
        st_sessionsettings = 8;
        delta = 0;
        break;
      }


    case 8:{//reset
        st_sessionsettings = 0;
        break;
      }
  }
}


//STATE-MACHİNE 4 : THE COUNTDOWN
void countdown() {
  prev_st_countdown = st_countdown;//countdown state is saved for debugging
  if (HIGH == digitalRead(TILT)) {
    switch (st_countdown) {
      
      case 0:{//start the initial time for the countdown event
        if(0 == started_before) {mode = 0;}
        if(sessions_left > 0) {st_countdown = 1;}
        break;
      }

      case 1:{
        t_countdown=millis();
        if(t_countdown - t_0_countdown > second) {
          if(0 == started_before) {
            focus_timer = initial_focus;
            break_timer = initial_break;
            started_before = 1;
          }
          st_countdown = 2;
        }
        break;
      }

      case 2:{//direct the countdown depending on the mode
        t_0_countdown=millis();
        if (0 == mode) { st_countdown = 3; }//focus countdown
        if (1 == mode) { st_countdown = 4; }//break countdown
        if (2 == mode) { st_countdown = 5; }//session end
        break;
      }

      case 3:{//focus countdown
        focus_timer--;
        if(0 > focus_timer){
          focus_timer = initial_focus;
          mode = 1;
        }
        started_before= 1;
        st_countdown = 6;
      break;
      }

      case 4:{//break countdown
        break_timer--;
        if(0 > break_timer){
          break_timer = initial_break;
          mode = 2;
        }
        started_before= 1;
        st_countdown = 6;
      break;
      }

      case 5:{//session end
        sessions_left--;
        st_countdown = 6;
        if(0 == sessions_left){
          colorwipe(pixels.Color(0, 20, 0), 50);// colorwipe green when all the sessions end
          st_countdown = 7;
        }
        else {
          mode = 0;
          started_before=0;
        }
        delay(3000);
        break;
      }

      case 6:{//reset
        st_countdown = 0;
        break;
      }

      case 7:{//when all the sessions end, wait for device to be turned upside down
        theaterChase(pixels.Color(0,20,0),1000);
        if(LOW == digitalRead(TILT)) {
          mode = 0;
          st_countdown = 0;
          sessions_left = 1;
          started_before=0;
        }
        break; 
      }
    }
  }
}



//LED ANIMATIONS
void colorwipe(uint32_t color, int wait) {
  for(uint8_t i=0; i<pixels.numPixels(); i++) { // For each pixel in pixels...
    pixels.setPixelColor(i, color);             //  Set pixel's color (in RAM)
    pixels.show();                              //  Update pixels to match
    delay(wait);                                //  Pause for a moment
  }
}

void theaterChase(uint32_t c, uint16_t wait) {
  for (int j=0; j<1; j++) {//do 1 cycles of chasing
    for (int q=0; q < 2; q++) {
      for (uint16_t i=0; i < pixels.numPixels(); i=i+2) {
        pixels.setPixelColor(i+q, c);//turn every third pixel on
      }
      pixels.show();
      delay(wait);
      for (uint16_t i=0; i < pixels.numPixels(); i=i+2) {
        pixels.setPixelColor(i+q, 0);//turn every third pixel off
      }
    }
  }
}
