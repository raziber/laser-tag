#pragma once

#include <memory>
#include "ProtocolTypes.h"
#include "IProtocolSettings.h"
#include "NECProtocolSettings.h"
#include "SamsungProtocolSettings.h"
#include "LTTOProtocolSettings.h"
#include "CustomProtocolSettings.h"

class ProtocolManager {
public:
    ProtocolManager() : currentProtocol(nullptr) {}

    void selectProtocol(Protocol protocol) {
        switch (protocol) {
            case Protocol::NEC:
                currentProtocol = std::make_unique<NECProtocolSettings>();
                break;
            case Protocol::SAMSUNG:
                currentProtocol = std::make_unique<SamsungProtocolSettings>();
                break;
            case Protocol::LTTO:
                currentProtocol = std::make_unique<LTTOProtocolSettings>();
                break;
                case Protocol::CUSTOM:
                currentProtocol = std::make_unique<CustomProtocolSettings>();
                break;
            default:
                currentProtocol = std::make_unique<NECProtocolSettings>();
                break;
        }
    }

    const IProtocolSettings* getSettings() const {
        return currentProtocol.get();
    }

private:
    std::unique_ptr<IProtocolSettings> currentProtocol;
};
