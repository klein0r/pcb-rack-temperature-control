#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define LED_PIN_R 5
#define LED_PIN_Y 6
#define LED_PIN_G 7
#define FAN_PIN 9
#define REED_PIN 13

#define SHIFT_PIN 11 // SH_CP
#define STORE_PIN 10 // ST_CP
#define DATA_PIN 12  // DS

#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

bool numbers[10][8] = {
  {HIGH, LOW, HIGH, HIGH, HIGH, LOW, HIGH, HIGH},
  {LOW, LOW, LOW, LOW, HIGH, LOW, LOW, HIGH},
  {LOW, HIGH, HIGH, HIGH, LOW, LOW, HIGH, HIGH},
  {LOW, HIGH, HIGH, LOW, HIGH, LOW, HIGH, HIGH},
  {HIGH, HIGH, LOW, LOW, HIGH, LOW, LOW, HIGH},
  {HIGH, HIGH, HIGH, LOW, HIGH, LOW, HIGH, LOW},
  {HIGH, HIGH, HIGH, HIGH, HIGH, LOW, HIGH, LOW},
  {LOW, LOW, LOW, LOW, HIGH, LOW, HIGH, HIGH},
  {HIGH, HIGH, HIGH, HIGH, HIGH, LOW, HIGH, HIGH},
  {HIGH, HIGH, HIGH, LOW, HIGH, LOW, HIGH, HIGH},
};

int n = 0;
int temperature = 0;

void writeDigit(int digit) {
  if (digit >= 0 && digit <= 9) {
    for (int i = 0; i < 8; i++) {
      digitalWrite(SHIFT_PIN, LOW);
      digitalWrite(DATA_PIN, numbers[digit][i]);
      digitalWrite(SHIFT_PIN, HIGH);
    }
  }
}

void blackDisplay()
{
  digitalWrite(STORE_PIN, LOW);
  for (int i = 0; i < 16; i++) {
    digitalWrite(SHIFT_PIN, LOW);
    digitalWrite(DATA_PIN, LOW);
    digitalWrite(SHIFT_PIN, HIGH);
  }
  digitalWrite(STORE_PIN, HIGH);
}

void writeNumber(int number) {
  if (number >= 0 && number < 100) {
    digitalWrite(STORE_PIN, LOW);
    if (number < 10) {
      writeDigit(0);
      writeDigit(number);
    } else {
      writeDigit(floor(number / 10));
      writeDigit(number % 10);
    }
    digitalWrite(STORE_PIN, HIGH);
  }
}

void setup() {
  Serial.begin(9600);
  TCCR1B = TCCR1B & 0b11111000 | 0x01;

  sensors.begin();

  pinMode(LED_PIN_G, OUTPUT);
  pinMode(LED_PIN_Y, OUTPUT);
  pinMode(LED_PIN_R, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(FAN_PIN, OUTPUT);
  pinMode(REED_PIN, INPUT_PULLUP);

  pinMode(SHIFT_PIN, OUTPUT);
  pinMode(STORE_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  blackDisplay();

  digitalWrite(STORE_PIN, LOW);

  writeNumber(99);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);

  sensors.requestTemperatures();
  temperature = floor(sensors.getTempCByIndex(0));

  writeNumber(temperature);

  if (temperature < 25) {
    analogWrite(FAN_PIN, 0);
    digitalWrite(LED_PIN_R, LOW);
    digitalWrite(LED_PIN_Y, LOW);
    digitalWrite(LED_PIN_G, HIGH);
  } else if (temperature > 30) {
    analogWrite(FAN_PIN, 255);
    digitalWrite(LED_PIN_R, HIGH);
    digitalWrite(LED_PIN_Y, LOW);
    digitalWrite(LED_PIN_G, LOW);
  } else {
    analogWrite(FAN_PIN, 128);
    digitalWrite(LED_PIN_R, LOW);
    digitalWrite(LED_PIN_Y, HIGH);
    digitalWrite(LED_PIN_G, LOW);
  }

  delay(1000);
}
