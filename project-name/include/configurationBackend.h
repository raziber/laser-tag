#ifndef CONFIGURATION_BACKEND_H
#define CONFIGURATION_BACKEND_H

static const int sensorPins[] = SENSOR_PINS;
#define NUM_SENSORS (sizeof(sensorPins) / sizeof(sensorPins[0]))

static const int shooterPins[] = SHOOTER_PINS;
#define NUM_SHOOTERS (sizeof(shooterPins) / sizeof(shooterPins[0]))


// IR communications
#ifdef NEC
    #define DECODE_MARGIN NEC_DECODE_MARGIN
    #define PAYLOAD_ZERO_DURATION_0 NEC_PAYLOAD_ZERO_DURATION_0
    #define PAYLOAD_ZERO_DURATION_1 NEC_PAYLOAD_ZERO_DURATION_1
    #define PAYLOAD_ONE_DURATION_0 NEC_PAYLOAD_ONE_DURATION_0
    #define PAYLOAD_ONE_DURATION_1 NEC_PAYLOAD_ONE_DURATION_1
    #define LEADING_CODE_DURATION_0 NEC_LEADING_CODE_DURATION_0
    #define LEADING_CODE_DURATION_1 NEC_LEADING_CODE_DURATION_1
    #define REPEAT_CODE_DURATION_0 NEC_REPEAT_CODE_DURATION_0
    #define REPEAT_CODE_DURATION_1 NEC_REPEAT_CODE_DURATION_1
    #define ADDRESS_BITS NEC_ADRESS_BITS
    #define COMMAND_BITS NEC_COMMAND_BITS
    #define LEADING_ITEM_COUNT NEC_LEADING_ITEM_COUNT
    #define REPEAT_FRAME_ITEM_COUNT NEC_REPEAT_FRAME_ITEM_COUNT

    #ifdef HAS_STOP_BIT
        #define STOP_BIT_DURATION_0 NEC_STOP_BIT_DURATION_0
        #define STOP_BIT_DURATION_1 NEC_STOP_BIT_DURATION_1
    #endif
#endif
#ifdef SAMSUNG
    #define DECODE_MARGIN SAMSUNG_DECODE_MARGIN
    #define PAYLOAD_ZERO_DURATION_0 SAMSUNG_PAYLOAD_ZERO_DURATION_0
    #define PAYLOAD_ZERO_DURATION_1 SAMSUNG_PAYLOAD_ZERO_DURATION_1
    #define PAYLOAD_ONE_DURATION_0 SAMSUNG_PAYLOAD_ONE_DURATION_0
    #define PAYLOAD_ONE_DURATION_1 SAMSUNG_PAYLOAD_ONE_DURATION_1
    #define LEADING_CODE_DURATION_0 SAMSUNG_LEADING_CODE_DURATION_0
    #define LEADING_CODE_DURATION_1 SAMSUNG_LEADING_CODE_DURATION_1
    #define REPEAT_CODE_DURATION_0 SAMSUNG_REPEAT_CODE_DURATION_0
    #define REPEAT_CODE_DURATION_1 SAMSUNG_REPEAT_CODE_DURATION_1
    #define ADDRESS_BITS SAMSUNG_ADRESS_BITS
    #define COMMAND_BITS SAMSUNG_COMMAND_BITS
    #define LEADING_ITEM_COUNT SAMSUNG_LEADING_ITEM_COUNT
    #define REPEAT_FRAME_ITEM_COUNT SAMSUNG_REPEAT_FRAME_ITEM_COUNT
    
    #ifdef HAS_STOP_BIT
        #define STOP_BIT_DURATION_0 SAMSUNG_STOP_BIT_DURATION_0
        #define STOP_BIT_DURATION_1 SAMSUNG_STOP_BIT_DURATION_1
    #endif
