#pragma once

/**
 * @file    IRDecoder.hpp
 * @brief   Class definition for IRDecoder.
 *
 * The `IRDecoder` class is responsible for decoding a sequence of `rmt_item32_t` structures
 * representing received IR signals into address and command values according to a specific protocol.
 * It uses protocol settings provided by `IRProtocolSettings` to interpret the signal timings.
 *
 * ### Key Features:
 * - Decodes raw IR data into meaningful address and command.
 * - Supports multiple IR protocols through protocol settings.
 * - Can be used independently or in conjunction with `IRReceiver`.
 *
 * ### Usage Example:
 * ```cpp
 * // Include necessary headers
 * #include "IRDecoder.hpp"
 * 
 * // Assume rawData is a std::vector<rmt_item32_t> received from IRReceiver
 * std::vector<rmt_item32_t> rawData = *** received data ***;
 * 
 * // Create an IRDecoder instance for the NEC protocol
 * IRDecoder decoder(IRProtocol::NEC);
 * 
 * // Decode the raw data
 * uint32_t address = 0;
 * uint32_t command = 0;
 * esp_err_t err = decoder.decode(rawData, address, command);
 * if (err == ESP_OK) {
 *     // Successfully decoded
 * }
 * ```
 *
 * ### Notes:
 * - The decoder relies on `IRProtocolSettings` for protocol-specific parameters.
 * - Ensure that the raw data provided is correctly received and formatted.
 *
 * ### Important Methods:
 * - `decode(const std::vector<rmt_item32_t>& rawData, uint32_t& address, uint32_t& command)`: Decodes the raw IR data.
 *
 * ### Error Handling:
 * - Returns `esp_err_t` to indicate success or failure.
 * - Errors are logged for debugging purposes.
 *
 * @see IRProtocol
 * @see IRProtocolSettings
 * @see IRReceiver
 * @see IRReceiver.hpp
 * @see rmt_item32_t
 * 
 * @note This class does not handle the reception of IR signals. Use `IRReceiver` to receive and decode signals.
 * 
 * @warning Ensure that the raw data corresponds to the protocol specified during decoder initialization.
 * 
 * @author  Razi Berg
 * @date    23/09/2024
 */

#include <driver/rmt.h>
#include <Arduino.h>
#include <memory>
#include <vector>
#include "IRProtocols/IRProtocolFactory.hpp"

class IRDecoder {
public:
    IRDecoder(std::unique_ptr<IRProtocolSettings> protocolSettings);
    ~IRDecoder();

    esp_err_t decode(const std::vector<rmt_item32_t>& rawData, uint32_t& address, uint32_t& command);

private:
    std::unique_ptr<IRProtocolSettings> protocolSettings_;
    
    esp_err_t decodeProtocol(const std::vector<rmt_item32_t>& items, uint32_t& address, uint32_t& command) const;

    // Helper functions for decoding
    bool matchDuration(uint32_t measured_ticks, uint32_t expected_us) const;
};
