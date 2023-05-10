#ifndef __VDF_710_H
#define __VDF_710_H

typedef enum
{
    VDF_HEAD_VERIFY,
    VDF_CRC_VERIFY,
    VDF_CATEGORY_VERIFY,
    VDF_DATA_01,
    VDF_DATA_03,
    VDF_DATA_06,

} VDF_STEP;

typedef enum
{
    VDF_READ_REGISTER,
    VDF_READ_BLOW,
    VDF_OPEN_BLOW,
    VDF_CLOSE_BLOW,
} VDF_SEND_STEP;

extern __IO uint8_t v_data[20];
extern __IO uint8_t should_blower_open;
extern __IO uint8_t should_blower_close;
void vdf_init(void);

void vdf_send_proc(void);
void check_blower_status(void);

void vdf_receive_proc(void);

void vdf_send_blow_open(void);
void vdf_send_blow_close(void);
void modbus_open(void);
#endif
