#include <cassert>

#include "param.hpp"
#include <main.h>

const char* __assert_file = nullptr;
int __assert_line = 0;
const char* __assert_function = nullptr;
const char* __assert_expression = nullptr;

void __assert_func(const char* file, int line, const char* function, const char* expression)
{
    __disable_irq();
    __assert_file = file;
    __assert_line = line;
    __assert_function = function;
    __assert_expression = expression;

    device::led_r.set();
    device::led_g.reset();
    device::led_b.reset();

    while (true)
        ;
}