#pragma once

#include <Arduino.h>

// PCD (reader) Commands
enum class Command : uint8_t {
    Idle                 = 0x00,
    Memory               = 0x01,
    GenerateRandomID     = 0x02,
    CalculateCRC         = 0x03,
    Transmit             = 0x04,
    NoCommandChange      = 0x07,
    Receive              = 0x08,
    Transceive           = 0x0C,
    MIFAREAuthenticate   = 0x0E,
    SoftReset            = 0x0F
};

// PICC (tag) Commands
enum class PICCCommand : uint8_t {
    RequestA              = 0x26,
    WakeUpA               = 0x52,
    AntiCollisionCL1      = 0x93,
    AntiCollisionCL2      = 0x95,
    AntiCollisionCL3      = 0x97,
    SelectCascadeLevel1   = 0x93,
    SelectCascadeLevel2   = 0x95,
    SelectCascadeLevel3   = 0x97,
    CascadeTag            = 0x88,
    HaltA                 = 0x50,
    AuthenticateKeyA      = 0x60,
    AuthenticateKeyB      = 0x61,
    Read                  = 0x30,
    Write                 = 0xA0,
    Increment             = 0xC1,
    Decrement             = 0xC0,
    Restore               = 0xC2,
    Transfer              = 0xB0,
    // Add other commands as needed
};