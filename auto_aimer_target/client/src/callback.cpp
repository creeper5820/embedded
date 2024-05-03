#include "param.hpp"

extern "C" {
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart);
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t Size);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{
    device::can_server.callback(hcan);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    device::timer.callback(htim);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    device::serial.callback(huart, 0);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t Size)
{
    device::serial.callback(huart, Size);
}
