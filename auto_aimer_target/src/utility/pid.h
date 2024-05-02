#pragma once
#include "main.h"
typedef struct _PID//PID参数结构体
{
    float kp,ki,kd;
    float err,lastErr;
    float integral,maxIntegral; 
    float output,maxOutput;
}PID;
void PidSpeed_Init(void);
float Speed_PID_Realize(PID* pid,float target,float feedback);