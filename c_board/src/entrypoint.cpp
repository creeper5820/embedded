#include "./entrypoint.hpp"
#include "../Core/Inc/main.h"

#include "base/gpio.hpp"
#include "base/time.hpp"

#include <type_traits>

template <typename LedType, typename... LedTypes>
    requires std::is_same_v<LedType, base::GPIO>
void Toggle(LedType led, LedTypes... leds)
{
    if constexpr (sizeof...(leds) > 0) {
        Toggle(leds...);
    }

    led.toggle();
}

void entrypoint()
{
    auto led_1 = base::GPIO(LED_1_GPIO_Port, LED_1_Pin);
    auto led_2 = base::GPIO(LED_2_GPIO_Port, LED_2_Pin);
    auto led_3 = base::GPIO(LED_3_GPIO_Port, LED_3_Pin);

    while (true) {
        delay.ms(500);
        Toggle(led_1, led_2, led_3);
    }
}