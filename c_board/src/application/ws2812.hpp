#pragma once

#include "main.h"

#include "../base/pwm.hpp"

#include <bitset>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace application {

template <size_t n>
class WS2812 {
public:
    struct Color {
        uint8_t g;
        uint8_t b;
        uint8_t r;
    };

    WS2812(TIM_HandleTypeDef* htim, uint32_t channel)
        : pwm_(htim, channel)
    {
    }

    void init()
    {
        positive_ = pwm_.period() / 3 * 2;
        negative_ = pwm_.period() / 3 * 1;

        set({ 0, 0, 0 }, 0, n - 1);
    }

    void set(const Color& color, const size_t& index)
    {
        assert(index < n);

        auto array_g = static_cast<std::bitset<8>>(color.g);
        auto array_r = static_cast<std::bitset<8>>(color.r);
        auto array_b = static_cast<std::bitset<8>>(color.b);

        for (size_t i = 0; i < array_g.size(); i++) {
            data_[24 * index + 0 + i] = (array_g[i] ? positive_ : negative_);
        }
        for (size_t i = 0; i < array_r.size(); i++) {
            data_[24 * index + 8 + i] = (array_g[i] ? positive_ : negative_);
        }
        for (size_t i = 0; i < array_b.size(); i++) {
            data_[24 * index + 16 + i] = (array_g[i] ? positive_ : negative_);
        }
    }

    void set(const uint32_t& color, const size_t& index)
    {
        for (int i = 23; i >= 0; i--) {
            if (color & (1 << i)) {
                data_[24 * index + i] = positive_;
            } else
                data_[24 * index + i] = negative_;
        }
    }

    void set(const Color& color, const size_t& from, const size_t& to)
    {
        assert(from < to);

        for (size_t i = from; i <= to; i++) {
            set(color, to - i);
        }
    }

    void reset()
    {
        for (size_t i = 0; i < 24 * n; i++) {
            data_[i] = 0;
        }
    }

    void apply()
    {
        pwm_.init<base::PWM::Mode::DMA>((uint32_t*)(data_), 24 * n);
    }

    uint32_t* data()
    {
        return data_;
    }

private:
    uint16_t data_[24 * n + 300];
    base::PWM pwm_;

    uint32_t positive_;
    uint32_t negative_;
};

}