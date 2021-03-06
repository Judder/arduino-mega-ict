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
#include <CBus.h>
#include <CFast8BitBus.h>
#include <CFastPin.h>
#include <CGame.h>
#include <CGameCallback.h>
#include <CRamCheck.h>
#include <CRomCheck.h>
#include <Error.h>
#include <ICpu.h>
#include <IGame.h>
#include <main.h>
#include <PinMap.h>
#include <Types.h>

#include <LiquidCrystal.h>
#include <main.h>
#include <DFR_Key.h>
#include <zutil.h>

#include <CGalaxianGame.h>
#include <CJackrabbitGame.h>
#include <CScrambleGame.h>

//
// The initial selector to select the game to test.
//
static SELECTOR s_gameSelector[] = {//0123456789abcde
                                    {"- Set Repeat   ",  onSelectConfig, (void*) (&s_repeatSelectTimeInS),               false},
                                    {"Galaxian   (M1)",  onSelectGame,   (void*) (CGalaxianGame::createInstanceSet1),    false},
                                    {"Galaxian   (M2)",  onSelectGame,   (void*) (CGalaxianGame::createInstanceSet2),    false},
                                    {"Jackrabbit  (1)",  onSelectGame,   (void*) (CJackrabbitGame::createInstanceSet1),  false},
                                    {"Jackrabbit  (2)",  onSelectGame,   (void*) (CJackrabbitGame::createInstanceSet2),  false},
                                    {"Scramble   (S1)",  onSelectGame,   (void*) (CScrambleGame::createInstanceSet1),    false},
                                    { 0, 0 }
                                   };

void setup()
{
  mainSetup(s_gameSelector);
}

void loop()
{
  mainLoop();
}

