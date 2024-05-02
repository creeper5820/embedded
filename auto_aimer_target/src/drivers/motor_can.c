#include "motor_can.h"
#include "remote_control.h"
#include "../utility/pid.h"

extern CAN_HandleTypeDef hcan1;
extern RC_Ctl_t RC_CtrlData;
CAN_Data_TypeDef CAN_Moter[2];
extern PID pid_speed[2];
CAN_FilterTypeDef CAN1_FilterConfig;

void CAN_FIleter_init()
{
    CAN1_FilterConfig.FilterActivation     = ENABLE;                // 过滤器使能
    CAN1_FilterConfig.FilterMode           = CAN_FILTERMODE_IDMASK; // 掩码模式
    CAN1_FilterConfig.FilterScale          = CAN_FILTERSCALE_32BIT; // 32位
    CAN1_FilterConfig.FilterIdHigh         = 0x0000;                // 过滤器ID
    CAN1_FilterConfig.FilterIdLow          = 0x0000;
    CAN1_FilterConfig.FilterMaskIdHigh     = 0x0000;
    CAN1_FilterConfig.FilterMaskIdLow      = 0x0000;
    CAN1_FilterConfig.FilterBank           = 0;
    CAN1_FilterConfig.SlaveStartFilterBank = 14;
    CAN1_FilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0; // 关联FIFO0
    HAL_CAN_ConfigFilter(&hcan1, &CAN1_FilterConfig);
    HAL_CAN_Start(&hcan1);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING); // 打开FIFO待处理中断
}
void CAN_Data_Decode(CAN_Data_TypeDef *CAN_Data, uint8_t Rx_Data[]) // 数据解析函数
{
    CAN_Data->Current_MechAngle = (Rx_Data[0] << 8) | (Rx_Data[1]);
    CAN_Data->Current_Speed     = (Rx_Data[2] << 8) | (Rx_Data[3]);
    CAN_Data->Current           = (Rx_Data[4] << 8) | (Rx_Data[5]);
    CAN_Data->Temperature       = Rx_Data[6];
    // CAN_Data->Current_MechAngle = (CAN_Data->Current_MechAngle - 4096) * 180.0 /4096 ;
}
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef CAN1_Rx_Message; // 定义接收数据结构体，用于存放IDE，RTR，DLC等内容
    uint8_t CAN1_Rx_Data[8];             // 用于存储报文中的数据段
    if (hcan == &hcan1)                  // 判断是否为can1触发中断
    {
        HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &CAN1_Rx_Message, CAN1_Rx_Data);
        if ((CAN1_Rx_Message.IDE == CAN_ID_STD) && (CAN1_Rx_Message.RTR == CAN_RTR_DATA) && (CAN1_Rx_Message.DLC == 8)) {

            switch (CAN1_Rx_Message.StdId) {
                case 0x204:
                    CAN_Data_Decode(&CAN_Moter[0], CAN1_Rx_Data);
                    break;
                case 0x206:
                    CAN_Data_Decode(&CAN_Moter[1], CAN1_Rx_Data);
                    break;
            }
        }
    }
}
void CAN1_Tx_TrackData(int16_t motor_output)
{
    uint32_t tx_mail_box;
    CAN_TxHeaderTypeDef CAN1_Tx_Message;  // 定义发送数据结构体，用于存放IDE，RTR，DLC等内容
    uint8_t CAN1_Tx_Data[8];              // 用于暂存发送报文中的数据段
    CAN1_Tx_Message.DLC   = 8;            // 数据长度为8
    CAN1_Tx_Message.IDE   = CAN_ID_STD;   // 数据为标准格式
    CAN1_Tx_Message.RTR   = CAN_RTR_DATA; // 代表为数据帧
    CAN1_Tx_Message.StdId = 0x200;
    CAN1_Tx_Data[0]       = 0;
    CAN1_Tx_Data[1]       = 0;
    CAN1_Tx_Data[2]       = 0;
    CAN1_Tx_Data[3]       = 0;
    CAN1_Tx_Data[4]       = 0;
    CAN1_Tx_Data[5]       = 0;
    CAN1_Tx_Data[6]       = motor_output >> 8;
    CAN1_Tx_Data[7]       = motor_output;
    if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) != 0) // 判断发送邮箱中是否存在空邮箱
    {
        HAL_CAN_AddTxMessage(&hcan1, &CAN1_Tx_Message, CAN1_Tx_Data, &tx_mail_box); // 将自定义报文添加到邮箱中
    }
}
void CAN1_Tx_TurnData(int16_t motor_output)
{
    uint32_t tx_mail_box;
    CAN_TxHeaderTypeDef CAN1_Tx_Message;  // 定义发送数据结构体，用于存放IDE，RTR，DLC等内容
    uint8_t CAN1_Tx_Data[8];              // 用于暂存发送报文中的数据段
    CAN1_Tx_Message.DLC   = 8;            // 数据长度为8
    CAN1_Tx_Message.IDE   = CAN_ID_STD;   // 数据为标准格式
    CAN1_Tx_Message.RTR   = CAN_RTR_DATA; // 代表为数据帧
    CAN1_Tx_Message.StdId = 0x1FF;
    CAN1_Tx_Data[0]       = 0;
    CAN1_Tx_Data[1]       = 0;
    CAN1_Tx_Data[2]       = motor_output >> 8;
    CAN1_Tx_Data[3]       = motor_output;
    CAN1_Tx_Data[4]       = 0;
    CAN1_Tx_Data[5]       = 0;
    CAN1_Tx_Data[6]       = 0;
    CAN1_Tx_Data[7]       = 0;
    if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) != 0) // 判断发送邮箱中是否存在空邮箱
    {
        HAL_CAN_AddTxMessage(&hcan1, &CAN1_Tx_Message, CAN1_Tx_Data, &tx_mail_box); // 将自定义报文添加到邮箱中
    }
}
void track_auto_mode(uint16_t speed)
{
    int16_t motor3508_output;
    if (CAN_Moter[0].Current_Speed >= 0) {
        motor3508_output = Speed_PID_Realize(&pid_speed[0], speed, CAN_Moter[0].Current_Speed);
        CAN1_Tx_TrackData(motor3508_output);
    } else {

        motor3508_output = Speed_PID_Realize(&pid_speed[0], -speed, CAN_Moter[0].Current_Speed);
        CAN1_Tx_TrackData(motor3508_output);
    }
    // if(RC_CtrlData.rc.s2 != 1)
    // {
    //     turn_stop_mode();
    // }
}
void track_stop_mode()
{
    // CAN1_Tx_TrackData(0);
    int16_t motor3508_output;
    motor3508_output = Speed_PID_Realize(&pid_speed[0], 0, CAN_Moter[0].Current_Speed);
    CAN1_Tx_TrackData(motor3508_output);
}
// int16_t motor3508_output1;
void track_manual_mode()
{
    int16_t motor3508_output;
    motor3508_output = Speed_PID_Realize(&pid_speed[0], 2 * RC_CtrlData.rc.ch2, CAN_Moter[0].Current_Speed);
    CAN1_Tx_TrackData(motor3508_output);
}

void turn_auto_mode(uint16_t speed)
{
    int16_t motor6020_output, motor3508_output;
    motor6020_output = Speed_PID_Realize(&pid_speed[0], speed, CAN_Moter[1].Current_Speed);
    CAN1_Tx_TurnData(motor6020_output);
    // if(RC_CtrlData.rc.s1 == 3 )
    // {
    //     motor3508_output = Speed_PID_Realize(&pid_speed[1], 0, CAN_Moter[0].Current_Speed);
    //     CAN1_Tx_TrackData(motor3508_output);
    // }
}
// int16_t motor6020_output;
void turn_stop_mode()
{
    // CAN1_Tx_TurnData(0);
    int16_t motor6020_output;
    motor6020_output = Speed_PID_Realize(&pid_speed[1], 0, CAN_Moter[1].Current_Speed);
    CAN1_Tx_TurnData(motor6020_output);
}
void turn_manual_mode()
{
    ;
}