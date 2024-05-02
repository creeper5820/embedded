#include "remote_control.h"
RC_Ctl_t RC_CtrlData;
extern UART_HandleTypeDef huart3;
uint8_t pData[18];
void RemoteDataProcess(uint8_t *pData)
{
 if(pData == NULL)
 {
 return;
 }
 
 RC_CtrlData.rc.ch0 = ((int16_t)pData[0] | ((int16_t)pData[1] << 8)) & 0x07FF; 
 RC_CtrlData.rc.ch1 = (((int16_t)pData[1] >> 3) | ((int16_t)pData[2] << 5)) & 0x07FF;
 RC_CtrlData.rc.ch2 = (((int16_t)pData[2] >> 6) | ((int16_t)pData[3] << 2) |((int16_t)pData[4] << 10)) & 0x07FF;
 RC_CtrlData.rc.ch3 = (((int16_t)pData[4] >> 1) | ((int16_t)pData[5]<<7)) & 0x07FF;
 
 RC_CtrlData.rc.s1 = ((pData[5] >> 4) & 0x000C) >> 2;
 RC_CtrlData.rc.s2 = ((pData[5] >> 4) & 0x0003);
 RC_CtrlData.rc.ch0 -= 1024;
 RC_CtrlData.rc.ch1 -= 1024;
 RC_CtrlData.rc.ch2 -= 1024;
 RC_CtrlData.rc.ch3 -= 1024;
}
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//   if(huart == &huart3)
//   {
//     HAL_UART_Receive_DMA(&huart3, pData, sizeof(pData));
//     RemoteDataProcess(pData);
//   }
// }
void control_task()
{ 
  switch(RC_CtrlData.rc.s1)
  {
  case 1: track_auto_mode();  break;
  case 2: track_stop_mode();  break;
  case 3: track_manual_mode(); break;
  }
  switch(RC_CtrlData.rc.s2)
  {
  case 1: turn_auto_mode();  break;
  case 2: turn_stop_mode();  break;
  case 3: turn_manual_mode(); break;
  }
}