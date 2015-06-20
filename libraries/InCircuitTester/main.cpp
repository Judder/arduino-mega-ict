//
// Copyright (c) 2015, Paul R. Swan
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
#include "main.h"

#include "Arduino.h"
#include <LiquidCrystal.h>
#include <DFR_Key.h>
#include <CGameCallback.h>

//
// Basic LCD diplay object (in this case, Sain 16 x 2).
//
static LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//
// Sain supplied keypad driver.
//
static DFR_Key keypad;

//
// The single on board LED is used to flash a heartbeat of sorts
// to be able to tell if the system in not running in the main loop.
//
static const UINT8 led = 13;

//
// This is the current selector.
//
static const SELECTOR *s_currentSelector;

//
// This is the game selector.
//
static const SELECTOR *s_gameSelector;

//
// This is the current selection.
//
static int s_currentSelection;

//
// When set (none-zero) causes the select to repeat the selection callback
// for the set number of seconds.
//
bool s_repeatSelectTimeInS;

//
// Handler for the configuration callback to set options.
//
PERROR
onSelectConfig(
    void *context,
    int  key
)
{
    PERROR error = errorCustom;

    if (context == (void *) &s_repeatSelectTimeInS)
    {
        if (s_repeatSelectTimeInS == 0)
        {
            s_repeatSelectTimeInS = 5;
        }
        else
        {
            s_repeatSelectTimeInS = 0;
        }

        errorCustom->description = String("OK: Repeat ") + String(s_repeatSelectTimeInS, DEC) + String("S");
        errorCustom->code = ERROR_SUCCESS;
    }

    return error;
}


//
// Handler for the game select callback that will switch the current
// game to the one supplied.
//
PERROR
onSelectGame(
    void *context,
    int  key
)
{
    PERROR error = errorSuccess;
    GameConstructor gameConstructor = (GameConstructor) context;

    if (CGameCallback::game != NULL)
    {
        delete CGameCallback::game;
        CGameCallback::game = (IGame *) NULL;
    }

    //
    // Assign a new game and reset the selector & selection.
    //

    CGameCallback::game = (IGame *) gameConstructor();

    s_currentSelector  = CGameCallback::selector;
    s_currentSelection = 0;

    return error;
}


void mainSetup(
    const SELECTOR *gameSelector
)
{
    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("In Circuit Test");
    pinMode(led, OUTPUT);
    digitalWrite(led, LOW);

    delay(2000);

    keypad.setRate(10);

    s_gameSelector = gameSelector;
    s_currentSelector = s_gameSelector;
}

void mainLoop()
{
    int previousKey = SAMPLE_WAIT;

    do {

        int currentKey = keypad.getKey();

        //
        // Special case of the first pass through to park at the first selector.
        //
        if (previousKey == SAMPLE_WAIT)
        {
            currentKey = LEFT_KEY;
        }

        if ( (currentKey == SAMPLE_WAIT) ||
             (currentKey == previousKey) )
        {
            digitalWrite(led, LOW);
            delay(100);
            digitalWrite(led, HIGH);

            continue;
        }

        switch (currentKey)
        {
            case NO_KEY : { break; }

            case LEFT_KEY :
            {
                if (s_currentSelection > 0)
                {
                    s_currentSelection--;
                }

                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print(s_currentSelector[s_currentSelection].description);

                if (s_currentSelector[s_currentSelection].subMenu)
                {
                    PERROR error = s_currentSelector[s_currentSelection].function(
                        s_currentSelector[s_currentSelection].context,
                        NO_KEY );

                    lcd.setCursor(0, 1);
                    lcd.print(error->description);
                }

                break;
            }

            case RIGHT_KEY :
            {
                if (s_currentSelector[s_currentSelection+1].function != NULL)
                {
                    s_currentSelection++;
                }

                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print(s_currentSelector[s_currentSelection].description);

                if (s_currentSelector[s_currentSelection].subMenu)
                {
                    PERROR error = s_currentSelector[s_currentSelection].function(
                        s_currentSelector[s_currentSelection].context,
                        NO_KEY );

                    lcd.setCursor(0, 1);
                    lcd.print(error->description);
                }

                break;
            }

            case UP_KEY     :
            case DOWN_KEY   :
            {
                if (s_currentSelector[s_currentSelection].subMenu)
                {
                    lcd.setCursor(0, 1);
                    lcd.print(BLANK_LINE_16);

                    PERROR error = s_currentSelector[s_currentSelection].function(
                                    s_currentSelector[s_currentSelection].context,
                                    currentKey );

                    lcd.setCursor(0, 1);
                    lcd.print(error->description);
                }
                break;
            }

            case SELECT_KEY :
            {
                unsigned long startTime = millis();
                unsigned long endTime = startTime + ((unsigned long) s_repeatSelectTimeInS * 1000);
                const SELECTOR *inSelector = s_currentSelector;
                PERROR error = errorSuccess;

                lcd.setCursor(0, 1);
                lcd.print(BLANK_LINE_16);

                do {

                    error = s_currentSelector[s_currentSelection].function(
                               s_currentSelector[s_currentSelection].context,
                               currentKey );
                }
                while ( SUCCESS(error) &&                 // No failures.
                        (millis() < endTime) &&           // Times not up.
                        (inSelector != s_gameSelector) ); // The input selector wasn't the game selector.

                //
                // The selection may have changed so update the whole display.
                //
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print(s_currentSelector[s_currentSelection].description);

                lcd.setCursor(0, 1);
                lcd.print(error->description);
            }

            default : { break; };
        }

        previousKey = currentKey;

    } while (1);
}
