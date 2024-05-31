#ifdef EMBEDDED_BUILD
#ifndef IR_TRANSMITTER_H
#define IR_TRANSMITTER_H

class IRTransmitter {
public:
    IRTransmitter(gpio_num_t gpio_num, rmt_channel_t channel);
    void init();
    void sendCommand(uint32_t address, uint32_t command);

private:
    gpio_num_t gpio_num_;
    rmt_channel_t channel_;
};

#endif // IR_TRANSMITTER_H
#endif // EMBEDDED_BUILD
