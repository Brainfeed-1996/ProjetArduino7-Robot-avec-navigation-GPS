#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

TinyGPSPlus gps;
SoftwareSerial ss(4, 3); // RX, TX
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

#define trigPin 9
#define echoPin 10
#define motor1Pin1 2
#define motor1Pin2 3
#define motor2Pin1 4
#define motor2Pin2 5

float targetLat = 48.117266;  // Latitude cible
float targetLon = -1.677792;  // Longitude cible

void setup() {
  Serial.begin(9600);
  ss.begin(9600);
  if (!mag.begin()) {
    Serial.println("Erreur de connexion avec le HMC5883L");
    while (1);
  }
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
}

void loop() {
  while (ss.available() > 0) {
    gps.encode(ss.read());
    if (gps.location.isUpdated()) {
      float distanceToTarget = gps.distanceBetween(gps.location.lat(), gps.location.lng(), targetLat, targetLon);
      float angleToTarget = gps.courseTo(gps.location.lat(), gps.location.lng(), targetLat, targetLon);
      
      sensors_event_t event;
      mag.getEvent(&event);
      float heading = atan2(event.magnetic.y, event.magnetic.x);
      if (heading < 0) {
        heading += 2 * PI;
      }
      float headingDegrees = heading * 180 / PI;

      float turnAngle = angleToTarget - headingDegrees;
      if (turnAngle < -180) turnAngle += 360;
      if (turnAngle > 180) turnAngle -= 360;

      if (distanceToTarget > 5) {  // Si la distance à la cible est supérieure à 5 mètres
        if (turnAngle > 10) {
          // Tourner à droite
          digitalWrite(motor1Pin1, HIGH);
          digitalWrite(motor1Pin2, LOW);
          digitalWrite(motor2Pin1, LOW);
          digitalWrite(motor2Pin2, HIGH);
        } else if (turnAngle < -10) {
          // Tourner à gauche
          digitalWrite(motor1Pin1, LOW);
          digitalWrite(motor1Pin2, HIGH);
          digitalWrite(motor2Pin1, HIGH);
          digitalWrite(motor2Pin2, LOW);
        } else {
          // Avancer
          digitalWrite(motor1Pin1, HIGH);
          digitalWrite(motor1Pin2, LOW);
          digitalWrite(motor2Pin1, HIGH);
          digitalWrite(motor2Pin2, LOW);
        }
      } else {
        // Arrêter
        digitalWrite(motor1Pin1, LOW);
        digitalWrite(motor1Pin2, LOW);
        digitalWrite(motor2Pin1, LOW);
        digitalWrite(motor2Pin2, LOW);
      }
    }
  }
}
