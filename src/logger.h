#ifndef LOGGER_H
#define LOGGER_H

typedef enum TYPE {
   INFO,
   WARNING,
   ERROR
} TYPE;

void logger(TYPE type, const char* message, ...);

#endif
