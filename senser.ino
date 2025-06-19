#include <DHT.h> //온습도
#include <Wire.h>
#include "RTClib.h"
#include <SD.h>

// 핀 설정
#define MOTOR_HALL_PIN 2 //모터 RPM
#define WHEEL_IR_PIN   3 //타이어 RPM
#define DHTPIN         4 //온습도
#define DHTTYPE        DHT22
#define LM35_PIN       A0 //모터 온도
#define SD_CS_PIN      10 //SD카드

DHT dht(DHTPIN, DHTTYPE);
RTC_DS3231 rtc;

volatile int motorPulses = 0;
volatile int wheelPulses = 0;
unsigned long lastMillis = 0;

float motorRPM = 0;
float wheelRPM = 0;
const int motorMagnets = 1;
const int wheelReflectors = 1;

File dataFile;

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(MOTOR_HALL_PIN, INPUT_PULLUP);
  pinMode(WHEEL_IR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(MOTOR_HALL_PIN), countMotorPulse, FALLING);
  attachInterrupt(digitalPinToInterrupt(WHEEL_IR_PIN), countWheelPulse, FALLING);

  if (!rtc.begin()) {
    Serial.println("RTC not found!");
    while (1);
  }

  // SD 카드 초기화
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD 카드 초기화 실패!");
    while (1);
  }

  // 첫 실행 시 헤더 작성
  dataFile = SD.open("log.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println("시간,모터RPM,바퀴RPM,모터온도(℃),외부온도(℃),습도(%)");
    dataFile.close();
  }
}

void loop() {
  if (millis() - lastMillis >= 1000) {
    detachInterrupt(digitalPinToInterrupt(MOTOR_HALL_PIN));
    detachInterrupt(digitalPinToInterrupt(WHEEL_IR_PIN));

    motorRPM = (motorPulses * 60.0) / motorMagnets;
    wheelRPM = (wheelPulses * 60.0) / wheelReflectors;
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    int lm35Value = analogRead(LM35_PIN);
    float motorTemp = lm35Value * (5.0 / 1023.0) * 100.0;
    DateTime now = rtc.now();

    // 시리얼 출력
    Serial.print("시간: ");
    Serial.print(now.timestamp(DateTime::TIMESTAMP_TIME));
    Serial.print(" | 모터 RPM: "); Serial.print(motorRPM);
    Serial.print(" | 바퀴 RPM: "); Serial.print(wheelRPM);
    Serial.print(" | 모터 온도: "); Serial.print(motorTemp); Serial.print(" °C");
    Serial.print(" | 외부 온도: "); Serial.print(temperature); Serial.print(" °C");
    Serial.print(" | 습도: "); Serial.print(humidity); Serial.println(" %");

    // CSV 파일 저장
    dataFile = SD.open("log.csv", FILE_WRITE);
    if (dataFile) {
      dataFile.print(now.timestamp(DateTime::TIMESTAMP_TIME)); dataFile.print(",");
      dataFile.print(motorRPM); dataFile.print(",");
      dataFile.print(wheelRPM); dataFile.print(",");
      dataFile.print(motorTemp); dataFile.print(",");
      dataFile.print(temperature); dataFile.print(",");
      dataFile.println(humidity);
      dataFile.close();
    } else {
      Serial.println("파일 저장 실패!");
    }

    motorPulses = 0;
    wheelPulses = 0;
    lastMillis = millis();

    attachInterrupt(digitalPinToInterrupt(MOTOR_HALL_PIN), countMotorPulse, FALLING);
    attachInterrupt(digitalPinToInterrupt(WHEEL_IR_PIN), countWheelPulse, FALLING);
  }
}

void countMotorPulse() {
  motorPulses++;
}

void countWheelPulse() {
  wheelPulses++;
}