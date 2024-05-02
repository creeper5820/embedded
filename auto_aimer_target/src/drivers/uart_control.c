#include "uart_control.h"
#include "motor_can.h"
extern UART_HandleTypeDef huart1;
uint8_t buff[10]= {'0','0','0','0','0','0','0','0','0','0'};
uint16_t track_speed;
uint16_t turn_speed;
uint16_t uart_error_flag = 0;
void usart1_init()
{
    HAL_UART_Receive_IT(&huart1,buff,sizeof(buff));
}
// void uart_error(void)
// {
//     if((huart1.RxXferCount < 10)&&(uart_error_flag == 0))
//     {
//         uart_error_flag = 1;
//         //huart1.RxXferCount = 10;
//     }
// }
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart == &huart1)
    {
        HAL_GPIO_WritePin(GPIOH,GPIO_PIN_12,1);
        HAL_UART_Receive_IT(&huart1,buff,sizeof(buff));
    }
    
}
void usart_control_mode()
{
    track_speed_set();
    turn_speed_set();
    switch (buff[0])
    {
    case '0' :track_stop_mode();
            break;
    case '1' :track_auto_mode(track_speed);
            break;
    default:
        break;
    }
    switch (buff[1])
    {
    case '0' :turn_stop_mode();
            break;
    case '1' :turn_auto_mode(turn_speed);
            break;
    default:
        break;
    }
}
void track_speed_set()
{
        track_speed = (buff[2]-'0')*1000 + (buff[3]-'0')*100 + (buff[4]-'0')*10 +buff[5]-'0';

}
void turn_speed_set()
{
        turn_speed = (buff[6]-'0')*1000 + (buff[7]-'0')*100 + (buff[8]-'0')*10 +buff[9]-'0';
}