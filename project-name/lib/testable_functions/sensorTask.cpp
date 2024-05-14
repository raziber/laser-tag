#ifdef EMBEDDED_BUILD

#include "sensorTask.h"
#include "rmtManager.h"
#include "driver/rmt.h"
#include "IRReceiver.h"
#include "configuration.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

void rmtRxTask(void* arg) {
    int sensorId = (int)arg;
    initRmtRx(sensorId, (gpio_num_t)sensorPins[sensorId]);

    // set up RMT buffer
    RingbufHandle_t rb = NULL;
    rmt_get_ringbuf_handle((rmt_channel_t)sensorId, &rb);
    rmt_rx_start((rmt_channel_t)sensorId, true);

    while (true) {
        processIrData(rb, sensorId);
    }
}

#endif // EMBEDDED_BUILD
