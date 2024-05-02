#pragma once
#include "main.h"

typedef struct
{
    int16_t Current_MechAngle;
    int16_t Current_Speed;
    int16_t Current;
    int16_t Temperature;
} CAN_Data_TypeDef;

void CAN_FIleter_init();
void CAN_Data_Decode(CAN_Data_TypeDef *CAN_Data, uint8_t Rx_Data[]);
void track_auto_mode(uint16_t speed);
void track_stop_mode();
void track_manual_mode();
void turn_auto_mode(uint16_t speed);
void turn_stop_mode();
void turn_manual_mode();