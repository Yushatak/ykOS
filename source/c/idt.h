#include <stdint.h>

void generateIDT();
void lidt(uint32_t base, uint16_t size);
