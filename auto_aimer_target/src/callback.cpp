#include "callback.hpp"

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{
    can_server.callback(hcan);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    timer.callback(htim);
}

#ifdef __USART_H__
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
}
#endif