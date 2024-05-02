#pragma once

#include "../base/can.hpp"
#include "../utility/pid.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>

namespace module {
class Motor {
public:
    static constexpr int16_t current_limit = 16384;

    struct Status {
        int16_t angle;
        int16_t speed;
        int16_t current;
        int8_t temperature;
        int8_t null_value;
    };

    struct SendData {
        int16_t current[4];
    };

    enum class MotorId {
        MOVE,
        ROTATE,
    };

public:
    Motor(const base::CanClient& client)
        : client_(client)
    {
    }

    void init()
    {
        auto header = base::CanClient::HeaderType();
        header.StdId = 0x200;
        header.IDE = CAN_ID_STD;
        header.RTR = CAN_RTR_DATA;
        header.DLC = 8;
        header.TransmitGlobalTime = DISABLE;

        client_.set_header_tx(header);
    }

    void set(const int16_t& current_move, const int16_t& current_rotate)
    {
        auto data = SendData { 0, current_rotate, 0, current_move };

        client_.send(reinterpret_cast<uint8_t*>(&data));
    }

    void set(const double& speed_move, const double& speed_rotate)
    {
        set(static_cast<int16_t>(std::clamp(speed_move, -1.0, 1.0) * current_limit),
            static_cast<int16_t>(std::clamp(speed_rotate, -1.0, 1.0) * current_limit));
    }

    void update(const uint8_t data[8], const MotorId& id)
    {
        status_[static_cast<int>(id)] = *reinterpret_cast<Status*>(&data);
    }

private:
    Status status_[2];
    base::CanClient client_;
};
}