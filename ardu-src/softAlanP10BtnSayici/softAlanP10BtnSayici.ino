/*
  Sketch for P10 led module button counting game.
  Game status and score information is displayed in the P10 module.
*/

#include <DMD2.h>
#include "hc595_spi.h"
#include <EEPROM.h>
#include <SPI.h>
#include <fonts/Arial_Black_16.h>
#include <fonts/SystemFont5x7.h>
//#include <fonts/Arial12.h>

#include "btn_counter_game.h"

SoftDMD dmd(3, 1); // DMD controls the entire display 3 sutun 1 satır
//SoftDMD dmd(2, 1, 9, 8, 7, 4, 5, 3);                        //2 panels width, 1 panel height, 8,7,4,5,3 are my defined pins

//different width TextBoxs
// DMD_TextBox(DMDFrame &dmd, int left = 0, int top = 0, int width = 0, int height = 0);
DMD_TextBox boxOy1Digit3(dmd, 2, 1, 31, 16);
DMD_TextBox boxOy1Digit2(dmd, 6, 1, 25, 16);
DMD_TextBox boxOy1Digit1(dmd, 10, 1, 22, 16);

DMD_TextBox boxOy2Digit3(dmd, 66, 1, 32, 16);
DMD_TextBox boxOy2Digit2(dmd, 69, 1, 25, 16);
DMD_TextBox boxOy2Digit1(dmd, 73, 1, 22, 16);

DMD_TextBox boxSureDigit3(dmd, 36, 1, 28, 16);
DMD_TextBox boxSureDigit2(dmd, 39, 1, 25, 16);
DMD_TextBox boxSureDigit1(dmd, 45, 1, 18, 16);

int oyuncu1Skor = 0;
int oyuncu2Skor = 0;
int maxOySkor = 0;
int sure = 60;
int ee_game_time = 60;
int pushed_btn = -1;
bool oyunBaslat = false;

long currentMillis;
long currentMillisSure;
long currentMillisP10Update;
long currentMillisGameLife;
long currentMillisMaxSkor;
int timeGameOver = 0;
int gameOfLifeDelay = 100;
bool gameLifeDisp = true;

String msg = "";

// the setup routine runs once when you press reset:
void setup()
{
  Serial.begin(9600);

  dmd.setBrightness(100);
  dmd.selectFont(Arial_Black_16);
  dmd.begin();

  Serial.println("begin");

  pinMode(BUZZER_PIN, OUTPUT);

  initBtnPins();
  init_595();

  randomSeed(analogRead(0));

  for (int i = 0; i < 1024; i++)
  {
    writeSPI595(i);
    delay(1);
  }

  gameTimeSetting(); //B1 enter for setting game time

  ledFull();                                                                        //ledClear();
  maxOySkor = word(EEPROM.read(EE_ADR_MAX_SKOR), EEPROM.read(EE_ADR_MAX_SKOR + 1)); //(h, l)
  if (maxOySkor == 0xFFFF)
    maxOySkor = 10;
  ee_game_time = EEPROM.read(EE_ADR_GAME_TIME);
  if (ee_game_time == 0xFF)
    ee_game_time = 60;

  currentMillis = currentMillisP10Update = currentMillisSure = currentMillisMaxSkor = millis();
}

// the loop routine runs over and over again forever:
void loop()
{
  if (gameLifeDisp) gameOfLife();

  pushed_btn = checkPush();

  if (pushed_btn != -1)
  {
    bip_kart_yazildi();

    oyunBaslat = true;
    oyuncu1Skor = oyuncu2Skor = 0;
    dmd.clearScreen();

    gameWorking();
  }
  else
  {
    oyunBaslat = false;
  }

  if ((millis() - currentMillisMaxSkor > 15000) && (millis() - currentMillisMaxSkor < 20000))
  {
    if (millis() - currentMillisMaxSkor < 15500)
    {
      gameLifeDisp = false;
      dmd.selectFont(SystemFont5x7);
      dmd.clearScreen();
      msg = "Oyun REKORU:";
      dmd.drawString(0, 0, msg);
      msg = String(maxOySkor);
      dmd.drawString(40, 8, msg);
    }

    if (millis() - currentMillisMaxSkor > 19500)
    {
      gameLifeDisp = true;
      dmd.fillScreen(true);
    }
  }
  else if ((millis() - currentMillisMaxSkor > 30000) && (millis() - currentMillisMaxSkor < 35000))
  {
    if (millis() - currentMillisMaxSkor < 30500)
    {
      gameLifeDisp = false;
      dmd.selectFont(SystemFont5x7);
      dmd.clearScreen();

      msg = "Bir butona bas,";
      dmd.drawString(3, 0, msg);
      msg = "oyunu oyna...";
      dmd.drawString(3, 8, msg);
    }

    if (millis() - currentMillisMaxSkor > 34500)
    {
      gameLifeDisp = true;
      dmd.fillScreen(true);
    }
  }
  else if (millis() - currentMillisMaxSkor > 35000)
    currentMillisMaxSkor = millis();


  if (millis() - currentMillisGameLife > 15000)
  {
    gameOfLifeDelay = 100;
  }

  //box2.print(F("Index = ")); box2.print(charIndex);

} //loop

