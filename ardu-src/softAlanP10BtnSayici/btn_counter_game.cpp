#include "btn_counter_game.h"
#include "hc595_spi.h"

unsigned int rndArr[10] = {0};
unsigned int bufRndLed = 0;

void initBtnPins()
{

    for (int i = 0; i < IN_OUT_PINS; i++)
    {
        pinMode(BUTTON_PINS[i], INPUT);
    }

   
    
}

void randomArray()
{
    memset(rndArr, 0, sizeof(rndArr));

    for (size_t i = 0; i < IN_OUT_PINS; i++)
    {
        int randNumber = random(0, 9);
        rndArr[i] = randNumber;
    }
    // Serial.println(randNumber);
}

void randomLed()
{
    randomArray();
    bufRndLed = 0x0000;
    for (size_t i = 0; i < IN_OUT_PINS; i++)
    {
        bitSet(bufRndLed, rndArr[i]);
    }
    writeSPI595(bufRndLed);
}

int checkLedBtn()
{
    int btn = checkPush(); // return btn number
    
    for (size_t i = 0; i < IN_OUT_PINS; i++)
    {
        if (rndArr[i] == btn)
            return btn;
    }

    return (-1);
}

int checkPush()
{
    for (int x = 0; x < 8; x++)
    {
        int anBtn = analogRead(BUTTON_PINS[x]);
        if (anBtn < 1000)
        {
            delay(200);
            return x;
        }
    }

    if (!digitalRead(BUTTON_PINS[8]))
    {
        delay(200);
        return 8;
    }
    else if (!digitalRead(BUTTON_PINS[9]))
    {
        delay(200);
        return 9;
    }

    return (-1);
}

void bipp(int sure, int sayi, int pin) {
    int a = 0;
    for (a = 0; a < sayi; a++) {
        digitalWrite(pin, HIGH);
        delay(sure / 2);
        digitalWrite(pin, LOW); 
        delay(sure / 2);
    }
    
}//

void testLed(int sure)
{
    for (int x = 0; x < 10; x++)
    {
        //pow(11, 3); // 11 sayısının üçüncü kuvvetini aldık (11 üzeri 3);
        //say=(uint16_t)pow(0x0002,x);
        writeSPI595(LED_DEGERLER[x]);
        Serial.println(LED_DEGERLER[x]);
        delay(sure);
    }
}

void testLedBtn()
{
    int btn = checkPush();

    if (btn != -1)
    {
        writeSPI595(LED_DEGERLER[btn]);
        delay(500);
    }

    //  if(!digitalRead(12)){
    //    writeSPI595(LED_DEGERLER[9]);  //led 10
    //    delay(500);
    //  }else if(!digitalRead(10)){
    //    writeSPI595(LED_DEGERLER[8]);
    //    delay(500);
    //  }
    //  /* analog*/
    //  else if(analogRead(A7)<1000){
    //    writeSPI595(LED_DEGERLER[7]);
    //    delay(500);
    //  }else if(analogRead(A6)<1000){
    //    writeSPI595(LED_DEGERLER[6]);
    //    delay(500);
    //  }else if(analogRead(A5)<1000){
    //    writeSPI595(LED_DEGERLER[5]);
    //    delay(500);
    //  }else if(analogRead(A4)<1000){
    //    writeSPI595(LED_DEGERLER[4]);
    //    delay(500);
    //  }else if(analogRead(A3)<1000){
    //    writeSPI595(LED_DEGERLER[3]);
    //    delay(500);
    //  }else if(analogRead(A2)<1000){
    //    writeSPI595(LED_DEGERLER[2]);
    //    delay(500);
    //  }else if(analogRead(A1)<1000){
    //    writeSPI595(LED_DEGERLER[1]);
    //    delay(500);
    //  }else if(analogRead(A0)<1000){
    //    writeSPI595(LED_DEGERLER[0]);
    //    delay(500);
    //  }

    ledClear();
}
