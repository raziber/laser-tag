#pragma once

/**
 * @file    IREncoder.hpp
 * @brief   Class definition for IREncoder.
 *
 * The `IREncoder` class is responsible for encoding data into a sequence of `rmt_item32_t` structures
 * that represent the IR signal according to a specific protocol. It uses protocol settings provided
 * by `IRProtocolSettings` to generate the correct signal timings.
 *
 * ### Key Features:
 * - Encodes address and command into IR signal format.
 * - Supports multiple IR protocols through protocol settings.
 * - Provides an easy interface to create IR packets for transmission.
 *
 * ### Usage Example:
 * ```cpp
 * // Include necessary headers
 * #include "IREncoder.hpp"
 * 
 * // Create an IREncoder instance for the NEC protocol
 * IREncoder encoder(IRProtocol::NEC);
 * 
 * // Create a packet
 * std::vector<rmt_item32_t> packet;
 * esp_err_t err = encoder.createPacket(packet, 0x10, 0x20);
 * if (err == ESP_OK) {
 *     // Packet is ready for transmission
 * }
 * ```
 *
 * ### Notes:
 * - The encoder relies on `IRProtocolSettings` for protocol-specific parameters.
 * - Ensure that the protocol specified is supported and properly implemented.
 *
 * ### Important Methods:
 * - `createPacket(std::vector<rmt_item32_t>& packet, uint32_t address, uint32_t command)`: Generates the IR signal packet.
 *
 * ### Error Handling:
 * - Returns `esp_err_t` to indicate success or failure.
 * - Errors are logged for debugging purposes.
 *
 * @see IRProtocol
 * @see IRProtocolSettings
 * @see IRTransmitter
 * @see IRTransmitter.hpp
 * @see IRProtocolFactory
 * @see IRProtocolFactory.hpp
 * @see rmt_item32_t
 * 
 * @note This class does not handle the actual transmission of the IR signal. Use `IRTransmitter` for sending packets.
 * 
 * @author  Razi Berg
 * @date    23/09/2024
 */

#include <driver/rmt.h>
#include <Arduino.h>
#include <vector>
#include <memory>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "IRProtocols/IRProtocolFactory.hpp"

class IREncoder {
public:
    IREncoder(IRProtocol protocol);
    ~IREncoder();

    esp_err_t createPacket(std::vector<rmt_item32_t>& packet, uint32_t address, uint32_t command);
private:
    mutable SemaphoreHandle_t encoderMutex_;
    std::unique_ptr<IRProtocolSettings> protocolSettings_;

    rmt_item32_t createPulseItem(uint32_t duration0, uint32_t duration1) const;
    esp_err_t appendPulseToPacket(std::vector<rmt_item32_t>& packet, bool isOne) const;
    esp_err_t appendHeaderToPacket(std::vector<rmt_item32_t>& packet) const;
    esp_err_t appendDataToPacket(std::vector<rmt_item32_t>& packet, uint32_t data, uint32_t bitLength) const;
    esp_err_t appendStopToPacket(std::vector<rmt_item32_t>& packet) const;
    esp_err_t appendInvertedDataToPacket(std::vector<rmt_item32_t>& packet, uint32_t data, uint32_t bitLength) const;

    esp_err_t validatePacketInput(uint32_t address, uint32_t command) const;
    esp_err_t initializePacket(std::vector<rmt_item32_t>& packet) const;
    esp_err_t assemblePacket(std::vector<rmt_item32_t>& packet, uint32_t address, uint32_t command) const;

    // Prevent copying
    IREncoder(const IREncoder&) = delete;
    IREncoder& operator=(const IREncoder&) = delete;
};
