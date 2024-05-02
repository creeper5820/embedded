#pragma once

#include "dma.h"
#include "usart.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace base {

class Serial {
public:
    enum class Status {
        WAIT,
        READY,
    };

    enum class Mode {
        NORMAL,
        IT,
        DMA,
        IDLE_DMA
    };

    using Callback = void (*)(UART_HandleTypeDef*);

public:
    Serial(UART_HandleTypeDef* huart)
        : huart_(huart)
    {
    }

    void init()
    {
        callback_ = [](UART_HandleTypeDef* huart) { (void)huart; };
    }

    void set(const Callback& callback)
    {
        callback_ = callback;
    }

    template <Mode mode>
        requires(mode != Mode::IDLE_DMA)
    HAL_StatusTypeDef send(uint8_t* data, const uint16_t& size = 0, const uint32_t& timeout = 0)
    {
        if constexpr (mode == Mode::NORMAL)
            return HAL_UART_Transmit(huart_, data, size, timeout);
        else if constexpr (mode == Mode::IT)
            return HAL_UART_Transmit_IT(huart_, data, size);
        else if constexpr (mode == Mode::DMA)
            return HAL_UART_Transmit_DMA(huart_, data, size);
    }

    template <Mode mode>
    HAL_StatusTypeDef receive(uint8_t* data, const uint16_t& size = 0, const uint32_t& timeout = 0)
    {
        if constexpr (mode == Mode::NORMAL)
            return HAL_UART_Receive(huart_, data, size, timeout);
        else if constexpr (mode == Mode::IT)
            return HAL_UART_Receive_IT(huart_, data, size);
        else if constexpr (mode == Mode::DMA)
            return HAL_UART_Receive_DMA(huart_, data, size);
        else if constexpr (mode == Mode::IDLE_DMA)
            return HAL_UARTEx_ReceiveToIdle_DMA(huart_, data, size);
    }

    void callback(UART_HandleTypeDef* huart)
    {
        if (huart_ != huart)
            return;

        callback_(huart);
    }

    template <Mode mode>
    void HelloWorld()
    {
        send<mode>((uint8_t*)&"Hello World!\n", sizeof("Hello World!"), 50);
    }

private:
    UART_HandleTypeDef* huart_;

    Callback callback_;
};
}