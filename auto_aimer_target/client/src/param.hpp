#pragma once

#include "base/gpio.hpp"
#include "base/serial.hpp"
#include "base/timer.hpp"

#include "module/target.hpp"

namespace device {

inline auto target = module::Target(0x204, 0x206, &hcan1, &hcan1);
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

namespace param {
inline double p_m = 10;
inline double i_m = 0;
inline double d_m = 0;
inline double p_r = 1;
inline double i_r = 0;
inline double d_r = 0;

}