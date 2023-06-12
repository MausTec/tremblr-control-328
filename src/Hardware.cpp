#include "Hardware.hpp"
#include "PINOUT.h"
#include <Arduino.h>
#include <SPI.h>

static uint8_t s_motorSpeed = 0;
static uint32_t s_sledFlashMS = 0;
static uint32_t s_sledToggleAt = 0;

void Hardware::tick(void) {
    if (s_sledToggleAt > 0 && millis() > s_sledToggleAt) {
        if (s_sledFlashMS == 0) {
            s_sledToggleAt = 0;
            digitalWrite(STATUS_GPIO, LOW);
        } else {
            s_sledToggleAt = millis() + s_sledFlashMS;
            digitalWrite(STATUS_GPIO, !digitalRead(STATUS_GPIO));
        }
    }
}

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
    digitalWrite(PWRDN_GPIO, HIGH);

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
    s_motorSpeed = speed;
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

bool Hardware::isFaultPresent(void) {
    return digitalRead(FAULT_GPIO);
}

void Hardware::startMotor(void) {
    digitalWrite(PWRDN_GPIO, LOW);
}

void Hardware::stopMotor(void) {
    digitalWrite(PWRDN_GPIO, HIGH);
}

bool Hardware::isStopped(void) {
    return digitalRead(PWRDN_GPIO);
}

uint8_t Hardware::getMotorSpeed(void) {
    return s_motorSpeed;
}

void Hardware::setStatusLED(bool on, uint32_t duration_ms) {
    s_sledFlashMS = 0;
    s_sledToggleAt = duration_ms > 0 ? millis() + duration_ms : 0;
    digitalWrite(STATUS_GPIO, on);
}

void Hardware::flashStatusLED(uint32_t duration_flash_ms) {
    s_sledFlashMS = duration_flash_ms;
    s_sledToggleAt = millis() + duration_flash_ms;
    digitalWrite(STATUS_GPIO, HIGH);
}