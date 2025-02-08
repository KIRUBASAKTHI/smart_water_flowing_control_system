#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "Your_Blynk_Auth_Token";   // Enter Blynk authentication token
char ssid[] = "Your_WiFi_SSID";          // Enter WiFi name
char pass[] = "Your_WiFi_Password";      // Enter WiFi password

#define RELAY_PIN D1    // Relay connected to D1 (GPIO5)
#define FLOW_SENSOR D2  // Water Flow Sensor input

volatile int pulseCount;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
unsigned long oldTime;

void IRAM_ATTR pulseCounter() {
  pulseCount++;
}

BLYNK_WRITE(V1) {
  int value = param.asInt();
  digitalWrite(RELAY_PIN, value);
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  pinMode(FLOW_SENSOR, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR), pulseCounter, FALLING);

  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  oldTime = millis();
}

void loop() {
  Blynk.run();

  if (millis() - oldTime > 1000) {
    detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR));

    flowRate = (pulseCount / 7.5);  // Flow rate in L/min
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;

    Serial.print("Flow rate: ");
    Serial.print(flowRate);
    Serial.println(" L/min");

    Serial.print("Total: ");
    Serial.print(totalMilliLitres);
    Serial.println(" mL");

    Blynk.virtualWrite(V2, flowRate);
    Blynk.virtualWrite(V3, totalMilliLitres);

    pulseCount = 0;
    oldTime = millis();
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR), pulseCounter, FALLING);
  }
}