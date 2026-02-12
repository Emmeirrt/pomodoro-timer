# pomodoro-timer
Pomodoro timer that can be customised and used with only one rotary encoder
# What you need for the project
- arduino nano
- neopixel adressable LED strip (you can choose your own size, i used 24-LED rings)
- tilt mercury sensor module (ky017)
- rotary encoder module

Combine the electronics as shown in the diagram
<img width="827" height="631" alt="circuit_image (1)" src="https://github.com/user-attachments/assets/6907375a-94d8-4683-b748-eda6751b6163" />


Download Arduino IDE and connect your arduino to it

Add *adafruit neopixel library* to your arduino from library manager (i used 1.15.2 at the time)

You can change the number of LEDs depending on the LED-strip you choose by changing the LED_COUNT parameter at line 12

And the time assing to each led (in seconds) by changing the chunk parameter at line 14

To change the defult timer that starts whenever you plug it, change the initial_focus, initial_break and sessions_left values at lines 31 trough 33. 

If you dont want it to start immidietly when you plug it, just change the started_before parameter to 0 at line 61

Upload the code to you arduino

# How the device works?
Device automatically starts when its plugged in

Meaning of LED colors
- Red; focus timer
- Blue; break timer
- Green; number of sessions left

Countdown starts when the device is at uprigt position

Countdown stops when the device is turn upside-down

When there is no more sessions left, device waits with rotating green lights; to be turned upside-down and seta a new timer parameter as decribed below.
  or you can just unplug and plug it back again to starts with initial parameters (adding a switch to the power cables coming out of the arduino will also work)
  İnitial parameters are 25 mins of focus timer and 5 minutes of break timer for 2 sessions

## Changing the countdown timer
When the device is at upright position; rotary encoder changes the ongoing countdown by the value assing to each LED 

When the device is upside-down; rotary encoder sets up the new countdowns parameters, and turning the device back to upright position starts a new timer with the set parameters

## Pressing on the rotary encoder
When the device is at upright position; pressin on the rotary encoder, skips the active timer (if its on focus, it skips onto the next break timer and vise verca. Skipping/ending the break timer end that session

When the device is turned upside-down; pressing the rotary encoder cycles between the settings in order to change parameters of; 
Focus timer, 
Break timer 
and the number of sessions.
