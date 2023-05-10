#include "usart2.h"
#include "vdf_710.h"
#include <string.h>
#include "sk_hmi.h"

static __IO uint8_t receive_data[50] = {0};

static __IO uint8_t send_data[20] = {0x01, 0x03, 0x10, 0x00, 0x00, 0x0D, 0x80, 0xCF};         // 查寄存器的值
static __IO uint8_t check_blow_status[20] = {0x01, 0x03, 0x30, 0x00, 0x00, 0x01, 0x8B, 0x0A}; // 检查风机运行状态

static __IO uint8_t modbus_open_data[20] = {0x01, 0x06, 0x00, 0x02, 0x00, 0x02, 0xA9, 0xCB}; // 开启通讯控制
static __IO uint8_t pannel_open_data[20] = {0x01, 0x06, 0x00, 0x02, 0x00, 0x00, 0x28, 0x0A}; // 开启面板控制

static __IO uint8_t blow_open[20] = {0x01, 0x06, 0x20, 0x00, 0x00, 0x01, 0x43, 0xCA};  // 开启风机
static __IO uint8_t blow_close[20] = {0x01, 0x06, 0x20, 0x00, 0x00, 0x05, 0x42, 0x09}; // 关闭风机

__IO uint8_t v_data[20] = {0};

// __IO uint8_t blower_status = {0};

__IO uint8_t should_blower_open = 0;
__IO uint8_t should_blower_open_next = 0;
__IO uint8_t should_blower_close = 0;
__IO uint8_t should_blower_close_next = 0;
VDF_STEP vdf_step = VDF_HEAD_VERIFY;
VDF_SEND_STEP send_step = VDF_READ_REGISTER;
static u8 receive_length = 0;

static const uint8_t auchCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40};
/* CRC��λ�ֽ�ֵ��*/
static const uint8_t auchCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40};

static uint16_t getCoilVal(uint16_t addr, uint16_t *tempData) // ȡ��Ȧ״̬ ����0��ʾ�ɹ�
{
    uint16_t result = 0;
    uint16_t tempAddr;

    tempAddr = addr & 0xfff;
    // ֻȡ��8λ��ַ
    switch (tempAddr & 0xff)
    {

    case 1:
        *tempData = 1; // ����1��־
        break;
    case 2:
        *tempData = 1; // ����2��־
        break;
    case 3:
        *tempData = 1; // ����1��־
        break;
    case 4:
        *tempData = 1; // ����2��־
        break;
    case 5:
        *tempData = 1; // ������־
        break;
    case 6:
        *tempData = 1;
        break;
    case 7:
        *tempData = 1;
        break;
    case 8:
        *tempData = 1;
        break;
    case 9:
        *tempData = 1;
        break;
    case 10:
        *tempData = 1;
        break;
    case 11:
        *tempData = 1;
        break;
    case 12:
        *tempData = 1;
        break;
    case 13:
        *tempData = 1;
        break;
    case 14:
        *tempData = 1;
        break;
    case 15:
        *tempData = 1;
        break;
    case 16:
        *tempData = 1;
        break;
    case 17:
        *tempData = 1;
        break;
    case 18:
        *tempData = 1;
        break;
    case 19:
        *tempData = 1;
        break;
    case 20:
        *tempData = 1;
        break;
    case 21:
        *tempData = 1;
        break;
    case 22:
        *tempData = 1;
        break;

    case 23:
        *tempData = 1; // ����ˮ���򱨾�
        break;
    case 24:
        *tempData = 1; // ˮ�����㱨��
        break;

    default:
        break;
    }

    return result;
}

static uint16_t getRegisterVal(uint16_t addr, int *tempData) // ȡ�Ĵ���ֵ ����0��ʾ�ɹ�
{
    int result = 0;
    uint16_t tempAddr;

    tempAddr = addr & 0xfff;

    switch (tempAddr & 0xff)
    {
    case 1:
        *tempData = 1; // ��ѹֵ
        break;
    case 2:
        *tempData = 2; // ��ѹֵ
        break;
    case 3:
        *tempData = 3; // COOL1
        break;
    case 4:
        *tempData = 4; // HEAT1
        break;
    case 5:
        *tempData = 5; // COOL2
        break;
    case 6:
        *tempData = 6; // HEAT2
        break;
    case 7:
        *tempData = 7; // ��ˮWATER
        break;
    case 8:
        *tempData = 8; // HIGH
        break;
    case 9:
        *tempData = 9; // �����¶�
        break;
    case 10:
        *tempData = 10; // ����1
        break;

    case 11:
        *tempData = 11; // ����2
        break;

    case 12:
        *tempData = 12; // ����3
        break;

    case 13:
        *tempData = 13; // WATER
        break;
    case 14:
        *tempData = 14; // HIGH
        break;
    case 15:
        *tempData = 15; // HEAT1
        break;
    case 16:
        *tempData = 16; // HEAT2
        break;

    case 17:
        *tempData = 17; // HEAT2
        break;

    case 18:
        *tempData = 18; // ����2
        break;

    case 19:

        *tempData = 19;

        break;

    case 20:

        *tempData = 20;
        break;

    case 21:
        *tempData = 21;
        break;

    case 22:
        *tempData = 22; // ��ˮWATER
        break;
    case 23:
        break;
    case 24:
        break;
    case 25:
        break;
    case 26:
        break;
    case 27:
        break;
    case 28:
        break;
    case 29:
        break;

    default:
        break;
    }

    return result;
}

void vdf_init(void)
{
    vdf_step = VDF_HEAD_VERIFY;
    usart2_init();
}

