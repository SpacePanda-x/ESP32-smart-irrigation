#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <WebSerial.h> // Adjust include if needed for your setup

class Logger
{
public:
    void printf(const char *format, ...);
    void print(const char *msg);
    void println(const char *msg);
};

extern Logger Log;

#endif // LOGGER_H