#endif
#ifdef LTTO
    #define DECODE_MARGIN LTTO_DECODE_MARGIN
    #define PAYLOAD_ZERO_DURATION_0 LTTO_PAYLOAD_ZERO_DURATION_0
    #define PAYLOAD_ZERO_DURATION_1 LTTO_PAYLOAD_ZERO_DURATION_1
    #define PAYLOAD_ONE_DURATION_0 LTTO_PAYLOAD_ONE_DURATION_0
    #define PAYLOAD_ONE_DURATION_1 LTTO_PAYLOAD_ONE_DURATION_1
    #define LEADING_CODE_DURATION_0 LTTO_LEADING_CODE_DURATION_0
    #define LEADING_CODE_DURATION_1 LTTO_LEADING_CODE_DURATION_1
    #define REPEAT_CODE_DURATION_0 LTTO_REPEAT_CODE_DURATION_0
    #define REPEAT_CODE_DURATION_1 LTTO_REPEAT_CODE_DURATION_1
    #define ADDRESS_BITS LTTO_ADRESS_BITS
    #define COMMAND_BITS LTTO_COMMAND_BITS
    #define LEADING_ITEM_COUNT LTTO_LEADING_ITEM_COUNT
    #define REPEAT_FRAME_ITEM_COUNT LTTO_REPEAT_FRAME_ITEM_COUNT
    
    #ifdef HAS_STOP_BIT
        #define STOP_BIT_DURATION_0 LTTO_STOP_BIT_DURATION_0
        #define STOP_BIT_DURATION_1 LTTO_STOP_BIT_DURATION_1
    #endif
#endif
#ifdef CUSTOM_PROTOCOL
    #define DECODE_MARGIN CUSTOM_PROTOCOL_DECODE_MARGIN
    #define PAYLOAD_ZERO_DURATION_0 CUSTOM_PROTOCOL_PAYLOAD_ZERO_DURATION_0
    #define PAYLOAD_ZERO_DURATION_1 CUSTOM_PROTOCOL_PAYLOAD_ZERO_DURATION_1
    #define PAYLOAD_ONE_DURATION_0 CUSTOM_PROTOCOL_PAYLOAD_ONE_DURATION_0
    #define PAYLOAD_ONE_DURATION_1 CUSTOM_PROTOCOL_PAYLOAD_ONE_DURATION_1
    #define LEADING_CODE_DURATION_0 CUSTOM_PROTOCOL_LEADING_CODE_DURATION_0
    #define LEADING_CODE_DURATION_1 CUSTOM_PROTOCOL_LEADING_CODE_DURATION_1
    #define REPEAT_CODE_DURATION_0 CUSTOM_PROTOCOL_REPEAT_CODE_DURATION_0
    #define REPEAT_CODE_DURATION_1 CUSTOM_PROTOCOL_REPEAT_CODE_DURATION_1
    #define ADDRESS_BITS CUSTOM_PROTOCOL_ADRESS_BITS
    #define COMMAND_BITS CUSTOM_PROTOCOL_COMMAND_BITS
    #define LEADING_ITEM_COUNT CUSTOM_PROTOCOL_LEADING_ITEM_COUNT
    #define REPEAT_FRAME_ITEM_COUNT CUSTOM_PROTOCOL_REPEAT_FRAME_ITEM_COUNT
    
    #ifdef HAS_STOP_BIT
        #define STOP_BIT_DURATION_0 CUSTOM_PROTOCOL_STOP_BIT_DURATION_0
        #define STOP_BIT_DURATION_1 CUSTOM_PROTOCOL_STOP_BIT_DURATION_1
    #endif
#endif

// macros for all protocols
#ifdef HAS_INVERTED_ADDRESS
    #define HAS_INVERTED_ADDRESS_BOOL true
    #define ADERESS_ITEM_COUNT 2 * ADDRESS_BITS
#else
#define HAS_INVERTED_ADDRESS_BOOL false
    #define ADERESS_ITEM_COUNT ADDRESS_BITS
#endif
#ifdef HAS_INVERTED_COMMAND
    #define HAS_INVERTED_COMMAND_BOOL true
    #define COMMAND_ITEM_COUNT 2 * COMMAND_BITS
#else
    #define HAS_INVERTED_COMMAND_BOOL false
    #define COMMAND_ITEM_COUNT COMMAND_BITS
#endif
#ifdef HAS_STOP_BIT
    #define STOP_ITEM_COUNT 1
#else
    #define STOP_ITEM_COUNT 0
#endif

#define FRAME_ITEM_COUNT (LEADING_ITEM_COUNT + ADERESS_ITEM_COUNT + COMMAND_ITEM_COUNT + STOP_ITEM_COUNT)

#endif // CONFIGURATION_BACKEND_H
