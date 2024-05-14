#ifdef EMBEDDED_BUILD
#ifndef RMT_MANAGER_H
#define RMT_MANAGER_H

#include "driver/rmt.h"

void initRmtRx(int sensorId, gpio_num_t gpioNum);

#endif // RMT_MANAGER_H
#endif // EMBEDDED_BUILD
