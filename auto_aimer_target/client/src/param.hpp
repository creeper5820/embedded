#pragma once

#include "base/gpio.hpp"
#include "base/serial.hpp"
#include "base/timer.hpp"

#include "module/motor.hpp"

namespace device {

inline auto motor = module::Motor(0x201, 0x202, &hcan1);
inline auto serial = base::Serial(&huart1);
inline auto timer = base::Timer(&htim1);
inline auto can_server = base::CanServer(&hcan1);

inline auto led_g = base::GPIO(LED_G_GPIO_Port, LED_G_Pin);
inline auto led_b = base::GPIO(LED_B_GPIO_Port, LED_B_Pin);
inline auto led_r = base::GPIO(LED_R_GPIO_Port, LED_R_Pin);

}