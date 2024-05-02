#pragma once

#include "../base/serial.hpp"

#include <memory>

namespace module {
class Remote {
public:
    Remote(UART_HandleTypeDef* huart)
        : serial_(std::make_shared<base::Serial>(huart))
    {
    }

    std::shared_ptr<base::Serial> serial()
    {
        return serial_;
    }

private:
    std::shared_ptr<base::Serial> serial_;
};
}