void gameWorking()
{
  dmd.selectFont(SystemFont5x7);
  sure = 5;
  while (sure != 0)
  {
    if (!oyunBaslat)
      break;

    if (millis() - currentMillisSure >= 1000)
    {
      boxSureDigit3.clear();
      boxSureDigit2.clear();
      boxSureDigit1.clear();

      if (sure < 10)
        boxSureDigit1.print(sure--);

      bip_tus();

      currentMillisSure = millis();
    }
  }

  sure = ee_game_time;

  while (1)
  {
    randomLed();
    if (!oyunBaslat)
      break;

    pushed_btn = -1;
    timeGameOver = 0;

    while (pushed_btn == (-1))
    {
      if (!oyunBaslat)
        break;

      pushed_btn = checkLedBtn();

      if (millis() - currentMillisSure >= 1000)
      {
        boxSureDigit3.clear();
        boxSureDigit2.clear();
        boxSureDigit1.clear();

        dmd.selectFont(Arial_Black_16);

        if (sure >= 100)
          boxSureDigit3.print(sure--);
        if (sure >= 10 && sure < 100)
          boxSureDigit2.print(sure--);
        if (sure < 10)
          boxSureDigit1.print(sure--);

        if (sure == 0)
        {
          sure = ee_game_time;
          oyunBaslat = false;
        }

        dmd.drawBox(0, 0, 31, 15);  //X1,Y1,X2,Y2  oyuncu 1
        dmd.drawBox(64, 0, 95, 15); // oyuncu 2
        dmd.drawBox(32, 0, 63, 15); //sure
        //  boxSure.print(F("-."));

        if (timeGameOver > 9 && timeGameOver <= 12) {
          dmd.selectFont(SystemFont5x7);
          dmd.clearScreen();
          dmd.drawString(3, 3, " Yoruldun mu?");
        } else if (timeGameOver == 13) {
          dmd.clearScreen();
        }
        else if (timeGameOver > 19)  oyunBaslat = false;
        timeGameOver++;
        currentMillisSure = millis();
      }
    }

    bip_tus();
    
    if (!oyunBaslat) {
      if ((pushed_btn < 5) && (pushed_btn > -1))
      {
        oyuncu1Skor++;
        Serial.print("o1 : ");
        Serial.println(oyuncu1Skor);
      }
      else if (pushed_btn >= 5)
      {
        oyuncu2Skor++;
        Serial.print("o2 : ");
        Serial.println(oyuncu2Skor);
      }
    }


    if (millis() - currentMillisP10Update > 250)
    {
      //dmd.selectFont(SystemFont5x7);
      boxOy1Digit1.clear();
      boxOy1Digit2.clear();
      boxOy1Digit3.clear();

      boxOy2Digit1.clear();
      boxOy2Digit2.clear();
      boxOy2Digit3.clear();

      // boxOy1.print(F(" "));

      if (oyuncu1Skor < 10)
        boxOy1Digit1.print(oyuncu1Skor);
      if (oyuncu1Skor >= 10 && oyuncu1Skor < 100)
        boxOy1Digit2.print(oyuncu1Skor);
      if (oyuncu1Skor > 99)
        boxOy1Digit3.print(oyuncu1Skor);

      // boxOy2.print(F(" "));

      if (oyuncu2Skor < 10)
        boxOy2Digit1.print(oyuncu2Skor);
      if (oyuncu2Skor >= 10 && oyuncu2Skor < 100)
        boxOy2Digit2.print(oyuncu2Skor);
      if (oyuncu2Skor > 99)
        boxOy2Digit3.print(oyuncu2Skor);

      dmd.drawBox(0, 0, 31, 15);  //X1,Y1,X2,Y2  oyuncu 1
      dmd.drawBox(64, 0, 95, 15); // oyuncu 2
      dmd.drawBox(32, 0, 63, 15); //sure

      currentMillisP10Update = millis();
    }
  } //while

  bip_hata();
  delay(200);
  bip_hata();
  delay(200);
  bip_hata();
  delay(200);

  dmd.clearScreen();

  boxOy1Digit3.print(oyuncu1Skor);
  boxOy2Digit3.print(oyuncu2Skor);

  dmd.selectFont(SystemFont5x7);

  for (size_t i = 0; i < 7; i++)
  {
    boxSureDigit3.clear();
    delay(500);
    boxSureDigit3.println("OYUN");
    boxSureDigit3.print("SONU");
    delay(500);
  }

  if ((oyuncu1Skor > oyuncu2Skor) || (oyuncu1Skor == oyuncu2Skor))
  {
    if (oyuncu1Skor > maxOySkor)
      maxOySkor = oyuncu1Skor;
  }
  else if (oyuncu2Skor > oyuncu1Skor)
  {
    if (oyuncu2Skor > maxOySkor)
      maxOySkor = oyuncu2Skor;
  }

  EEPROM.write(EE_ADR_MAX_SKOR, highByte(maxOySkor));
  EEPROM.write(EE_ADR_MAX_SKOR + 1, lowByte(maxOySkor));

  gameOfLifeDelay = 1000;
  ledFull();

  gameLifeDisp = true;
  currentMillisMaxSkor = millis();
  currentMillisGameLife = millis();

} //

