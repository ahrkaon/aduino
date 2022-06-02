#include <LiquidCrystal.h>
#include <Servo.h>
#include <Wire.h>
#include <DHT.h>
#include <RTClib.h>

// 워터펌프 핀
#define water1 13
#define water2 12

// 서보모터 핀
#define servopin 11
Servo servo;
// 서보모터 초기 각도
int angle = 0;

// DHT22 
#define dhtpin 10
#define DhtType dht22
DHT dht(dhtpin, DhtType);

// LCD(주소, 가로, 세로)
// 주소확인 필요
LiquidCrystal lcd(22,24, 5, 4, 3, 2);

// RTC
ThreeWire myWire(4,5,2);
RtcDS1302<ThreeWire> Rtc(myWire);

// 수위측정센서 최저,최대치
int Min_water = 300;
int Max_water = 600;

void lcd_show(int* hum, int* temp, int* water_level)
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

void setup() {
  Serial.begin(9600);
  Rtc.Begin();
  
  // LCD 셋팅
  lcd.Begin(16, 2);
  lcd.backlight();
  
  // 서보모터 할당
  servo.attach(servopin);
  
  // 워터모터 핀
  pinMode(water1, OUTPUT);
  pinMode(water2, OUTPUT);

  // RTC 모듈
  

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
  // 센서값 lcd출력
  lcd_show(pHum, pTemp, pWater_level);
  // 수위에 따른 워터펌프 on/off
  if(Min_water > water_level)
  {
    digitalWrite(water1, HIGH);
  }
  else if(Max_water <= water_level)
  {
    digitalWrite(water1, LOW);
  }

  // 먹이기계
  // RTC + 서보모터
  RtcDateTime now = Rtc.GetDateTime();
  

}
