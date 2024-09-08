#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

void setup() {
  Serial.begin(9600);
  if (!mag.begin()) {
    Serial.println("Erreur de connexion avec le HMC5883L");
    while (1);
  }
}

void loop() {
  sensors_event_t event;
  mag.getEvent(&event);
  float heading = atan2(event.magnetic.y, event.magnetic.x);
  if (heading < 0) {
    heading += 2 * PI;
  }
  float headingDegrees = heading * 180 / PI;
  Serial.print("Direction : ");
  Serial.print(headingDegrees);
  Serial.println(" degrés");
  delay(500);
}
