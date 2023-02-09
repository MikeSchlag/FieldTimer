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

void playChargeSound(void)
{
  Brain.playNote(1, 1, 150);
  Brain.playNote(1, 2, 150);
  Brain.playNote(1, 3, 150);
  Brain.playNote(1, 4, 250);
  Brain.playNote(1, 2, 150);
  Brain.playNote(1, 4, 450);
}

void runTheTimer(void)
{
  Brain.Screen.setFont(fontType::mono60);
  Brain.Screen.clearScreen(color::red);
  Brain.Screen.printAt(60, 60, "3");
  Brain.playNote(1, 4, 250);
  wait(750, timeUnits::msec);
  Brain.Screen.clearScreen(color::yellow);
  Brain.Screen.printAt(60, 60, "2");
  Brain.playNote(1, 4, 250);
  wait(750, timeUnits::msec);
  Brain.Screen.clearScreen(color::yellow);
  Brain.Screen.printAt(60, 60, "1");
  Brain.playNote(1, 4, 250);
  wait(750, timeUnits::msec);
  Brain.Screen.clearScreen(color::green);
  Brain.Screen.printAt(45, 60, "GO!");
  countdown.reset();
  playChargeSound();
  countdownRunning = true;
  playChangeNote = true;
}


int main() {
	
  Brain.buttonLeft.released(runTheTimer);

  while(1) {
      
    // Sleep the thread at 250ms, to avoid some screen refresh hiccups
    this_thread::sleep_for(250);

    if(countdownRunning == true)
    {
      Brain.Screen.setFont(fontType::mono60);
      switch((int)countdown.value())
      {
        case 0 ... 24:
          Brain.Screen.clearScreen(color::green);
          Brain.Screen.printAt(45, 60, "%02d", 60 - (int)countdown.value());
        break;

        case 25 ... 35:
          Brain.Screen.clearScreen(color::blue);
          if(((int)countdown.value() % 2) == 0)
          {
            Brain.Screen.setFont(fontType::mono40);
            Brain.Screen.printAt(05, 60, "CHANGE!");
            if(playChangeNote == true)
            {
              Brain.playNote(1, 3, 100);
              wait(100, timeUnits::msec);
              playChangeNote = false;
            }
          }
          else
          {
            Brain.Screen.setFont(fontType::mono60);
            Brain.Screen.printAt(45, 60, "%02d", 60 - (int)countdown.value());
            playChangeNote = true;
          }

       break;

        case 36 ... 50:
          Brain.Screen.setFont(fontType::mono60);
          Brain.Screen.clearScreen(color::green);
          Brain.Screen.printAt(45, 60, "%02d", 60 - (int)countdown.value());
        break;

        case 51 ... 59:
          Brain.Screen.clearScreen(color::yellow);
          Brain.Screen.printAt(45, 60, "%02d", 60 - (int)countdown.value());
        break;

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
        
        default:
          countdownRunning = false;
        break;
      }
    } // End switch
  } // End while(1)
} // End main()