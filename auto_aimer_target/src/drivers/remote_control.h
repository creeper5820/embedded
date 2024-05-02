#pragma once
#include "main.h"
typedef __packed struct
{
 struct 
 {
 int16_t ch0;//此处为有符号类型，不然不能表示负的
 int16_t ch1;
 int16_t ch2;
 int16_t ch3;
 uint8_t s1;
 uint8_t s2;
 }rc;
}RC_Ctl_t;
void control_task();