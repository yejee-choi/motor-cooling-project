volatile int pulseCount = 0;
unsigned long lastTime = 0;
float rpm = 0;

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);  // 홀 센서 출력핀 (D2)  
  attachInterrupt(digitalPinToInterrupt(2), countPulse, FALLING);
  lastTime = millis();
}

void loop() {
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= 1000) {  // 1초마다 RPM 계산
    rpm = (pulseCount * 60.0);  // 자석 1개 기준, 1초 동안 펄스 수 * 60 = RPM
    Serial.print("RPM: ");
    Serial.println(rpm);
    pulseCount = 0;
    lastTime = currentTime;
  }
}

void countPulse() {
  pulseCount++;
}