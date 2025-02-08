#include <SoftwareSerial.h>
#include <BlynkSimpleSerialBLE.h>

char auth[] = "Your_Blynk_Auth_Token";
SoftwareSerial DebugSerial(2, 3); // RX, TX for Bluetooth module
#define RELAY_PIN 4
#define FLOW_SENSOR 5

volatile int pulseCount;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
unsigned long oldTime;

void pulseCounter() {
  pulseCount++;
}

BLYNK_WRITE(V1) {
  int value = param.asInt();
  digitalWrite(RELAY_PIN, value);
}

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  pinMode(FLOW_SENSOR, INPUT);
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR), pulseCounter, FALLING);

  Serial.begin(9600);
  DebugSerial.begin(9600);
  Blynk.begin(DebugSerial, auth);

  pulseCount = 0;
  oldTime = millis();
}

void loop() {
  Blynk.run();

  if (millis() - oldTime > 1000) {
    detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR));

    flowRate = (pulseCount / 7.5);
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;

    Blynk.virtualWrite(V2, flowRate);
    Blynk.virtualWrite(V3, totalMilliLitres);

    pulseCount = 0;
    oldTime = millis();
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR), pulseCounter, FALLING);
  }
}