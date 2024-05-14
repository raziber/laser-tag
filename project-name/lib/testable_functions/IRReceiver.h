#ifdef EMBEDDED_BUILD
#ifndef IR_RECEIVER_H
#define IR_RECEIVER_H

#include "driver/rmt.h"

void processIrData(RingbufHandle_t rb, int sensorId);

#endif // IR_RECEIVER_H
#endif // EMBEDDED_BUILD