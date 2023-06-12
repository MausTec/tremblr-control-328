#include "Hardware.hpp"
#include "PINOUT.h"
#include "Remote.hpp"
#include "tscode.h"
#include "tscode_capabilities.h"
#include <Arduino.h>
#include <SPI.h>

#define REMOTE_SPEED_STEP 16

tscode_command_response_t TSCodeHandler(tscode_command_t* cmd, char* response, size_t resp_len);

void setup() {
    Hardware::init();
    Remote::init();
    Serial.begin(115200);
}

void loop_safety();
void loop_tscode();
void loop_remote();

void loop() {
    loop_safety();
    loop_tscode();
    loop_remote();

    Hardware::tick();

    // Register TS-Code capabilities
    static tscode_device_vendor_details_t vendor = {
        .vendor = "Maus-Tec Electronics",
        .device = "Tremblr Control 238",
        .version = "v1",
    };

    tscode_register_vendor_details(&vendor);

    tscode_cap_channel_t reciprocating = {
        .capability = TSCODE_CAP_RECIPROCATING,
        .channel = 0,
        .range = {
            .unit = TSCODE_UNIT_BYTE,
            .min = 0.0f,
            .max = 255.0f,
        },
        .multi_axis = NULL,
    };

    tscode_register_channel(&reciprocating);

    tscode_cap_channel_t valves = {
        .capability = TSCODE_CAP_AIR_VALVES,
        .channel = 0,
        .range = TSCODE_CAP_RANGE_DEFAULT,
        .multi_axis = NULL,
    };

    tscode_register_channel(&valves);
}

void loop_safety() {
    static bool last_fault = false;
    bool fault = Hardware::isFaultPresent();

    if (fault && !last_fault) {
        last_fault = fault;
        Serial.println("FAULT DETECTED");
        Hardware::flashStatusLED(512);
    }
}

void loop_tscode() {
    static const size_t CMD_MAX_RESPONSE = 64;
    static char cmd_buffer[CMD_MAX_RESPONSE + 1] = "";
    static size_t cmd_buffer_idx = 0;
    static char cmd_response[CMD_MAX_RESPONSE + 1] = "";
    static size_t last_cmd_buffer_idx = 0;

    while (Serial.available()) {
        if (cmd_buffer_idx > CMD_MAX_RESPONSE) {
            break;
        }

        char c = Serial.read();

        if (isprint(c)) {
            cmd_buffer[cmd_buffer_idx++] = c;
            cmd_buffer[cmd_buffer_idx] = '\0';
        } else if (c == '\n') {
            tscode_process_buffer(cmd_buffer, TSCodeHandler, cmd_response, CMD_MAX_RESPONSE);
            Serial.println(cmd_response);

            cmd_response[0] = '\0';
            cmd_buffer[0] = '\0';
            cmd_buffer_idx = 0;
        }
    }

    if (cmd_buffer_idx != last_cmd_buffer_idx) {
        last_cmd_buffer_idx = cmd_buffer_idx;
        Serial.printf("Buffer: \"");

        for (size_t i = 0; i < cmd_buffer_idx; i++) {
            char c = cmd_buffer[i];
            if (isprint(c)) {
                Serial.printf("%c", cmd_buffer[i]);
            } else {
                Serial.printf("<%02x>", c);
            }
        }

        Serial.printf("\"\n");
        Hardware::setStatusLED(HIGH, 100);
    }
}

void loop_remote() {
    static RemoteButton last_button = REMOTE_NO_BUTTON;
    RemoteButton button = Remote::getRemoteButton();

    if (button == last_button) return;
    last_button = button;
    Serial.printf("Button: %02X\n", button);

    if (button != 0x00) {
        Hardware::setStatusLED(HIGH, 300);
    }

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
            Hardware::setMotorSpeed(0);
        } else {
            Hardware::setMotorSpeed(speed - REMOTE_SPEED_STEP);
        }
    } break;

    case RemoteButton::REMOTE_SPEED_UP: {
        uint8_t speed = Hardware::getMotorSpeed();
        if (speed >= (0xFF - REMOTE_SPEED_STEP)) {
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
    default: Hardware::closeAirValves(); break;
    }
}

tscode_command_response_t TSCodeHandler(tscode_command_t* cmd, char* response, size_t resp_len) {
    switch (cmd->type) {

    case TSCODE_HALT_IMMEDIATE: {
        Hardware::stopMotor();
    } break;

    case TSCODE_RECIPROCATING_MOVE: {
        if (cmd->speed->value == 0.0f) {
            Hardware::stopMotor();
        } else {
            Hardware::startMotor();
        }

        if (cmd->speed->unit == TSCODE_UNIT_BYTE) {
            Hardware::setMotorSpeed(cmd->speed->value);
        } else if (cmd->speed->unit == TSCODE_UNIT_PERCENTAGE) {
            Hardware::setMotorSpeed(cmd->speed->value * 0xFFf);
        } else {
            return TSCODE_RESPONSE_FAULT;
        }
    } break;

    case TSCODE_AIR_CLOSE: {
        Hardware::closeAirValves();
    } break;

    case TSCODE_AIR_IN_OPEN: {
        Hardware::openAirIn();
    } break;

    case TSCODE_AIR_OUT_OPEN: {
        Hardware::openAirOut();
    } break;

    default: return TSCODE_RESPONSE_FAULT;
    }

    return TSCODE_RESPONSE_OK;
}