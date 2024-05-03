#include "./entrypoint.hpp"

#include "base/gpio.hpp"
#include "base/serial.hpp"
#include "base/timer.hpp"

#include "param.hpp"

#include <cassert>
#include <cstdint>
#include <cstring>

double speed = 0;
uint8_t data[50];

void entrypoint()
{

    device::can_server.init();
    device::motor.init();
    device::serial.init();

    device::can_server.set_callback([](CAN_RxHeaderTypeDef& _header, const uint8_t* _data) -> void {
        device::motor.update(_data, _header.StdId);
    });

    device::serial.set_callback([](UART_HandleTypeDef* huart, uint16_t size) {
        (void)huart;
        (void)size;
        device::serial.receive_idle<base::Serial::Mode::DMA>(data, 50);
    });

    device::timer.add_task(1000, []() -> void {
        device::led_b.toggle();
    });
    device::timer.add_task(2000, []() -> void {
        constexpr auto send = "keep alive";
        device::serial.send<base::Serial::Mode::NORMAL>((uint8_t*)&send, strlen(send));
    });
    device::timer.add_task(1, []() -> void {
        device::led_b.toggle();
    });

    device::timer.start_it();

    device::serial.receive_idle<base::Serial::Mode::DMA>(data, 50);

    while (true) {
    }
}
