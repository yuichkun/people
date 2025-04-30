#include "buffer.h"

// Buffer implementation
uint8_t ring[RING];
volatile size_t head = 0, tail = 0;      // write / read index

size_t freeBytes() { 
  return (tail + RING - head - 1) % RING; 
}

size_t usedBytes() { 
  return (head + RING - tail) % RING;     
} 