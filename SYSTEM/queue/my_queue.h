#ifndef __MY_QUEUE_H
#define __MY_QUEUE_H
#include "common.h"

typedef struct
{
    uint8_t *head;
    uint8_t *tail;
    uint8_t buff[4];
    FunctionalState is_interrupt;
} Queqe4;

typedef struct
{
    uint8_t *head;
    uint8_t *tail;
    uint8_t buff[128];
    FunctionalState is_interrupt;
} Queqe128;

typedef struct
{
    uint8_t *head;
    uint8_t *tail;
    uint8_t buff[256];
    FunctionalState is_interrupt;
} Queqe256;

typedef struct
{
    uint8_t *head;
    uint8_t *tail;
    uint8_t buff[512];
    FunctionalState is_interrupt;
} Queqe512;

void queue_init(uint8_t **head, uint8_t **tail, uint8_t *buff, uint8_t len);
void queue_in(uint8_t **head, uint8_t **tail, uint8_t *buff, uint8_t len, uint8_t *data, uint8_t data_len, FunctionalState is_interrupt);
uint8_t queue_out(uint8_t **head, uint8_t **tail, uint8_t *buff, uint8_t len, uint8_t *data, FunctionalState is_interrupt);
uint16_t queue_len(uint8_t **head, uint8_t **tail, uint8_t *buff, uint8_t len);

#define QUEUE_INIT(x) queue_init((uint8_t **)&(x).head, (uint8_t **)&(x).tail, (uint8_t *)(x).buff, sizeof((x).buff))
#define QUEUE_IN(x, y, z) queue_in((uint8_t **)&(x).head, (uint8_t **)&(x).tail, (uint8_t *)(x).buff, sizeof((x).buff), y, z, (FunctionalState)(x).is_interrupt)
#define QUEUE_OUT(x, y) queue_out((uint8_t **)&(x).head, (uint8_t **)&(x).tail, (uint8_t *)(x).buff, sizeof((x).buff), y, (FunctionalState)(x).is_interrupt)
#define QUEUE_LEN(x) queue_len((uint8_t **)&(x).head, (uint8_t **)&(x).tail, (uint8_t *)(x).buff, sizeof((x).buff))

#endif