static uint16_t crc16(uint8_t *puchMsg, uint16_t usDataLen)
{
    uint8_t uchCRCHi = 0xFF; /* ��CRC�ֽڳ�ʼ�� */
    uint8_t uchCRCLo = 0xFF; /* ��CRC �ֽڳ�ʼ�� */
    uint32_t uIndex;         /* CRCѭ���е����� */
    while (usDataLen--)      /* ������Ϣ������ */
    {
        uIndex = uchCRCHi ^ *puchMsg++; /* ����CRC */
        uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
        uchCRCLo = auchCRCLo[uIndex];
    }
    return (uchCRCHi << 8 | uchCRCLo);
}

void modbus_open()
{
    RS485_TX_ENABLE
    usart2_send_array(modbus_open_data, 8);
    RS485_RX_ENABLE
}

void vdf_send_blow_open(void)
{
    // if (should_blower_open)
    // {
    //     should_blower_open = 0;
    //     should_blower_open_next = 1;
    //     RS485_TX_ENABLE
    //     usart2_send_array(modbus_open, 8);
    //     RS485_RX_ENABLE
    // }
    if (should_blower_open)
    {
        should_blower_open = 0;
        RS485_TX_ENABLE
        usart2_send_array(blow_open, 8);
        RS485_RX_ENABLE
    }
    else
    {
        // send_step_change();
    }
}

void vdf_send_blow_close(void)
{
    // if (should_blower_close)
    // {
    // should_blower_close = 0;
    // should_blower_close_next = 1;
    // RS485_TX_ENABLE
    // usart2_send_array(modbus_open, 8);
    // RS485_RX_ENABLE
    // }

    if (should_blower_close)
    {
        // should_blower_close_next = 0;
        should_blower_close = 0;
        RS485_TX_ENABLE
        usart2_send_array(blow_close, 8);
        RS485_RX_ENABLE
    }
    else
    {
        // send_step_change();
    }
}

void check_blower_status(void)
{
    RS485_TX_ENABLE
    usart2_send_array(check_blow_status, 8);
    RS485_RX_ENABLE
}

void send_step_change(void)
{
    if (send_step == VDF_CLOSE_BLOW)
    {
        send_step = VDF_READ_REGISTER;
    }
    else
    {
        send_step++;
    }
}
void vdf_send_proc(void)
{
    //    uint16_t crcData;
    //    memset(send_data, 0, 20);
    //    send_data[0] = 0x01;
    //    send_data[1] = 0x03;
    //    send_data[2] = 0x10;
    //    send_data[3] = 0x00;
    //    send_data[4] = 0x00;
    //    send_data[5] = 0x05;
    //    crcData = crc16(send_data, 6);
    //    send_data[6] = crcData >> 8;
    //    send_data[7] = crcData & 0xFF;

    while (1)
    {

        switch (send_step)
        {
        case VDF_READ_REGISTER:
            RS485_TX_ENABLE
            usart2_send_array(send_data, 8);
            RS485_RX_ENABLE
            vTaskDelay(30); // 丢弃剩余的时间片资源

            break;
        case VDF_READ_BLOW:
            check_blower_status();
            break;

        case VDF_OPEN_BLOW: 
            vdf_send_blow_open();
            break;
        case VDF_CLOSE_BLOW:
            vdf_send_blow_close();
            break;

        default:  
            break;
        }
        send_step_change();
        vTaskDelay(20); // 丢弃剩余的时间片资源
    }
}

void vdf_receive_proc(void)
{
    while (1)
    {
        /* code */

        switch (vdf_step)
        {
        case VDF_HEAD_VERIFY:
            if (usart2_fram.InfBit.FramLength < 3)
            {
                break;
            }
            if (usart2_fram.Data_RX_BUF[0] != 1)
            {
                clear_usart2_frame();
                break;
            }

            if (usart2_fram.Data_RX_BUF[1] == 6)
            {
                receive_length = 8;
            }
            else if (usart2_fram.Data_RX_BUF[1] == 3)
            {
                receive_length = usart2_fram.Data_RX_BUF[2] + 5;
            }
            if (usart2_fram.InfBit.FramLength < receive_length)
            {
                break;
            }
            memcpy((char *)receive_data, &(usart2_fram.Data_RX_BUF[0]), receive_length);
            vdf_step = VDF_CRC_VERIFY;
            break;

        case VDF_CRC_VERIFY:
            uint16_t crc_data = crc16(receive_data, receive_length - 2);
            if (crc_data == receive_data[receive_length - 1] + (receive_data[receive_length - 2] << 8))
            {
                vdf_step = VDF_CATEGORY_VERIFY;
            }
            else
            {
                clear_usart2_frame();
                vdf_step = VDF_HEAD_VERIFY;
            }

            break;
        case VDF_CATEGORY_VERIFY:

            if (receive_data[1] == 1)
            {
                // vdf_step = VDF_DATA_01;
            }
            else if (receive_data[1] == 3)
            {
                vdf_step = VDF_DATA_03;
            }
            else if (receive_data[1] == 6)
            {
                vdf_step = VDF_DATA_06;
            }

            break;
        case VDF_DATA_01:;
            break;

        case VDF_DATA_03:

            if (receive_length == 31)
            {
                memcpy((char *)sk_hold_register, (char *)&receive_data[3], 26);
            }
            else if (receive_length == 7)
            {
                if (receive_data[4] == 1)
                {
                    sk_coil_register[3] = 1;
                }
                else
                {
                    sk_coil_register[3] = 0;
                }
            }
            vdf_step = VDF_HEAD_VERIFY;
            clear_usart2_frame();
            // send_step_change();

            break;
        case VDF_DATA_06:
            vdf_step = VDF_HEAD_VERIFY;
            clear_usart2_frame();
            // send_step_change();

            break;

        default:
            break;
        }
    }
}
