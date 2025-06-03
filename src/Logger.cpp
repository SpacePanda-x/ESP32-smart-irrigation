#include "Logger.h"

#define LOG_BUFFER_SIZE 256

Logger Log;

void Logger::printf(const char *format, ...)
{
    char buffer[LOG_BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, LOG_BUFFER_SIZE, format, args);
    va_end(args);

    Serial.printf("%s", buffer);
    WebSerial.printf("%s", buffer);
}

void Logger::print(const char *msg)
{
    Serial.print(msg);
    WebSerial.print(msg);
}

void Logger::println(const char *msg)
{
    Serial.println(msg);
    WebSerial.println(msg);
}
