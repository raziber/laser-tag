#pragma once

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

    ProtocolSettings(
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

ProtocolSettings getProtocolSettings(Protocol protocol);