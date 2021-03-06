/*
 * Copyright (C) 2016 Brian Masney
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <Wire.h>
#include <Servo.h> 
#include <LiquidCrystal.h> 

#define RIGHT_SENSOR_ECHO_PIN	8
#define RIGHT_SENSOR_TRIG_PIN	7

#define LEFT_SENSOR_ECHO_PIN	6
#define LEFT_SENSOR_TRIG_PIN	5 

#define SENSOR_SERVO_PIN	4

#define LEFT_SERVO_PIN		3
#define RIGHT_SERVO_PIN		2

#define CLOSEST_DISTANCE_CM	35

Servo leftServo;
Servo rightServo;
Servo sensorServo;

int sensorServoPositions[] = { 70, 80, 90, 100, 110, 120, 110, 100, 90, 80 };
int numSensorServoPositions = 10;
int currentSensorServoPosIndex = 0;

void setup() {
  Serial.begin(9600);

  leftServo.attach(LEFT_SERVO_PIN);
  rightServo.attach(RIGHT_SERVO_PIN);
  sensorServo.attach(SENSOR_SERVO_PIN);

  pinMode(RIGHT_SENSOR_TRIG_PIN, OUTPUT);
  pinMode(RIGHT_SENSOR_ECHO_PIN, INPUT);

  pinMode(LEFT_SENSOR_TRIG_PIN, OUTPUT);
  pinMode(LEFT_SENSOR_ECHO_PIN, INPUT);
}

long measureUltrasonicDistance(int trigPin, int echoPin) {
  long microsecs;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  microsecs = pulseIn(echoPin, HIGH);

  return microsecs / 29 / 2;
}

void loop()
{
  char buf[25];

  sensorServo.write(sensorServoPositions[currentSensorServoPosIndex]);
  currentSensorServoPosIndex = (currentSensorServoPosIndex + 1) % numSensorServoPositions;

  int leftServoPosition;
  int rightServoPosition;

  long rightDistanceCm = measureUltrasonicDistance(RIGHT_SENSOR_TRIG_PIN, RIGHT_SENSOR_ECHO_PIN);
  long leftDistanceCm = measureUltrasonicDistance(LEFT_SENSOR_TRIG_PIN, LEFT_SENSOR_ECHO_PIN);

  if (leftDistanceCm < CLOSEST_DISTANCE_CM and rightDistanceCm < CLOSEST_DISTANCE_CM) {
    if(leftDistanceCm < rightDistanceCm) {
      leftServoPosition = 0;
      rightServoPosition = 0;
    }
    else {
      leftServoPosition = 180;
      rightServoPosition = 180;
    }
  }
  else if (leftDistanceCm < CLOSEST_DISTANCE_CM) {
    leftServoPosition = 180;
    rightServoPosition = 180;
  }
  else if (rightDistanceCm < CLOSEST_DISTANCE_CM) {
    leftServoPosition = 0;
    rightServoPosition = 0;
  }
  else {
    leftServoPosition = 180;
    rightServoPosition = 0;
  }

  leftServo.write(leftServoPosition);
  rightServo.write(rightServoPosition);

  snprintf(buf, sizeof(buf), "D: L:%ld R:%ld        ", leftDistanceCm, rightDistanceCm);
  Serial.print(buf);
  Serial.print(" ");

  snprintf(buf, sizeof(buf), "S: L:%d R:%d        ", leftServoPosition, rightServoPosition);
  Serial.println(buf);

  delay(50);
}

