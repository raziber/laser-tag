#include <gtest/gtest.h>
#include "decoderHelperFunctions.h"
#include "mocks/conversionFunctions.h"
#include "mocks/mockRmt.h"

TEST(DecoderHelperFunctionsTest, CheckInRange) {
    EXPECT_TRUE(checkInRange(1000, 1000));
    EXPECT_TRUE(checkInRange(1000 + DECODE_MARGIN - 1, 1000));
    EXPECT_TRUE(checkInRange(1000 - DECODE_MARGIN + 1, 1000));
    EXPECT_FALSE(checkInRange(1000 + DECODE_MARGIN + 1, 1000));
    EXPECT_FALSE(checkInRange(1000 - DECODE_MARGIN - 1, 1000));
}

TEST(DecoderHelperFunctionsTest, ParseLogic0) {
    mock_rmt_item32_t mock_item = {560, 1, 560, 0};
    rmt_item32_t real_item;
    convertToReal(&mock_item, &real_item);
    EXPECT_TRUE(parseLogic0(&real_item));
}

TEST(DecoderHelperFunctionsTest, ParseLogic1) {
    mock_rmt_item32_t mock_item = {560, 1, 1690, 0};
    rmt_item32_t real_item;
    convertToReal(&mock_item, &real_item);
    EXPECT_TRUE(parseLogic1(&real_item));
}

TEST(DecoderHelperFunctionsTest, ParseLeadingCode) {
    mock_rmt_item32_t mock_item = {9000, 1, 4500, 0};
    rmt_item32_t real_item;
    convertToReal(&mock_item, &real_item);
    EXPECT_TRUE(parseLeadingCode(&real_item));
}

TEST(DecoderHelperFunctionsTest, GetBit) {
    mock_rmt_item32_t mock_item1 = {560, 1, 1690, 0}; // Logic 1
    mock_rmt_item32_t mock_item2 = {560, 1, 560, 0};  // Logic 0
    rmt_item32_t real_item1, real_item2;
    convertToReal(&mock_item1, &real_item1);
    convertToReal(&mock_item2, &real_item2);

    uint16_t data = 0;
    EXPECT_TRUE(getBit(&real_item1, data, 0));
    EXPECT_EQ(data, 1);
    EXPECT_TRUE(getBit(&real_item2, data, 1));
    EXPECT_EQ(data, 1); // Should still be 1 because we're setting the second bit to 0
}

TEST(DecoderHelperFunctionsTest, CheckInvertedBit) {
    mock_rmt_item32_t mock_item1 = {560, 1, 1690, 0}; // Logic 1
    mock_rmt_item32_t mock_item2 = {560, 1, 560, 0};  // Logic 0
    rmt_item32_t real_item1, real_item2;
    convertToReal(&mock_item1, &real_item1);
    convertToReal(&mock_item2, &real_item2);

    uint16_t data = 0;
    EXPECT_TRUE(getBit(&real_item1, data, 0)); // Set data to 1
    EXPECT_FALSE(checkInvertedBit(&real_item1, data, 0)); // Inverted check, should fail
    EXPECT_TRUE(checkInvertedBit(&real_item2, data, 1));  // Should pass
    EXPECT_FALSE(checkInvertedBit(&real_item2, data, 0)); // Inverted check, should fail
}

TEST(DecoderHelperFunctionsTest, ParseFrame) {
    mock_rmt_item32_t mock_items[34]; // Example frame data, fill with appropriate test values
    rmt_item32_t real_items[34];
    uint16_t address = 0, command = 0;

    // Fill mock_items with test data matching a valid frame structure
    mock_items[0] = {9000, 1, 4500, 0}; // Leading code
    // Fill address bits
    for (int i = 1; i <= 8; ++i) {
        mock_items[i] = {560, 1, 1690, 0}; // Logic 1 bits
    }
    // Fill command bits
    for (int i = 9; i <= 16; ++i) {
        mock_items[i] = {560, 1, 560, 0}; // Logic 0 bits
    }
    for (int i = 0; i < 34; ++i) {
        convertToReal(&mock_items[i], &real_items[i]);
    }
    EXPECT_TRUE(parseFrame(real_items, address, command));
}

TEST(DecoderHelperFunctionsTest, ParseRepeatFrame) {
    mock_rmt_item32_t mock_item = {9000, 1, 2250, 0};
    rmt_item32_t real_item;
    convertToReal(&mock_item, &real_item);
    EXPECT_TRUE(parseRepeatFrame(&real_item));
}
