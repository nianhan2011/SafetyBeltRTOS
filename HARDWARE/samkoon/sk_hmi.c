#include "sk_hmi.h"
#include "usart3.h"
#include <string.h>
#include "vdf_710.h"
#include <stdio.h>
#include "os_system__typedef.h"
static __IO uint8_t sk_receive_data[10] = {0};
static __IO uint8_t sk_send_data[50] = {0};
__IO uint8_t sk_coil_register[50] = {0};
__IO uint8_t sk_hold_register[50] = {0};

QueueHandle_t xqueue;

SK_STEP sk_step = SK_HEAD_VERIFY;
static void modbus01(void);
static void modbus03(void);
static void modbus05(void);

const uint8_t auchCRCHi[] = {
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
const uint8_t auchCRCLo[] = {
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

uint16_t getCoilVal(uint16_t addr, uint16_t *tempData) // ȡ��Ȧ״̬ ����0��ʾ�ɹ�
{
    uint16_t result = 0;
    uint16_t tempAddr;

    tempAddr = addr & 0xfff;
    // ֻȡ��8λ��ַ
    switch (tempAddr & 0xff)
    {

    case 1:
        *tempData = sk_coil_register[1]; // ����1��־
        break;
    case 2:
        *tempData = sk_coil_register[2]; // ����2��־
        break;
    case 3:
        *tempData = sk_coil_register[3]; // ����1��־
        break;
    case 4:
        *tempData = sk_coil_register[4]; // ����1��־
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

void open_frequery(void)
{
    while (1)
    {
        /* code */

        u32 isOpen;
        // xTaskNotifyWait(0x0, 0xFFFFFFFF, &isOpen, portMAX_DELAY);

        xQueueReceive(xqueue, &isOpen, portMAX_DELAY);
  
        if (isOpen == 1 && sk_coil_register[1] == 0)
        {
            PCout(4) = 1;
        }
        if (isOpen == 0 && sk_coil_register[1] == 1)
        {
            PCout(4) = 0;
        }
    }
}

uint16_t getRegisterVal(uint16_t addr, int *tempData) // ȡ�Ĵ���ֵ ����0��ʾ�ɹ�
{
    int result = 0;
    uint16_t tempAddr;

    tempAddr = addr & 0xfff;
    if (sizeof(v_data) < 10)
    {
        return result;
    }
    switch (tempAddr & 0xff)
    {
    case 1:

        *tempData = (sk_hold_register[2] << 8) + sk_hold_register[3]; // ��ѹֵ
        break;
    case 2:
        *tempData = (sk_hold_register[4] << 8) + sk_hold_register[5]; // ��ѹֵ
        break;
    case 3:
        *tempData = (sk_hold_register[6]) + sk_hold_register[7]; // COOL1
        break;
    case 4:
        *tempData = (sk_hold_register[8] << 8) + sk_hold_register[9]; // HEAT1

        break;
    case 5:
        *tempData = (sk_hold_register[10] << 8) + sk_hold_register[11]; // HEAT1

        // *tempData = 5; // COOL2
        break;
    case 6:
        *tempData = (sk_hold_register[12] << 8) + sk_hold_register[13]; // HEAT1

        // *tempData = 6; // HEAT2
        break;
    case 7:
        *tempData = (sk_hold_register[14] << 8) + sk_hold_register[15]; // HEAT1

        // *tempData = 7; // ��ˮWATER
        break;
    case 8:
        *tempData = (sk_hold_register[16] << 8) + sk_hold_register[17]; // HEAT1

        // *tempData = 8; // HIGH
        break;
    case 9:
        *tempData = (sk_hold_register[18] << 8) + sk_hold_register[19]; // HEAT1

        // *tempData = 9; // �����¶�
        break;
    case 10:
        *tempData = (sk_hold_register[20] << 8) + sk_hold_register[21]; // HEAT1

        // *tempData = 10; // ����1
        break;

    case 11:
        *tempData = (sk_hold_register[22] << 8) + sk_hold_register[23]; // HEAT1

        // *tempData = 11; // ����2
        break;

    case 12:
        *tempData = (sk_hold_register[24] << 8) + sk_hold_register[25]; // HEAT1

        // *tempData = 12; // ����3
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

uint16_t setCoilVal(uint16_t addr, uint16_t tempData) // 设定线圈状态
{
    uint16_t result = 0;
    uint16_t tempAddr;

    tempAddr = addr & 0xfff;

    switch (tempAddr & 0xff)
    {

    case 1:
        u32 isOpen = tempData;
        // xTaskNotify(open_frequery, isOpen, eSetValueWithOverwrite);

        xQueueSendToBack(xqueue, &isOpen, 0);

        // COOL1_FLAG = tempData;
        // if (tempData == 1 && sk_coil_register[1] == 0)
        // {
        //     PCout(4) = 1;
        // }
        // if (tempData == 0 && sk_coil_register[1] == 1)
        // {
        //     PCout(4) = 0;
        // }

        sk_coil_register[1] = tempData; // 变频器电源开关
        break;
    case 2:
        // HEAT1_FLAG = tempData;

        sk_coil_register[2] = tempData; // 自动开关 0手动，1自动

        break;
    case 3:
        // COOL2_FLAG = tempData;
        if (tempData == 1 && sk_coil_register[3] == 0)
        {
            should_blower_open = 1;
        }
        if (tempData == 0 && sk_coil_register[3] == 1)
        {
            should_blower_close = 1;
        }
        // sk_coil_register[3] = tempData; // 风机

        break;
    case 4:
        if (tempData == 1 && sk_coil_register[4] == 0)
        {
            PCout(5) = 1;
        }
        if (tempData == 0 && sk_coil_register[4] == 1)
        {
            PCout(5) = 0;
        }
        sk_coil_register[4] = tempData; // 净化器

        break;

    case 5:
        // WORK_FLAG = tempData; // 工作标志
        break;

    case 6:
        // LOCAL_FLAG = tempData; // 手动，自动切换
        break;
    case 7: // RESET_FLAG=tempData;  //复位
        break;
    case 8: // SET_FLAG=tempData;
        break;
    case 16:
        // RESET_FLAG = tempData; // 加热1时间

        break;
    case 17: //	HEAT2_TIME=tempData;//加热2时间
        break;
    default:
        break;
    }

    return result;
}
void sk_init(void)
{
    sk_step = SK_HEAD_VERIFY;
    usart3_init();
    xqueue = xQueueCreate(1, sizeof(u32));
}

uint16_t crc16(uint8_t *puchMsg, uint16_t usDataLen)
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

void sk_proc(void)
{
    while (1)
    {
        /* code */

        switch (sk_step)
        {
        case SK_HEAD_VERIFY:
            // if ((QUEUE_LEN(usart3_fram)) < 8)
            // {
            //     break;
            // }
            if (usart3_fram.InfBit.FramLength < 8)
            {

                break;
            }

            // for (uint8_t i = 0; i < 8; i++)
            // {
            //     QUEUE_OUT(usart3_fram, &sk_receive_data[i]);
            // }

            // if (sk_receive_data[0] == 1)
            // {
            //     sk_step = SK_CRC_VERIFY;
            // }
            if (usart3_fram.Data_RX_BUF[0] != 1)
            {
                clearFrame();
                break;
            }
            memcpy((char *)sk_receive_data, &(usart3_fram.Data_RX_BUF[0]), 8);
            sk_step = SK_CRC_VERIFY;
            break;
        case SK_CRC_VERIFY:;
            uint16_t crc_data = crc16(sk_receive_data, 6);
            if (crc_data == sk_receive_data[7] + (sk_receive_data[6] << 8))
            {
                sk_step = SK_CATEGORY_VERIFY;
            }
            else
            {
                clearFrame();
                sk_step = SK_HEAD_VERIFY;
            }

            break;
        case SK_CATEGORY_VERIFY:

            if (sk_receive_data[1] == 1)
            {
                sk_step = SK_DATA_01;
            }
            else if (sk_receive_data[1] == 3)
            {
                sk_step = SK_DATA_03;
            }
            else if (sk_receive_data[1] == 5)
            {
                sk_step = SK_DATA_05;
            }
            else if (sk_receive_data[1] == 6)
            {
                sk_step = SK_DATA_06;
            }

            break;
        case SK_DATA_01:;
            modbus01();
            break;
        case SK_DATA_03:
            modbus03();
            break;
        case SK_DATA_05:
            modbus05();
            break;
        default:
            break;
        }
    }
}

static void modbus01()
{
    uint8_t position;
    uint8_t exit = 0;
    uint8_t addr = sk_receive_data[3];
    uint8_t bit_count = sk_receive_data[5];
    uint8_t byte_count = bit_count / 8;
    uint16_t crcData;
    uint16_t tempData;
    uint8_t tempAddr;

    tempAddr = addr;
    memset(sk_send_data, 0, 50);
    if (bit_count % 8 != 0)
    {
        byte_count++;
    }

    for (uint8_t k = 0; k < byte_count; k++)
    {
        position = k + 3;
        sk_send_data[position] = 0;
        for (uint8_t i = 0; i < 8; i++)
        {

            tempData = 0;
            getCoilVal(tempAddr, &tempData);

            sk_send_data[position] |= tempData << i;
            tempAddr++;
            if (tempAddr >= addr + bit_count)
            {
                exit = 1;
                break;
            }
        }
        if (exit == 1)
            break;
    }

    sk_send_data[0] = 1;
    sk_send_data[1] = 1;
    sk_send_data[2] = byte_count;

    byte_count += 3;
    crcData = crc16((uint8_t *)sk_send_data, byte_count);
    sk_send_data[byte_count] = crcData >> 8;
    byte_count++;
    sk_send_data[byte_count] = crcData & 0xff;

    usart3_send_array((uint8_t *)sk_send_data, byte_count + 1);
    sk_step = SK_HEAD_VERIFY;

    clearFrame();
}

static void modbus03()
{
    uint8_t addr;
    uint8_t tempAddr;
    uint16_t crcData;
    uint8_t read_count;
    uint8_t byte_count;
    int tempData2 = 0;
    addr = sk_receive_data[3];
    tempAddr = addr;
    memset((uint8_t *)sk_send_data, 0, 50);

    read_count = sk_receive_data[5];
    byte_count = read_count * 2;

    for (uint8_t i = 0; i < byte_count; i += 2, tempAddr++)
    {
        tempData2 = 0;
        getRegisterVal(tempAddr, &tempData2);
        sk_send_data[i + 3] = tempData2 >> 8;
        sk_send_data[i + 4] = tempData2 & 0xFF;
        /* code */
    }

    sk_send_data[0] = 1;
    sk_send_data[1] = 3;
    sk_send_data[2] = byte_count;
    byte_count += 3;
    crcData = crc16((uint8_t *)sk_send_data, byte_count);
    sk_send_data[byte_count] = crcData >> 8;
    byte_count++;
    sk_send_data[byte_count] = crcData & 0xFF;
    usart3_send_array((uint8_t *)sk_send_data, byte_count + 1);
    sk_step = SK_HEAD_VERIFY;

    clearFrame();
}
static void modbus05()
{
    uint8_t addr;
    uint8_t tempAddr;
    uint16_t tempData;
    uint8_t onOff;
    uint8_t i;

    // addr = (receBuf[2]<<8) + receBuf[3];
    // tempAddr = addr & 0xfff;
    addr = sk_receive_data[3];
    tempAddr = addr;

    // onOff = (receBuf[4]<<8) + receBuf[5];
    onOff = sk_receive_data[4];

    // if(onOff == 0xff00)
    if (onOff == 0xff)
    {
        tempData = 1;
    }
    // else if(onOff == 0x0000)
    else if (onOff == 0x00)
    {
        tempData = 0;
    }

    setCoilVal(tempAddr, tempData);

    for (i = 0; i < 7; i++)
    {
        sk_send_data[i] = sk_receive_data[i];
    }
    usart3_send_array((uint8_t *)sk_send_data, 8);
    sk_step = SK_HEAD_VERIFY;

    clearFrame();
}