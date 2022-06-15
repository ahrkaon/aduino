#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Wire.h>
#include <DHT.h>
#include "RTClib.h"

// 워터펌프 어항->화분핀
#define water1a 7
#define water1b 5
// 워터펌프 예비탱크->어항핀
#define water2a 11
#define water2b 10

// 서보모터 핀
#define servopin 25
Servo servo;
// 서보모터 초기 각도
int angle = 0;

// DHT22 
#define dhtpin 2
#define DhtType DHT22
DHT dht(dhtpin, DhtType);

// LCD(주소, 가로, 세로)
// 주소확인 필요
LiquidCrystal_I2C lcd(0x27, 16, 2);

// RTC
RTC_DS3231 rtc;
// 요일 이름
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// 쿨링팬
#define fan 3
// 수위측정센서 최저,최대치
int Min_water = 100;
int Max_water = 180;

void lcd_show(int* temp, int* hum, int* water)
{

  lcd.setCursor(0,0);
  lcd.print("temp : ");
  lcd.print(*temp);
  lcd.setCursor(0,1);
  lcd.print("hum : ");
  lcd.print(*hum);
  delay(500);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("water : ");
  lcd.print(*water);
  delay(500);
  lcd.clear();
}

void watch()
{
    DateTime now = rtc.now();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(now.year(), DEC);
    lcd.print('/');
    lcd.print(now.month(), DEC);
    lcd.print('/');
    lcd.print(now.day(), DEC);
    lcd.setCursor(0,1);
    lcd.print(" (");
    lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
    lcd.print(") ");
    delay(500);
    lcd.clear();
    
    lcd.setCursor(0,0);
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    lcd.print(now.minute(), DEC);
    lcd.print(':');
    lcd.print(now.second(), DEC);
    delay(500);
    lcd.clear();
    int mysecond = now.second();
    int myminute = now.minute();
    if(mysecond < 32 && mysecond > 29)
    {
      servo.write(angle);
      Serial.println("servo working"); 
      delay(300);
      servo.write(90);
    }
    Serial.println();
}

void setup() {
  Serial.begin(9600);
  //dht모듈 시작
  dht.begin();
  // LCD 셋팅
  lcd.init();
  lcd.backlight();
  // 서보모터 할당
  servo.attach(servopin);
  
  // 워터모터 핀
  pinMode(water1a, OUTPUT);
  pinMode(water1b, OUTPUT);
  pinMode(water2a, OUTPUT);
  pinMode(water2b, OUTPUT);

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
  int hum = dht.readHumidity();
  int* pHum = &hum;
  int temp = dht.readTemperature();
  int* pTemp = &temp;
  // 수위측정센서
  int water_level = analogRead(A0);
  int* pWater_level = &water_level;
  Serial.print(water_level);
  // 먹이기계
  // RTC + 서보모터
  watch();
  // 센서값 lcd출력
  lcd_show(pHum, pTemp, pWater_level);
  
  // 수위에 따른 워터펌프 on/off
  // 수위최저치 : 펌프작동
  // 수위최대치 : 펌프정지
  if(Min_water > *pWater_level)
  {
    digitalWrite(water1a, HIGH);
    digitalWrite(water1b, LOW);
    digitalWrite(water2a, HIGH);
    digitalWrite(water2b, LOW);
    
  }
  else if(Max_water <= *pWater_level)
  {
    digitalWrite(water1a, LOW);
    digitalWrite(water1b, LOW);
    digitalWrite(water2a, LOW);
    digitalWrite(water2b, LOW);
  }
  // 온도에 따른 쿨링팬 작동
  if(temp > 28)
  {
    digitalWrite(fan, HIGH);
    Serial.print("fan working");
  }
  else if(temp <= 28)
  {
    digitalWrite(fan, LOW);
  }

  

}
