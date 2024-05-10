#pragma once

#include "../base/can.hpp"
#include "../utility/pid.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>

namespace module {
class Target {
public:
    static constexpr int16_t current_limit_ = 16384;

    static constexpr int16_t speed_rotate_max_ = 300;
    static constexpr int16_t speed_move_max_ = 10000;

    using Pid = utility::Pid<double, double>;

    struct Status {
        int16_t angle;
        int16_t speed;
        int16_t current;
        int8_t temperature;
        int8_t reserved;

        void friend operator<<(uint8_t be_data[8], const Status& status)
        {
            auto le_data = reinterpret_cast<const uint8_t*>(&status);

            be_data[0] = le_data[1];
            be_data[1] = le_data[0];

            be_data[2] = le_data[3];
            be_data[3] = le_data[2];

            be_data[4] = le_data[5];
            be_data[5] = le_data[4];

            be_data[6] = le_data[6];

            be_data[7] = le_data[7];
        }

        void friend operator<<(Status& status, const uint8_t be_data[8])
        {
            auto le_data = reinterpret_cast<uint8_t*>(&status);

            le_data[0] = be_data[1];
            le_data[1] = be_data[0];

            le_data[2] = be_data[3];
            le_data[3] = be_data[2];

            le_data[4] = be_data[5];
            le_data[5] = be_data[4];

            le_data[6] = be_data[6];

            le_data[7] = be_data[7];
        }
    };

    struct Control {
        int16_t current[4];

        int16_t& operator[](size_t index)
        {
            assert(index < sizeof(current));
            return current[index];
        }

        void friend operator<<(uint8_t be_data[8], Control& control)
        {
            auto le_data = reinterpret_cast<uint8_t*>(&control);

            be_data[0] = le_data[1];
            be_data[1] = le_data[0];

            be_data[2] = le_data[3];
            be_data[3] = le_data[2];

            be_data[4] = le_data[4];
            be_data[5] = le_data[5];

            be_data[6] = le_data[7];
            be_data[7] = le_data[6];
        }
    };

public:
    Target(uint32_t id_move, uint32_t id_rotate,
        const base::CanClient& client_move, const base::CanClient& client_rotate)
        : id_move_(id_move)
        , id_rotate_(id_rotate)
        , client_move_(client_move)
        , client_rotate_(client_rotate)
    {
    }

    void init()
    {
        auto temp = base::CanClient::HeaderType();

        temp.StdId = 0x200;
        temp.IDE = CAN_ID_STD;
        temp.RTR = CAN_RTR_DATA;
        temp.DLC = 8;
        temp.TransmitGlobalTime = DISABLE;

        client_move_.set_header_tx(temp);

        temp.StdId = 0x1ff;
        temp.IDE = CAN_ID_STD;
        temp.RTR = CAN_RTR_DATA;
        temp.DLC = 8;
        temp.TransmitGlobalTime = DISABLE;

        client_rotate_.set_header_tx(temp);
    }

    void set_speed(const int16_t& current_move, const int16_t& current_rotate)
    {
        auto control_move = Control {};
        control_move[id_move_ - 0x201] = current_move;

        auto control_rotate = Control {};
        control_rotate[id_rotate_ - 0x205] = current_rotate;

        uint8_t data_move[8];
        data_move << control_move;

        uint8_t data_rotate[8];
        data_rotate << control_rotate;

        client_move_.send(data_move);
        client_rotate_.send(data_rotate);
    }

    void set_speed(const double& speed_move, const double& speed_rotate)
    {
        set_speed(static_cast<int16_t>(std::clamp(speed_move, -1.0, 1.0) * current_limit_),
            static_cast<int16_t>(std::clamp(speed_rotate, -1.0, 1.0) * current_limit_));
    }

    void set_pid_param(const Pid& pid_move, const Pid& pid_rotate)
    {
        pid_move_ = pid_move;
        pid_rotate_ = pid_rotate;
    }

    void set_pid_param(
        const double& p_m, const double& i_m, const double& d_m,
        const double& p_r, const double& i_r, const double& d_r)
    {
        pid_move_.set(p_m, i_m, d_m);
        pid_rotate_.set(p_r, i_r, d_r);
    }

    void set_speed_with_pid(const double& speed_move, const double& speed_rotate)
    {
        assert(pid_move_.is_initialized());
        assert(pid_rotate_.is_initialized());

        auto real_speed_move = static_cast<double>(status_move_.speed) / speed_move_max_;
        auto real_speed_rotate = static_cast<double>(status_rotate_.speed) / speed_rotate_max_;

        auto output_move = pid_move_.update(real_speed_move, speed_move);
        auto output_rotate = pid_rotate_.update(real_speed_rotate, speed_rotate);

        set_speed(output_move, output_rotate);
    }

    void reset()
    {
        pid_move_.reset();
        pid_rotate_.reset();

        set_speed(0., 0.);
    }

    void update(const uint8_t data[8], const uint32_t& id)
    {
        if (id == id_move_)
            status_move_ << data;

        else if (id == id_rotate_)
            status_rotate_ << data;
    }

private:
    uint32_t id_move_;
    uint32_t id_rotate_;

    Status status_move_;
    Status status_rotate_;

    Pid pid_move_;
    Pid pid_rotate_;

    base::CanClient client_move_;
    base::CanClient client_rotate_;
};
}