#pragma once

#include "tim.h"

#include <cassert>
#include <cstdint>

namespace base {
constexpr size_t task_size_max = 10;

inline void delay(uint32_t ms)
{
    HAL_Delay(ms - 1);
}

inline void delay(TIM_HandleTypeDef* htim, uint32_t tick)
{
    htim->Instance->CNT = 0;
    HAL_TIM_Base_Start(htim);

    while (htim->Instance->CNT < tick)
        ;

    HAL_TIM_Base_Stop(htim);
}

class Timer {
public:
    using Callback = void (*)(void);

    Timer(TIM_HandleTypeDef* htim)
        : htim_(htim)
    {
    }

    void start()
    {
        HAL_TIM_Base_Start_IT(htim_);
    }

    void stop()
    {
        HAL_TIM_Base_Stop_IT(htim_);
    }

    void callback(TIM_HandleTypeDef* htim)
    {
        if (htim != htim_)
            return;

        for (int i = 0; i < task_size_; i++) {
            if (count_tick_ % task_[i].tick == 0)
                task_[i].callback();
        }

        count_tick_++;
    }

    void add_task(uint16_t tick, Callback callback)
    {
        assert(task_size_ < task_size_max);

        task_size_++;

        task_[task_size_ - 1].callback = callback;
        task_[task_size_ - 1].tick = tick;
    }

private:
    struct Task {
        Callback callback;
        uint16_t tick;
    };

private:
    TIM_HandleTypeDef* htim_;
    Task task_[task_size_max];
    uint64_t count_tick_ = 0;
    uint8_t task_size_ = 0;
};
}
