#include "./entrypoint.hpp"

#include "base/gpio.hpp"
#include "base/serial.hpp"
#include "base/timer.hpp"

#include "module/target.hpp"

#include "param.hpp"

#include <cassert>
#include <cstdint>
#include <cstring>

uint8_t data_from_serial[10];
uint64_t watchdog_count = 0;
bool is_serial_received = false;
int8_t direction_move = 1;

// TIMER CALLBACK ////////////
void watchdog_timer_callback()
{
    if (!is_serial_received)
        return;

    assert(watchdog_count < 50);
    watchdog_count++;
}
void serial_timer_callback()
{
    constexpr auto send = "keep alive";
    device::serial.send<base::Serial::Mode::NORMAL>((uint8_t*)&send, strlen(send));
}
void led_toggle_timer_callback()
{
    if (!is_serial_received)
        return;

    device::led_b.toggle();
}
void motor_control_timer_callback()
{
    if (!is_serial_received)
        return;
    if (data_from_serial[0] != 233 && data_from_serial[3] != 233)
        return;

    // full speed is crazy
    // the speed of motion: -0.4% -> 0.4%
    auto speed_move = static_cast<double>(data_from_serial[1]) / 250;
    // the speed of rotation:
    auto speed_rotate = static_cast<double>(data_from_serial[2]) / 100;

    device::target.set_speed_with_pid(speed_move * direction_move, speed_rotate);
}

// SERIAL CALLBACK ///////////////////////////////////////////////////
void serial_receive_callback(UART_HandleTypeDef* huart, uint16_t size)
{
    (void)huart;
    (void)size;

    is_serial_received = true;
    watchdog_count = 0;
    device::serial.receive_idle<base::Serial::Mode::DMA>(data_from_serial, 10);
}

// CAN CALLBACK /////////////////////////////////////////////////////////////
CAN_RxHeaderTypeDef global_debug_in_ozone;
void can_receive_callback(CAN_RxHeaderTypeDef& _header, const uint8_t* _data)
{
    global_debug_in_ozone = _header;
    device::target.update(_data, _header.StdId);
}
// kEY CALLBACK /////////////////////
void key_callback(uint16_t pin)
{
    (void)pin;
    assert(0);
}
void button_1_callback(uint16_t pin)
{
    (void)pin;

    base::delay(&htim2, 100 * 1000);

    if (device::button_1.read() == GPIO_PIN_SET) {
        device::timer.stop();
        device::target.reset();

        direction_move = 1;

        base::delay(&htim2, 100 * 1000);
        device::timer.start();
    }
}
void button_2_callback(uint16_t pin)
{
    (void)pin;

    base::delay(&htim2, 100 * 1000);

    if (device::button_2.read() == GPIO_PIN_SET) {
        device::timer.stop();
        device::target.reset();

        direction_move = -1;

        base::delay(&htim2, 100 * 1000);
        device::timer.start();
    }
}

// ENTRYPOINT ///
void entrypoint()
{
    // init
    device::can_server.init();
    device::serial.init();
    device::target.init();

    device::target.set_pid_param(
        param::p_m, param::i_m, param::d_m,
        param::p_r, param::i_r, param::d_r);

    // callback register
    device::can_server.set_callback(can_receive_callback);
    device::serial.set_callback(serial_receive_callback);
    device::key.set_callback(key_callback);
    device::button_1.set_callback(button_1_callback);
    device::button_2.set_callback(button_2_callback);

    device::timer.register_activity(1000, led_toggle_timer_callback);
    device::timer.register_activity(2000, serial_timer_callback);
    device::timer.register_activity(10, watchdog_timer_callback);
    device::timer.register_activity(1, motor_control_timer_callback);

    // activity start
    device::serial.receive_idle<base::Serial::Mode::DMA>(data_from_serial, 10);
    device::timer.start();

    while (true) {
        // be careful, not to block other thread
    }
}
