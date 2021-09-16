
#if !defined(BTN_COUNTER_GAME_H)
#define BTN_COUNTER_GAME_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define IN_OUT_PINS 10
#define BUZZER_PIN 5

#define EE_ADR_MAX_SKOR 0

#define EE_ADR_GAME_TIME 5


#define bip_kart_yazildi() bipp(10, 20, BUZZER_PIN) //sure,sayi,pin
#define bip_kart_bulundu() bipp(50, 2, BUZZER_PIN) //sure,sayi,pin
#define bip_pic_kurulum() bipp(25, 2, BUZZER_PIN)  //sure,sayi,pin
#define bip_hata() bipp(3, 100, BUZZER_PIN)
#define bip_tus() bipp(10, 1, BUZZER_PIN) //sure,sayi,pin  5,2

//L1,L2,L3      ...             L10
const uint16_t LED_DEGERLER[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512};

const int BUTTON_PINS[10] = {A0, A1, A2, A3, A4, A5, A6, A7, 10, 12};

void randomArray();
void randomLed();
int checkPush();
int checkLedBtn();

void bipp(int sure, int sayi, int pin);

void initBtnPins();
void testLed(int sure);
void testLedBtn();

#endif // BTN_COUNTER_GAME_H