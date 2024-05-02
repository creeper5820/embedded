#include "pid.h"
PID pid_speed[2];
float KP_speed[2] = {20,40};
float KI_speed[2]  = {0,40};
float KD_speed[2] =  {0,0};
void PidSpeed_Init(void)//PID参数初始化
{   for(int i = 0;i < 2;i++)
    {
        pid_speed[i].err= 0;
        pid_speed[i].integral = 0;
        pid_speed[i].maxIntegral = 10000; 
        pid_speed[i].maxOutput = 20000;
        pid_speed[i].lastErr = 0;
        pid_speed[i].output = 0;
        pid_speed[i].kp = KP_speed[i];
        pid_speed[i].ki = KI_speed[i];
        pid_speed[i].kd = KD_speed[i];
    }
}
float Speed_PID_Realize(PID* pid,float target,float feedback)//一次PID计算
{
    //if(target == 0)
    //{
      //   pid->output = 0; //输出0
    //}
    //else    
    //{   
        pid->err = target - feedback;
        
        pid->integral += pid->err;
        if(pid->err < 0.5 && pid->err > -0.5) 
        {
            pid->err = 0;
            
        }
        if(pid->ki * pid->integral < -pid->maxIntegral) pid->integral = -pid->maxIntegral / pid->ki;//I限幅
        else if(pid->ki * pid->integral > pid->maxIntegral) pid->integral = pid->maxIntegral / pid->ki;

        pid->output = (pid->kp * pid->err) + (pid->ki * pid->integral) + (pid->kd * (pid->err - pid->lastErr));
        if(target == 0)
        {
            pid->integral = 0; 
        }
        pid->lastErr = pid->err;
        if(pid->output > pid->maxOutput ) pid->output = pid->maxOutput;
        else if(pid->output < -pid->maxOutput) pid->output = -pid->maxOutput;
    //}
    
    
    return pid->output;
}