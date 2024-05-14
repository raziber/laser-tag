#ifdef EMBEDDED_BUILD
#include "IRReceiver.h"

void processIrData(RingbufHandle_t rb, int sensorId){
    size_t rxSize = 0;
    // block until buffer is not empty
    rmt_item32_t* item = (rmt_item32_t*) xRingbufferReceive(rb, &rxSize, portMAX_DELAY);

    if (item) {
        // TODO: run code to parse the received IR signal

        // release buffer for reuse
        vRingbufferReturnItem(rb, (void*) item);
    }
}

#endif // EMBEDDED_BUILD