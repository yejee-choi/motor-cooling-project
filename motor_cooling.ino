// 핀 설정
const int tempPin = A0;         // 온도 센서 핀 (예: LM35)
const int fanPin = 9;           // 팬 제어 핀 (릴레이 or 트랜지스터 사용)
const float threshold = 30.0;   // 온도 기준값 (℃)

void setup() {
  Serial.begin(9600);           // 시리얼 모니터 확인용
  pinMode(fanPin, OUTPUT);      // 팬 핀 출력 설정
  digitalWrite(fanPin, LOW);    // 팬 초기 OFF
}

void loop() {
  int sensorValue = analogRead(tempPin);            // 센서 값 읽기
  float voltage = sensorValue * (5.0 / 1023.0);     // 전압 변환
  float temperature = voltage * 100.0;              // ℃ 변환 (LM35 기준)

  Serial.print("온도: ");
  Serial.print(temperature);
  Serial.println(" °C");

  if (temperature >= threshold) {
    digitalWrite(fanPin, HIGH);   // 팬 작동
    Serial.println("팬 ON");
  } else {
    digitalWrite(fanPin, LOW);    // 팬 정지
    Serial.println("팬 OFF");
  }

  delay(1000); // 1초마다 측정
}