#include "irInitFunctions.h"

#include "configurationBackend.h"
#include "protocolManager.h"
#include "driver/rmt.h"
#include <Arduino.h>
#include "utils.h"

namespace IRDevices {
    std::vector<std::unique_ptr<IRReceiver>> irReceivers;
    std::vector<std::unique_ptr<IRTransmitter>> irTransmitters;

    void initReceivers(const Decoder* decoder, const IProtocolSettings* settings) {
        int channel = 0;

        for (int i = 0; i < IRConfig::NUM_SENSORS; ++i) {
            if (channel >= RMT_CHANNEL_MAX) {
                Serial.printf("Exceeded max RMT channels for receivers at channel %d\n", channel);
                return;
            }
            IRDevices::irReceivers.push_back(std::make_unique<IRReceiver>((gpio_num_t)IRConfig::sensorPins[i], (rmt_channel_t)channel++, settings, decoder));
        }
    }

    void initTransmitters(const Encoder* encoder, const IProtocolSettings* settings) {
        int channel = 0;
        
        for (int i = 0; i < IRConfig::NUM_SHOOTERS; ++i) {
            if (channel >= RMT_CHANNEL_MAX) {
                Serial.printf("Exceeded max RMT channels for transmitters at channel %d\n", channel);
                return;
            }
            IRDevices::irTransmitters.push_back(std::make_unique<IRTransmitter>((gpio_num_t)IRConfig::shooterPins[i], (rmt_channel_t)channel++, settings, encoder));
        }
    }

    void begin(Protocol protocol){
        ProtocolManager protocolManager;
        protocolManager.selectProtocol(protocol);                           // Select a protocol at runtime
        const IProtocolSettings* settings = protocolManager.getSettings();  // Retrieve settings
        auto decoder = std::make_unique<Decoder>(settings);
        auto encoder = std::make_unique<Encoder>(settings);
        initReceivers(decoder.get(), settings);
        initTransmitters(encoder.get(), settings);
        Utils::safeSerialPrintln("All IR receivers and transmitters initialized.");
    }
}
