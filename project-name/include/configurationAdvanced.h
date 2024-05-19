#ifndef CONFIGURATION_ADVANCED_H
#define CONFIGURATION_ADVANCED_H

// -----------------------------------------------------
// ----------------- IR communications -----------------
// -----------------------------------------------------

// protocol timings
#ifdef NEC
    #define NEC_DECODE_MARGIN 300    // Increase decode margin to 300

    #define NEC_LEADING_CODE_DURATION_0 9000
    #define NEC_LEADING_CODE_DURATION_1 4500
    #define NEC_PAYLOAD_ZERO_DURATION_0 560
    #define NEC_PAYLOAD_ZERO_DURATION_1 560
    #define NEC_PAYLOAD_ONE_DURATION_0  560
    #define NEC_PAYLOAD_ONE_DURATION_1  1690
    #define NEC_REPEAT_CODE_DURATION_0  9000
    #define NEC_REPEAT_CODE_DURATION_1  2250

    // Frame length constants
    #define NEC_LEADING_ITEM_COUNT 2            // changed from 1 because i am getting 24 total
    #define NEC_REPEAT_FRAME_ITEM_COUNT 4       // changed from 2 because i am getting 4

    #define NEC_ADRESS_BITS             8       // excluding inverted bits
    #define NEC_COMMAND_BITS            8       // excluding inverted bits
    #define HAS_INVERTED_ADDRESS                // uncomment if true
    #define HAS_INVERTED_COMMAND                // uncomment if true
#endif

#ifdef LTTO
    #define LTTO_BASE_ZERO              600     // Base duration for a '0' bit in microseconds
    #define LTTO_BASE_ONE               1200    // Base duration for a '1' bit in microseconds
    #define LTTO_TOLERANCE              100     // Tolerance in microseconds
    #define LTTO_START_BIT              2400    // Duration for the start bit in microseconds
    // #define HAS_INVERTED_ADDRESS // uncomment if true
    // #define HAS_INVERTED_COMMAND // uncomment if true
#endif

#ifdef CUSTOM_PROTOCOL
    // #define HAS_INVERTED_ADDRESS // uncomment if true
    // #define HAS_INVERTED_COMMAND // uncomment if true
#endif

// RMT settings
#define RMT_CLK_DIV 80
#define RMT_RX_BUFFER_SIZE 1024

#endif // CONFIGURATION_ADVANCED_H
