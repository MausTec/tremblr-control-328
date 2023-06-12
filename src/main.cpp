#include "Hardware.hpp"
#include "PINOUT.h"
#include "Remote.hpp"
#include <Arduino.h>
#include <SPI.h>

#define REMOTE_SPEED_STEP 16

void setup() {
  Hardware::init();
  Remote::init();
  Serial.begin(115200);
}

void loop() {
  RemoteButton button = Remote::getRemoteButton();

  switch (button) {
  case RemoteButton::REMOTE_START_STOP:
    if (Hardware::isFaultPresent()) {
      Hardware::clearFault();
      Hardware::startMotor();
    } else if (Hardware::isStopped()) {
      Hardware::startMotor();
    } else {
      Hardware::stopMotor();
    }
    break;

  case RemoteButton::REMOTE_SPEED_DOWN: {
    uint8_t speed = Hardware::getMotorSpeed();
    if (speed < REMOTE_SPEED_STEP) {
      Hardware::setMotorSpeed(1);
    } else {
      Hardware::setMotorSpeed(speed - REMOTE_SPEED_STEP);
    }
  } break;

  case RemoteButton::REMOTE_SPEED_UP: {
    uint8_t speed = Hardware::getMotorSpeed();
    if (speed > (0xFF - REMOTE_SPEED_STEP)) {
      Hardware::setMotorSpeed(0xFF);
    } else {
      Hardware::setMotorSpeed(speed + REMOTE_SPEED_STEP);
    }
  } break;

  case RemoteButton::REMOTE_AIR_IN: {
    Hardware::openAirIn();
  } break;

  case RemoteButton::REMOTE_AIR_OUT: {
    Hardware::openAirOut();
  } break;

  case RemoteButton::REMOTE_NO_BUTTON:
  default:
    Hardware::closeAirValves();
    break;
  }
}