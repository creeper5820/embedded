#include "./entrypoint.hpp"

#include "base/gpio.hpp"
#include "base/timer.hpp"

#include "param.hpp"
#include <cassert>

void entrypoint()
{

    while (true) {

        device::led_r.toggle();
        base::delay(100);
    };
}
