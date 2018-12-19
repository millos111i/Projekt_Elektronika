#include <hd44780.h>
#include <Wire.h>
#include "RTClib.h"
#include <Keypad.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

hd44780_I2Cexp lcd;

int hourNow();

RTC_DS1307 RTC;
const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns
// Define the Keymap
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = { 5, 4, 3 };
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

bool alarmIsOn = false;
bool alarmIsActive = false;
bool backlightIsOn = true;

int loops = 0;

short alarmHour = 0, alarmMinute = 0, alarmSecond = 0;

void setup() {

  pinMode(11, OUTPUT);

  // put your setup code here, to run once:
  lcd.begin(16, 2);
  Wire.begin();
  RTC.begin();
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // This will reflect the time that your sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  Serial.begin(9600);
}

void loop() {
  if (loops % 5 == 0) {
    showTime();
    checkAlarm();
  }
  //  turnAlarmOn();
  getKeypadInput();
  delay(200);
  loops++;
  if (loops == 100) loops = 0;
}

String numForm(int num) {
  if (num < 10) {
    return "0" + String(num, DEC);
  } else {
    return String(num, DEC);
  }
}
void showTime() {
  DateTime now = RTC.now();
  //Czas zimowy
  
  
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print(numForm(hourNow()));
  lcd.print(":");
  lcd.print(numForm(now.minute()));
  lcd.print(":");
  lcd.print(numForm(now.second()));

  lcd.setCursor(3, 1);
  lcd.print(numForm(now.day()));
  lcd.print("-");
  lcd.print(numForm(now.month()));
  lcd.print("-");
  lcd.print(numForm(now.year()));
  lcd.setCursor(15 , 0);
  if (alarmIsActive)  lcd.print('@');
}
void turnAlarmOn() {
  digitalWrite(11, HIGH);
  delay(100);
  digitalWrite(11, LOW);
  delay(100);

  digitalWrite(11, HIGH);
  delay(100);
  digitalWrite(11, LOW);
  delay(100);

  digitalWrite(11, HIGH);
  delay(100);
  digitalWrite(11, LOW);
}
void checkAlarm() {
  if (alarmIsActive) {
    DateTime now = RTC.now();
    if (alarmHour == hourNow() && alarmMinute == now.minute() && alarmSecond == now.second()) {
      turnAlarmOn();
      alarmIsOn = true;
    }
  }
  if(alarmIsOn){
    turnAlarmOn();
  }
}

void getKeypadInput() {
  char z = kpd.getKey();
  switch (z) {
    case '*':
      if (alarmIsOn) {
        alarmIsOn = false;
      } else {
        alarmIsActive = !alarmIsActive;
        lcd.clear();
        lcd.print("Alarm zostal ");
        lcd.setCursor(0, 1);
        lcd.print(alarmIsActive ? "wlaczony" : "wylaczony");
        delay(1000);
      }
      break;
    case '#':
      setAlarm();
      break;
    case '0':
      backlightIsOn = !backlightIsOn;
      if(backlightIsOn){
        lcd.backlight();
      } else {
        lcd.noBacklight();
      }
      break;
  }
}

void setAlarm() {
  lcd.clear();
  lcd.print("Podaj godz:");
  int h = getNumber();
  if (h > 23) {
    h = 0;
  }
  lcd.clear();
  lcd.print("Podaj minute:");
  int m = getNumber();
  if (m > 59) {
    m = 0;
  }
  lcd.clear();
  lcd.print("Podaj sekundy:");
  int s = getNumber();
  if (s > 59) {
    s = 0;
  }
  alarmHour = h;
  alarmMinute = m;
  alarmSecond = s;

  alarmIsActive = true;
}

int getNumberFromChar(char c) {
  if (c != '*' && c != '#') {
    return ((int)c - (int)'0');
  } else {
    return -1;
  }
}

int getNumber() {
  int n = 0;
  char c = NO_KEY;
  while (c == NO_KEY) {
    c = kpd.getKey();
  }
  switch (c) {
    case '#':
      return -1;
      break;
    default:
      n = getNumberFromChar(c);
      lcd.setCursor(0, 1);
      lcd.print(c);
      break;
  }
  c = NO_KEY;
  while (c == NO_KEY) {
    c = kpd.getKey();
  }
  switch (c) {
    case '*':
      return n;
      break;
    case '#':
      return -1;
      break;
    default:
      n *= 10;
      n += getNumberFromChar(c);
      lcd.setCursor(1, 1);
      lcd.print(c);
      break;
  }
  c = NO_KEY;
  while (c == NO_KEY) {
    c = kpd.getKey();
  }
  switch (c) {
    case '*':
      return n;
      break;
    case '#':
      return -1;
      break;
  }
}

int hourNow(){
//  if((RTC.now().day() >= 28 && RTC.now().month() == 10) || (RTC.now().month()>10 && RTC.now().month()<= 12) || (RTC.now().month() >= 1 && RTC.now().month() < 4)) {
//    return RTC.now().hour() + 1;
//  } 
  return RTC.now().hour();
}
