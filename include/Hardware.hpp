#ifndef __Hardware_hpp
#define __Hardware_hpp

#include <stdint.h>

namespace Hardware {

void init(void);

bool isStopped(void);
void stopMotor(void);
void startMotor(void);
void setMotorSpeed(uint8_t speed);
uint8_t getMotorSpeed(void);

void openAirOut(void);
void openAirIn(void);
void closeAirValves(void);

bool isFaultPresent(void);
void clearFault(void);
void setStatusLED(bool on);

} // namespace Hardware

#endif