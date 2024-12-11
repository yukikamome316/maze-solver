#include <Wire.h>

#include "Adafruit_TCS34725.h"

#define addr 0x29

// Line Tracking IO define
#define LT_R !digitalRead(10)
#define LT_M !digitalRead(4)
#define LT_L !digitalRead(2)

#define ENA 5
#define ENB 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11

#define carSpeedTracking 150
#define carSpeedRotate 250

// 左後方の車輪の力が弱いため、回転時間を増やして釣り合いを取る
#define rotationDurationRight 700
#define rotationDurationLeft 730

void forward() {
  analogWrite(ENA, carSpeedTracking);
  analogWrite(ENB, carSpeedTracking);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  // Serial.println("go forward!");
}

void back() {
  analogWrite(ENA, carSpeedRotate);
  analogWrite(ENB, carSpeedRotate);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  // Serial.println("go back!");
}

void left(int carSpeed) {
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  // Serial.println("go left!");
}

void right(int carSpeed) {
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  // Serial.println("go right!");
}

void stop() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
  // Serial.println("Stop!");
}

// Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS,
// TCS34725_GAIN_1X);
Adafruit_TCS34725 tcs =
    Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_4X);

void setup() {
  Serial.begin(9600);

  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

  //  tcs.setInterrupt(false);

  pinMode(10, INPUT);
  pinMode(4, INPUT);
  pinMode(2, INPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

bool isGoal = false;

void loop() {
  if (isGoal) {
    Serial.println("Goal!");
    stop();
    return;
  }

  char recv;
  if (Serial.available()) {
    recv = char(Serial.read());
    //    Serial.println(recv);

    if (recv == 'F') {
      forward();
      delay(100);
    } else if (recv == 'R') {
      right(carSpeedRotate);
      delay(750);
      forward();
      delay(100);
    } else if (recv == 'L') {
      left(carSpeedRotate);
      delay(800);
      forward();
      delay(100);
    } else if (recv == 'G') {
      isGoal = true;
      return;
    }
  }

  // Line tracking logic
  if (LT_M) {
    forward();
  } else if (LT_R) {
    right(carSpeedTracking);
    while (LT_R);
  } else if (LT_L) {
    left(carSpeedTracking);
    while (LT_L);
  }

  // check color
  uint16_t r, g, b, c;

  tcs.getRawData(&r, &g, &b, &c);

  Serial.println(String(r) + "," + String(g) + "," + String(b));

  //  delay(30);
}
