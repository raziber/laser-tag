#pragma once

namespace irProtocolConfig {

    namespace NEC {
        constexpr int DECODE_MARGIN = 300;
        constexpr int LEADING_CODE_DURATION_0 = 9000;
        constexpr int LEADING_CODE_DURATION_1 = 4500;
        constexpr int PAYLOAD_ZERO_DURATION_0 = 560;
        constexpr int PAYLOAD_ZERO_DURATION_1 = 560;
        constexpr int PAYLOAD_ONE_DURATION_0 = 560;
        constexpr int PAYLOAD_ONE_DURATION_1 = 1690;
        constexpr int REPEAT_CODE_DURATION_0 = 9000;
        constexpr int REPEAT_CODE_DURATION_1 = 2250;
        constexpr int STOP_BIT_DURATION_0 = 700;
        constexpr int STOP_BIT_DURATION_1 = 4500;
        constexpr int ADDRESS_BITS = 8;
        constexpr int COMMAND_BITS = 8;
        constexpr bool HAS_STOP_BIT = true;
        constexpr bool HAS_INVERTED_ADDRESS = true;
        constexpr bool HAS_INVERTED_COMMAND = true;
        constexpr int ADDRESS_ITEM_COUNT = ADDRESS_BITS * (1 + HAS_INVERTED_ADDRESS);
        constexpr int COMMAND_ITEM_COUNT = COMMAND_BITS * (1 + HAS_INVERTED_COMMAND);
        constexpr int LEADING_ITEM_COUNT = 1;
        constexpr int REPEAT_FRAME_ITEM_COUNT = 2;
        constexpr int STOP_ITEM_COUNT = HAS_STOP_BIT;
        constexpr int FRAME_ITEM_COUNT = LEADING_ITEM_COUNT + ADDRESS_ITEM_COUNT + COMMAND_ITEM_COUNT + STOP_ITEM_COUNT;
    }

    namespace SAMSUNG {
        constexpr int DECODE_MARGIN = 300;
        constexpr int LEADING_CODE_DURATION_0 = 4600;
        constexpr int LEADING_CODE_DURATION_1 = 4400;
        constexpr int PAYLOAD_ZERO_DURATION_0 = 700;
        constexpr int PAYLOAD_ZERO_DURATION_1 = 1570;
        constexpr int PAYLOAD_ONE_DURATION_0 = 700;
        constexpr int PAYLOAD_ONE_DURATION_1 = 430;
        constexpr int REPEAT_CODE_DURATION_0 = 9000;
        constexpr int REPEAT_CODE_DURATION_1 = 2250;
        constexpr int STOP_BIT_DURATION_0 = 700;
        constexpr int STOP_BIT_DURATION_1 = 4500;
        constexpr int ADDRESS_BITS = 16;
        constexpr int COMMAND_BITS = 16;
        constexpr bool HAS_STOP_BIT = true;
        constexpr bool HAS_INVERTED_ADDRESS = false;
        constexpr bool HAS_INVERTED_COMMAND = false;
        constexpr int ADDRESS_ITEM_COUNT = ADDRESS_BITS * (1 + HAS_INVERTED_ADDRESS);
        constexpr int COMMAND_ITEM_COUNT = COMMAND_BITS * (1 + HAS_INVERTED_COMMAND);
        constexpr int LEADING_ITEM_COUNT = 1;
        constexpr int REPEAT_FRAME_ITEM_COUNT = 4;
        constexpr int STOP_ITEM_COUNT = HAS_STOP_BIT ? 1 : 0;
        constexpr int FRAME_ITEM_COUNT = LEADING_ITEM_COUNT + ADDRESS_ITEM_COUNT + COMMAND_ITEM_COUNT + STOP_ITEM_COUNT;
    }

    namespace LTTO {
        constexpr int DECODE_MARGIN = 100;
        constexpr int LEADING_CODE_DURATION_0 = 2400;
        constexpr int LEADING_CODE_DURATION_1 = 0;
        constexpr int PAYLOAD_ZERO_DURATION_0 = 600;
        constexpr int PAYLOAD_ZERO_DURATION_1 = 0;
        constexpr int PAYLOAD_ONE_DURATION_0 = 1200;
        constexpr int PAYLOAD_ONE_DURATION_1 = 0;
        constexpr int REPEAT_CODE_DURATION_0 = 0;
        constexpr int REPEAT_CODE_DURATION_1 = 0;
        constexpr int STOP_BIT_DURATION_0 = 0;
        constexpr int STOP_BIT_DURATION_1 = 0;
        constexpr int ADDRESS_BITS = 0;
        constexpr int COMMAND_BITS = 0;
        constexpr bool HAS_STOP_BIT = false;
        constexpr bool HAS_INVERTED_ADDRESS = false;
        constexpr bool HAS_INVERTED_COMMAND = false;
        constexpr int ADDRESS_ITEM_COUNT = ADDRESS_BITS * (1 + HAS_INVERTED_ADDRESS);
        constexpr int COMMAND_ITEM_COUNT = COMMAND_BITS * (1 + HAS_INVERTED_COMMAND);
        constexpr int LEADING_ITEM_COUNT = 0;
        constexpr int REPEAT_FRAME_ITEM_COUNT = 0;
        constexpr int STOP_ITEM_COUNT = HAS_STOP_BIT ? 1 : 0;
        constexpr int FRAME_ITEM_COUNT = LEADING_ITEM_COUNT + ADDRESS_ITEM_COUNT + COMMAND_ITEM_COUNT + STOP_ITEM_COUNT;
    }

    namespace CUSTOM {
        constexpr int DECODE_MARGIN = 0;
        constexpr int LEADING_CODE_DURATION_0 = 0;
        constexpr int LEADING_CODE_DURATION_1 = 0;
        constexpr int PAYLOAD_ZERO_DURATION_0 = 0;
        constexpr int PAYLOAD_ZERO_DURATION_1 = 0;
        constexpr int PAYLOAD_ONE_DURATION_0 = 0;
        constexpr int PAYLOAD_ONE_DURATION_1 = 0;
        constexpr int REPEAT_CODE_DURATION_0 = 0;
        constexpr int REPEAT_CODE_DURATION_1 = 0;
        constexpr int STOP_BIT_DURATION_0 = 0;
        constexpr int STOP_BIT_DURATION_1 = 0;
        constexpr int ADDRESS_BITS = 0;
        constexpr int COMMAND_BITS = 0;
        constexpr bool HAS_STOP_BIT = false;
        constexpr bool HAS_INVERTED_ADDRESS = false;
        constexpr bool HAS_INVERTED_COMMAND = false;
        constexpr int ADDRESS_ITEM_COUNT = ADDRESS_BITS * (1 + HAS_INVERTED_ADDRESS);
        constexpr int COMMAND_ITEM_COUNT = COMMAND_BITS * (1 + HAS_INVERTED_COMMAND);
        constexpr int LEADING_ITEM_COUNT = 0;
        constexpr int REPEAT_FRAME_ITEM_COUNT = 0;
        constexpr int STOP_ITEM_COUNT = HAS_STOP_BIT ? 1 : 0;
        constexpr int FRAME_ITEM_COUNT = LEADING_ITEM_COUNT + ADDRESS_ITEM_COUNT + COMMAND_ITEM_COUNT + STOP_ITEM_COUNT;
    }

} // namespace Config
