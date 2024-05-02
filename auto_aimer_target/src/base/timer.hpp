#pragma once

#include "tim.h"

#include <cassert>

namespace base {
constexpr size_t task_size_max = 10;

inline void delay(uint32_t ms)
{
    HAL_Delay(ms - 1);
}

class Timer {
public:
    Timer(TIM_HandleTypeDef* htim)
        : htim_(htim)
    {
    }

    void start_it()
    {
        HAL_TIM_Base_Start_IT(htim_);
    }

    void callback(TIM_HandleTypeDef* htim)
    {
        if (htim != htim_)
            return;

        for (int i = 0; i < task_size; i++) {
            if (count_tick % task[i].tick == 0)
                task[i].function();
        }

        count_tick++;
    }

    void add_task(void (*function)(), uint16_t tick)
    {
        assert(task_size < task_size_max);

        task_size++;

        task[task_size - 1].function = function;
        task[task_size - 1].tick = tick;
    }

private:
    struct Task {
        void (*function)();
        uint16_t tick;
    };

private:
    TIM_HandleTypeDef* htim_;
    Task task[task_size_max];
    uint16_t count_tick = 0;
    uint8_t task_size = 0;
};
}
