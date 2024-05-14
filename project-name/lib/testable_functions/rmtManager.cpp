#ifdef EMBEDDED_BUILD
#include "rmtManager.h"
#include "configuration.h"

void initRmtRx(int sensorId, gpio_num_t gpioNum) {
    rmt_config_t rmtRxConfig;
    rmtRxConfig.rmt_mode = RMT_MODE_RX;
    rmtRxConfig.channel = (rmt_channel_t)sensorId;  // Use sensorId as RMT channel
    rmtRxConfig.clk_div = RMT_CLK_DIV;
    rmtRxConfig.gpio_num = gpioNum;  // Set GPIO pin
    rmtRxConfig.mem_block_num = 1;
    rmtRxConfig.rx_config.filter_en = true;
    rmtRxConfig.rx_config.filter_ticks_thresh = 100;
    rmtRxConfig.rx_config.idle_threshold = RMT_CLK_DIV * 10;

    rmt_config(&rmtRxConfig);
    rmt_driver_install(rmtRxConfig.channel, RMT_RX_BUFFER_SIZE, 0);
}

#endif // EMBEDDED_BUILD