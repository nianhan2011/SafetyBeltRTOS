#ifndef __LED_H
#define __LED_H

#include "common.h"

#define LED PAout(8) // PA8

void LED_Init(void);
void led_turn(void);

#endif
