#pragma once

#include "base/gpio.hpp"
#include "base/serial.hpp"
#include "base/timer.hpp"

#include "main.h"
#include "module/motor.hpp"
#include "tim.h"

namespace device {

inline auto motor = module::Motor(0x201, 0x202, &hcan1);
inline auto serial = base::Serial(&huart1);
inline auto timer = base::Timer(&htim1);
inline auto can_server = base::CanServer(&hcan1);

inline auto led_g = base::GPIO(LED_G_GPIO_Port, LED_G_Pin);
inline auto led_b = base::GPIO(LED_B_GPIO_Port, LED_B_Pin);
inline auto led_r = base::GPIO(LED_R_GPIO_Port, LED_R_Pin);

inline auto key = base::GPIO(KEY_GPIO_Port, KEY_Pin);
inline auto button_1 = base::GPIO(BUTTON_1_GPIO_Port, BUTTON_1_Pin);
inline auto button_2 = base::GPIO(BUTTON_2_GPIO_Port, BUTTON_2_Pin);

}