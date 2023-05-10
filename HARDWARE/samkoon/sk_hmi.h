#ifndef __SK_HMI_H
#define __SK_HMI_H

#include "stm32f10x.h"
typedef enum
{
    SK_HEAD_VERIFY,
    SK_CRC_VERIFY,
    SK_CATEGORY_VERIFY,
    SK_DATA_01,
    SK_DATA_03,
    SK_DATA_05,
    SK_DATA_06,

} SK_STEP;

extern __IO uint8_t sk_coil_register[50];
extern __IO uint8_t sk_hold_register[50];

void sk_init(void);
void sk_proc(void);
void open_frequery(void);

#endif
