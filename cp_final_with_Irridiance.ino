#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "ACS712.h"

// Define constants
#define REFERENCE_VOLTAGE 5.0
#define SENSOR_READING_MULTIPLIER 5.0
#define SENSOR_CALIBRATION_SENSITIVITY 1000.0 / 200.0
#define SENSOR_CALIBRATION_VREF 2500
#define AVERAGE_READINGS 1000

// Define relay pins
#define RELAY_1 2
#define RELAY_2 3
#define RELAY_3 4
#define RELAY_4 5

// Ds18b20 pin define, one wire instance and Dallastemp library setup
const int SENSOR_PIN = 13;
OneWire oneWire(SENSOR_PIN);
DallasTemperature tempSensor(&oneWire);

ACS712 ACS(A0, 5.0, 1023, 66);

#define BPW34_PIN_1 A2
#define BPW34_PIN_2 A3
#define BPW34_PIN_3 A4
#define SAMPLES 16

unsigned int sample_1[SAMPLES];
unsigned int sample_2[SAMPLES];
unsigned int sample_3[SAMPLES];

unsigned char s = 0;
unsigned int avg_1 = 0;
unsigned int avg_2 = 0;
unsigned int avg_3 = 0;
unsigned long sum_1 = 0;
unsigned long sum_2 = 0;
unsigned long sum_3 = 0;
float irValue = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for Serial Monitor to connect
  }
  tempSensor.begin(); // Initialize the temperature sensor
  delay(10000);       // Delay to give time for Serial Monitor to connect

  // Initialize relay pins as outputs
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);
  pinMode(RELAY_4, OUTPUT);
  digitalWrite(RELAY_1, LOW);
  digitalWrite(RELAY_2, LOW);
  digitalWrite(RELAY_3, LOW);
  digitalWrite(RELAY_4, LOW);

  for (unsigned char i = 0; i < SAMPLES; i++) {
    sample_1[i] = 0;
    sample_2[i] = 0;
    sample_3[i] = 0;
  }
}

void loop() {
  int numReadings = 100;
  long sum = 0;

  // Temperature calculation
  float tempCelsius;
  tempSensor.requestTemperatures(); // Send the command to get temperatures
  tempCelsius = tempSensor.getTempCByIndex(0);
  for (int i = 0; i < numReadings; i++) {
    sum += tempCelsius;
    delay(5);
  }
  float tempAverage = sum / numReadings;

  // Print the temperature average
  Serial.print("Temperature: ");
  Serial.print(tempAverage, 2); // Print temperature reading with 2 decimal places
  Serial.println(" C");
  // Calculate average for sensor 1
  sum_1 -= sample_1[s];
  sample_1[s] = analogRead(BPW34_PIN_1);
  sum_1 += sample_1[s];

  // Calculate average for sensor 2
  sum_2 -= sample_2[s];
  sample_2[s] = analogRead(BPW34_PIN_2);
  sum_2 += sample_2[s];

  // Calculate average for sensor 3
  sum_3 -= sample_3[s];
  sample_3[s] = analogRead(BPW34_PIN_3);
  sum_3 += sample_3[s];

  s += 1;

  if (s >= SAMPLES) {
    s = 0;
    avg_1 = (sum_1 >> 4);
    avg_2 = (sum_2 >> 4);
    avg_3 = (sum_3 >> 4);

    irValue = ((16.3366 * avg_1) - 30.69);
    irValue += ((16.3366 * avg_2) - 30.69);
    irValue += ((16.3366 * avg_3) - 30.69);
    irValue /= 3.0;
    irValue *= 0.079;

    if (irValue < 0) {
      irValue = 0;
    }

    Serial.print("Irradiance Meter");
    Serial.print("Ir W/m2: ");
    Serial.println(irValue);
    delay(600);
  }

  // Turn on relay 1
  digitalWrite(RELAY_1, HIGH);
  digitalWrite(RELAY_3, HIGH);
  delay(5000);

  // Measure voltage for relay 1
  float voltage1 = getAnalogReading(A1) * REFERENCE_VOLTAGE / 1023.0 * SENSOR_READING_MULTIPLIER;

  // Measure current for relay 1
  sum = 0;
  for (int i = 0; i < numReadings; i++) {
    sum += ACS.mA_DC();
    delay(10); // Delay between readings
  }
  int current1Average = sum / numReadings;

  // Print the voltage and current averages for relay 1
  Serial.print("Voltage for Relay 1: ");
  Serial.print(voltage1, 2);
  Serial.println(" V");
  Serial.print("Current for Relay 1: ");
  Serial.print(current1Average);
  Serial.println(" mA");

  digitalWrite(RELAY_1, LOW);
  digitalWrite(RELAY_3, LOW);
  delay(3000);

  // Repeat the same process for the other relays...

  // Turn on relay 2
  digitalWrite(RELAY_2, HIGH);
  digitalWrite(RELAY_4, HIGH);
  delay(5000);

  // Measure voltage for relay 2
  float voltage2 = getAnalogReading(A1) * REFERENCE_VOLTAGE / 1023.0 * SENSOR_READING_MULTIPLIER;

  // Measure current for relay 2
  sum = 0;
  for (int i = 0; i < numReadings; i++) {
    sum += ACS.mA_DC();
    delay(10); // Delay between readings
  }
  int current2Average = sum / numReadings;

  // Print the voltage and current averages for relay 2
  Serial.print("Voltage for Relay 2: ");
  Serial.print(voltage2, 2);
  Serial.println(" V");
  Serial.print("Current for Relay 2: ");
  Serial.print(current2Average);
  Serial.println(" mA");

  digitalWrite(RELAY_2, LOW);
  digitalWrite(RELAY_4, LOW);
  delay(3000);

  // // Calculate average for sensor 1
  // sum_1 -= sample_1[s];
  // sample_1[s] = analogRead(BPW34_PIN_1);
  // sum_1 += sample_1[s];

  // // Calculate average for sensor 2
  // sum_2 -= sample_2[s];
  // sample_2[s] = analogRead(BPW34_PIN_2);
  // sum_2 += sample_2[s];

  // // Calculate average for sensor 3
  // sum_3 -= sample_3[s];
  // sample_3[s] = analogRead(BPW34_PIN_3);
  // sum_3 += sample_3[s];

  // s += 1;

  // if (s >= SAMPLES) {
  //   s = 0;
  //   avg_1 = (sum_1 >> 4);
  //   avg_2 = (sum_2 >> 4);
  //   avg_3 = (sum_3 >> 4);

  //   irValue = ((16.3366 * avg_1) - 30.69);
  //   irValue += ((16.3366 * avg_2) - 30.69);
  //   irValue += ((16.3366 * avg_3) - 30.69);
  //   irValue /= 3.0;
  //   irValue *= 0.079;

  //   if (irValue < 0) {
  //     irValue = 0;
  //   }

  //   Serial.print("Irradiance Meter");
  //   Serial.print("Ir W/m2: ");
  //   Serial.println(irValue);
  //   delay(600);
  // }

  delay(1000);
}

// Function to take the average of analog readings
float getAnalogReading(int pin) {
  long sum = 0;
  for (int i = 0; i < AVERAGE_READINGS; i++) {
    sum += analogRead(pin);
    delay(2);
  }
  return (float)sum / AVERAGE_READINGS;
}
