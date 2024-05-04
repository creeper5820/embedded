#include "param.hpp"

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

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    device::key.callback(GPIO_Pin);
    device::button_1.callback(GPIO_Pin);
    device::button_2.callback(GPIO_Pin);
}