#include "./entrypoint.hpp"
#include "../Core/Inc/main.h"

#include "application/ws2812.hpp"
#include "base/gpio.hpp"
#include "base/time.hpp"

#include "tim.h"

#include <algorithm>
#include <memory>
#include <type_traits>

namespace led {
template <typename LedType, typename... LedTypes>
    requires std::is_same_v<LedType, base::GPIO>
void toggle(LedType led, LedTypes... leds)
{
    if constexpr (sizeof...(leds) > 0) {
        toggle(leds...);
    }

    led.toggle();
}

template <typename LedType, typename... LedTypes>
    requires std::is_same_v<LedType, base::GPIO>
void set(LedType led, LedTypes... leds)
{
    if constexpr (sizeof...(leds) > 0) {
        set(leds...);
    }

    led.set();
}
}

using WS2812 = application::WS2812<30>;

auto ws2812 = WS2812(&htim1, TIM_CHANNEL_1);

auto color = WS2812::Color { 50, 60, 50 };

auto from_led = 5;
auto to_led = 20;

bool flag = 0;

auto led_1 = base::GPIO(LED_1_GPIO_Port, LED_1_Pin);
auto led_2 = base::GPIO(LED_2_GPIO_Port, LED_2_Pin);
auto led_3 = base::GPIO(LED_3_GPIO_Port, LED_3_Pin);

void entrypoint()
{
    ws2812.init();

    led::set(led_1, led_2, led_3);

    while (true) {
        delay.ms(100);

        ws2812.reset();
        ws2812.set(0x111111, 10);
        ws2812.apply();

        flag = 1;

        while (flag)
            ;
    }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef* htim)
{
    HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_1);
    flag = 0;

    led::toggle(led_1);
}