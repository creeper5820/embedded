#include "./entrypoint.hpp"

#include "base/gpio.hpp"
#include "base/serial.hpp"
#include "base/timer.hpp"

#include "module/motor.hpp"
#include "param.hpp"
#include "stm32_hal_legacy.h"
#include "stm32f4xx_hal_gpio.h"
#include "tim.h"

#include <cassert>
#include <cstdint>
#include <cstring>

double speed = 0;
uint8_t data[10];
uint64_t watchdog_count = 0;
bool is_serial_received = false;
int8_t direction_move = 1;

// timer callback ////////////
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
    if (data[0] != 233 && data[3] != 233)
        return;

    auto speed_move = static_cast<double>(data[1]) / 100;
    auto speed_rotate = static_cast<double>(data[2]) / 100;

    device::motor.set_speed_with_pid(speed_move * direction_move, speed_rotate);
}

// serial callback ///////////////////////////////////////////////////
void serial_receive_callback(UART_HandleTypeDef* huart, uint16_t size)
{
    (void)huart;
    (void)size;

    is_serial_received = true;
    watchdog_count = 0;
    device::serial.receive_idle<base::Serial::Mode::DMA>(data, 10);
}

// can callback/////////////////////////////////////////////////////////////
void can_receive_callback(CAN_RxHeaderTypeDef& _header, const uint8_t* _data)
{
    device::motor.update(_data, _header.StdId);
}
// key callback///
void key_callback(uint16_t GPIO_Pin)
{
    (void)GPIO_Pin;
    assert(0);
}
void button_1_callback(uint16_t GPIO_Pin)
{
    (void)GPIO_Pin;

    base::delay(&htim2, 100 * 1000);

    if (device::button_1.read() == GPIO_PIN_SET) {
        device::timer.stop();
        device::motor.reset();

        direction_move = 1;

        base::delay(&htim2, 100 * 1000);
        device::timer.start();
    }
}
void button_2_callback(uint16_t GPIO_Pin)
{
    (void)GPIO_Pin;

    base::delay(&htim2, 100 * 1000);

    if (device::button_2.read() == GPIO_PIN_SET) {
        device::timer.stop();
        device::motor.reset();

        direction_move = -1;

        base::delay(&htim2, 100 * 1000);
        device::timer.start();
    }
}

// entrypoint ///
void entrypoint()
{
    // init
    device::can_server.init();
    device::serial.init();
    device::motor.init();

    device::motor.set_pid_param(10, 0, 0, 10, 0, 0);

    // callback register
    device::can_server.set_callback(can_receive_callback);
    device::serial.set_callback(serial_receive_callback);
    device::key.set_callback(key_callback);
    device::button_1.set_callback(button_1_callback);
    device::button_2.set_callback(button_2_callback);
    device::timer.add_task(1000, led_toggle_timer_callback);
    device::timer.add_task(2000, serial_timer_callback);
    device::timer.add_task(10, watchdog_timer_callback);
    device::timer.add_task(1, motor_control_timer_callback);

    // activity start
    device::timer.start();
    device::serial.receive_idle<base::Serial::Mode::DMA>(data, 10);

    while (true) {
        // be careful, not to block other thread
    }
}
