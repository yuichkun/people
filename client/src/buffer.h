#ifndef BUFFER_H
#define BUFFER_H

#include "config.h"
#include <Arduino.h>

// Ring buffer
extern uint8_t ring[RING];
extern volatile size_t head, tail;  // write / read index

// Buffer operations
size_t freeBytes();
size_t usedBytes();

#endif // BUFFER_H 