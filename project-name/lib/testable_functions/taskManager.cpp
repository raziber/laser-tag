#ifdef EMBEDDED_BUILD

#include "taskManager.h"
#include "configuration.h"
#include "sensorTask.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

void createSensorTasks() {
    // create a new task for each sensor
    for (int i = 0; i < NUM_SENSORS; i++) {
        xTaskCreate(rmtRxTask, "rmt_rx_task", 2048, (void*)i, 10, NULL);
    }
}

#endif // EMBEDDED_BUILD
