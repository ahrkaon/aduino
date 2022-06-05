#include <LiquidCrystal.h>
#include <Servo.h>
#include <Wire.h>
#include <DHT.h>
#include "RTClib.h"

// 워터펌프 핀
#define water1 13
#define water2 12

// 서보모터 핀
#define servopin 25
Servo servo;
// 서보모터 초기 각도
int angle = 30;

// DHT22 
#define dhtpin 2
#define DhtType DHT22
DHT dht(dhtpin, DhtType);

// LCD(주소, 가로, 세로)
// 주소확인 필요
LiquidCrystal lcd(22,24, 5, 4, 3, 2);

// RTC
RTC_DS3231 rtc;
// 요일 이름
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// 쿨링팬
#define fan 3
// 수위측정센서 최저,최대치
int Min_water = 300;
int Max_water = 600;

/*void lcd_show(int* hum, int* temp, int* water_level)
{
  lcd.setCursor(0,0);
  lcd.print("temp : ");
  lcd.print(*pTemp);
  lcd.setCursor(0,1);
  lcd.print("hum : ");
  lcd.print(*pHum);
  delay(1000);
  lcd.clear();
  delay(300);
  lcd.setCursor(0,0);
  lcd.print("water : ");
  lcd.print(*pWater_level);
  delay(1000);
  lcd.clear();
  delay(300);
}
*/
void watch()
{
    DateTime now = rtc.now();

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    int aa = now.second();
    if(aa > 30 && 34 >aa)
    {
      servo.write(angle);
      Serial.print("servo working"); 
      Serial.println(); 
      delay(300);
      servo.write(90);
    }
    Serial.println();
    delay(1000);
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  // LCD 셋팅
  /*lcd.Begin(16, 2);
  lcd.backlight();
  */
  // 서보모터 할당
  servo.attach(servopin);
  
  // 워터모터 핀
  pinMode(water1, OUTPUT);
  pinMode(water2, OUTPUT);

  // 쿨링팬
  pinMode(fan, OUTPUT);

  // RTC 모듈
#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  // Rtc모듈의 배터리 부족 시
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // Rtc모듈에 컴파일 당시의 시간을 기록
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

}

void loop() {
  // DHT 습도, 온도 측정
  delay(1000);
  int hum = dht.readHumidity();
  int* pHum = &hum;
  int temp = dht.readTemperature();
  int* pTemp = &temp;
  Serial.print("Hum:");
  Serial.print(*pHum);
  Serial.print(" ");
  Serial.print("Temp:");
  Serial.print(*pTemp);
  Serial.println();
  // 수위측정센서
  int water_level = analogRead(A0);
  int* pWater_level = &water_level;
  // 센서값 lcd출력
  // lcd_show(pHum, pTemp, pWater_level);
  // 수위에 따른 워터펌프 on/off
  if(Min_water > water_level)
  {
    digitalWrite(water1, HIGH);
  }
  else if(Max_water <= water_level)
  {
    digitalWrite(water1, LOW);
  }
  // 온도에 따른 쿨링팬 작동
  if(*pTemp > 30)
  {
    digitalWrite(fan, HIGH);
    Serial.print("fan working");
  }
  else if(*pTemp <= 30)
  {
    digitalWrite(fan, LOW);
  }

  // 먹이기계
  // RTC + 서보모터
  watch();

}
