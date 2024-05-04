#pragma once

#include "gpio.h"

namespace base {
class GPIO {
public:
    using Callback = void (*)(uint16_t GPIO_Pin);

    GPIO(GPIO_TypeDef* type, uint16_t pin)
        : type_(type)
        , pin_(pin)
    {
        callback_ = [](uint16_t) {};
    }

    void set() const noexcept
    {
        HAL_GPIO_WritePin(type_, pin_, GPIO_PIN_SET);
    }

    void reset() const noexcept
    {
        HAL_GPIO_WritePin(type_, pin_, GPIO_PIN_RESET);
    }

    void toggle() const noexcept
    {
        HAL_GPIO_TogglePin(type_, pin_);
    }

    void set_callback(const Callback& callback)
    {
        callback_ = callback;
    }

    void callback(uint16_t GPIO_Pin)
    {
        if (GPIO_Pin != pin_)
            return;

        callback_(GPIO_Pin);
    }

    GPIO_PinState read() const noexcept
    {
        return HAL_GPIO_ReadPin(type_, pin_);
    }

private:
    GPIO_TypeDef* type_;
    uint16_t pin_;
    Callback callback_;
};
}