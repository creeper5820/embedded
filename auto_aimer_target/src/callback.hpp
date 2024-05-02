#pragma once

#include "base/can.hpp"
#include "base/serial.hpp"
#include "base/timer.hpp"

inline base::Timer timer(&htim1);
inline base::CanServer can_server(&hcan1);
