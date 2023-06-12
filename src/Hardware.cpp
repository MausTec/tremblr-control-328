#include "Hardware.hpp"
#include "PINOUT.h"
#include <Arduino.h>
#include <SPI.h>

void Hardware::init(void) {
  pinMode(AIR_OUT_GPIO, OUTPUT);
  digitalWrite(AIR_OUT_GPIO, LOW);

  pinMode(AIR_IN_GPIO, OUTPUT);
  digitalWrite(AIR_IN_GPIO, LOW);

  pinMode(STATUS_GPIO, OUTPUT);
  digitalWrite(STATUS_GPIO, LOW);

  pinMode(FCLR_GPIO, OUTPUT);
  digitalWrite(FCLR_GPIO, LOW);

  pinMode(PWRDN_GPIO, OUTPUT);
  digitalWrite(PWRDN_GPIO, LOW);

  pinMode(FAULT_GPIO, INPUT);

  pinMode(PWM_SS_GPIO, OUTPUT);
  SPI.begin();

  Hardware::setMotorSpeed(0);
  Hardware::clearFault();
}

void Hardware::setMotorSpeed(uint8_t speed) {
  digitalWrite(PWM_SS_GPIO, LOW);
  SPI.transfer(0x10 | 0x01);
  SPI.transfer(speed);
  digitalWrite(PWM_SS_GPIO, HIGH);
}

void Hardware::clearFault(void) {
  digitalWrite(FCLR_GPIO, HIGH);
  delay(10);
  digitalWrite(FCLR_GPIO, LOW);
  delay(10);
  digitalWrite(FCLR_GPIO, HIGH);
  delay(10);
  digitalWrite(FCLR_GPIO, LOW);
  delay(10);
}

void Hardware::openAirIn(void) {
  digitalWrite(AIR_OUT_GPIO, LOW);
  digitalWrite(AIR_IN_GPIO, HIGH);
}

void Hardware::openAirOut(void) {
  digitalWrite(AIR_IN_GPIO, LOW);
  digitalWrite(AIR_OUT_GPIO, HIGH);
}

void Hardware::closeAirValves(void) {
  digitalWrite(AIR_IN_GPIO, LOW);
  digitalWrite(AIR_OUT_GPIO, LOW);
}