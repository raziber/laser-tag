#pragma once

#include <stdint.h>

// MFRC522 Registers
enum class Register : uint8_t {
    // Page 0: Command and Status
    CommandRegister             = 0x01,
    CommIEnReg                  = 0x02,
    DivIEnReg                   = 0x03,
    CommIrqReg                  = 0x04,
    DivIrqReg                   = 0x05,
    ErrorReg                    = 0x06,
    Status1Reg                  = 0x07,
    Status2Reg                  = 0x08,
    FIFODataRegister            = 0x09,
    FIFOLevelRegister           = 0x0A,
    WaterLevelReg               = 0x0B,
    ControlReg                  = 0x0C,
    BitFramingRegister          = 0x0D,
    CollisionReg                = 0x0E,
    // Page 1: Command
    ModeReg                     = 0x11,
    TxModeReg                   = 0x12,
    RxModeReg                   = 0x13,
    TxControlRegister           = 0x14,
    TxASKReg                    = 0x15,
    TxSelReg                    = 0x16,
    RxSelReg                    = 0x17,
    RxThresholdReg              = 0x18,
    DemodReg                    = 0x19,
    // Page 2: Configuration
    MfTxReg                     = 0x1C,
    MfRxReg                     = 0x1D,
    SerialSpeedReg              = 0x1F,
    CRCResultHighRegister       = 0x21,
    CRCResultLowRegister        = 0x22,
    ModWidthReg                 = 0x24,
    RFConfigurationRegister     = 0x26,
    GsNReg                      = 0x27,
    CWGsPReg                    = 0x28,
    ModGsPReg                   = 0x29,
    TModeReg                    = 0x2A,
    TPrescalerReg               = 0x2B,
    TReloadHighReg              = 0x2C,
    TReloadLowReg               = 0x2D,
    // Page 3: Test Registers
    VersionReg                  = 0x37,
    // ... add other registers as needed ...
};
