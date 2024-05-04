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
class Motor {
public:
    static constexpr int16_t current_limit = 16384;
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
    Motor(uint32_t id_move, uint32_t id_rotate, const base::CanClient& client)
        : id_move_(id_move)
        , id_rotate_(id_rotate)
        , client_(client)
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

        pid_move_ = std::make_unique<Pid>();
        pid_rotate_ = std::make_unique<Pid>();
    }

    void set_speed(const int16_t& current_move, const int16_t& current_rotate)
    {
        auto control = Control {};
        control[id_move_ - 0x201] = current_move;
        control[id_rotate_ - 0x201] = current_rotate;

        uint8_t data[8];
        data << control;

        client_.send(reinterpret_cast<uint8_t*>(&data));
    }

    void set_speed(const double& speed_move, const double& speed_rotate)
    {
        set_speed(static_cast<int16_t>(std::clamp(speed_move, -1.0, 1.0) * current_limit),
            static_cast<int16_t>(std::clamp(speed_rotate, -1.0, 1.0) * current_limit));
    }

    void set_pid_param(Pid& pid_move, Pid& pid_rotate)
    {
        pid_move_.reset(&pid_move);
        pid_rotate_.reset(&pid_rotate);
    }

    void set_pid_param(
        const double& p_m, const double& i_m, const double& d_m,
        const double& p_r, const double& i_r, const double& d_r)
    {
        pid_move_->set(p_m, i_m, d_m);
        pid_rotate_->set(p_r, i_r, d_r);
    }

    void set_speed_with_pid(const double& speed_move, const double& speed_rotate)
    {
        assert(pid_move_->is_initialized());
        assert(pid_rotate_->is_initialized());

        auto real_speed_move = static_cast<double>(status_move_.speed) / current_limit;
        auto real_speed_rotate = static_cast<double>(status_rotate_.speed) / current_limit;

        auto output_move = pid_move_->update(real_speed_move, speed_move);
        auto output_rotate = pid_rotate_->update(real_speed_rotate, speed_rotate);

        set_speed(output_move, output_rotate);
    }

    void reset()
    {
        pid_move_->reset();
        pid_rotate_->reset();
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

    std::unique_ptr<Pid> pid_move_;
    std::unique_ptr<Pid> pid_rotate_;

    base::CanClient client_;
};
}