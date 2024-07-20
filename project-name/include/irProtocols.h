#pragma once

#include "irProtocolsConfig.h"

enum class Protocol {
    NEC,
    SAMSUNG,
    LTTO,
    CUSTOM
};

struct ProtocolSettings {
    int decode_margin;
    int leading_code_duration_0;
    int leading_code_duration_1;
    int payload_zero_duration_0;
    int payload_zero_duration_1;
    int payload_one_duration_0;
    int payload_one_duration_1;
    int repeat_code_duration_0;
    int repeat_code_duration_1;
    bool has_stop_bit;
    int stop_bit_duration_0;
    int stop_bit_duration_1;
    int leading_item_count;
    int repeat_frame_item_count;
    int address_bits;
    int command_bits;
    bool has_inverted_address;
    bool has_inverted_command;
    int address_item_count;
    int command_item_count;
    int stop_item_count;
    int frame_item_count;

    constexpr ProtocolSettings(
        int decode_margin,
        int leading_code_duration_0,
        int leading_code_duration_1,
        int payload_zero_duration_0,
        int payload_zero_duration_1,
        int payload_one_duration_0,
        int payload_one_duration_1,
        int repeat_code_duration_0,
        int repeat_code_duration_1,
        bool has_stop_bit,
        int stop_bit_duration_0,
        int stop_bit_duration_1,
        int leading_item_count,
        int repeat_frame_item_count,
        int address_bits,
        int command_bits,
        bool has_inverted_address,
        bool has_inverted_command,
        int address_item_count,
        int command_item_count,
        int stop_item_count,
        int frame_item_count)
        : decode_margin(decode_margin),
          leading_code_duration_0(leading_code_duration_0),
          leading_code_duration_1(leading_code_duration_1),
          payload_zero_duration_0(payload_zero_duration_0),
          payload_zero_duration_1(payload_zero_duration_1),
          payload_one_duration_0(payload_one_duration_0),
          payload_one_duration_1(payload_one_duration_1),
          repeat_code_duration_0(repeat_code_duration_0),
          repeat_code_duration_1(repeat_code_duration_1),
          has_stop_bit(has_stop_bit),
          stop_bit_duration_0(stop_bit_duration_0),
          stop_bit_duration_1(stop_bit_duration_1),
          leading_item_count(leading_item_count),
          repeat_frame_item_count(repeat_frame_item_count),
          address_bits(address_bits),
          command_bits(command_bits),
          has_inverted_address(has_inverted_address),
          has_inverted_command(has_inverted_command),
          address_item_count(address_item_count),
          command_item_count(command_item_count),
          stop_item_count(stop_item_count),
          frame_item_count(frame_item_count) {}
};

