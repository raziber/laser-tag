#pragma once

#include <Arduino.h>

// MFRC522 Registers
enum class Register : uint8_t {
    CommandRegister             = 0x01,
    CommInterruptEnableRegister = 0x02,
    DivInterruptEnableRegister  = 0x03,
    CommInterruptRequestRegister= 0x04,
    DivInterruptRequestRegister = 0x05,
    ErrorRegister               = 0x06,
    Status1Register             = 0x07,
    Status2Register             = 0x08,
    FIFODataRegister            = 0x09,
    FIFOLevelRegister           = 0x0A,
    WaterLevelRegister          = 0x0B,
    ControlRegister             = 0x0C,
    BitFramingRegister          = 0x0D,
    CollisionRegister           = 0x0E,
    // Page 1: Command
    ModeRegister                = 0x11,
    TxModeRegister              = 0x12,
    RxModeRegister              = 0x13,
    TxControlRegister           = 0x14,
    TxASKRegister               = 0x15,
    RxSelRegister               = 0x17,
    RFConfigurationRegister     = 0x26,
    GsNRegister                 = 0x27,
    CWGsPRegister               = 0x28,
    ModGsPRegister              = 0x29,
    TModeRegister               = 0x2A,
    TPrescalerRegister          = 0x2B,
    TReloadHighRegister         = 0x2C,
    TReloadLowRegister          = 0x2D,
    CRCResultHighRegister       = 0x21,
    CRCResultLowRegister        = 0x22,
    VersionRegister             = 0x37,
};
