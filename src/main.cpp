/*
 * Saber-Core <https://github.com/adih-20/saber-core>
 * Copyright (C) 2022 Aditya Hadavale
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation under version 3 of the License.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Arduino.h>
#include <MPU6050_light.h>
#include <Wire.h>

// Pin declarations:
#define GREEN_PIN 0 // PICO GP0
#define RED_PIN 1 // PICO GP1
#define BLUE_PIN 2 // PICO GP2

// Method declarations:
void changeColor(int r, int g, int b);

MPU6050 accel(Wire); // If I2C1 is used instead, change to Wire1 throughout
unsigned long timer = 0; // Used to see if two seconds have elapsed since lightsaber entered color-switch state
unsigned long primTimer = 0; // Used to see if five seconds have elapsed since lightsaber was oriented down
int colorSwitchDurationMs = 2000; // Amount of time in milliseconds to wait before switching colors
int colorSwitchThresholdMs = 5000; // Amount of time in milliseconds needed for the lightsaber to start switching colors when oriented down

void setup() {
  // Set alternative pins for I2C0:
  Wire.setSCL(17);
  Wire.setSDA(16);
  Wire.begin();
  Wire.setClock(400000);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  accel.begin();
  changeColor(0,0,255); // Set lightsaber color to blue initially
}



int i = 0;
bool first = true;

void loop() {
  /* 
    I2C Watchdog:
    If either SDA or SCL is pulled low, the I2C connection is disrupted indicating communication failure with the MPU6050.
    The lightsaber flashes red to the user to indicate failure, and attempts to reestablish connection with the MPU6050.
  */
  while((digitalRead(17) == LOW) || (digitalRead(16) == LOW)) {
    changeColor(255,0,0);
    delay(500);
    Wire.endTransmission();
    Wire.begin();
    changeColor(0,0,0);
    delay(400);
  }
  accel.update();
  double x_angle = -accel.getAngleX(); // Depending on your MPU6050, you may need to remove the negative applied to x_angle
  if((x_angle > 75) && (x_angle < 105)) { // Begins primTimer if oriented down
    if(first) {
      primTimer = millis();
      first = false;
    }
    if(primTimer + colorSwitchThresholdMs < millis()) {
      if(millis()-timer > colorSwitchDurationMs) {
        if(i != 6) {
          
          // TODO: change to switch-case
          
          if(i == 0) {
            changeColor(255,0,0); // red
          } else if(i == 1) {
            changeColor(255,150,0); //orange
          } else if(i == 2) {
            changeColor(0,255,0); // green
          } else if(i == 3) {
            changeColor(0,0,255); // blue
          } else if(i == 4) {
            changeColor(0,200,255); // cyan
          } else if(i == 5) {
            changeColor(120,0,255); // purple
          }
          i++;
        } else {
          i = 0;
          changeColor(235,235,255); // white - may need to be changed depending on LED strip chosen
        }
        timer = millis();
      }
    }
  } else {
    primTimer = 0;
    first = true;
  }
}

// This method changes the color of the lightsaber using PWM to change the brightness of the RGB color channels of the LED strip.
void changeColor(int r, int g, int b) {
  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
}