constexpr ProtocolSettings getProtocolSettings(Protocol protocol) {
    switch (protocol) {
        case Protocol::NEC:
            return ProtocolSettings{
                irProtocolConfig::NEC::DECODE_MARGIN,
                irProtocolConfig::NEC::LEADING_CODE_DURATION_0,
                irProtocolConfig::NEC::LEADING_CODE_DURATION_1,
                irProtocolConfig::NEC::PAYLOAD_ZERO_DURATION_0,
                irProtocolConfig::NEC::PAYLOAD_ZERO_DURATION_1,
                irProtocolConfig::NEC::PAYLOAD_ONE_DURATION_0,
                irProtocolConfig::NEC::PAYLOAD_ONE_DURATION_1,
                irProtocolConfig::NEC::REPEAT_CODE_DURATION_0,
                irProtocolConfig::NEC::REPEAT_CODE_DURATION_1,
                irProtocolConfig::NEC::HAS_STOP_BIT,
                irProtocolConfig::NEC::STOP_BIT_DURATION_0,
                irProtocolConfig::NEC::STOP_BIT_DURATION_1,
                irProtocolConfig::NEC::LEADING_ITEM_COUNT,
                irProtocolConfig::NEC::REPEAT_FRAME_ITEM_COUNT,
                irProtocolConfig::NEC::ADDRESS_BITS,
                irProtocolConfig::NEC::COMMAND_BITS,
                irProtocolConfig::NEC::HAS_INVERTED_ADDRESS,
                irProtocolConfig::NEC::HAS_INVERTED_COMMAND,
                irProtocolConfig::NEC::ADDRESS_ITEM_COUNT,
                irProtocolConfig::NEC::COMMAND_ITEM_COUNT,
                irProtocolConfig::NEC::STOP_ITEM_COUNT,
                irProtocolConfig::NEC::FRAME_ITEM_COUNT
            };
        case Protocol::SAMSUNG:
            return ProtocolSettings{
                irProtocolConfig::SAMSUNG::DECODE_MARGIN,
                irProtocolConfig::SAMSUNG::LEADING_CODE_DURATION_0,
                irProtocolConfig::SAMSUNG::LEADING_CODE_DURATION_1,
                irProtocolConfig::SAMSUNG::PAYLOAD_ZERO_DURATION_0,
                irProtocolConfig::SAMSUNG::PAYLOAD_ZERO_DURATION_1,
                irProtocolConfig::SAMSUNG::PAYLOAD_ONE_DURATION_0,
                irProtocolConfig::SAMSUNG::PAYLOAD_ONE_DURATION_1,
                irProtocolConfig::SAMSUNG::REPEAT_CODE_DURATION_0,
                irProtocolConfig::SAMSUNG::REPEAT_CODE_DURATION_1,
                irProtocolConfig::SAMSUNG::HAS_STOP_BIT,
                irProtocolConfig::SAMSUNG::STOP_BIT_DURATION_0,
                irProtocolConfig::SAMSUNG::STOP_BIT_DURATION_1,
                irProtocolConfig::SAMSUNG::LEADING_ITEM_COUNT,
                irProtocolConfig::SAMSUNG::REPEAT_FRAME_ITEM_COUNT,
                irProtocolConfig::SAMSUNG::ADDRESS_BITS,
                irProtocolConfig::SAMSUNG::COMMAND_BITS,
                irProtocolConfig::SAMSUNG::HAS_INVERTED_ADDRESS,
                irProtocolConfig::SAMSUNG::HAS_INVERTED_COMMAND,
                irProtocolConfig::SAMSUNG::ADDRESS_ITEM_COUNT,
                irProtocolConfig::SAMSUNG::COMMAND_ITEM_COUNT,
                irProtocolConfig::SAMSUNG::STOP_ITEM_COUNT,
                irProtocolConfig::SAMSUNG::FRAME_ITEM_COUNT
            };
        case Protocol::LTTO:
            return ProtocolSettings{
                irProtocolConfig::LTTO::DECODE_MARGIN,
                irProtocolConfig::LTTO::LEADING_CODE_DURATION_0,
                irProtocolConfig::LTTO::LEADING_CODE_DURATION_1,
                irProtocolConfig::LTTO::PAYLOAD_ZERO_DURATION_0,
                irProtocolConfig::LTTO::PAYLOAD_ZERO_DURATION_1,
                irProtocolConfig::LTTO::PAYLOAD_ONE_DURATION_0,
                irProtocolConfig::LTTO::PAYLOAD_ONE_DURATION_1,
                irProtocolConfig::LTTO::REPEAT_CODE_DURATION_0,
                irProtocolConfig::LTTO::REPEAT_CODE_DURATION_1,
                irProtocolConfig::LTTO::HAS_STOP_BIT,
                irProtocolConfig::LTTO::STOP_BIT_DURATION_0,
                irProtocolConfig::LTTO::STOP_BIT_DURATION_1,
                irProtocolConfig::LTTO::LEADING_ITEM_COUNT,
                irProtocolConfig::LTTO::REPEAT_FRAME_ITEM_COUNT,
                irProtocolConfig::LTTO::ADDRESS_BITS,
                irProtocolConfig::LTTO::COMMAND_BITS,
                irProtocolConfig::LTTO::HAS_INVERTED_ADDRESS,
                irProtocolConfig::LTTO::HAS_INVERTED_COMMAND,
                irProtocolConfig::LTTO::ADDRESS_ITEM_COUNT,
                irProtocolConfig::LTTO::COMMAND_ITEM_COUNT,
                irProtocolConfig::LTTO::STOP_ITEM_COUNT,
                irProtocolConfig::LTTO::FRAME_ITEM_COUNT
            };
        case Protocol::CUSTOM:
        default:
            return ProtocolSettings{
                irProtocolConfig::CUSTOM::DECODE_MARGIN,
                irProtocolConfig::CUSTOM::LEADING_CODE_DURATION_0,
                irProtocolConfig::CUSTOM::LEADING_CODE_DURATION_1,
                irProtocolConfig::CUSTOM::PAYLOAD_ZERO_DURATION_0,
                irProtocolConfig::CUSTOM::PAYLOAD_ZERO_DURATION_1,
                irProtocolConfig::CUSTOM::PAYLOAD_ONE_DURATION_0,
                irProtocolConfig::CUSTOM::PAYLOAD_ONE_DURATION_1,
                irProtocolConfig::CUSTOM::REPEAT_CODE_DURATION_0,
                irProtocolConfig::CUSTOM::REPEAT_CODE_DURATION_1,
                irProtocolConfig::CUSTOM::HAS_STOP_BIT,
                irProtocolConfig::CUSTOM::STOP_BIT_DURATION_0,
                irProtocolConfig::CUSTOM::STOP_BIT_DURATION_1,
                irProtocolConfig::CUSTOM::LEADING_ITEM_COUNT,
                irProtocolConfig::CUSTOM::REPEAT_FRAME_ITEM_COUNT,
                irProtocolConfig::CUSTOM::ADDRESS_BITS,
                irProtocolConfig::CUSTOM::COMMAND_BITS,
                irProtocolConfig::CUSTOM::HAS_INVERTED_ADDRESS,
                irProtocolConfig::CUSTOM::HAS_INVERTED_COMMAND,
                irProtocolConfig::CUSTOM::ADDRESS_ITEM_COUNT,
                irProtocolConfig::CUSTOM::COMMAND_ITEM_COUNT,
                irProtocolConfig::CUSTOM::STOP_ITEM_COUNT,
                irProtocolConfig::CUSTOM::FRAME_ITEM_COUNT
            };
    }
}