void gameTimeSetting()
{

  int count = 0;
  int btn = -1;
  bool saveIt = false;
  String m = "Oyun Sure : ";

  dmd.selectFont(SystemFont5x7);
  do
  {
    btn = checkPush();
    if (btn == 0) //B1
    {
      count++;
      delay(500);
      bip_tus();
    }

    if (count > 5)
    {
      dmd.clearScreen();
      dmd.drawString(1, 3, m + String(ee_game_time));
      delay(3000);

      while (1)
      {

        btn = checkPush();

        if (btn == 0)
        {
          ee_game_time += 5;
          bip_tus();
        }
        if (btn == 2)
        {
          ee_game_time -= 5;
          bip_tus();
        }

        if (ee_game_time < 5)
          ee_game_time = 5;
        if (ee_game_time > 250)
          ee_game_time = 250;

        dmd.drawString(1, 3, m + String(ee_game_time));

        if (btn == 1)
        {
          bip_tus();
          saveIt = true;
          delay(1000);
          break;
        }
      }

      if (saveIt)
      {
        EEPROM.write(EE_ADR_GAME_TIME, lowByte(ee_game_time));

        dmd.clearScreen();
        dmd.drawString(1, 3, "-.-. OK .-.-");
        delay(2000);
        dmd.fillScreen(true);
        break;
      }
    }
  } while (btn != -1);
}

void populate_random_cells()
{
  // Populate the initial display randomly
  for (int x = 0; x < dmd.width; x++)
  {
    for (int y = 0; y < dmd.height; y++)
    {
      if (random(100) < 30) // Increase 30 to a higher number to set more initial pixels
        dmd.setPixel(x, y, GRAPHICS_ON);
    }
  }
}

void gameOfLife()
{
  DMDFrame current_generation(dmd);

  long start = millis();

  // Update next generation of every pixel
  bool change = false;

  for (int x = 0; x < dmd.width; x++)
  {
    for (int y = 0; y < dmd.height; y++)
    {
      bool state = current_generation.getPixel(x, y);
      int live_neighbours = 0;

      // Count how many live neighbours we have in the current generation
      for (int nx = x - 1; nx < x + 2; nx++)
      {
        for (int ny = y - 1; ny < y + 2; ny++)
        {
          if (nx == x && ny == y)
            continue;
          if (current_generation.getPixel(nx, ny))
            live_neighbours++;
        }
      }

      // Update pixel count for the next generation
      if (state && (live_neighbours < 2 || live_neighbours > 3))
      {
        state = false;
        change = true;
      }
      else if (!state && (live_neighbours == 3))
      {
        state = true;
        change = true;
      }
      dmd.setPixel(x, y, state ? GRAPHICS_ON : GRAPHICS_OFF);
    }
  }

  //Serial.println(String("Generation time: ") + (millis() - start) + " ms");

  if (!change)
  {
    // We've made it to an unchanging state
    delay(gameOfLifeDelay);
    populate_random_cells();
    // (We can't detect steady states where things change forward
    // and back, for these you need to press reset!)
  }
}
