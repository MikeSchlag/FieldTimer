/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       mikes                                                     */
/*    Created:      2/8/2023, 6:27:10 PM                                      */
/*    Description:  IQ2 project                                               */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#include "vex.h"

using namespace vex;

// A global instance of vex::brain used for printing to the IQ2 brain screen
vex::brain      Brain;

// define your global instances of motors and other devices here
timer countdown;
bool countdownRunning = false;
bool playChangeNote = false;

/*! ****************************************************************************************************
  @brief Plays the charge fanfare melody
  @return None
  *****************************************************************************************************/  
void playChargeSound(void)
{
  Brain.playNote(1, 1, 150);
  Brain.playNote(1, 2, 150);
  Brain.playNote(1, 3, 150);
  Brain.playNote(1, 4, 250);
  Brain.playNote(1, 2, 150);
  Brain.playNote(1, 4, 450);
}

/*! ****************************************************************************************************
  @brief Starts the timer. Currently there's no guard on this, so pressing the button a second time once
    the timer is counting down will start a second run-through of the first countdown, and then reset
    the timer display. Need to add in some sort of guard mechanism to not run it if things are already
    running.
  @return None
  *****************************************************************************************************/  
void runTheTimer(void)
{
  // Make the font big, background red, and put up a "3"
  Brain.Screen.setFont(fontType::mono60);
  Brain.Screen.clearScreen(color::red);
  Brain.Screen.printAt(60, 60, "3");
  
  // Play a beep note for about 250ms, then wait about 750ms
  Brain.playNote(1, 4, 250);
  wait(750, timeUnits::msec);

  // Make the screen yellow and make it "2"
  Brain.Screen.clearScreen(color::yellow);
  Brain.Screen.printAt(60, 60, "2");

  // Play a beep note for about 250ms, then wait about 750ms
  Brain.playNote(1, 4, 250);
  wait(750, timeUnits::msec);

  // Make the screen yellow and make it "2"
  Brain.Screen.clearScreen(color::yellow);
  Brain.Screen.printAt(60, 60, "1");

  // Play a beep note for about 250ms, then wait about 750ms
  Brain.playNote(1, 4, 250);
  wait(750, timeUnits::msec);

  // Green means GO GO GO!
  Brain.Screen.clearScreen(color::green);
  Brain.Screen.printAt(45, 60, "GO!");
  
  // Reset the countdown timer so it starts counting and play the sound
  countdown.reset();

  // Ideally should put this in a task so that it's running parallel
  playChargeSound();

  // Set our boolean checkers for what we're good to do or not do
  countdownRunning = true;
  playChangeNote = true;
}


int main() {
	
  // Set up the callback to run whenever the brain's "Left" button is pressed
  Brain.buttonLeft.released(runTheTimer);

  while(1) {
      
    // Sleep the thread at 250ms, to avoid some screen refresh hiccups
    //   This is hacky and something I will fix with another push at some point
    //   to only refresh when a number has actually changed
    this_thread::sleep_for(250);

    // If the countdown is running, we should be doing the other things
    if(countdownRunning == true)
    {
      Brain.Screen.setFont(fontType::mono60);

      // Switch/case is easy to do this with ranges and something I wanted to try
      // Note that value() returns a double, so to make the case check easier, we
      //   cast it to an int so that it's treated as a whole number.
      // TODO: look at doing better than just casting, which is probably truncating
      //    and ultimately allowing an extra second or two
      switch((int)countdown.value())
      {
        // First 25 seconds, green background and count down
        case 0 ... 24:
          Brain.Screen.clearScreen(color::green);
          // Abuse the format and just do the countdown timer inline; this is also
          //  allowing the screen to visibly repaint
          Brain.Screen.printAt(45, 60, "%02d", 60 - (int)countdown.value());
        break; // 0 ... 24

        // For 25 to 35, it's driver switch window, so make it blue background, and
        //  alternate showing the timer and reminding to change; also give a beep every
        //  time we show the change text
        case 25 ... 35:
          Brain.Screen.clearScreen(color::blue);
          // Using % 2 is an easy way to get it to toggle as it counts down and check for even numbers
          if(((int)countdown.value() % 2) == 0)
          {
            Brain.Screen.setFont(fontType::mono40);
            Brain.Screen.printAt(05, 60, "CHANGE!");

            // Use a checker to prevent the sound from playing multiple times in a row
            if(playChangeNote == true)
            {
              Brain.playNote(1, 3, 100);
              wait(100, timeUnits::msec);
              playChangeNote = false;
            }
          }
          else // %2 != 0, so the countdown timer is on an odd number
          {
            Brain.Screen.setFont(fontType::mono60);
            Brain.Screen.printAt(45, 60, "%02d", 60 - (int)countdown.value());
            playChangeNote = true;
          }
        break; // 25 ... 35

        // For the next 15 seconds, screen is green and counting down
        case 36 ... 50:
          Brain.Screen.setFont(fontType::mono60);
          Brain.Screen.clearScreen(color::green);
          Brain.Screen.printAt(45, 60, "%02d", 60 - (int)countdown.value());
        break; // 36 ... 50

        // For the last 10 seconds, screen goes yellow and keeps counting down
        case 51 ... 59:
          Brain.Screen.clearScreen(color::yellow);
          Brain.Screen.printAt(45, 60, "%02d", 60 - (int)countdown.value());
        break;

        // At 60 (or moderately beyond), time to stop the countdown checker,
        //  turn the screen red, say to stop, and play a sound to get their
        //  attention
        case 60 ... 100:
          countdownRunning = false;
          Brain.Screen.setFont(fontType::mono60);
          Brain.Screen.clearScreen(color::red);
          Brain.Screen.printAt(15, 60, "STOP!");  
          Brain.playSound(soundType::powerDown);
          wait(350, timeUnits::msec);
          Brain.playSound(soundType::powerDown);
          wait(350, timeUnits::msec);
          Brain.playSound(soundType::powerDown);
        break;
        
        // Always have a default to catch if something unexpected happens
        //  And have a way to know that it was hit so you can try to figure out
        //  how it went off the rails. In this case we print out what countdown's
        //  value was.
        default:
          countdownRunning = false;
          Brain.Screen.setFont(fontType::mono20);
          Brain.Screen.clearScreen(color::violet);
          Brain.Screen.printAt(0, 60, "Something went wrong!");  
          Brain.Screen.printAt(0, 80, "Countdown: %02d", (int)countdown.value());  
        break;
      }
    } // End switch
  } // End while(1)
} // End main()